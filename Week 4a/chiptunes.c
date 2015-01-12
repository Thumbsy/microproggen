#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "noten_3686400.h"

volatile uint8_t playnote = 1;

ISR(TIMER0_OVF_vect) {
	static uint16_t count = 0;

	if (count == 1240) {
		playnote	= 1;
		count		= 0;
	}
	else {
		count++;
	}
}

/* This interrupt generates the square wave for the sound */
ISR(TIMER1_COMPA_vect) {
	/* Nothing needs to be done, OC1A is automatically inverted */
}

int main(void) {
	volatile uint16_t	index = 0;
	uint16_t	musicnotes[] = {
	/*					|			beat 1				|			beat 2				|			beat 3				|			beat 4 		|	*/
	/* Measure 01 */	C2,		C2,		F0,		F0,		C2,		C2,		F0,		C2,		F0,		C2,		C2,		C2,		B1,		B1,		G1,		G1,
	/* Measure 02 */	F0,		F0,		C2,		F0,		C2,		F0,		C2,		B1,		F0,		F0,		C2,		F0,		G2,		F0,		B1,		D2,
	/* Measure 03 */	C2,		C2,		A0,		A0,		C2,		C2,		A0,		C2,		A0,		C2,		C2,		C2,		B1,		B1,		G1,		G1,
	/* Measure 04 */	A0,		A0,		D2,		A0,		D2,		A0,		D2,		E2,		E2,		A0,		E2,		A0,		E2,		G2,		B1,		C2,
	/* Measure 05 */	C1,		C1,		C2,		C1,		C2,		C2,		B1,		C2,		C2,		C1,		C2,		C2,		D2,		D2,		E2,		E2,
	/* Measure 06 */	C1,		C1,		G1,		C1,		G1,		C1,		G1,		G2,		G2,		C1,		G2,		G2,		F2,		F2,		E2,		E2,
	/* Measure 07 */	D2,		D2,		G0,		G0,		D2,		D2,		G0,		D2,		G0,		C2,		C2,		G0,		B1,		B1,		C2,		C2,
	/* Measure 08 */	G0,		G0,		G2,		G2,		C2,		C2,		B1,		C2,		C2,		B1,		A2,		A2,		G2,		G0,		C2,		G0, H
	};
	
	DDRD	= 0x20; /* Set PD5 as output */
	
	/* Set up TIMER0. normal mode */
	/* 175 BPM desired,	 every note is a 16th long,	 so need to count to (60/175)/4 seconds */
	/* This is roughly equal to 315977 clk cycles with a CPU speed of 3.6864 MHz */
	/* We use an 8-bit variable to help keep track of the time,	 incrementing its value each time TIMER0 overflows */
	/* TIMER0 goes up to 255,	 so once the variable hits (315977/255)+1 = 1240,	 the desired time has passed */
	TCCR0	|= 1<<CS00;
	TCNT0	= 0;

	/* Set up TIMER1. CTC,	 toggle OC1A upon match,	 no prescaler */
	TCCR1A	|= 1<<COM1A0;
	TCCR1B	|= 1<<WGM12 | 1<<CS10;
	TCNT1	= 0;
	OCR1A = musicnotes[index];

	TIMSK	|= 1<<OCIE1A | 1<<TOIE0;	/* Enable OCR1A output compare interrupt and TIMER0 overflow interrupt */

	sei();								/* Enable interrupts */

	while (1) {
		if (playnote == 1) {
			
			DDRD	= 0x20;							/* Dirty hack to re-enable speaker */
			/* If no rest, repeat, stop or invalid index number is selected */
			if (musicnotes[index] > 3) {
				OCR1A = musicnotes[index];			/* Load current note and select the next one */
				index++;
			}
			/* If there's a rest */
			else if (musicnotes[index] == 2) {
				PORTD	= 0x20;						/* Force high on output to prevent clicks */
				DDRD	= 0x00;						/* Dirty hack to disable speaker */
				index++;							/* Select next note */
			}
			/* If we reached the end of the tune and should repeat */
			else if (musicnotes[index] == 1) {
				index = 0;							/* End of the tune, select the first note again */
				OCR1A = musicnotes[index];
				index++;
			}
			/* If we should stop */
			else {
				cli();
				DDRD	= 0x00;						/* Dirty hack to disable speaker */
			}

			playnote = 0;
		}
	}
	return 0;
}
