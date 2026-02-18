#include "interrupts.h"

void init_interrupts(void) {
    // PD2 (INT0) как вход с подтяжкой
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    
    // Настройка режима срабатывания
    EICRA |= (1 << ISC01) | (1 << ISC00);  // RISING edge
    // EICRA |= (1 << ISC01);                // FALLING edge
    // EICRA |= (1 << ISC01) | (1 << ISC00); // RISING
    // EICRA |= (1 << ISC01);                // FALLING
    // EICRA |= (1 << ISC00);                // CHANGE
    // EICRA &= ~((1 << ISC01) | (1 << ISC00)); // LOW level
    
    // Разрешить прерывание INT0
    EIMSK |= (1 << INT0);
    
    // Глобально разрешить прерывания
    sei();
}

// Обработчик прерывания INT0
ISR(INT0_vect) {
    extern bool to_display;

    to_display = !to_display;
}