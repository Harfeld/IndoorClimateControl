/*
Functions are from optional i2c material from the AMS course specifically the file LAB_5.c
https://blackboard.au.dk/bbcswebdav/pid-2485499-dt-content-rid-7940242_1/courses/BB-Cou-UUVA-89578/BB-Cou-STADS-UUVA-52360_ImportedContent_20160107025559/LAB/LAB5%20I2C%20and%20LM75%20temperature%20sensor/Files%20for%20LAB5/LAB_5.c
https://blackboard.au.dk/webapps/blackboard/content/listContent.jsp?course_id=_134234_1&content_id=_2485474_1&mode=reset
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