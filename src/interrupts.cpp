#include "interrupts.h"

extern volatile bool to_display;
extern volatile bool display_needs_update;

void init_interrupts(void) {
    // PD2 (INT0) as an input
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    cli(); // block interrupts
    EICRA |= (1 << ISC01) | (1 << ISC00);  // RISING edge
    EIMSK |= (1 << INT0); // allow INT0    
    sei(); // globally allow interrupts
}

// INT0 interrupt handler
ISR(INT0_vect) {
    to_display = !to_display;
    display_needs_update = true;  // запросить обновление дисплея
}
