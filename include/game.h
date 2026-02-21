#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

class game_t {
    U8G2_SSD1306_128X64_NONAME_1_HW_I2C monitor;
public:
    game_t() : monitor(U8G2_R0, U8X8_PIN_NONE) {}

    inline void update_state();

    inline void display();
};
