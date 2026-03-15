#pragma once

#include <stdint.h>
#include <U8g2lib.h>

class GameTimer {
private:
    volatile uint32_t ticks_;        // счётчик тиков (1 мс)
    uint32_t last_second_ticks_;     
    uint32_t elapsed_seconds_;       
    uint8_t minutes_;                 
    uint8_t seconds_;                  
    
public:
    GameTimer();
    
    void init();
    void update();
    void display(U8G2 &monitor, uint8_t x, uint8_t y);
    void reset();
    
    uint8_t getMinutes() const { return minutes_; }
    uint8_t getSeconds() const { return seconds_; }
    
    // Для доступа из прерывания
    // friend void timer_tick();

    inline void tick() { ticks_++; }
};

// Глобальный указатель для прерывания
extern GameTimer* gameTimer;
