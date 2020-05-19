/*
Functions are from optional i2c material from the AMS course.
*******************************************
* "I2C.H"                                 *
* Header for "mega2560 i2c driver"        *
* Author: Andreas Harfeld Jakobsen		  *
*         Jonas Hingebjerg Hansen         *
* Date:   29.5.2020			              *
*******************************************/
void i2c_init();
void i2c_start();
void i2c_write(unsigned char data);
unsigned char i2c_read (unsigned char isLast);
void i2c_stop();
/****************************************/