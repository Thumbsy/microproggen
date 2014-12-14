#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

ISR(TIMER0_OVF_vect) {
	PORTB	^= 0x20;					/* Invert PB5 (the LED) */
}

ISR(TIMER1_OVF_vect) {
	/*Insert code to sample temperature*/
}

int main(void) {
	TCCR0	|= (1<<CS11 | 1<<CS10);	/* Set to normal mode with prescaler 64 */
	TCNT0	= 0;
	OCR0	= 57599;
	TCCR1A	= 0x00;
	TCCR1B	|= (1<<CS11 | 1<<CS10);	/* Set to normal mode with prescaler 64 */
    TCNT1	= 0;
	DDRA	= 0x00;
    DDRB	= 0xE0;
    TIMSK	|= 1<<TOIE1;
    sei();
	while(1) {
		/* Insert code to write things to LCD and maybe do other stuff idk*/
	}
	return 0;
}
