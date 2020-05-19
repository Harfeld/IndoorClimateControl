/*
 * ADC.c
 *
 * Created: 18-05-2020 18:26:37
 *  Author: Bruger
 */ 

#include "ADC.h"
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

void Init_ADC()
{
	// PF pins are inputs (ADC7-ADC0 inputs)
	DDRK = 0;
	// Internal 5 volt reference, ADLAR = 0, Input = ADC0 single ended (potentiometer)
	ADMUX = 0b01000000;
	ADMUX |= 0b00100111;
	// ADC enable
	// ADC interrupt disabled
	// ADC prescaler = 128 (=> ADC clock = 16 MHz / 128 = 125 kHZ)
	ADCSRA = 0b10000111;
}

int AnalogRead(){
	// Start new A/D conversion
	ADCSRA |= 0b01000000;
	// Wait for A/D conversion ended
	while (ADCSRA & 0b01000000)
	{}
	// Send ADC result to terminal
	float adcValue = ADCW;
	return adcValue;
}
