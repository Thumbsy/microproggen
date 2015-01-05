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
	/* Nothing needs to be done,	 OC1A is automatically inverted */
}

int main(void) {
	uint8_t		index = 0;
	uint16_t	musicnotes[] = {
	/*					|			beat 1				|			beat 2				|			beat 3				|			beat 4 		|	*/
	/* Measure 01 */	G0,		G0,		G1,		G1,		R,		R,		G1,		G1,		R,		R,		G1,		G1,		R,		R,		G1,		G1,
	/* Measure 02 */	R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		G1,		G1,
	/* Measure 03 */	B0,		B0,		C1,		C1,		R,		R,		C1,		C1,		R,		R,		C1,		C1,		R,		R,		C1,		C1,
	/* Measure 04 */	R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		D1,		D1,		R,		R,		D1,		D1,
	/* Measure 05 */	B0,		B0,		E1,		E1,		R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		D1,		D1,
	/* Measure 06 */	B0,		B0,		E1,		E1,		R,		R,		R,		R,		R,		R,		R,		R,		R,		R,		D1,		D1,
	/* Measure 07 */	B0,		B0,		C1,		C1,		R,		R,		C1,		C1,		B0,		B0,		C1,		C1,		R,		R,		C1,		C1,
	/* Measure 08 */	B0,		B0,		C1,		C1,		B0,		B0,		C1,		C1,		D1,		D1,		E1,		E1,		FIS1,	FIS1,	G1,		G1,
	/* Measure 09 */	G0,		G0,		D1,		D1,		G1,		G1,		G0,		G0,		G1,		G1,		D1,		D1,		B0,		B0,		G1,		G1,
	/* Measure 10 */	G0,		G0,		D1,		D1,		G1,		G1,		G0,		G0,		G1,		G1,		D1,		D1,		B0,		B0,		G1,		G1,
	/* Measure 11 */	C1,		C1,		G1,		G1,		C2,		C2,		B0,		B0,		C1,		C1,		C2,		C2,		E1,		E1,		D1,		D1,
	/* Measure 12 */	C1,		C1,		G1,		G1,		C2,		C2,		B0,		B0,		C1,		C1,		C2,		C2,		E1,		E1,		D1,		D1,
	/* Measure 13 */	E2,		E2,		B1,		B1,		E1,		E1,		E2,		E2,		B1,		B1,		E1,		E1,		E2,		E2,		D2,		D2,
	/* Measure 14 */	E2,		E2,		B1,		B1,		E1,		E1,		E2,		E2,		B1,		B1,		E1,		E1,		FIS2,	FIS2,	D2,		D2,
	/* Measure 15 */	G2,		G2,		D1,		D1,		C1,		C1,		G2,		G2,		D2,		D2,		C1,		C1,		G2,		G2,		FIS2,	FIS2,
	/* Measure 16 */	G2,		G2,		D1,		D1,		C1,		C1,		G2,		G2,		D2,		D2,		C1,		C1,		G2,		G2,		A2,		A2,
	/* Measure 17 */	G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0,		G0, S
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
			/* If no rest,	 repeat,	 stop or invalid index number is selected */
			if (musicnotes[index] > 3 || (index < (sizeof(musicnotes) / sizeof(musicnotes[0])))) {
				OCR1A = musicnotes[index];			/* Load current note and select the next one */
				TIMSK	|= 1<<OCIE1A;				/* Enable OCR1A output compare interrupt (in case it was disabled) */
				index++;
			}
			/* If there's a rest */
			else if (musicnotes[index] == 2) {
				TIMSK	&= ~(1<<OCIE1A);			/* Temporarily disable OCR1A output compare interrupt (not working?) */
				PORTD	= 0x20;						/* Force high on output to prevent clicks */
				DDRD	= 0x00;						/* Dirty hack to disable speaker */
				index++;							/* Select next note */
			}
			/* If we reached the end of the tune and should repeat */
			else if (musicnotes[index] == 1) {
				index = 0;							/* End of the tune,	 select the first note again */
				OCR1A = musicnotes[index];
				index++;
			}
			/* If we should stop */
			else {
				TIMSK	&= ~(1<<OCIE1A | 1<<TOIE0); /* Disable all interrupts,	 effectively making the program stop doing things */
			}

			playnote = 0;
		}
	}
	return 0;
}
