#include "interrupts.h"

extern volatile bool button_up;
extern volatile bool button_down;
extern volatile bool button_left;
extern volatile bool button_right;
extern volatile bool button_cross;

void init_pins_and_interrupts() {
    // PD 1/3/4 (INT1 and PCINT) as an input
    DDRD &= ~((1 << PD1) | (1 << PD3) | (1 << PD4));
    PORTD |= (1 << PD1) | (1 << PD3) | (1 << PD4);

    cli(); // block interrupts  

    EICRA |= (1 << ISC10); // INT1 falling
    EIMSK |= (1 << INT1); // allow INT0, INT1    

    // PCINT for PD4
    PCMSK2 |= (1 << PCINT20); // PD4
    PCMSK2 |= (1 << PCINT17); // PD1
    PCICR  |= (1 << PCIE2);

    sei(); // globally allow interrupts
}

// INT1 interrupt handler
ISR(INT1_vect) {
    // button_cross = true;
    button_cross = !(PIND & (1 << PD3));
}

// PCINT interrupt handler
ISR(PCINT2_vect) {
    uint8_t current = PIND;  // читаем все пины порта D

    button_up = !(current & (1 << PD4));
    button_down = !(current & (1 << PD1));
}

