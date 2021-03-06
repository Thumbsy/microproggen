#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

ISR(TIMER1_OVF_vect) {
    TCNT1	= 64636;
	TIFR	= 1<<TOV1;
	
	/* Check for button input, then go through pattern code */  
	static uint8_t c1, c2, i = 3, mode = 0x00, toggle = 0;
	if (PINA == 0xFF) {
		toggle = 0;		/* Toggle is switched on when button is pressed and reset when let go off */
	}

	if (PINA == 0x7F && toggle == 0) {
		mode = ~mode;	/* When button is pressed, switch pattern mode */
		toggle = 1;		/* Turn toggle on so the mode-switching is not repeated when button is held */
		if (mode == 0xFF) {
			i = 9;
		}
		else {
			i = 3;
		}
	}
	/* Pattern 1 */
	if (mode == 0x00) {
		if (i != 3) {
			c1 >>= 2;
			PORTB = ~(c1);
			i++;
		}
		else {
			c1		= 0xC0;
			PORTB	= ~(c1);
			i = 0;
		}
	}
	/* Pattern 2 */
	if (mode == 0xFF) {
        if (i < 3) {
            c1 >>= 1;
            PORTB = ~(c1 | c2);
			i++;
		}
        else if (i == 4) {
            c1 <<= 1;
            PORTB = ~(c1 | c2);
			i++;
		}
        else if (i < 9) {
            c1 >>= 1;
            PORTB = ~(c1 | c2);
			i++;
		}
		else {
	        c1		= 0xA0;
	        c2		= 0x00;
			PORTB	= ~(c1 | c2);
			i = 0;
        }
	}
}

int main(void) {
	TCCR1A	= 0x00;
	TCCR1B	|= (1<<CS12 | 1<<CS10);	/* Set to normal mode with prescaler 1024 */
    TCNT1	= 64636;
	DDRA	= 0x00;
    DDRB	= 0xFF;
    TIMSK	|= 1<<TOIE1;
    sei();
    while (1) {
        /* Wait for overflow interrupt to kick in and execute the ISR */
    }
    return 0;
}
