#pragma once

#include <vector>
#include <cstdint>

#include "driver/i2s_std.h"
#include "driver/gpio.h"

class dac {
    i2s_chan_handle_t tx_handle;
    i2s_chan_config_t chan_cfg;
    i2s_std_config_t std_cfg;

public:
    dac(const std::uint32_t rate = 44100, const i2s_data_bit_width_t bits = I2S_DATA_BIT_WIDTH_16BIT, const i2s_slot_mode_t mode = I2S_SLOT_MODE_STEREO);
    ~dac();

    template <typename T>
    void write(const std::vector<T> data)
    {
        ESP_ERROR_CHECK(i2s_channel_write(tx_handle, data.data(), data.size() * sizeof(T), NULL, 100));
    }
};