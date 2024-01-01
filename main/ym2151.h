#include <cstdint>
#include <istream>
#include <vector>
#include <tuple>
#include <algorithm>

#include "devices.h"

class YM2151 :  public Device {

public:
    constexpr std::uint8_t command_id() const override {return 0x54;}
    constexpr std::uint8_t block_id() const override {return 0;}
    void regist(std::istream &s) override;
    void block(std::istream &s) override {};
};

