#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
#include "lcd.h"

volatile uint8_t status = 0;					/* 0 = perfect, 1 = cool, 2 = heat */

ISR(TIMER1_OVF_vect) {
	static uint8_t seconds = 0;
	PORTB	^= 0x20;							/* Invert PB5 (the LED) using an XOR */
	seconds++;
	if (seconds == 60) {
		/* Insert code to measure temperature and set indicators if needed */
		seconds = 0;
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
		/* Print the right text on the LCD display */
		if (status == 0) {
			lcd_cls();
			lcd_puts("De temperatuur");
			lcd_goto(1, 0);
			lcd_puts("is perfect!");
		}
		else if (status == 1) {
			lcd_cls();
			lcd_puts("Het koelelement");
			lcd_goto(1, 0);
			lcd_puts("is aan.");
			}
		else if (status == 2) {
			lcd_cls();
			lcd_puts("Het verwarmings-");
			lcd_goto(1, 0);
			lcd_puts("element is aan.");
			}
		else {
			lcd_cls();
			lcd_puts("uwotm8");
		}
	}
	return 0;
}
