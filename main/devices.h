#pragma once

#include <cstdint>
#include <istream>

class Device {
public:
    virtual std::uint8_t command_id() const {return 0;};
    virtual std::uint8_t block_id() const {return 0;};
    virtual void regist(std::istream& s) = 0;
    virtual void block(std::istream& s) = 0;
    virtual void proceed(){};
    virtual void close(){};
};

