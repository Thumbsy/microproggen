#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void wait(void) {
	TIFR	= 0b00000100;
	TCNT1H	= 0x0E;
	TCNT1L	= 0x10;
	while (TIFR != 0b00000100) {

	}
}

int main(void) {
	void wait(void);
	uint8_t c1, c2, i, mode;

	TCCR1A	= 0x00;
	TCCR1B	|= (1<<CS12 | 1<<CS10);
	TIMSK	|= (1<<TOIE1);

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

