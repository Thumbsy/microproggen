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
	uint8_t c1, c2, i, mode;

	DDRA = 0x00;
	DDRB = 0xFF;
	mode = 0x00;
    while (1) {
		if (PINA == 0x7F) {
			mode = ~mode; /*When button is pressed, switch pattern mode*/
			while (PINA != 0xFF) {
				/*Wait untill the button is released again*/
			}
		}
		/*Pattern 1*/
		if (mode == 0x00) {
			c1 = 0xC0;
			for (i = 0; i < 4; i++) {
				if (PINA != 0x7F) {
					wait();
					PORTB = ~(c1);
					c1 >>= 2;
				}
				else {
					break;
				}
			}
		}
		/*Pattern 2*/
		if (mode == 0xFF) {
	        c1 = 0xA0;
	        c2 = 0x00;
	        for (i = 0; i < 4; i++) {
				if (PINA != 0x7F) {
		            wait();
		            PORTB = ~(c1 | c2);
		            c1 >>= 1;
				}
				else {
					break;
				}
	        }
	        for (i = 0; i < 1; i++) {
				if (PINA != 0x7F) {
		            wait();
		            PORTB = ~(c1 | c2);
		            c1 <<= 1;
				}
				else {
					break;
				}
	        }
	        for (i = 0; i < 5; i++) {
				if (PINA != 0x7F) {
		            wait();
		            PORTB = ~(c1 | c2);
		            c1 >>= 1;
				}
				else {
					break;
				}
	        }
		}
	}
	return 0;
}

