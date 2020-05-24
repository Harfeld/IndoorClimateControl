/*
Inspiration: 
DS3231 library by LittleBuster - https://github.com/LittleBuster/avr-rtc3231
Datasheet - https://pdf1.alldatasheet.com/datasheet-pdf/view/254832/MAXIM/DS3231.html
*******************************************
* "I2C.H"                                 *
* Header for "DS3231 RTC driver"          *
* Author: Andreas Harfeld Jakobsen		  *
*         Jonas Hingebjerg Hansen         *
* Date:   29.5.2020			              *
*******************************************/
#include <stdio.h>

struct dateTime
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint16_t year;
};

void readDS3231(struct dateTime *dateTime);
void calibrateDS3231(struct dateTime *dateTime);
char* getDay(uint8_t dayNum);
/******************************************/