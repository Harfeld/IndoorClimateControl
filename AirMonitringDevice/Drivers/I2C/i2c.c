/*
Functions are from optional i2c material from the AMS course.
*******************************************
* "I2C.c"                                 *
* Implementation for "mega2560 i2c driver"*
* Author: Andreas Harfeld Jakobsen		  *
*         Jonas Hingebjerg Hansen         *
* Date:   29.5.2020			              *
*******************************************/

#include <avr/io.h>
#include "i2c.h"

void i2c_init()
{
	//setting scl frequenzy:
	// Mellem 100 og 400kHz som defineret i databladet for ds3231
	//
	TWSR &= ~(14);
	TWBR = 10;
	
}

void i2c_start()
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0)
	{}
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0)
	{}
}

unsigned char i2c_read (unsigned char isLast)
{
	if (isLast == 0)
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	else
	TWCR = (1<<TWINT) | (1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0)
	{}
	return TWDR;
}

void i2c_stop()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}