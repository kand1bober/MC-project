#include "basics.h"

#pragma once

void draw_grid(U8G2 &u);

struct bitmap_t {
    const uint8_t* bits_;   // points to XBM array in PROGMEM
    uint8_t w_, h_;

    bitmap_t(const uint8_t* par_bits, uint8_t par_w, uint8_t par_h)
        : bits_(par_bits), w_(par_w), h_(par_h) 
    {}
};

extern const uint8_t kPlayerBits[];
extern const uint8_t kEnemyBits[];

constexpr int kStarsAmount = 17;
extern const uint8_t stars[kStarsAmount][2];

extern const uint8_t explosion_r3[];
extern const uint8_t explosion_r5[];
extern const uint8_t explosion_r7[];
