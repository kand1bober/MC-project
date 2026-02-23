#include <U8g2lib.h>

#include "bitmap.h"

#pragma once

struct buttons_vector_t {
    bool up;
    bool down;
    bool left;
    bool right;
    bool cross;
};

// moving rectangle 
class bullet_t {
public:
    uint8_t x_, y_;
    uint8_t w_, h_;
    uint8_t vx_, vy_;
    bool active_;

    bullet_t()
      : x_(0), y_(0),
        w_(0), h_(0),
        vx_(0), vy_(0),
        active_(false)
    {}

    // //ctor
    // bullet_t(uint8_t x, uint8_t y, 
    //          uint8_t w, uint8_t h, 
    //          uint8_t vx, uint8_t vy, 
    //          bool active) : 
    //     x_(x), y_(y), 
    //     w_(w), h_(h),
    //     vx_(vx), vy_(vy), 
    //     active_(active) 
    // {}  
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

enum game_state_t {
    kRunning,
    kPause,
    kHlt,
};

class game_t {
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C monitor_;
    buttons_vector_t buttons_vector_; 

public:
    game_state_t game_state_;
    entity_t player_;
    entity_t enemy_; 

    static constexpr uint8_t kMaxBullets = 8;
    bullet_t bullets_[kMaxBullets]; // craeate static array of possible bullets

    game_t() :
        monitor_(U8G2_R0, U8X8_PIN_NONE), 
        player_(0, 28, 8, 0, kPlayerBits, 8, 8), 
        enemy_(120, 28, 7, 7, kEnemyBits, 8, 8), 
        game_state_(kHlt)
    {
        monitor_.begin();
        monitor_.setBusClock(800000);
    }

    inline void read_buttons();

    void shoot(uint8_t x, 
               uint8_t y, 
               uint8_t w, 
               uint8_t h, 
               uint8_t vx, 
               uint8_t vy);

    // hitbox size can be different from bitmap size
    inline void draw_entity(entity_t& entity); 

    inline void draw_bitmap(uint8_t x, uint8_t y, bitmap_t& bitmap);

    inline void update_bullets();

    inline void update_collisions(bullet_t& bullet);

    inline void update_state();
};
