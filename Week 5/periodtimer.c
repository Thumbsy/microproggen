#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "lcd.h"


int main(void) {
	uint16_t period_clk, period_us ;

	PORTD = 0xFF; 								/*activate pull-up*/

	TCCR1A 	= 0;								/* Timer Mode = normal*/
	TCCR1B 	= 0x42;								/* Capture on rising edge, noise canceller disabled, prescaler 8 */
	ACSR   	= 0;

	lcd_init();									/* Initialise the LCD display */
	lcd_cursor(false, false);					/* Turn off cursor */

	TCNT1 	= 0;								/* Set TCNT1 zero */

	while (1){
		while ((TIFR& (1<<ICF1)) == 0);
		TCNT1 		= 0;						/* Making sure that TCNT1 begins at 0 */
		TIFR		= (1<<ICF1);				/* Clear ICF1 on first */

		while ((TIFR& (1<<ICF1)) == 0);
		period_clk 	= ICR1;
		TIFR		= (1<<ICF1);				/* Clear ICF1 on second */

		period_us	= period_clk * 2.17;	/* Niet zeker als je zo 2,17 kan gebruiken. Of alleen hele getallen? */	/* Calculating the actual period in microseconds */

		lcd_cls();
		lcd_puts("De periodetijd");
		lcd_goto(1, 0);
		lcd_puts("is: %d us", period_us); /* Andrew is niet zeker als het zo gedaan moet worden. Is het een %d of een %10d? Of iets anders? */
		
		while(1){
			/* Stay in here, FOREVER! */
		}
	}
	return 0;
}
