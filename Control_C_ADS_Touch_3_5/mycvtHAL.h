#ifndef  CVTHAL_H
#define  CVTHAL_H


// Input readings are converted in two stages: counts to Volts; then Volts to real world values
// HAL (conversion device) counts to Volts 
float convertHAL(int reading, inHAL * hp){
// convert reading from HAL to volts
// assumes minimum reading is zero counts
float volts;
int maxCount;
maxCount = (1 << hp->ADCbits) -1;
  
  if (hp->threePoint){ // dual slope approximation
    if (reading <= hp->midCount)
      volts = hp->minVolts + (hp->midVolts - hp->minVolts) * reading / hp->midCount;
    else
      volts = hp->midVolts + (hp->maxVolts - hp->midVolts) * (reading - hp->midCount) / (maxCount - hp->midCount);
  }
  else {
    volts = hp->minVolts + (hp->maxVolts - hp->minVolts) * reading / maxCount;
  }
  return volts;  
}

// convert Volts read to the real world value that represents
/////////////// should be in myHelpers.h????
float convertADC(float reading, inADC * ap){
// convert reading from volts to real world value
float val;
  //Serial.print("CvtADC mV = "); Serial.print(ap->maxVolts);
  if (ap->threePoint) // dual slope approximation
  { 
    if (reading <= ap->maxVolts)
      val = ap->minVal + (ap->midVal - ap->minVal) * (reading - ap->minVolts) / (ap->midVolts - ap->minVolts);
    else
      val = ap->midVal + (ap->maxVal - ap->midVal) * (reading - ap->midVolts) / (ap->maxVolts - ap->midVolts);
  }
  else { // two points
    val = ap->minVal + (ap->maxVal - ap->minVal) * (reading - ap->minVolts) / (ap->maxVolts - ap->minVolts);
  }
  //Serial.print("CvtADC RW = "); Serial.print(val); Serial.println(ap->units);
 // Serial.printf("CVT [%s]: %f - %f\n", ap->iname, reading, ap->minVolts);
  return val;  
}

// uses the Steinhart-Hart equation - https://learn.adafruit.com/thermistor/using-a-thermistor
//int xggqtl;
float convertTherm(float reading, inADC * ap){
  // convert reading from volts to degrees C
  // myADC for Thermistor re-uses values with for Thermistor parameters
  // maxVolts = Vcc (normal usage); minvolts = Ro; midVolts = B; maxVal = To (deg C)
  // rTherm ==  thermistor resistance
  //if((xggqtl++ % 500) == 0)
	 // Serial.printf("Stein Coeff = %f\n", ap->midVolts);
  float rTherm = ap->midVal /((ap->maxVolts/reading) - 1);
  float steinhart;
  steinhart = rTherm / ap->minVolts;           // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= ap->midVolts;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (ap->maxVal + 273.15);    // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  return steinhart;
}


#endif // CVTHAL_H
