/*
 * GccApplication28.c
 *
 * Created: 5/8/2024 3:36:49 PM
 * Author : Glenn
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void ADC_init() {
	ADMUX |= (0 << REFS1) | (1 << REFS0); // Use AVCC as reference voltage
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADPS2); // Enable ADC, enable auto-triggering, set prescaler to 16
	ADCSRB = 0x00;
	DIDR0 |= (1 << ADC0D); // Disable digital input on ADC0 pin
}

uint16_t ADC_read() {
	ADCSRA |= (1 << ADSC); // Start ADC conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	return ADC; // Return ADC value
}

int main(void)
{
	DDRB |= (1 << PB1); // Set PB1 as output for blinking LED
	DDRB |= (1 << PB3); // Set PB3 as output for PWM LED

	TCCR1A |= (1 << COM1A1) | (1 << WGM10); // Fast PWM, non-inverting mode
	TCCR1B |= (1 << CS11); // Prescaler of 8
	OCR1A = 0; // Set initial PWM duty cycle to 0
	
	ADC_init(); // Initialize ADC
	
	// Enable interrupts for Timer1 overflow
	TIMSK1 |= (1 << TOIE1);
	sei(); // Enable global interrupts

	while (1)
	{
		PORTB ^= (1 << PORTB1); // Toggle PB1
		_delay_ms(1000); // Adjust the delay for blinking speed
	}
	return 0;
}

ISR(TIMER1_OVF_vect) {
	// Adjust brightness based on potentiometer
	uint16_t potValue = ADC_read(); // Read ADC value from potentiometer
	OCR1A = potValue >> 2; // Adjust PWM duty cycle (potValue ranges from 0 to 1023, but OCR1A ranges from 0 to 255)
}

