#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void wait(void) {
    uint8_t i;
    for (i = 0; i < 10; ++i)
        _delay_ms(25);
}

int main(void) {
    void wait(void);
    uint8_t c1, c2, i;

    DDRB = 0xFF;
    while (1) {
        c1 = 0x80;
        c2 = 0x01;
        for (i = 0; i < 4; i++) {
            wait();
            PORTB = ~(c1 | c2);
            c1 >>= 1;
            c2 <<= 1;
        }
    }
    return 0;
}

