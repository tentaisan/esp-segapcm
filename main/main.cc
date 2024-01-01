#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "esp_err.h"
#include "sdcard.h"
#include "i2s.h"
#include "vgm.h"

void play()
{
    auto f = std::ifstream("/sdcard/test.wav");
    auto buffer = std::vector<std::int16_t>(64);
    auto d = dac(31250);
    f.seekg(44, std::ios::beg);

    while(!f.eof()) {
        f.read(reinterpret_cast<char *>(buffer.data()), 64 * sizeof(std::int16_t));
        for_each(buffer.begin(), buffer.end(), [](std::int16_t& v){ v /= 2;});
        d.write(buffer);
    }
}

extern "C" void app_main(void)
{
    auto s = sdcard();
    s.mount("/sdcard");

    auto v = Vgm("/sdcard/test.vgm");
    v.proceed();
    // play();
    return;
 }
