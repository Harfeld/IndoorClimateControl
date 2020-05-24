/*
Inspiration: 
DHT library by Adafruit - https://github.com/adafruit/DHT-sensor-library
Datasheet - https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf
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