#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "lcd.h"

volatile bool update_display = false;

ISR(TIMER0_OVF_vect) {
	static uint16_t count = 0;

	if (count == 14457) {						/* This is the amount of overflows it takes before 1 second passes */
		count		= 0;
		update_display = true;					/* Toggle display updating */
	}
	else {
		count++;
	}
}

int main(void) {
	uint16_t period_clk, period_us;
	char buffer[13];

	PORTD	= 0xFF; 				/* Activate pull-up */

	TCCR0	|= (1<<CS00);

	TCCR1A 	= 0;					/* Timer Mode = normal */;
	TCCR1B 	= 0x41;					/* Capture on rising edge, noise canceller disabled, no prescaler */
	ACSR   	|= (1<<ACD);			/* Turn off Analog Comparator to save power */

	lcd_init();						/* Initialise the LCD display */
	lcd_cursor(false, false);		/* Turn off cursor */

	TIMSK	|= (1<<TOV0);			/* Enable TIMER0 overflow interrupt */

	sei();

	while (1) {
		while ((TIFR & (1<<ICF1)) == 0);			/* Wait for next clk edge */
		TCNT1 		= 0;							/* Making sure that TCNT1 begins at 0 */
		TIFR		= (1<<ICF1);					/* Clear ICF1 flag */

		while ((TIFR & (1<<ICF1)) == 0);			/* Wait for next clk edge */
		period_clk 	= ICR1;							/* Save the amount of time it took between the previous and this clk edge */
		TIFR		= (1<<ICF1);					/* Clear ICF1 flag again */

		if (update_display == true) {
			period_us = round(period_clk * (1 / 3.686400));				/* Calculating the last measured period in microseconds */
			snprintf(buffer, sizeof buffer, "is: %5d us", period_us);	/* Create string (called buffer) that will soon be printed on the second line of the LCD display */

			lcd_cls();
			lcd_puts("De periodetijd");
			lcd_goto(1, 0);
			lcd_puts(buffer);											/* Print second line with microseconds */

			update_display = false;										/* Turn off display updating again */
		}
	}
	return 0;
}
