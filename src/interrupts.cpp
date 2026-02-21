#include "interrupts.h"

// extern volatile bool to_display;
// extern volatile bool display_needs_update;

extern volatile bool button_up;
extern volatile bool button_down;
extern volatile bool button_cross;


void init_pins_and_interrupts() {
    DDRD |= (1 << PD5); // PD5 as output

    // PD2/3/4 (INT0/1 and PCINT) as an input
    DDRD &= ~((1 << PD2) | (1 << PD3));
    PORTD |= (1 << PD2) | (1 << PD3);

    // PCINT for PD4
    DDRB &= ~(1 << PD4);
    PORTB |= (1 << PD4);

    cli(); // block interrupts

    EICRA |= (1 << ISC01) | (1 << ISC00);  // INT0 rising
    EICRA |= (1 << ISC11) | (1 << ISC10);  // INT1 rising
    EIMSK |= (1 << INT0) | (1 << INT1); // allow INT0/1    

    // PCINT for PD4
    PCMSK0 |= (1 << PCINT20);
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
    button_up = true;
}

// INT1 interrupt handler
ISR(INT1_vect) {
    button_down = true;
}

// PCINT interrupt handler
ISR(PCINT2_vect) {
    if (!(PIND & (1 << PD4))) {  // если PD4 стал низким (кнопка нажата на GND)
        button_cross = true;
    }
}

