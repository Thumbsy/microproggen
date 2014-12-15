#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include "lcd.h"

int main(void) {
	char buffer[5];
	lcd_init();
	lcd_cursor(false, false);
	ADMUX	= 0;
	ADCSRA	= 1<<ADEN | 1<<ADPS2 | 1<<ADPS0;
	while (1) {
		ADCSRA |= 1<<ADSC | 1<<ADIF;
		while (~ADCSRA & 1<<ADIF);
		lcd_home();
		snprintf(buffer, sizeof buffer, "%4d", ADC);
		lcd_puts(buffer);
	
	}
	return 0;
}
