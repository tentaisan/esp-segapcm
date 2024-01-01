#include <string>

#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

class sdcard {
    static const gpio_num_t PIN_NUM_CS = GPIO_NUM_5;
    static const int PIN_NUM_MOSI = 23;
    static const gpio_num_t PIN_NUM_MISO = GPIO_NUM_19;
    static const gpio_num_t PIN_NUM_CLK = GPIO_NUM_18;

    esp_vfs_fat_sdmmc_mount_config_t mount_config;
    sdmmc_card_t *card;
    sdmmc_host_t host;
    spi_bus_config_t bus_cfg;
    sdspi_device_config_t slot_config;

public:
    sdcard();
    esp_err_t mount(const std::string mount_point);
};