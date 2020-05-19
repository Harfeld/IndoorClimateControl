/*
Inspiration: https://lastminuteengineers.com/dht11-dht22-arduino-tutorial/ - and referenced libraries from link.
*******************************************
* "AM2302.H"                              *
* Header for "AM2302 Temperature driver"  *
* Author: Andreas Harfeld Jakobsen		  *
*         Jonas Hingebjerg Hansen         *
* Date:   29.5.2020			              *
*******************************************/
void initAM2302(bool debugEnabled);
void enableDebugging();
void disableDebugging();
void sendStartSignal();
bool waitForSensorConfirmation();
uint32_t* receiveData();
uint8_t* readSensor();
float readHumidity();
float readTemperature();
/******************************************/