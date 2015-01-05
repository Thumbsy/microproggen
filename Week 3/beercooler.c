/*
	At 25°C, the NTC has a resistance of 20k Ohm, which is where the ratio R/R25 is 1.000
	At 5°C, the NTC has a resistance of 50792 Ohm, which is where the ratio R/R25 is 2.5396
	At 9°C, the NTC has a resistance of 41772 Ohm, which is where the ratio R/R25 is 2.0886

	The thresholds for the low and high temperature threshold respectively: ADC>532 and ADC<482
*/
	
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "lcd.h"

volatile uint8_t update_display = 0;


ISR(TIMER1_OVF_vect) {
	static uint8_t seconds = 0;
	PORTB	^= 0x20;							/* Invert PB5 (the LED) using an XOR */
	seconds++;
	if (seconds == 01) {						/* One minute's passed, measure beer temperature */
		ADCSRA |= 1<<ADSC | 1<<ADIF;			/* Start conversion and reset interrupt flag */
		seconds = 0;							/* Reset the seconds-timer so we can count the next minute */
		update_display = 1;						/* Activate display writing */
	}
}

int main(void) {	
	DDRA	= 0x00;
    DDRB	= 0xE0;								/* Set PB7, PB6 and PB5 as outputs */

	TCCR1A	= 0x00;
	TCCR1B	|= (1<<CS11 | 1<<CS10);				/* Set to normal mode with prescaler 64 */
	TCNT1	= 0;
	OCR1A	= 57599;							/* With a prescaler of 64, it takes 3686400/64 = 57600 clk periods for one second */
    TIMSK	|= 1<<TOIE1;						/* Enable the overflow interrupt on timer1 */

	lcd_init();									/* Initialise the LCD display */
	lcd_cursor(false, false);					/* Turn off cursor */

	ADMUX	= 0;								/* Use AREF as reference, use ADC0 as input */
	ADCSRA	= 1<<ADEN | 1<<ADPS2 | 1<<ADPS0;	/* Enable the ADC, set ADC prescaler to 32 */

	sei();										/* Enable interrupts */
	
	while(1) {		
		if (update_display == 1) {
			while (~ADCSRA & 1<<ADIF);			/* Wait for conversion to finish */

			if (ADC < 482) {					/* If ADC is lower than 482, cool the beer */
				cli();
				PORTB	|= (1<<7);				/* Set PB6 */
				PORTB	&= ~(1<<6);				/* Clear PB7 */
				sei();
				lcd_cls();
				lcd_puts("Het koelelement");
				lcd_goto(1, 0);
				lcd_puts("is aan.");
			}
			else if (ADC > 532) {				/* If ADC is higher than 532, heat the beer */
				cli();
				PORTB	&= ~(1<<7);				/* Clear PB6 */
				PORTB	|= (1<<6);				/* Set PB7 */
				sei();
				lcd_cls();
				lcd_puts("Het verwarmings-");
				lcd_goto(1, 0);
				lcd_puts("element is aan.");
			}
			else {								/* Just about right! */
				cli();
				PORTB	|= (1<<7) | (1<<6);		/* Clear both PB6 and PB7 */
				sei();
				lcd_cls();
				lcd_puts("De temperatuur");
				lcd_goto(1, 0);
				lcd_puts("is perfect!");
			}
		
			update_display = 0;					/* Disable display writing (untill the next interrupt) */
		}
	}
	return 0;
}
