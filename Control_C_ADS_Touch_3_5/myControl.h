#ifndef MYCONTROL_H
#define MYCONTROL_H
/*
Single instrument control for PSU - v2 will allow a control structure to be input, allowing multiple hardware to be controlled via a single controller. (I2C bus extender...)
 *** need to abstract input and outputs for V2 
 Safe start values are written in setup(), prior to control loop firing or previous setpoints being read from EEPROM
 (Enough SMPS to fire up 5V regulator, zero volts main out)
 Uses proportional control, with hysteresis
 iLimit: overrides voltage setpoint. Uses linear V-I approx to estimate correct voltage setpoint to reach current setting
 Remote limiting behaves in the same way, reducing voltage setting proportionately
 Short circuit protection and other errors (overvoltage) turn off output
*/
void onOff(int8_t channel, bool status);
void calSMPS(void);
short SCPIsendGrpMessage(uint8_t tgrp, char *buf);


bool changedLocal = true; // last value changed was local (or remote)

bool fanOn(bool fanSet){
  // turn fan on or off
	pinMode(FANPIN, OUTPUT);
	digitalWrite(FANPIN, fanSet);
	fanIsOn = fanSet;
	return fanIsOn;
}

// update readings from ADC to control values - both actual volts read and real-world values
void getReadings(void){
   // duplicates ADS1115 code, but missing from native ADC handlers
   // convert reading counts to volts
   portENTER_CRITICAL(&mux); // turn off interrupts while getting the ADS converted values
	   myADC[0][VOUT].curVolts = convertHAL(adsReadings[VOUT], &ADS_HAL); 
	   myADC[0][VIN].curVolts  = convertHAL(adsReadings[VIN],  &ADS_HAL); 
	   myADC[0][IOUT].curVolts = convertHAL(adsReadings[IOUT], &ADS_HAL); 
	   myADC[0][TEMP].curVolts = convertHAL(adsReadings[TEMP], &ADS_HAL); 
   portEXIT_CRITICAL(&mux);
   // convert volts read to real world values
  //convertHAL(adsReadings[adsMux], &ADS_HAL); 
  //Serial.print("*");
   myADC[0][VOUT].curVal = myADCcalls[VOUT].convert(myADC[0][VOUT].curVolts, &myADC[0][VOUT]);
   myADC[0][VIN].curVal  = myADCcalls[VIN].convert(myADC[0][VIN].curVolts,   &myADC[0][VIN]);
   myADC[0][IOUT].curVal = myADCcalls[IOUT].convert(myADC[0][IOUT].curVolts, &myADC[0][IOUT]);
   myADC[0][TEMP].curVal = myADCcalls[TEMP].convert(myADC[0][TEMP].curVolts, &myADC[0][TEMP]);
}
bool shutDown = false;
void errorI(bool shutOff, bool powerDown, float reading, char * errMsg)
{
	char msgBuf[128];
	if (shutOff){		
		onOff(-1, false);    // shut off output		
	}
	if(powerDown){
		shutDown = true;  //change setpoints to safe values		
	}
	// display screen error message, indicator, and store for SCPI 
	sprintf(msgBuf,"*** Error: \n%s \n[%5.2f]\n", errMsg, reading);
	screenError(msgBuf, ERR_BG_B, 10, false);
}

// coarse and fine control for main output with limiting and tracking
// the actual setting (DPOTsetting) may differ from the desired voltage setting (pSet[0].voltage)
//   as a result of external tracking and iLimit
// SMPS tracks the actual output voltage + VDROPOUT 
//   but does not go the minimum (start) voltage (VSTART_S)
//  returns the calculated setpoint (after limiting and tracking)

// ******** move myACD to pointer referenced argument for 2CH

/*
int DPOTsetting = WSTART, vSetcS = WSTART_S ,DACsetting = DSTART; // values are settings, not Volts
float vSetpointX, vSetpointR, vSetpointI, vSetpointS, localLimitRatio;
float vLast = 0, vLastSet = 0;
bool iLimit = false;
bool oldTrack = false;
*/
char estop[] = ":TRAC:ESTO";
float control(int chan)
{
	settings * sp = &pSet[chan];
	controls * cp = &pCont[chan];
  float vSetpoint, iSetpoint, iTrack;
  bool limiting, trackEn;
  float vRead, vReadS, iRead, tRead, vGap;  
  int jump;
  bool rLimit;
  short fjump;
  // vX = myADCcalls[VOUT].readVolts(VPIN);
 
   vSetpoint = sp->voltage; // update settings 
   iSetpoint = sp->current ;
   iTrack = eTrack;
   limiting = cp->limiting;
   trackEn = mSet.trackOn;
   // sp->voltageS  = vSetpoint + VDROPOUT; // no loger used
   //************ these need to be extracted to a pointer ******************
   vRead  = myADC[0][VOUT].curVal; // for convenience
   vReadS = myADC[0][VIN].curVal;
   iRead  = myADC[0][IOUT].curVal;
   tRead  = myADC[0][TEMP].curVal; 
   cp->watts = vRead * iRead;
   vGap = vSetpoint - vRead;
 //  Serial.print("CONT");
#ifdef C_DEBUG   
  //  Serial.printf("%i",loopcntr);
	//Serial.printf(" Vin count = %i", adsReadings[VIN]);
	Serial.printf("Lim %s, Trac %s ", (limiting)? "T" : "F", (trackEn)? "T" : "F");
	if(trackEn) Serial.printf("(%3.1f%)", iTrack*100);
   // Serial.print(": vSet = "); Serial.print(vSetpoint);
  if (limiting)  Serial.printf(", Iset = %5.3f",  pSet[0].current);
    Serial.printf(", Iread = %5.3f", iRead);
	Serial.printf(", IreadV = %5.3f", myADC[0][IOUT].curVolts);
	//Serial.printf(", Icount = %i", adsReadings[IOUT]);  
   // out of bounds checking: lowError; hiError;
#endif
   if(!startUp) // don't process these immediately after turn on
   {
	   //Serial.printf("\nCE: %5.3f %5.3f\n",iRead , myADC[0][IOUT].hiError);
	   // serious error conditions - will turn off output and reduce voltage values to safe
	   // will also turn off outputs in the group, if tracking 
	   if (iRead > myADC[0][IOUT].hiError) // Short circuit protection
	   {
		 //Serial.print("Short circuit\n");
	   //	 if(sp->trackEn) SCPIsendGrpMessage(sp->trackGrp, estop);
		 errorI(true, false, iRead, (char *)"Short circuit \nprotection"); 
	   }
	   
	   if (vRead > myADC[0][VOUT].hiError) // Output overvoltage
	   {
		 //Serial.printf("Out over error %5.3f [limit  %5.3f]\n",  vRead, myADC[0][VOUT].hiError);
		 //if(mSet.trackOn)			SCPIsendGrpMessage(sp->trackGrp, estop);
		 errorI(true, true, vRead, (char *)"Output overvoltage"); 
	   }
	   
	   if (vReadS > myADC[0][VIN].hiError) // Input overvoltage
	   {
		 //if(mSet.trackOn)			SCPIsendGrpMessage(sp->trackGrp, estop);
		 errorI(true, true, vReadS, (char *)"Input overvoltage "); 
	   }
		 
	   if (tRead > myADC[0][TEMP].hiError)
	   {	
	    // if(mSet.trackOn) // heatsink over temperature
			//SCPIsendGrpMessage(sp->trackGrp, estop);
		 errorI(true, true, tRead, (char *)"Over temperature");  
	   }	  
   }
   
   if (shutDown || (!outOn) || (!pSet[0].outEn)) // set output voltage to zero if E-stop or, output is off (enable SoftStart)
	 vSetpoint = 0.0; 
 
   // fan control
   fanOn(tRead >= mSet.temperature); // needs hysteresis (fan hunts around)
  
    // control 	
	if (abs(cp->vLastSet - vSetpoint) > 0.05) // reset limiting when setting changes significantly - might not be needed???
	{
		//Serial.print(" R ");
		cp->iLimit = false; // local current limiting needed

		cp->vSetpointI = cp->vSetpointR = vSetpoint; // reset to not limiting values
		cp->localLimitRatio = 1.0;
	}
	
    // local iLimit
	cp->limitInd = (limiting) ? IND_EN : IND_OFFC; //  indicator (active limiting set below) 0==disabled; 1==enabled; 2 == limiting
	//Serial.print(" L");
	float iDiff = iRead -  sp->current;
	if(abs(iDiff) > I_HYST) // don't change limiting state if only a small error
	{
		if ((iRead >  sp->current || cp->vSetpointI < vSetpoint) && limiting) // start or stop limiting 
		{ 
		  // track against last used V setting, rather than desired value; wrong for first iteration of a new setting
		 // cp->vSetpointI = vRead * sp->current / iRead ;    //  linear V-I approximation is a little too aggressive
		  cp->vSetpointI = vRead * (1 -  DAMP_ILIMIT *  (iRead - sp->current)/ iRead) ;    // lightly damped linear V-I approximation 
		  cp->localLimitRatio = cp->vSetpointI / sp->voltage;
		  cp->iLimit = true; // telltale for display
		  cp->limitInd = IND_ACT;
		 //Serial.print("+");
		}
		else
		{
			cp->localLimitRatio = 1.0;
			cp->iLimit = false; // not limiting current locally
			//Serial.print("-");
		}
	}
	
	dirtyScreen =  dirtyScreen || (limiting != cp->iLimit); // cause a screen redraw if changed
	limiting = cp->iLimit;
	if (limiting) // button highlights follow limiting
	{
		if (cp->iLimit)
			but[LBUT].selColour = but[LBUT].unselColour = ITRIG;
		else
			but[LBUT].selColour = but[LBUT].unselColour = IENABLE;
	}
	else 
	{
		but[LBUT].selColour   = BBGHIGH;
		but[LBUT].unselColour = BBXHIGH;
	}
	
    //  ******************************* TEST remote tracking
	cp->trackInd = (trackEn)? 1 : 0; // indicator (as for limitInd)
    if (trackEn && (iTrack <  (1.0 - SMALL_DIFF)))  { // iTrack always <= 1.0
       cp->vSetpointR = vSetpoint * iTrack; // scale to remote tracking request
       rLimit = true;
	   cp->trackInd = 2;
	   //Serial.print(" *rL* ");
       // display telltale is already available
    }
	else 
	{
		rLimit = false; 
		cp->vSetpointR = vSetpoint; // reset ext tracking
	}
	dirtyScreen |= cp->oldTrack != rLimit;
	cp->oldTrack = rLimit;
	if (trackEn) // button highlights follow tracking
	{
		if (rLimit)
			but[TBUT].selColour = but[TBUT].unselColour = ITRIG;
		else
			but[TBUT].selColour = but[TBUT].unselColour = IENABLE;
	}
	else 
	{
		but[TBUT].selColour   = BBGHIGH;
		but[TBUT].unselColour = BBXHIGH;
	}
    // ***************** main regulator control *************
	cp->vSetpointX = vSetpoint;
	// when limiting or tracking, value will be the minimum of active values
	if (cp->iLimit)
		cp->vSetpointX = min(cp->vSetpointX , cp->vSetpointI);
	if(rLimit)
		cp->vSetpointX = min(cp->vSetpointX , cp->vSetpointR);
   
    vGap = (cp->vSetpointX - vRead);  
	
	// stop overshoot by controlling dV/dT - only on rising voltages, as Dv/DT is more load dependent on falling.
    if (vGap > DVDC) 
	{
		cp->vSetpointX = vRead + DVDC;
		vGap = (cp->vSetpointX - vRead);   
	}
#ifdef	C_DEBUG
	printfloatX((void *)&vGap, "", &FONT1, 1, 2,3,0,20,170, 'F', TEXTCOL, TEXTCOL, BGCOL);
#endif

    // COARSE (DigiPot)- attain correct output in 2-3 steps (moderate damping)
	// change in coarse voltage required = current voltage offset on fine control + actual gap
	float vDiff;
	//vDiff= vGap;
	vDiff = vGap + ((float)cp->DACsetting - DSTART)/F_STEPS_VOLT;	
    jump = vDiff * potV.polarity * STEPS_VOLT;  // setting change
    if (abs(jump) > HYSTERESIS) 
    {
	  if (abs(jump) > BIGJUMP)
		jump *= DAMP_VOLT; // for big jumps, be conservative to avoid overshoot
      cp->DPOTsetting += jump;
      // there's something wrong if we need to constrain the setting - cause an error?
      cp->DPOTsetting = constrain(cp->DPOTsetting, 0, MCPMAXVAL);
      MCP45writeW(cp->DPOTsetting, DPOT_BASE);
      cp->DACsetting = DSTART; // reset fine to midpoint with every coarse step change
      fjump = 0; // for debug only
    }
    else  // FINE control (DAC)
    {
      fjump = vGap * FPOL * F_STEPS_VOLT;
      if (abs(fjump) > FHYSTERESIS)
        cp->DACsetting += fjump;
      cp->DACsetting = constrain(cp->DACsetting, 0, DAC_MASK);   
    }
      dac.setVoltage(cp->DACsetting, false);// always update fine control
#ifdef C_DEBUG   
          
		Serial.print(": vSetI = ");
        Serial.print(cp->vSetpointI);
        Serial.print(", Vrd = ");
        Serial.printf("%7.4f",vRead);       
       // Serial.print(", Vcount = ");
       // Serial.printf("%i", adsReadings[VOUT]);
        Serial.print(", vGap = ");
        Serial.printf("%4.2f mV",vGap*1000);

        Serial.print(", Set = ");
        Serial.print(cp->DPOTsetting);
        Serial.print(", jump = ");
        Serial.print(jump);
        //Serial.print(", fJump = ");
        //Serial.print(fjump);
        Serial.print(", FSet = ");
        Serial.print(cp->DACsetting);
 // Serial.print("vX = "); Serial.print(vX);
 // Serial.print(", aR = "); Serial.print(analogRead(A0));Serial.print(",  ");
  //Serial.print(myADC[0][VOUT].iname); Serial.print("  = "); Serial.print( myADC[0][VOUT].curVal);  Serial.print(myADC[0][VOUT].units); Serial.print(", "); 
  //Serial.print(myADC[0][VIN ].iname); Serial.print(" = ");  Serial.print( myADC[0][VIN].curVal);   Serial.print(myADC[0][VIN].units);  Serial.print(" \n"); 
  //Serial.print(myADC[0][IOUT].iname); Serial.print(" = ");  Serial.print(myADC[0][IOUT].curVal);   Serial.print(myADC[0][IOUT].units); Serial.print(", "); 
  //Serial.print(myADC[0][TEMP].iname); Serial.print(" = ");  Serial.print(myADC[0][TEMP].curVal);   Serial.println(myADC[0][TEMP].units);
#endif
  cp->vLast = cp->vSetpointX; // retain last actual set point 
	cp->vLastSet = vSetpoint; // retain last set point call argument

#ifdef C_DEBUG   
	/*
	Serial.printf(", Sread = %5.2f",vReadS);
	Serial.printf(", Sgap = %5.2f",vGap);
	Serial.print(", SetS = ");
    Serial.print(cp->vSetcS);
	*/
   Serial.println("");
#endif
   // return main output voltage - why????
  
   return vSetpoint;
} 
/**************************** end control **********************/

void control_setup(void)
{
  outOn = false;  // ensure output is off 
	
  pinMode(SW_ON, OUTPUT);
  digitalWrite(SW_ON, LOW); // OFF to start,  always in Output mode after this to drive LED, except when reading switch
  
  // Brief pulse to reset output relay (OFF)
  pinMode(SW_OFF, OUTPUT);
  digitalWrite(SW_OFF, HIGH);
  delay(50);
  digitalWrite(SW_OFF, LOW);
  pinMode(SW_OFF, INPUT_PULLDOWN); // always in Input mode after this 

  //float volts, realWorld;
  
  // ********** Digipot  & DAC***********
  MCP45begin(WSTART, DPOT_BASE); 

  dac.begin(0x60);
  dac.setVoltage(DAC_MASK / 2, false);

  // watchdog?
  // wTimerBegin(100); // mS

  
  // ****************** ADC ***********
  // start ADS1115 ADC
  ads.setGain(ADS_GAIN);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  pinMode(ADSRDY, INPUT_PULLUP);
  ads.begin(I2C_SPEED);  // I2C speed, 1000000 (1M = FMPlus) is probably OK, 400000 is safe
 // Serial.println("ADS BEGIN done");
  //  delay(100);
  
   // zero the output current reading
  delay(10);
  int iZero = ads.readADC_SingleEnded(IOUT);
 // myADC[0][IOUT].minVolts =  convertHAL(iZero, &ADS_HAL); 
  
  // start the regular conversion process
  attachInterrupt(digitalPinToInterrupt(ADSRDY), ads_ISR, FALLING);
 // Serial.println("ATTACH int done");
  //  delay(100);  
  ads.adsStartSingleRdy(adsConfig, ADSMUX_START);
  
/********** ENCODER and Tac Switches *************/
  enc.setChangedHandler(rotate);
  butL.setPressedHandler(clickedL);
  butR.setPressedHandler(clickedR);
  
  // calibrate the control step/Voltage settings for the SMPS (very non-linear)
  // calSMPS(); // also turns off output
}

#define DEBOUNCE_OO  5
uint8_t onSwitch, offSwitch;
int oo_samestate = 0;
// more complex conditions, so can't use Button2 library
void processOnOffSw(void)
{
	uint8_t onVal, offVal;
	pinMode(SW_ON, INPUT_PULLDOWN); // pin drives LED, so only Input while reading
	// SW_OFF is always in Input mode, except when being pulsed by an Off command
	onVal = digitalRead(SW_ON);
	offVal = digitalRead(SW_OFF);	
	pinMode(SW_ON, OUTPUT);
	digitalWrite(SW_ON, (pSet[0].outEn)? HIGH : LOW); // re-assert LED status using first channel - will be the same for all channels
	
	//debounce: must have same state for DEBOUNCE_OO cycles
	if(onVal == onSwitch && offVal == offSwitch)
		oo_samestate++;
	else
	{
		onSwitch = onVal;
		offSwitch = offVal; 
		oo_samestate = 0;
		return;
	}	
	
	if (oo_samestate >= DEBOUNCE_OO)
	{
		if (onVal == HIGH && !pSet[0].outEn) // switch on, if not already on
		{
			onOff(-1, true);
		}
		if (offVal == HIGH && pSet[0].outEn) // switch off
		{
			onOff(-1, false);
		}	
	}	
}

//used by SCPI as well as panel buttons
// status false = OFF
void onOff(int8_t channel, bool status)
{
	short i, j , k;
	if (channel == -1) // do all channels
	{
		j = 0; 
		k = NUMCHANNELS;
	}
	else
	{
		j = channel; 
		k = channel + 1;
	}
		for (i = j; i < k; i++)
		{
			chDef[i].sp->outEn  = status;
			//Serial.printf("Changing On/Off [ch %i] to %i [pin %i]\n", i, status, chanPins[i].onPin);
			// physical control 
			if(status) 		// ON
			{
				digitalWrite(chanPins[i].onPin, HIGH);  // always in OUTPUT mode, except when being explicitly read
			}
			else 			// OFF
			{ 
				digitalWrite(chanPins[i].onPin, LOW);		
				
				pinMode(chanPins[i].offPin, OUTPUT); 	// pulse Off pin - may be a SCPI command		
				digitalWrite(chanPins[i].offPin, HIGH); 
				delay(1);
				digitalWrite(chanPins[i].offPin, LOW);
				pinMode(chanPins[i].offPin, INPUT_PULLDOWN);  // reset pin for panel control 
			}
		}
}
// display counts and converted values
void dispConv(void)
{
	Serial.printf("Vin  [%i] = %5.2f [%i counts = %5.2fV]\n", VIN,    myADC[0][VIN].curVal,  adsReadings[VIN],  myADC[0][VIN].curVolts );
	Serial.printf("Vout [%i] = %5.2f [%i counts = %5.2fV]\n", VOUT,   myADC[0][VOUT].curVal, adsReadings[VOUT], myADC[0][VOUT].curVolts );
	Serial.printf("Iout [%i] = %5.2f [%i counts = %5.2fV]\n", IOUT,   myADC[0][IOUT].curVal, adsReadings[IOUT], myADC[0][IOUT].curVolts );
	Serial.printf("Temp [%i] = %5.2f [%i counts = %5.2fV]\n\n", TEMP, myADC[0][TEMP].curVal, adsReadings[TEMP], myADC[0][TEMP].curVolts );
	
}
/* NO LONGER USED
// SMPS control is not linear
// create a lookup table to allow accurate estimation of setpoints and appropriate steps for fine control
#define CALDELAY 100 // wait to settle at new value - at least several complete ADS reading cycles 
void calSMPS(void)
{
	short i, j = 0;
	long wait;
	onOff(-1, false); // turn output off for this
	MCP45writeW(0, DIGI_S);
	delay(1000); // may take a while to slump to min value
	for (i = 0; i < SMPS_STEPS; i++)
	{	
		MCP45writeW(j, DIGI_S); // [0 .. MCPMAXVAL]
		wait = millis();
		while (millis() < wait + CALDELAY) // wait for reading to stabilise, while processing samples
			adsProcess();
		getReadings();   
		stepsSMPS[i].step = j;
		stepsSMPS[i].volts = myADC[0][VIN].curVal;
		j += (MCPMAXVAL + 1) / (SMPS_STEPS - 1);
		j = constrain(j, 0, MCPMAXVAL);
		//Serial.print("*");
	}

	for (i = 1; i < SMPS_STEPS -1; i++)
	{
		stepsSMPS[i].stepsVolt = (stepsSMPS[i+1].step - stepsSMPS[i-1].step) / (stepsSMPS[i+1].volts - stepsSMPS[i-1].volts); // average of slope either side of this point
		//Serial.printf("%i ",i);
	}	
	// make first and last entries the same as the adjacent ones: possibly out of control range and producing erroneous answers
	stepsSMPS[0].stepsVolt = stepsSMPS[1].stepsVolt;   //(stepsSMPS[1].step - stepsSMPS[0].step)/(stepsSMPS[1].volts - stepsSMPS[0].volts); // first entry - same as the next one
	stepsSMPS[SMPS_STEPS-1].stepsVolt = stepsSMPS[SMPS_STEPS-2].stepsVolt/2;//(stepsSMPS[SMPS_STEPS-1].step - stepsSMPS[SMPS_STEPS - 2].step)/(stepsSMPS[SMPS_STEPS-1].volts - stepsSMPS[SMPS_STEPS - 2].volts); // last entry, half the one before (as probably out of range)
	
	for (i = 0; i < SMPS_STEPS; i++)
		Serial.printf("CAL SMPS: slot %i, step %i, volts %5.2f, steps/v %5.2f\n", i, stepsSMPS[i].step, stepsSMPS[i].volts, stepsSMPS[i].stepsVolt);
	
}
*/
/*	

****** Software SMPS control is no longer used *****

	// *********** SMPS CONTROL ***************
	// non-linear, so uses a lookup table for both coarse setpoint and steps/volt
	// uses potS.polarity to determine the direction of volts / steps 
	// attain correct output in 2-3 steps (moderate damping)
	#define SOFF  (1.5)
	#define S2OFF (2.7)
	#define SMUL   (3.5)
	cp->vSetpointS = cp->vSetpointX + VDROPOUT; // track main output during limiting
	vGap = (cp->vSetpointS - vReadS);   
	short bestGuess, i;
	float stepsV, prop;
	for (i = 0; i < SMPS_STEPS; i++)
	{
		if (potS.polarity > 0 & stepsSMPS[i].volts > cp->vSetpointS)
		{

			break;
		}
		if (potS.polarity < 0 & stepsSMPS[i].volts < cp->vSetpointS)
		{
			break;		
		}
	}
	if (i == 0 || i == SMPS_STEPS) // outside the limits
	{
		if (i == SMPS_STEPS) // above the highest value?
			i--;
		bestGuess = stepsSMPS[i].step; // select the value at the limit
		stepsV = stepsSMPS[i].stepsVolt;
		prop = 1;
		//Serial.print(" @! ");
	}
	else // linear approx
	{
			prop = (stepsSMPS[i].volts - cp->vSetpointS) /(stepsSMPS[i].volts - stepsSMPS[i-1].volts);
			stepsV 	   = ((1 - prop) * stepsSMPS[i].stepsVolt + prop * stepsSMPS[i-1].stepsVolt);
			bestGuess  = ((1 - prop) * stepsSMPS[i].step + prop * stepsSMPS[i-1].step);
	}

	//Serial.printf("Ctrl: SMPS set  %5.3f [read %5.3f, gap %5.3f], prop %5.2f, steps/v %5.2f, ",cp->vSetpointS, vReadS, vGap, prop, stepsV);
	
	if (abs(vGap) > S_DELTA) // big gap: use the bestGuess setting
	{ 
		cp->vSetcS = bestGuess;
		//Serial.printf(" # set %i \n", cp->vSetcS);
	    cp->vSetcS = constrain(cp->vSetcS, 0, MCPMAXVAL);
        MCP45writeW(cp->vSetcS, DIGI_S);			
	}	
	else // increment from where we are until within the desired limits
	{		
		jump = vGap * potS.polarity * stepsV * SMPS_DAMP ; 
		if (abs(jump) > SHYSTERESIS)  // SHYSTERESIS
		{
		  cp->vSetcS += jump;
		  // there's something wrong if we need to constrain the setting - cause an error?
		  cp->vSetcS = constrain(cp->vSetcS, 0, MCPMAXVAL);
		  MCP45writeW(cp->vSetcS, DIGI_S);
		}
		//Serial.printf(" * set %i, jump %i\n", cp->vSetcS, jump);
	}
*/	 

#endif