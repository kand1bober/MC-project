#include <U8g2lib.h>

#include "bitmap.h"
#include "timer.h"

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
    int8_t vx_, vy_;
    bool active_;

    bullet_t() : 
        x_(0), y_(0),
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

enum entity_type_t {
    kPlayer,
    kEnemy,
};

class entity_t {
public:
    entity_type_t entity_type_;

    uint8_t x_, y_; // position
    uint8_t w_, h_; // hitbox (hitbox != bitmap size)
    const bitmap_t bitmap;

    uint8_t hp_; // health

    uint8_t y_target_; // y pos, to which entity is moving
    int8_t vx_, vy_; // velocity

    uint8_t y_shoot_target_; // y pos, where entity shoots

    // ctor
    entity_t(entity_type_t entity_type, 
             uint8_t par_x, 
             uint8_t par_y, 
             uint8_t par_w, 
             uint8_t par_h,

             const uint8_t* PROGMEM bits,
             uint8_t bits_w, 
             uint8_t bits_h,

             uint8_t hp, 
             uint8_t y_shoot_target) :
        entity_type_(entity_type),

        x_(par_x),
        y_(par_y),
        w_(par_w),
        h_(par_h), 
        bitmap(bits, bits_w, bits_h), 
        hp_(hp), 

        y_target_(par_y),
        y_shoot_target_(y_shoot_target),
        vx_(0), 
        vy_(0)
    {}
};

enum game_state_t {
    kRunning,
    kPause,
    kHlt,
};

class game_t {
    const uint8_t display_w = 128;
    const uint8_t display_h = 64;
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C monitor_;
    buttons_vector_t buttons_vector_; 
    GameTimer gameTimer;

public:
    game_state_t game_state_;
    entity_t player_;
    entity_t enemy_;

    static constexpr uint8_t kMaxBullets = 8;
    bullet_t bullets_[kMaxBullets]; // craeate static array of possible bullets

// --------- Methods ---------
    game_t() :
        monitor_(U8G2_R0, U8X8_PIN_NONE), 
        player_(kPlayer, 
                0, 28, 
                10, 9, 
                kPlayerBits, 
                10, 9, 
                1, 
                0), 
        enemy_(kEnemy, 
               128 - 11 - 1, 32 - 8/2, 
               11, 8, 
               kEnemyBits, 
               11, 8, 
               5, 
               rand() % (display_h - 8 + 1)), 
        game_state_(kHlt)
    {
        init_controller();
        init_pins_and_interrupts();
        gameTimer.init();
        // srand((unsigned int)time(NULL)); // works better without this line
        
        monitor_.begin();
        monitor_.setBusClock(800000);
    }

    void init_game_state() {
        player_.x_ = 0;
        player_.y_ = 28;
        player_.hp_ = 1;

        enemy_.x_ = 128 - 11 - 1;
        enemy_.y_ = 32 - 8/2;
        enemy_.hp_ = 5;
    }

    inline void play();

    inline void read_buttons();

    inline void update_pause_state();

    inline void shoot(uint8_t x, 
                      uint8_t y, 
                      uint8_t w, 
                      uint8_t h, 
                      int8_t vx, 
                      int8_t vy);

    // hitbox size can be different from bitmap size
    inline void draw_entity(entity_t& entity); 

    inline void draw_bitmap(uint8_t x, uint8_t y, bitmap_t& bitmap);

    inline void update_bullets();

    inline void end_game();

    inline void update_collisions(bullet_t& bullet);

    inline void update_player_pos();

    inline void update_enemy_state(entity_t&);

    inline void update_visual_info_state();

    inline void update_state();
};
