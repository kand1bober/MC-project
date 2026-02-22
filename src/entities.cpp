#include "entities.h"

const uint8_t stars[kStarsAmount][2] = {
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
    {5, 5},
    {8, 7},
    {70, 12},
    {10, 56},
    {70, 25}
};

// triangle, 8x8
const uint8_t PROGMEM kPlayerBits[] = {
    0x01,
    0x03,
    0x07,
    0x0F,
    0x07,
    0x03,
    0x01,
    0x00 
};

// spider, 8x8
const uint8_t PROGMEM kEnemyBits[] = {
    0x3C,
    0x7E,
    0xFF,
    0xD3,
    0xFF,
    0xC6,
    0xC4,
    0x00
};
