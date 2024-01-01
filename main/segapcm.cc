#include "segapcm.h"

SegaPCM::Register::Register()
    : flags(1), volume_l(0), volume_r(0), addr(0), loop(0), end(0), delta(0)
{}

SegaPCM::SegaPCM() 
    : tick(0)
{}

void SegaPCM::regist(std::istream& s)
{
    struct {
        std::uint16_t offset;
        std::uint8_t data;
    } __attribute__((__packed__)) d;

    s.read(reinterpret_cast<char *>(&d), sizeof(d));

    std::uint8_t channel = d.offset >> 3 & 0xF;
    std::uint16_t offset = d.offset & 0x0087;

    switch (offset)
    {
        case 0x02:
            registers[channel].volume_l = d.data;
            break;
        
        case 0x03:
            registers[channel].volume_r = d.data;
            break;
        
        case 0x04:
            registers[channel]._loop.low = d.data;
            break;

        case 0x05:
            registers[channel]._loop.high = d.data;
            break;

        case 0x06:
            registers[channel].end = d.data;
            break;

        case 0x07:
            registers[channel].delta = d.data;
            break;

        case 0x84:
            registers[channel]._addr.low = d.data;
            break;

        case 0x85:
            registers[channel]._addr.high = d.data;
            break;

        case 0x86:
            registers[channel].flags = d.data;
            registers[channel]._addr.bank = registers[channel]._loop.bank = (d.data & 0x70) >> 4;
            break;

        default:
            break;
    }

    return;
}

void SegaPCM::block(std::istream &s)
{
    struct {
        std::uint32_t data_length;
        std::uint32_t rom_size;
        std::uint32_t start_address;
    } __attribute__((__packed__)) t;

    s.read(reinterpret_cast<char *>(&t), sizeof(t));

    auto len = t.data_length - 8;
    auto addr = t.start_address;
    auto data = std::vector<uint8_t>(len);

    s.read(reinterpret_cast<char *>(data.data()), len);
    rom.push_back({addr, std::move(data)});
    std::sort(rom.begin(), rom.end(), [](const RomData& l, RomData& r){return l.first < r.first;});
    return;
}

inline const SegaPCM::RomData& SegaPCM::romdata(std::uint32_t addr)
{
    for(const auto& r: rom) {
        if(r.first <= addr && r.first + r.second.size() > addr) return r;
    }

    return rom[0];
}

void SegaPCM::proceed_inter()
{
    prv = cur;
    cur = {0, 0};

	for (int ch = 0; ch < CHANNELS; ch++)
	{
		auto& reg = registers[ch];

		if (!(reg.flags & 1)) {

            if (reg._addr.high == reg.end) {
                if (reg.flags & 2) {
                    reg.flags |= 1;
                    continue;
                } else reg.addr = reg.loop;
            }

            auto addr = reg.addr >> 8;
            if(!(reg.rom.first <= addr && reg.rom.first + reg.rom.second.size() > addr)) reg.rom = romdata(addr);
            if(reg.rom.first <= addr && reg.rom.first + reg.rom.second.size() > addr) {
                auto v = static_cast<std::int16_t>(reg.rom.second[addr - reg.rom.first]) - 0x80;
                cur.first += v * (reg.volume_l & 0x7f);
                cur.second += v * (reg.volume_r & 0x7f);
            }
            reg.addr += reg.delta;
		}
	}

    return;
}

void SegaPCM::proceed()
{
    tick += 625;
    if(tick > 882) {
        proceed_inter();
        tick -= 882;
    }

    buffer.push_back((prv.first + (cur.first - prv.first) * static_cast<double>(tick) / 882) /2);
    buffer.push_back((prv.second + (cur.second - prv.second) * static_cast<double>(tick) / 882)/2);

    if(buffer.size() == 1024) {
        _dac.write(buffer);
        buffer.clear();
    }
}
