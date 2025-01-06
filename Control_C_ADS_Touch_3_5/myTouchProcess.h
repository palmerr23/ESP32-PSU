/* Touch Screen Processing and Value Editing 
myTouchProcess.h
*/
#ifndef MYTOUCHPROC_H
#define MYTOUCHPROC_H

void postProcess(void);
int blockTouched(TS_Point p);
void updateVal(short highButton, short butDigit);

int toggleLimit(int x)
{
	//pSet[0].limitOn  = !pSet[0].limitOn;
	//Serial.printf("[L %i] ",pSet[0].limitOn);
			  //	Serial.print("#");
	valChanged(VAL_CHGD) ;
	return CALL_NOEX;
}
int toggleTrack(int x)
{
	char valbuf[32];
	mSet.trackOn  = !mSet.trackOn;
	//Serial.printf("[T %i] ",mSet.trackOn);
		//	  	Serial.print("$G");
	valChanged(VAL_CHGD);
	// need to send an "start/end tracking reductions from me" message
	// also don't update any V/I changes since we went off tracking
	if (mSet.trackOn) 
	{
		sprintf(valBuf,":TRAC:REDU %3.6f\n",eTrack);
		//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
		SCPIheartbeatSent = true;
		changedLocal = false;
	}
	else 
	{
		sprintf(valBuf,":TRAC:REDU %3.6f\n",1.0);		
		//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
		SCPIheartbeatSent = true;
	}	
	return CALL_NOEX;
}
int toggleTrackVolts(int x)
{
	//mSet.trackSv  = !mSet.trackSv;
	//Serial.printf("[T %i] ",mSet.trackSv);
			  	//Serial.print("$V");
	valChanged(VAL_CHGD) ;
	return CALL_NOEX;
}
int toggleTrackAmps(int x)
{
	//mSet.trackSa  = !mSet.trackSa;
	//Serial.printf("[T %i] ",mSet.trackSa);
			  	//Serial.print("$A");
	valChanged(VAL_CHGD) ;
	return CALL_NOEX;
}
int toggleWconn(int x){
	myID.autoConnect = !myID.autoConnect;
Serial.printf("Wifi Auto = %i\n",myID.autoConnect);
	if(myID.autoConnect) // restart Wifi
	{
		wifiBegin();
	}
	else
	{
		wifiEnd(false); // leave the wifi adapter on
	}
	valChanged(VAL_CHGD) ;
	return CALL_NOEX;
}

/*globals used by other routines
  highButton - the button currently highlighted on screen
  dirtyScreen - something has changed that requires the entire screen to be re-drawn (e.g. menu change)
  valChanged(VAL_CHGD) - requires settings to be redrawn/ changed in EEPROM / SCPI etc
*/
// process touch screen, switches and encoder
bool firstCall = true;
short procBut = -1, oldButton = -1;
void processTouchSwEnc()
{
  short blockT, lsv = -1, retVal, cmd;
  char lfmt = '\0';
  int cb = CALL_EX;	// default callback result
  bool oskStatus;
  
  newPosition = enc.getPosition(); // encoder change?
    //   Serial.print("a");
  if (ts.touched())       // keep checking until released - then use value.
    { //Serial.print("TS_T");
      p = ts.getPoint(); 
      wastouched = true;    
    } else 
    {
//Serial.print("c");
      if (wastouched) 
      {   
			wastouched = false;
			//Serial.print("1");
			blockT = blockTouched(p); 
			//Serial.print("2");
			//Serial.printf("%i <%s> [%i] (%i, %i) [%i, %i] \n",blockT, (blockT >=0) ? but[blockT].text : "#", currentMenu, p.x, p.y, pxh(p.x), pxv(p.y));
			// keep last touched block until another legal block is touched - ignored for OSK and ScreenCal
			if (blockT >= 0 && !oskActive()) // ignore touches outside buttons
			{	
				//Serial.print("d");
				//Serial.printf("Touch: block = %i [M %i], status %i, %s\n",blockT, currentMenu, callStatus, (but[blockT].callback != NULL) ? "CB" : "No CB" );
				// callbacks may persist for multiple cycles (state-variable) and must be non-blocking
				// not currently in an active callback
				if (callStatus == CALL_IDLE) 
				{	
					highButton = blockT; // persistent record of last menu button pressed
					lsv = but[blockT].sv; 
					lfmt = setVals[lsv].fmt; 
					//Serial.printf(" setting %i, format[%c]\n", lsv, lfmt);	
				
					if(but[blockT].callback != NULL)  //need to process a *new* callback?
					{												
						procBut = blockT;
						firstCall = true;	
						callStatus = CALL_START;
						//Serial.printf("Callback starting\n");					
					}	

					//  OSK needs special initialisation: touched block's related setVal.sv has ASCII ('A' or 'S') format
					if ((lfmt == 'A'|| lfmt == 'S') && !oskOn )
					{
						//Serial.printf("OSK starting on [%s]\n",(char *)setVals[lsv].vp);
						oskStart((char *)setVals[lsv].vp, 130); // nominal ypos, OSK could calc this for bottom part of the screen
					}					
				}
			}
		//	Serial.print("e");
			if (oskActive())
				oskStatus = oskProcess(CALL_CMD_PROC);

			// callback on originally highlighted button (screenCal and OSK) ??? for toggles?
			// use procBut here to maintain thread, despite what's touched on the screen
			if (but[procBut].callback != NULL && callStatus != CALL_IDLE)
			{
				if (firstCall) 
				{
					cmd = CALL_CMD_START;
					firstCall == false;
				}
				else
					cmd = CALL_CMD_PROC;
				
				//Serial.print("C ");
				delay(10);
				cb = but[procBut].callback(cmd); // issue callback
				
				switch (cb)
				{
					case CALL_PROC:
						callStatus = CALL_ACTIVE;
						retVal = procBut; // possibly some post-processing 
					break;
					case CALL_EX:
						callStatus = CALL_IDLE;
						wastouched = false;
						retVal = -1; // possibly some post-processing 
						break;
					case CALL_NOEX: 
						callStatus = CALL_IDLE;
						wastouched = false;
						retVal = procBut; // retain focus on this button
						break;
					case CALL_ERR:
						callStatus = CALL_IDLE;
						wastouched = false;
						return; // it's broken, so just exit						
				}
			} // end callback processing 	
				
		  } // end wasTouched
//	Serial.print("F");
	} // end null processing (not touched now, or before)
//delay(17);Serial.print("G ");
		
	// encoder and buttons 	- independent of screen touch
	if (callStatus == CALL_IDLE)  // not during active callbacks 
	{
//Serial.println("H");delay(1);
		// establish setVal and format for this button
		if (highButton >= 0 && highButton < NUMBUTS)
		{	
			lsv = but[highButton].sv; 	
			if(lsv == NOSETVAL)
				lfmt = 'Z';
			else
				lfmt = setVals[lsv].fmt;  			
		}
		else 
		{
			lsv = -1;
			lfmt ='Z';
		}
				
//Serial.print("I");	  
		if (newPosition != oldPosition) // encoder rotated, update value associated with last block touched
		{
	//Serial.printf("Rx %i, %i", newPosition,oldPosition);
		  updateVal(highButton, butDigit);      
		  oldPosition = newPosition; 
		//  	Serial.print("N");
		  valChanged(VAL_CHGD);		  
		}
		
		if (swPressed && lfmt == 'T') // L or R switch pressed (Toggles)
		{	
	//Serial.print("T ");
			*(uint8_t*)setVals[lsv].vp = (butDir > 0) ? false : true;  	
			//  button colours are handled dynamically in drawIndicators and drawButtons
		//			  	Serial.print("P");
			valChanged(VAL_CHGD);	 				
			swPressed = false;						
		}	  
			//			Serial.print("j");
		// whole screen redraw
		if (dirtyScreen || (highButton != oldButton)) 
		 {   
		 //Serial.printf("1: %i ", lsv);
		 	//Serial.print("R"); delay(1);
		   drawSettings(lsv, butDigit, true);
		   drawButtons(highButton);
		   drawReadings();
		   drawIndicators(currentMenu);
		  // dirtyScreen = false;
		   changedVal_scrn = false;	 	
		   oldButton = highButton; 
		 }
		 //	Serial.print("K");
		 // less radical redraw
		 if (changedVal_scrn)
		 {
			//Serial.printf("W [%i][%i]",changedVal_scrn, true);
			// Serial.print("Q"); delay(1);
			drawButtons(highButton);			  
			drawIndicators(currentMenu);	
	//Serial.printf("2: %i ", lsv);

			drawSettings(lsv, butDigit, (oldButton != highButton)? true : false);
			changedVal_scrn = false;	
//Serial.printf("WX [%i][%i]\n", changedVal_scrn, true);			
		 }
	 } // CALL_IDLE
//Serial.print("M");
}

int blockTouched(TS_Point p)
{
	uint8_t xbut, ybut;
    // which button has been pressed in the current menu?
    for (int i = 0; i< NUMBUTS; i++)
    { 
		  xbut = (((but[i].siz & 0x0f) + 1) * BUTSIZ)/2;
	    ybut = ((((but[i].siz & 0xf0) >> 4) + 1) * BUTSIZ)/2;
			//if((i >= 0 && i <= 6)) Serial.printf("B%i <%s>: x %i, w %i, y %i, h %i\n", i, but[i].text, but[i].xpos, xbut, but[i].ypos, ybut);
      if (	but[i].menu == currentMenu 
            && pxh(p.x) >= but[i].xpos && pxh(p.x) <= (but[i].xpos + xbut) 
            && pxv(p.y) >= but[i].ypos && pxv(p.y) <= (but[i].ypos + ybut)
						&& ((but[i].enabledIf == NULL) || *but[i].enabledIf)
          )
      {           
      	/*  Serial.printf("CM = %i, Touched (%i,%i) [%i, %i] <",currentMenu, p.x, p.y, pxh(p.x),pxv(p.y));
        Serial.print(but[i].text);  
        Serial.print(">, Highlight ");
        Serial.println(highButton);
	*/
        // change menu if required
        if (currentMenu != but[i].nextMenu)
		{
			highButton = 99;  // no highlight in new menu
			dirtyScreen = true;
		}
		currentMenu = but[i].nextMenu; // could be the same as this one        

        return i; // no need to try the rest of the blocks
      }
    }
    return -1;
}
// update the setting value using the (touch button) index to the settings array and the (L&R buttons) digit to change
// encoder position is global
// for simplicity, all arithmetic is done in float: convert *vp -> float -> process -> save as native format
void updateVal(short btn, short digit)
{
  float vTemp;
  char fTemp;
  uint8_t svx;
     
  if (btn > NUMBUTS  || btn < 0) // no button highlighted
    return;

  svx = but[btn].sv;
  fTemp = setVals[svx].fmt;
  //Serial.printf("updateVal: btn %i, digit %i, setting %i, ",btn, digit,svx);
   //Serial.print(fTemp);
   switch (fTemp) 
   {
    case 'T': // Toggle
       vTemp = *(uint8_t*)setVals[svx].vp;
       digit = 0; // true/false always +/- 1
       break;    
     case 'I': //  8 bit Integer 
       vTemp = *(uint8_t*)setVals[svx].vp;
       break;
     case 'B': // bracketed integer
       vTemp = *(uint8_t*)setVals[svx].vp ;
       break;
     case 'F': // float
       vTemp = *(float*)setVals[svx].vp ;
       break;    
  }
//Serial.printf("val %6.4f [%c]",vTemp, fTemp);
  if (newPosition > oldPosition) // increment
  {
   //Serial.print("+");
    //Serial.print(setVals[svx].maxVal);
    //  maximum set value 
    if ((vTemp + pow(10, digit) ) <=  setVals[svx].maxVal)
        vTemp += pow(10, digit);
    else  
        vTemp = setVals[svx].maxVal;
  }
  else // decrement
  {
   // Serial.print("-");
    //  minimum set value      
    if((vTemp - pow(10, digit) ) >=  setVals[svx].minVal)
      vTemp -= pow(10, digit);
    else  
      vTemp = setVals[svx].minVal;
  }
 //Serial.printf("-> %6.4f\n",vTemp);
  //Serial.println(vTemp);
    switch (fTemp) {
    case 'T': // Toggle
     *(uint8_t*)setVals[svx].vp = (uint8_t)vTemp;
     break;    
     case 'I': // 8 bit Integer 
     *(uint8_t*)setVals[svx].vp = (uint8_t)vTemp;
      break;
     case 'B': // bracketed integer
     *(uint8_t*)setVals[svx].vp = (uint8_t)vTemp;
      break;
     case 'F': // float
     *(float*)setVals[svx].vp = vTemp;
      break;    
  }
  valChanged(VAL_CHGD);
  changedLocal = true;
}

void postProcess(void)
{
	// redundant
}
#endif