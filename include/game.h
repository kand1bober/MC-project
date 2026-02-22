#include <U8g2lib.h>

#pragma once
    
const uint8_t stars[][2] = {
    {50, 30},
    {30, 17},
    {60, 18},
    {55, 16},
    {25, 43},
    {100, 43},
    {117, 52},
    {14, 49},
    {24, 24},
    {78, 36},
    {80, 57},
    {107, 11},
    {150, 11},
    {5, 5},
    {8, 7},
    {70, 12},
    {10, 56},
    {70, 25}
};

struct buttons_vector_t {
    bool up;
    bool down;
    bool left;
    bool right;
    bool cross;
};

class object_t {
public:
    uint8_t x, y;
    uint8_t len_x, len_y;
    
    // ctor
    object_t(uint8_t par_x, uint8_t par_y, uint8_t par_len_x, uint8_t par_len_y) :
        x(par_x),
        y(par_y),
        len_x(par_len_x),
        len_y(par_len_y)
    {}
};

class game_t {
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C monitor;

    buttons_vector_t buttons_vector; 
public:
    object_t player;

    game_t() : 
        monitor(U8G2_R0, U8X8_PIN_NONE), 
        player(0, 0, 0, 0) 
    {
        monitor.begin();
        monitor.setBusClock(800000);
    }

    inline void read_buttons();

    inline void draw_object(object_t& object);

    inline void update_state();
};

