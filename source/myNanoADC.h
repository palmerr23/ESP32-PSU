/* nano ADC
*/
#ifndef NANOADC_H
#define NANOADC_H
// each processor has an identical set of routines
#include "mycvtHAL.h"
#define EEPROM_SIZE 1024 // bytes available
  inHAL NANOADC_HAL = { 0.0, 0.0, 4.74, 0, 0, 10, false};
#define NUMREADS 3
// return read value in Volts - identically named for different inbuilt ADCs
float  readADCHAL(uint8_t ADCpin){
	// ADCchannel is a NANO pin reference
	// devAddr is ignored  
	int reading = 0;
	float volts;
	for (short i = 0; i < NUMREADS; i++)
		reading += analogRead(ADCpin);
	reading /= NUMREADS;
	volts = convertHAL(reading, &NANOADC_HAL);
	//Serial.print("ADC_HAL("); Serial.print(ADCchannel); Serial.print(") = ");  
	//Serial.print(reading);  Serial.print(", V = "); Serial.println(volts);
	return volts;
}
float  readVolts(uint8_t ADCpin)
{
	float volts;
	volts = convertHAL(readADCHAL(ADCpin), &NANOADC_HAL);
	return volts;
}
#endif