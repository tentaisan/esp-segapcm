#include <cstdint>
#include <istream>
#include <ostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <memory>
#include <algorithm>

#include "devices.h"
#include "i2s.h"

class SegaPCM : public Device {
    using RomData = std::pair<std::uint32_t, std::vector<std::uint8_t>>;

    class Register {
    public:
        std::uint8_t flags;     
        std::uint8_t volume_l; 
        std::uint8_t volume_r; 
        union {
            struct {
                std::uint8_t off;
                std::uint8_t low;
                std::uint8_t high;
                std::uint8_t bank;
            } __attribute__((__packed__)) _addr;
            uint32_t addr;
        };
        union {
            struct {
                std::uint8_t off;
                std::uint8_t low;
                std::uint8_t high;
                std::uint8_t bank;
            } __attribute__((__packed__)) _loop;
            uint32_t loop;
        };
        std::uint8_t end;   
        std::uint8_t delta; 
        std::uint8_t offset;
        RomData rom;
        Register();
    };
    
    static const int CHANNELS = 16;
    Register registers[CHANNELS];

    std::vector<RomData> rom;
    const RomData& romdata(std::uint32_t addr);
    void proceed_inter();

    std::pair<std::int16_t, std::int16_t> cur;
    std::pair<std::int16_t, std::int16_t> prv;
    int tick;
    std::vector<std::int16_t> buffer;
    dac _dac;

public:
    SegaPCM();
    constexpr std::uint8_t command_id() const override {return 0xC0;}
    constexpr std::uint8_t block_id() const override {return 0x80;}
    void regist(std::istream& s) override;
    void block(std::istream& s) override;
    void proceed() override;
};

