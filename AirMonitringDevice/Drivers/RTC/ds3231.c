/*
Inspiration: 
DS3231 library by LittleBuster - https://github.com/LittleBuster/avr-rtc3231
Datasheet - https://pdf1.alldatasheet.com/datasheet-pdf/view/254832/MAXIM/DS3231.html
*******************************************
* "I2C.c"                                 *
* Implementation for "DS3231 RTC driver"  *
* Author: Andreas Harfeld Jakobsen		  *
*         Jonas Hingebjerg Hansen         *
* Date:   29.5.2020			              *
*******************************************/
#include "ds3231.h"
#include "../I2C/i2c.h"

uint8_t toDecimal(uint8_t data){
	return ((((data >> 4) & 0b1111) * 10) + (data & 0b1111));
}

uint8_t toBinary(uint8_t data){
	uint8_t ones = data % 10;
	uint8_t tens = data / 10;
	return ((tens << 4) | ones);
}

void readDS3231(struct dateTime *dateTime){
	//first setting the register address where we want to start the reading
	i2c_start();
	i2c_write(0b11010000); //Slave Address(1101000) + w/r bit(lsb) = 0
	i2c_write(0x00);
	i2c_stop();
	
	//then we start reading		
	i2c_start();
	i2c_write(0b11010001); //Slave Address(1101000) + w/r bit(lsb) = 1
	dateTime->seconds = toDecimal(i2c_read(0));
	dateTime->minutes = toDecimal(i2c_read(0));
	dateTime->hours = toDecimal(i2c_read(0));
	dateTime->day = toDecimal(i2c_read(0));
	dateTime->date = toDecimal(i2c_read(0));
	dateTime->month = toDecimal(i2c_read(0));
	dateTime->year = 2000 + toDecimal(i2c_read(1));
	i2c_stop();
}

void calibrateDS3231(struct dateTime *dateTime){
	i2c_start();
	i2c_write(0b11010000); //Slave Address(1101000) + w/r bit(lsb) = 1
	i2c_write(0x00);
	i2c_write(toBinary(dateTime->seconds));
	i2c_write(toBinary(dateTime->minutes));
	i2c_write(toBinary(dateTime->hours));
	i2c_write(toBinary(dateTime->day));
	i2c_write(toBinary(dateTime->date));
	i2c_write(toBinary(dateTime->month));
	i2c_write(toBinary(dateTime->year - 2000));
	i2c_stop();
}

char* getDay(uint8_t dayNum){
	switch(dayNum) {
		case 1:
			return "Mon";
		break;
		case 2:
			return "Tue";
		break;
		case 3:
			return "Wed";
		break;
		case 4:
			return "Thu";
		break;
		case 5:
			return "Fri";
		break;
		case 6:
			return "Sat";
		break;
		case 7:
			return "Sun";
		break;
	}
}