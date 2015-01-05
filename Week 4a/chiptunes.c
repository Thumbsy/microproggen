#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "noten_3686400.h"

ISR(TIMER0_OVF_vect) {
	TIMSK	|= 1<<OCIE1A; /* Enable OCR1A output compare interrupt (in case it was disabled) */
	uint8_t		index = 0;
	uint16_t	musicnotes[] = {
	/*	|	beat 1		|	beat 2		|	beat 3		|	beat 4 	|	*/
		A1,	R,	A1,	R,	A1,	R,	A1,	R,	A1,	R,	A1,	R,	A1,	R,	A1,	R, H
	}
	
	/* If no rest, repeat, stop or invalid index number is selected */
	if (musicnotes[index] != (R || H || S) || (index >= (sizeof(musicnotes) / sizeof(musicnotes[0]))) {
		OCR1A = musicnotes[index];			/* Load current note and select the next one */
		index++;
	}
	/* If there's a rest */
	else if (musicnotes[index] == R) {
		TIMSK	&= ~(1<<OCIE1A);			/* Temporarily disable OCR1A output compare interrupt */
		OC1A	= 1;						/* Force high on output to prevent clicks */
		index++;							/* Select next note */
	}
	/* If we reached the end of the tune and should repeat */
	else if (musicnotes[index] == H) {
		index = 0;							/* End of the tune, select the first note again */
		OCR1A = musicnotes[index];
		index++;
	}
	/* If we should stop */
	else {
		TIMSK	&= ~(1<<OCIE1A | 1<<TOIE0); /* Disable all interrupts, effectively making the program stop doing things */
	}
}

/* This interrupt generates the square wave for the sound */
ISR(TIMER1_COMPA_vect) {
	/* Nothing needs to be done, OC1A is automatically inverted */
}

int main(void) {
	DDRD	= 0x30; /* Set PD5 as output */
	
	/* Set up TIMER0. normal mode */
	/* 175 BPM desired, every note is a 16th long, so need to count to (60/175)/4 seconds */
	/* This is roughly equal to 315977 clk cycles with a CPU speed of 3.6864 MHz */
	/* We use an 8-bit variable to help keep track of the time, incrementing its value each time TIMER0 overflows */
	/* TIMER0 goes up to 255, so once the variable hits (315977/255)+1 = 1240, the desired time has passed */
	TCCR0	|= 1<<CS00;
	TCNT0	= 0;

	/* Set up TIMER1. CTC, toggle OC1A upon match, no prescaler */
	TCCR1A	|= 1<<COM1A0;
	TCCR1B	|= 1<<WGM12 | 1<<CS10;
	TCNT1	= 0;

	TIMSK	|= 1<<OCIE1A | 1<<TOIE0;	/* Enable OCR1A output compare interrupt and TIMER0 overflow interrupt */

	sei();								/* Enable interrupts */

	while (1) {
		/* Idle infinitely unless an interrupt occurs */
	}
	return 0;
}
