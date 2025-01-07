#ifndef  MYPOST_H
#define  MYPOST_H
#define STR(x) STR_IMPL_(x)
bool i2cTest(uint8_t i2cAddress);

char postBuf[128];

// ******************** tests single channel ONLY 
bool post(void){
	char tempS[10];
	
	strcpy(postBuf, "POST Error\n");
	// fill postBuf[] if any test fails, then return false
	
	// test for I2C devices
	Wire.begin();	
	if (!i2cTest(ADS_ADDR))
	{
		strcat(postBuf, "No ADS1115 at 0x");
		strcat(postBuf, itoa(ADS_ADDR, tempS, 16));
		strcat(postBuf, "\n");
	}
	
	if (!i2cTest(DIGI_V))
	{
		strcat(postBuf, "No MCP51 at 0x");
		strcat(postBuf, itoa(DIGI_V, tempS, 16));
		strcat(postBuf, "\n");
	}
	
	if (!i2cTest(DAC_ADDR))
	{
		strcat(postBuf, "No DAC at 0x");
		strcat(postBuf, itoa(DAC_ADDR, tempS, 16));
		strcat(postBuf, "\n");
	}
	
	// screen failure will be obvious
	
	if (strlen(postBuf) > 16)
	{
		Serial.printf("\n%s", postBuf);
		return false;
	}
	Serial.println("Passed POST");
	return true;

}

bool i2cTest(uint8_t i2cAddress){		
	Wire.beginTransmission (i2cAddress);
	if (Wire.endTransmission () == 0)
		return true; 
	return false;
}
#endif
