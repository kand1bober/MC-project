// main.c - Не забудьте установить F_CPU в platformio.ini
#include <avr/io.h>
#include <util/delay.h>

int burst(const int num) {
    for (int i = 0; i < num; i++) {
        // 2. Включить светодиод (установить HIGH)
        PORTD |= (1 << PORTD5); // Установить 5-й бит PORTD в 1
        _delay_ms(5);        // Ждать 1000 мс

        // 3. Выключить светодиод (установить LOW)
        PORTD &= ~(1 << PORTD5); // Установить 5-й бит PORTD в 0
        _delay_ms(5);         // Ждать 1000 мс
    }

    return 0;
}

int poll_and_burst(void) {
    DDRD &= ~(1 << PD2);  // PD2 как вход
    PORTD |= (1 << PD2);  // Включить подтяжку к VCC
    DDRD |= (1 << PD5);   // PD5 как выход

    while(1) {
        if (!(PIND & (1 << PD2))) {  // Кнопка нажата (0)
            burst(6);               // LED ON
        } else {
            PORTD &= ~(1 << PD5);     // LED OFF
        }
    }

    // burst(5);
}