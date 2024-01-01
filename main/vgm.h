#include <memory>
#include <cstdint>
#include <vector>
#include <istream>
#include <string>

#include "devices.h"

class Vgm {
    std::ifstream file;
    uint32_t gd3_offset;
    uint32_t data_offset;
    uint32_t total_samples;

    std::vector<std::unique_ptr<Device>> devices;
    using deviceID = std::pair<const uint8_t, const uint8_t>;
    std::vector<deviceID> commands;
    std::vector<deviceID> blocks;

    void regist_device(std::unique_ptr<Device> device);

public:
    Vgm(const std::string filename);
    void init();
    void proceed();
    void block();
};

