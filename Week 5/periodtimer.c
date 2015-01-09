#include <avr/io.h>
#include <stdint.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "lcd.h"

volatile uint8_t update_display = 0;

ISR(TIMER0_OVF_vect) {
	static uint16_t count = 0;

	if (count == 14457) {						/* This is the amount of overflows it takes before 1 second passes */
		count		= 0;
		update_display = 1;						/* Toggle display updating */
	}
	else {
		count++;
	}
}

int main(void) {
	uint16_t period_clk;
	unsigned char period_us[1] PROGMEM = {0};

	PORTD	= 0xFF; 							/* Activate pull-up */

	TCCR0	|= (1<<CS00);

	TCCR1A 	= 0;								/* Timer Mode = normal */;
	TCCR1B 	= 0x41;								/* Capture on rising edge, noise canceller disabled, no prescaler */
	ACSR   	|= (1<<ACD);						/* Turn off Analog Comparator to save power */

	lcd_init();									/* Initialise the LCD display */
	lcd_cursor(false, false);					/* Turn off cursor */

	TCNT1 	= 0;								/* Set TCNT1 zero */

	sei();

	while (1) {
		while ((TIFR & (1<<ICF1)) == 0);
		TCNT1 		= 0;								/* Making sure that TCNT1 begins at 0 */
		TIFR		= (1<<ICF1);						/* Clear ICF1 on first */

		while ((TIFR & (1<<ICF1)) == 0);
		period_clk 	= ICR1;
		TIFR		= (1<<ICF1);						/* Clear ICF1 on second */

		if (update_display == 1) {
			period_us[1] = round(period_clk * 0.271);	/* Calculating the actual period in microseconds */

		lcd_cls();
		lcd_puts("De periodetijd");
		lcd_goto(1, 0);
		lcd_puts("is: ");
		lcd_puts_P(period_us);							/* Print microseconds (not sure if this works) */
		lcd_puts(" us");

			update_display = 0;							/* Turn off display updating again */
		}
	}
	return 0;
}
