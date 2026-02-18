// // main.c - Не забудьте установить F_CPU в platformio.ini
// #include <avr/io.h>
// #include <util/delay.h>

// #include "interrupts.h"

// volatile bool to_display = false;

// int poll_and_burst(void) {
//     DDRD &= ~(1 << PD2);  // PD2 как вход
//     PORTD |= (1 << PD2);  // Включить подтяжку к VCC
//     DDRD |= (1 << PD5);   // PD5 как выход

//     while(1) {
//         if (to_display) {  // Кнопка нажата (0)
//             PORTD |= (1 << PORTD5);    // LED ON
//         } else {
//             PORTD &= ~(1 << PD5);     // LED OFF
//         }
//     }
// }

// int main() {
//     init_interrupts();

//     poll_and_burst();

//     return 0;
// }