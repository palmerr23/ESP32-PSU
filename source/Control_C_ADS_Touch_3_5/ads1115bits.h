/* ADS1115bits - stuff that can't go into the library
calibration bits
*********** all except ISR can now be moved to library.
*/
#include "mycvtHAL.h"
#include "myADS1115.h"
// outside the object definition (ISR & child routines can't be in object)
volatile long adsISRcount = 0;
volatile bool adsReady = false, adsSetready = false;
long adSProcessCount = 0;
uint8_t adsMux = ADSMUX_START;
#define NUMADC 4   
int16_t adsReadings[NUMADC];

const uint16_t adsConfig =
      ADS1015_REG_CONFIG_CQUE_1CONV | // Assert ALERT/RDY after one conversions
      ADS1015_REG_CONFIG_CPOL_ACTVLOW |       // Alert/Rdy active low   (default val)
      ADS_RATE |                              // sample rate selected above
      ADS1015_REG_CONFIG_MODE_SINGLE;         // Single-shot mode (default)
      //ADS1015_REG_CONFIG_CLAT_LATCH |  //  Latching comparator
      //ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      //ADS1015_REG_CONFIG_OS_SINGLE ;   // single shot start      
      // ADS1115_REG_CONFIG_MUX_SE |      // single ended conversion
      // ADS1015_REG_CONFIG_PGA_4_096V;   // PGA GAIN = 1 


// interrupt code in IRAM to avoid kernel panics
void IRAM_ATTR ads_ISR(){  
  portENTER_CRITICAL_ISR(&mux);
    adsReady = true;
  portEXIT_CRITICAL_ISR(&mux);
  //adsISRcount++; 
}

void adsProcess(void){
  if (adsReady)
  {	// convert latest reading to "volts read" and "read-world" values
//Serial.print("%");
    if(adsMux == 1) // flag a complete set of readings
	{
	  adSProcessCount++;  // updated each 4 cycles = 16mS @ 250 sps
	  adsSetready = true; // complete set of readings has been processed
	}
	  
	  // don't bother about turning interrupts off - single shot mode
    adsReadings[adsMux] = ads.getLastConversionResults();	
	// conversions now done just before control() 
	//myADC[adsMux].curVolts = convertHAL(adsReadings[adsMux], &ADS_HAL); 
	//myADC[adsMux].curVal = myADC[adsMux].convert(myADC[adsMux].curVolts, &myADC[adsMux]);
						   
    // start next reading
	adsMux = ++adsMux % NUMADC;
    ads.adsStartSingleRdy(adsConfig | ADS_GAIN, adsMux); 
    adsReady = false; 
	//Serial.print("^");
  }
}


void adsTest()
{
 // start ADS1115 ADC
 int rdg;
  ads.setGain(ADS_GAIN);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  pinMode(ADSRDY, INPUT_PULLUP);
  ads.begin(400000);  // I2C speed, 1000000 (1M = FMPlus) is probably OK, 400000 is safe
  Serial.println("ADS BEGIN done");
  Wire.beginTransmission (ADS_ADDR);
	if (Wire.endTransmission () == 0)
		Serial.printf("Found ADS1115 at 0x%x\n", ADS_ADDR);
	else
		Serial.printf("ADS1115 not found at 0x%x\n", ADS_ADDR);
  //  delay(100);
  //attachInterrupt(digitalPinToInterrupt(ADSRDY), ads_ISR, FALLING);
 // Serial.println("ATTACH int done");
 delay(1000);
  while(1)
  {
	  for (int i = 0; i < 4; i++)
	  {
		//rdg = ads.readADC_SingleEnded(i);		
		ads.adsStartSingleRdy(adsConfig,i); // no wait, needs suitable delay before reading
		delay(2);
		rdg = ads.getLastConversionResults();	
		Serial.printf("ADS ch[%i] = %i\n", i, rdg);
	  }
	  Serial.println();
	  delay(40);
  }
}
// signal = logical channel [index to myADC]
/*
float  readADSreal(uint8_t signal){
	//uint8_t ADSchan = myADC[signal].ADCchannel;
	//float real = myADC[signal].minVal +  ADS_HAL.curCount * (myADC[signal].maxVal - myADC[signal].minVal)/(1 << ADSBITS);
	//Serial.printf("[CH: %i, CT: %i, V: %f]\n",signal, myADC[signal].curCount, real);
	return myADC[adsMux].curVal;
}
*/
/*
float  readADSvolts(uint8_t signal){
	// value already updated by interrupt
	//uint8_t ADSchan = myADC[signal].ADCchannel;
	//float volts = myADC[signal].minVolts +  ADS_HAL.curCount * (myADC[signal].maxVolts - myADC[signal].minVolts)/(1 << ADSBITS);
	//Serial.printf("[%f]", volts);
	//Serial.printf("[CH: %i, CT: %i, V: %f]\n",signal, myADC[signal].curCount, volts);
	return myADC[signal].curVolts ;

}
*/
/*
float  readADStherm(uint8_t signal){ // not used, now converted correctly in adsProcess()
	//uint8_t ADSchan = myADC[signal].ADCchannel;
    //float volts = myADCcalls[signal].readVolts(signal);
	//float degrees =  convertTherm(volts, &myADC[signal]); // FIX ME!
	//Serial.printf("[CH: %i, CT: %i, V: %f]\n",signal, myADC[signal].curCount, degrees);
	return myADC[adsMux].curVal;
}
*/

/*
// return read value in Volts **************** no longer used
float  readADS_HAL(uint16_t devAddr, uint8_t ADCchannel){
	float volts = convertHAL(adsReadings[ADCchannel], &ADS_HAL);
	//Serial.printf("[CH: %i, CT: %i, V: %f]\n",ADCchannel, adsReadings[ADCchannel], volts);
	return volts;
}
*/