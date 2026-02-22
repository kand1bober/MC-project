#include "interrupts.h"

// extern volatile bool to_display;
// extern volatile bool display_needs_update;

extern volatile bool button_up;
extern volatile bool button_down;
extern volatile bool button_left;
extern volatile bool button_right;
extern volatile bool button_cross;


void init_pins_and_interrupts() {
    DDRD |= (1 << PD5); // PD5 as output

    // PD2/3/4 (INT0/1 and PCINT) as an input
    DDRD &= ~((1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4));
    PORTD |= (1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4);

    cli(); // block interrupts

    EICRA |= (1 << ISC01) | (1 << ISC00);  // INT0 rising
    EICRA |= (1 << ISC11) | (1 << ISC10);  // INT1 rising
    EIMSK |= (1 << INT0) | (1 << INT1); // allow INT0/1    

    // PCINT for PD4
    PCMSK2 |= (1 << PCINT20); // PD4
    PCMSK2 |= (1 << PCINT17); // PD1
    PCICR  |= (1 << PCIE2);

    sei(); // globally allow interrupts
}

// // INT0 interrupt handler
// ISR(INT0_vect) {
//     to_display = !to_display;
//     display_needs_update = true;  // запросить обновление дисплея    
// }

// INT0 interrupt handler
ISR(INT0_vect) {
    // button_left = true;
    button_left = !button_left;
}

// INT1 interrupt handler
ISR(INT1_vect) {
    // button_right = true;
    button_right = !button_right;
}

// PCINT interrupt handler
ISR(PCINT2_vect) {
    uint8_t current = PIND;  // читаем все пины порта D

    // check PD4
    if (!(current & (1 << PD4))) {
        // button_up = true;
        button_up = !button_up;
    }
    // check PD1
    if (!(current & (1 << PD1))) { 
        // button_down = true;
        button_down = !button_down;
    }
}

