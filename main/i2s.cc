#include "i2s.h"

dac::dac(const std::uint32_t rate, const i2s_data_bit_width_t bits, const i2s_slot_mode_t mode) :
    chan_cfg(I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER)),
    std_cfg({
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(rate),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(bits, mode),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_0,
            .ws = GPIO_NUM_15,
            .dout = GPIO_NUM_2,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false
            }
        }
    })
{
    i2s_new_channel(&chan_cfg, &tx_handle, NULL);

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
}

dac::~dac()
{
    i2s_channel_disable(tx_handle);
    i2s_del_channel(tx_handle);
}
