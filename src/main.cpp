#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ssd1306.h"  // Лёгкая библиотека для OLED

volatile uint8_t button_flags = 0; // Флаги кнопок

// Прерывание на кнопку 1 (INT0)
ISR(INT0_vect) {
    button_flags |= (1 << 0);
}

// Прерывание на кнопку 2 (INT1)  
ISR(INT1_vect) {
    button_flags |= (1 << 1);
}

void setup_i2c(void) {
    TWBR = 72; // 100kHz при 8МГц
    TWSR = 0;
    TWCR = (1 << TWEN);
}

void setup_interrupts(void) {
    EICRA = (1 << ISC01) | (1 << ISC11); // FALLING edge
    EIMSK = (1 << INT0) | (1 << INT1);   // Включить INT0, INT1
    sei();
}

int main(void) {
    // Настройка
    DDRB = 0xFF;  // Светодиоды для отладки
    setup_i2c();
    setup_interrupts();
    
    // Инициализация OLED
    ssd1306_init();
    ssd1306_clear();
    ssd1306_draw_string(0, 0, "Starting...");
    ssd1306_update();
    
    uint8_t counter = 0;
    char buffer[16];
    
    while(1) {
        // Обработка кнопок в основном цикле
        if(button_flags & (1 << 0)) {
            button_flags &= ~(1 << 0);
            counter++;
            PORTB ^= (1 << PB0);
        }
        
        if(button_flags & (1 << 1)) {
            button_flags &= ~(1 << 1);
            counter--;
            PORTB ^= (1 << PB1);
        }
        
        // Обновление дисплея
        sprintf(buffer, "Count: %d", counter);
        ssd1306_clear();
        ssd1306_draw_string(0, 0, buffer);
        ssd1306_update();
        
        _delay_ms(10); // Anti-debounce в софте
    }
}