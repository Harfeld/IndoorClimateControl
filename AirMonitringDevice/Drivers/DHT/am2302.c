/*
Inspiration: 
DHT library by Adafruit - https://github.com/adafruit/DHT-sensor-library
Datasheet - https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf
**************************************************
* "AM2302.C"                                     *
* Implementation for "AM2302 Temperature driver" *
* Author: Andreas Harfeld Jakobsen				 *
*         Jonas Hingebjerg Hansen				 *
* Date:   29.5.2020								 *
**************************************************/
#define F_CPU 16000000
#define MAX_WAIT F_CPU/1000

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include "../UART/uart.h"
#include "am2302.h"

bool debugFlag;
uint8_t* latestRead;

void initTimer(){
	// Indstiller Timer 1 til overflow hvert andet sekund.
	TCCR1A = 0;			 //Normal port operation
	TCCR1B = 0b00001000; //Timer 1 - CTC mode
	TCCR1B |= 0b101;	 //Bruger 1024 prescaler
	OCR1A = 31250-1;	 //compare match på 31249
	TCNT1 = OCR1A-1;     //giver overflow med det samme (for at isBusy() virker ordentligt)
}

void resetTimer() {
	TIFR1 &= (0b111<<OCF1A);	//resetting overflow flags
	TCNT1 = 0;					//Restarting count
}

bool isBusy() {
	if (debugFlag)
	{
		SendString("isBusy == ");SendInteger((TIFR1 == 0));SendString("\n\r");
	}
	return TIFR1 == 0;
}

void initAM2302(bool debugEnabled) {
	debugFlag = debugEnabled;
	DDRL &= ~(1<<DDL3);
	PORTL |= (1<<DDL3);
	initTimer();
}

void enableDebugging() {
	debugFlag = true;
}

void disableDebugging() {
	debugFlag = false;
}

void sendStartSignal(){
	if (debugFlag)
	{
		SendString("sending start signal\n\r");
	}
	DDRL &= ~(1<<DDL3);
	PORTL |= (1<<DDL3);
	_delay_ms(1000);
	DDRL |= (1<<DDL3);
	PORTL &= ~(1<<DDL3);
	_delay_us(1100);
	DDRL &= ~(1<<DDL3);
	PORTL |= (1<<DDL3);
	_delay_us(40);
}

bool waitForSensorConfirmation(){
	// Før selve datasignalet trækker sensoren signalet lavt(80us) og der efter højt(80us)
	uint32_t count = 0;
	while ((PINL & (1<<PINL3)) == 0)
	{
		if (count++ >= MAX_WAIT)
		{
			SendString("Timeout - waiting for sensor initial low pulse\n\r");
			return false;
		}
		
	}
	count = 0;
	while ((PINL & (1<<PINL3)) != 0)
	{
		if (count++ >= MAX_WAIT)
		{
			SendString("Timeout - waiting for sensor initial high pulse\n\r");
			return false;
		}
	}
	return true;
}

uint32_t* receiveData() {
	static uint32_t data[80];
	uint32_t count = 0;
	//Der kommer 40 pulse, først en lav på ca. 50us og så en høj som viser
	//om det er et 1 eller 0 ved hjælp af længden (1 = ca.70us , 0 = ca.26-28us), der er altså 80 pladser
	//i data for at rumme både de lave signaler men også de høje data signaler.
	for (int i = 0; i<80; i+=2)
	{
		while ((PINL & (1<<PINL3)) == 0)
		{
			if (count++ >= MAX_WAIT)
			{
				data[i] =  -1;
				SendString("Read failure - timeout waiting for pulse\n\r");
			}
			
		}
		data[i] = count;
		count = 0;
		while ((PINL & (1<<PINL3)) != 0)
		{
			if (count++ >= MAX_WAIT)
			{
				data[i+1] =  -1;
				SendString("Read failure - timeout waiting for pulse\n\r");
			}
		}
		data[i+1] = count;
		count = 0;
	}
	return data;
}

uint8_t* rawToBinary(uint32_t* signal){
	static uint8_t data[5];
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
	//translate from timed signal to data
	for (int i = 0; i < 40; ++i){
		uint32_t highTime = signal[i*2+1];
		uint32_t lowTime = signal[(i*2)];
		data[i/8] <<= 1;
		if (highTime >= lowTime)
		{
			data[i/8] |= 1;
		}
	}
	if (debugFlag) {
		for (int i = 0; i < 5; i++) {
			SendInteger(data[i]);
			SendString("  ");
		}
		SendString("\n\r");
	}
	return data;
}

uint8_t* readSensor() {
	if (isBusy() == false) {
		resetTimer();
		sendStartSignal();
		waitForSensorConfirmation();
		uint32_t* rawSignal = receiveData();
		
		if (debugFlag)
		{
			for (int j = 0; j < 80; j+=1){
				SendInteger(rawSignal[j]);
				SendString("  ");
			}
			SendString("\n\r");
		}
		
		uint8_t* data = rawToBinary(rawSignal);
		if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xff)){
			if (debugFlag) {
				SendString("Checksum success\n\r");
			}
			latestRead = data;
			return data;
		} 
		else {
			if (debugFlag) {
				SendString("Checksum Failure\n\r");
			}
			return latestRead;
		}
	} else {
		if (debugFlag) {
			SendString("Read too soon - Returning previous data\n\r");
		}
		return latestRead;
	}

}

float readHumidity(){
	uint8_t* data = readSensor();
	float humidity = ((data[0] << 8) | data[1]) * 0.1;
	return humidity;
}
	
float readTemperature(){
	uint8_t* data = readSensor();
	float temperature = ((data[2] & 0x7F) << 8 | data[3]) * 0.1;
	if (data[2] >= 128){	//Hvis det forreste bit af de 8 temperatur bits er 1 er det et negativt tal
		temperature *= -1;
	}
	return temperature;
}