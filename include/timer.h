// #pragma once
// #include <stdint.h>

// class GameTimer {
// private:
//     volatile uint32_t ticks_;        // счётчик тиков (обновляется в прерывании)
//     uint32_t last_second_ticks_;     // для расчёта секунд
//     uint32_t elapsed_seconds_;       // общее количество прошедших секунд
//     uint8_t minutes_;                 // минуты
//     uint8_t seconds_;                  // секунды
    
// public:
//     GameTimer();
    
//     // Настройка таймера
//     void init();
    
//     // Обновление времени (вызывать в основном цикле)
//     void update();
    
//     // Форматированный вывод на дисплей
//     void display(class U8G2 &monitor, uint8_t x, uint8_t y);
    
//     // Сброс таймера
//     void reset();
    
//     // Получить текущее время
//     uint8_t getMinutes() const { return minutes_; }
//     uint8_t getSeconds() const { return seconds_; }
    
//     // Дружественная функция для прерывания
//     friend void timer_tick();
// };

// // Глобальный указатель для доступа из прерывания
// extern GameTimer* gameTimer;
