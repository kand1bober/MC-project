// #include "timer.h"
// #include <U8g2lib.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>

// // Глобальный указатель на экземпляр таймера
// GameTimer* gameTimer = nullptr;

// // Обработчик прерывания Timer1
// ISR(TIMER1_COMPA_vect) {
//     if (gameTimer) {
//         gameTimer->ticks_++;
//     }
// }

// GameTimer::GameTimer() : 
//     ticks_(0),
//     last_second_ticks_(0),
//     elapsed_seconds_(0),
//     minutes_(0),
//     seconds_(0) {
//     gameTimer = this;
// }

// void GameTimer::init() {
//     cli();  // запретить прерывания
    
//     // Настройка Timer1 в режим CTC (Clear Timer on Compare Match)
//     TCCR1A = 0;
//     TCCR1B = (1 << WGM12);  // режим CTC
    
//     // Делитель частоты 64 для 16 МГц
//     // 16 000 000 / 64 = 250 000 тиков/сек
//     // 250 000 / 1000 = 250 -> прерывание каждую 1 мс
//     TCCR1B |= (1 << CS11) | (1 << CS10);  // prescaler 64
    
//     OCR1A = 249;  // 1 мс при 16 МГц и prescaler 64
    
//     // Разрешить прерывание по совпадению с OCR1A
//     TIMSK1 |= (1 << OCIE1A);
    
//     ticks_ = 0;
//     last_second_ticks_ = 0;
//     elapsed_seconds_ = 0;
//     minutes_ = 0;
//     seconds_ = 0;
    
//     sei();  // разрешить прерывания
// }

// void GameTimer::update() {
//     uint32_t current_ticks;
//     uint8_t oldSREG = SREG;
//     cli();
//     current_ticks = ticks_;
//     SREG = oldSREG;
    
//     // Проверяем, прошла ли секунда (1000 тиков = 1 секунда)
//     if (current_ticks - last_second_ticks_ >= 1000) {
//         last_second_ticks_ = current_ticks;
//         elapsed_seconds_++;
        
//         // Обновляем минуты и секунды
//         minutes_ = (elapsed_seconds_ / 60) % 60;
//         seconds_ = elapsed_seconds_ % 60;
//     }
// }

// void GameTimer::display(U8G2 &monitor, uint8_t x, uint8_t y) {
//     char buffer[6];  // MM:SS + \0
    
//     // Форматируем время как ММ:СС
//     snprintf(buffer, sizeof(buffer), "%02u:%02u", minutes_, seconds_);
    
//     monitor.setFont(u8g2_font_tenthinnerguys_tr);
//     monitor.drawStr(x, y, buffer);
// }

// void GameTimer::reset() {
//     uint8_t oldSREG = SREG;
//     cli();
//     ticks_ = 0;
//     last_second_ticks_ = 0;
//     elapsed_seconds_ = 0;
//     minutes_ = 0;
//     seconds_ = 0;
//     SREG = oldSREG;
// }
