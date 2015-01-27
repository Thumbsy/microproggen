#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

volatile bool measure; /* If measure == true, we read ADC, convert the data and send it to the PC */

ISR(USART_RXC_vect) {
	PORTB ^= 0xFF;
	measure = true;
	ADCSRA |= 1<<ADSC | 1<<ADIF;                /* Start conversion and reset interrupt flag */
}

int main(void) {
	char volts_buffer[10];
	char start;
	float ADC_val;
	int ADC_intpart;
	int ADC_decpart;
	
	DDRB = 0xFF;

	ADMUX	= 0;                                /* Use AREF as reference, use ADC0 as input */
	ADCSRA	= 1<<ADEN | 1<<ADPS2 | 1<<ADPS0;    /* Enable the ADC, set ADC prescaler to 32 */

	/* 9600-8-E-1-23, enable receive interrupt */
	UCSRA = 0;
	UCSRB |= (1<<RXCIE) | (1<<RXEN) | (1<<TXEN);
	UCSRC |= (1<<URSEL) | (1<<UPM1) | (1<<UCSZ1);
	UBRRH = 0;
	UBRRL = 23;

	sei();
	while (1) {
		if (measure == true) {
			while (~ADCSRA & 1<<ADIF);                  /* Wait for conversion to finish */
			/* Save volts into string with a precision of two digits after the decimal point */
			ADC_val = 5.0 * (ADC + 0.5) / 1024;
			ADC_intpart = (int)ADC_val;
			ADC_decpart = (int)(100 * (ADC_val - ADC_intpart));
			snprintf(volts_buffer, sizeof volts_buffer, "%d.%02dV", ADC_intpart, ADC_decpart);
			strcat(volts_buffer,"Q");                   /* Stick 'Q' behind the value to denote end of string */

			if (UCSRA & (1<<FE | 1<<DOR | 1<<PE)) {     /* If errors are present, send X back to PC */
				start = UDR;
				while (~UCSRA & 1<<UDRE);
				UDR = 'X';
			}
			else {                                      /* Otherwise, send volts back to PC */
				start = UDR;
				if (start == 'S') {
					int i;
					for (i = 0; volts_buffer[i] != '\0'; i++) {
                    	while (~UCSRA & 1<<UDRE);
                    	UDR = volts_buffer[i];
                	}
            	}
			}
			measure = false;
		}
	}
	return 0;
}
