/*
 * AirMonitringDevice.c
 *
 * Created: 18/05/2020 08:33:52
 * Author : andre
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "Drivers/DHT/am2302.h"
#include "Drivers/RTC/ds3231.h"
#include "Drivers/Display/lcd162.h"
#include "Drivers/UART/uart.h"
#include "Drivers/I2C/i2c.h"
#include "Drivers/MQ135/MQ_135.h"

void addCustomSignsToDisp(){
	unsigned char degree[8] = {
		0b00110,
		0b01001,
		0b01001,
		0b00110,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
	};
	LCDLoadUDC(0, degree);
	unsigned char droplet[8] = {
		0b00000,
		0b00100,
		0b01010,
		0b10001,
		0b10001,
		0b01110,
		0b00000,
		0b00000
	};
	LCDLoadUDC(1, droplet);
}

void displayTimeStamp(struct dateTime* timeStamp){
	//display date
	LCDDispString(getDay(timeStamp->day));LCDDispChar(' ');
	if (timeStamp->date < 10)
	{
		LCDDispChar('0');
	}
	LCDDispInteger(timeStamp->date);LCDDispChar('/');
	if (timeStamp->month < 10)
	{
		LCDDispChar('0');
	}
	LCDDispInteger(timeStamp->month);LCDDispChar('-');
	LCDDispInteger(timeStamp->year);
	LCDGotoXY(0,1);
	
	//Display time
	if (timeStamp->hours < 10)
	{
		LCDDispChar('0');
	}
	LCDDispInteger(timeStamp->hours);LCDDispChar(':');
	if (timeStamp->minutes < 10)
	{
		LCDDispChar('0');
	}
	LCDDispInteger(timeStamp->minutes);LCDDispChar(':');
	if (timeStamp->seconds < 10)
	{
		LCDDispChar('0');
	}
	LCDDispInteger(timeStamp->seconds);
}

int main(void)
{
    InitUART(9600, 8, 'N');
    initAM2302(false);
	i2c_init();
	LCDInit();
	addCustomSignsToDisp();
	Init_MQ();
	MqCalibrate(CO2);
	
    while (1) 
    {
		//read gas data
		char co2Ppm[7];
		sprintf(co2Ppm, "%.1f", MQGetGasPercentage(CO2));
		
		//read temp and hum data		
		char hum[7];
		sprintf(hum, "%.1f", readHumidity());
		char temp[7];
		sprintf(temp, "%.1f", readTemperature());
		
		//read timestamp
		struct dateTime timeStamp;
		readDS3231(&timeStamp);
		
		//output to display
		//Display timestamp
		LCDClear();
		displayTimeStamp(&timeStamp);
		_delay_ms(3000);
		
		//Display Humidity and Temperature
		LCDClear();
		LCDDispString(hum);LCDDispChar('%');LCDDispChar(1);LCDDispChar(' ');
		LCDGotoXY(0,1);
		LCDDispString(temp);LCDDispChar(0);LCDDispChar('C');
		_delay_ms(3000);
		
		//Display Gas concentration
		LCDClear();
		LCDDispString("CO2: ");
		LCDDispString(co2Ppm);LCDDispString("ppm");
		_delay_ms(3000);
    }
}

