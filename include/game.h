#include <U8g2lib.h>

#include "entities.h"

#pragma once

struct buttons_vector_t {
    bool up;
    bool down;
    bool left;
    bool right;
    bool cross;
};

class entity_t {
public:
    uint8_t x_, y_;
    uint8_t w_, h_; // hitbox != bitmap size
    const bitmap_t bitmap;
    
    // ctor
    entity_t(uint8_t par_x, 
             uint8_t par_y, 
             uint8_t par_w, 
             uint8_t par_h,

             const uint8_t* PROGMEM bits,
             uint8_t bits_w, 
             uint8_t bits_h) :
        x_(par_x),
        y_(par_y),
        w_(par_w),
        h_(par_h), 
        bitmap(bits, bits_w, bits_h) 
    {}
};

class game_t {
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C monitor_;

    buttons_vector_t buttons_vector_; 
public:
    entity_t player_;
    entity_t enemy_; //TODO: наверное лучше private 

    game_t() : 
        monitor_(U8G2_R0, U8X8_PIN_NONE), 
        player_(0, 0, 0, 0, kPlayerBits, 8, 8), 
        enemy_(124, 32, 0, 0, kEnemyBits, 8, 8)
    {
        monitor_.begin();
        monitor_.setBusClock(800000);
    }

    inline void read_buttons();

    inline void draw_object(entity_t& entity);

    inline void update_state();
};
