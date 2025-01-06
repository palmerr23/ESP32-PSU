/* ESP32 ADC
*********************** NOT USED IN THIS PROJECT
*/
#ifndef ESP32ADC_H
#define ESP32ADC_H
#define EEPROM_SIZE 4096 // bytes available

// return read value in Volts - identically named for different inbuilt ADCs
 //three point linearisation - corrects increase in slope from 3000 counts onward
//inHAL ESPADC_HAL = {0.129, 2.731, 3.163, 3296, 0, 12, true}; // ESP ADC
#include "mycvtHAL.h"
#define ADCREADS 4
float  readADCHAL(uint8_t ADCpin){
	int reading = 0;
	float volts = 0;
	/*
	// ADCchannel is a ESP32 pin reference
	// devAddr is ignored  

	for (short i = 0; i < ADCREADS; i++)
		reading += analogRead(ADCpin);
	reading /= ADCREADS;
	volts = convertHAL(reading, &ESPADC_HAL);
	*/
	return volts;
}
float  readVolts(uint8_t ADCpin)
{
	float volts = 0; //convertHAL(readADCHAL(ADCpin), &ESPADC_HAL);
	return volts;
}
#endif