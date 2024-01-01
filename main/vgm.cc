#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <vector>
#include <utility>
#include <memory>

#include "vgm.h"
#include "devices.h"
#include "segapcm.h"
#include "ym2151.h"

Vgm::Vgm(const std::string filename) :
    file(std::ifstream(filename, std::ios_base::binary)) 
{
    struct {
        union {
            std::uint8_t id[4];
            std::uint32_t _id;
        };
        std::uint32_t eof_offset;
        std::uint32_t version;
        std::uint32_t sn76489_clock;
        std::uint32_t ym2413_clock;
        std::uint32_t gd3_offset;
        std::uint32_t total_samples;
        std::uint32_t loop_offset;
        std::uint32_t loop_samples;
        std::uint32_t rate;
        std::uint16_t sn76489_feedback;
        std::uint8_t sn76489_register_width;
        std::uint8_t sn76489_register_flags;
        std::uint32_t ym2612_clock;
        std::uint32_t ym2151_clock;
        std::uint32_t vgm_data_offset;
        std::uint32_t sega_pcm_clock;
        std::uint32_t sega_pcm_interface;
    } __attribute__((__packed__)) header;

    file.read(reinterpret_cast<char *>(&header), sizeof(header));

    gd3_offset = header.gd3_offset + 20;
    data_offset = header.vgm_data_offset + 0x34;
    total_samples = header.total_samples;

    if(header.sega_pcm_clock && header.sega_pcm_interface) {
        regist_device(std::make_unique<SegaPCM>());
    }

    if(header.ym2151_clock) {
        regist_device(std::make_unique<YM2151>());
    }

    init();
}

inline void Vgm::regist_device(std::unique_ptr<Device> device)
{
    auto p = devices.size();
    devices.push_back(std::move(device));
    commands.push_back({devices[p]->command_id(), p});
    blocks.push_back({devices[p]->block_id(), p});
}

void Vgm::init()
{
    file.seekg(data_offset, std::ios::beg);

    return;
}

void Vgm::block()
{
    uint8_t h[2];
    file.read(reinterpret_cast<char *>(&h[0]), sizeof(h));
    for(const auto& b: blocks)
        if(b.first == h[1]) devices[b.second]->block(file);

    return;
}

void Vgm::proceed()
{
    std::uint8_t command;

    while(1) {

        file.read(reinterpret_cast<char *>(&command), sizeof(std::uint8_t));
        if(command == 0x66 || file.eof()) break;

        std::uint16_t wait = 0;

        switch (command)
        {
        case 0x61:
            file.read(reinterpret_cast<char *>(&wait), sizeof(std::uint16_t));
            break;
        
        case 0x62:
            wait = 735;
            break;

        case 0x63:
            wait = 882;
            break;
        
        case 0x67:
            block();
            break;

        default:
            if(command >> 4 == 7)
                wait = (command & 0xF) + 1;
            break;
        }

        if(wait) {
            for(int i = 0; i < wait; i++)
                for(auto& d: devices) 
                    d->proceed();
            continue;
        }

        for(const auto& c: commands) 
            if(c.first == command) devices[c.second]->regist(file);
    }

    return;
}


