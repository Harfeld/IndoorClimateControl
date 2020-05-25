#include "MQ_135.h"
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include "../ADC/ADC.h"
#include "../UART/uart.h"
#include "MQ_135.h"

//Datasheet
//https://datasheetspdf.com/pdf/605076/Hanwei/MQ-135/1

//Calculation references
//http://davidegironi.blogspot.com/2014/01/cheap-co2-meter-using-mq135-sensor-with.html#.XsJNx2gzZPY
//https://da.co2.earth/
//https://components101.com/sensors/mq135-gas-sensor-for-air-quality

//Code inspiration MQ-2 driver
//https://sandboxelectronics.com/?p=165

/*******************************************
* "MQ_135.c"                                 *
* Implementation for "MQ-135 driver"*
* Author: Andreas Harfeld Jakobsen		  *
*         Jonas Hingebjerg Hansen         *
* Date:   29.5.2020			              *
*******************************************/

#define CALIBRATION_SAMPLES 10
#define CALIBRATION_INTERVAL 500
#define READ_SAMPLES 5
#define READ_INTERVAL 50
#define CO2_BASE_PPM 415
	
//Initialize Ro \n
float Ro_CO2 = 10;

//Measured on module
int RL_VALUE = 19;

//Gas curves
//Data format: ppm = a*(Rs/Ro)^b, {a, b}
float CO2Curve[2]  =  {114.97, -2.957};

//Input:   adc_value
//Output:  voltage
//Calculate Voltage from ADC
float AdcToVoltage(int adc_value){
	float result = (((float)adc_value)/1023.0)*5.0;
	return result;
}

//Input:   adc value
//Output:  Rs sensor resistance
//Calculate Sensor resistance from ADC
float MQResistanceCalculation(int adc_value){
	volatile float result = ((5.0*(float)RL_VALUE)-((float)RL_VALUE*AdcToVoltage(adc_value)))/AdcToVoltage(adc_value);
	return result;
}

//Initializes the sensor by initializing the dependencies
void Init_MQ(){
	InitUART(9600, 8, 'N');
	Init_ADC();
}

//Input:   Gas type
//Sets Ro value for selected type
void MqCalibrate(enum Gas gas){
	int i;
	float val = 0;
	for(i = 0; i < CALIBRATION_SAMPLES; i++){
		val+= MQResistanceCalculation(AnalogRead());
		_delay_ms(CALIBRATION_INTERVAL);
	}
	val = val/CALIBRATION_SAMPLES;
	
	if(gas == CO2){
		Ro_CO2 = val * exp( log(CO2Curve[0]/CO2_BASE_PPM) / CO2Curve[1]);
	}else{
		Ro_CO2 = -1;
	}
		
}

//Output:  Rs value
float MQRead(){
	int i;
	float rs=0;
		for (i=0;i<READ_SAMPLES;i++) {
		rs += MQResistanceCalculation(AnalogRead());
		_delay_ms(READ_INTERVAL);
	}
	rs = rs/READ_SAMPLES;
	return rs;
}

//Input:   Gas type
//Output:  ppm for selected gas type
float MQGetGasPercentage(enum Gas gas){
	volatile float Rs = MQRead();
	if(gas == CO2){
		return CO2Curve[0] * pow((Rs/Ro_CO2), CO2Curve[1]);
	}
	
	return -1;
}