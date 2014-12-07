#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

ISR(TIMER1_OVF_vect) {
    TCNT1 = 900;
	
	/* bepaal volgende patroon */  
	uint8_t c1, c2, i, mode;
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
				    /*vervangt wait()*/
					TIFR	= 1<<TOV1;
	   				while (!(TIFR & 1<<TOV1));
					/* einde vervangt wait()*/
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
				    /*vervangt wait()*/
					TIFR	= 1<<TOV1;
	   				while (!(TIFR & 1<<TOV1));
					/* einde vervangt wait()*/
		            PORTB = ~(c1 | c2);
		            c1 >>= 1;
				}
				else {
					break;
				}
	        }
	        for (i = 0; i < 1; i++) {
				if (PINA != 0x7F) {
				    /*vervangt wait()*/
					TIFR	= 1<<TOV1;
	   				while (!(TIFR & 1<<TOV1));
					/* einde vervangt wait()*/
		            PORTB = ~(c1 | c2);
		            c1 <<= 1;
				}
				else {
					break;
				}
	        }
	        for (i = 0; i < 5; i++) {
				if (PINA != 0x7F) {
				    /*vervangt wait()*/
					TIFR	= 1<<TOV1;
	   				while (!(TIFR & 1<<TOV1));
					/* einde vervangt wait()*/
		            PORTB = ~(c1 | c2);
		            c1 >>= 1;
				}
				else {
					break;
				}
	        }
		}
	}  
	/* einde bepaal volgende patroon */  



    PORTB = ...;
}

int main(void) {
	TCCR1A	&= ~(1<<WGM11 | 1<<WGM10);          /* normal mode */ 
	TCCR1B  &= ~(1<<WGM13 | 1<<WGM12 |1<<CS11); /* normal mode & prescaler 2014*/ 
	TCCR1B	|=   1<<CS12  | 1<<CS10;            /* prescaler 1024 */ 
    TCNT1 = 900;
    DDRB = 0xFF;
    TIMSK |= 1<<TOEI1;
    sei();
    while (1) {
        /* hier kun je iets anders doen */
    }
    return 0;
}
