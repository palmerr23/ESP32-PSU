/* Touch Screen Calibrator
 and ADC Calibrator

Need to define HMAX/VMAX = screen width/height in pixels
These are the touch screen readings corresponding to pixels (0,0) & (HMAX,VMAX)
	int thmin = ;
	int tvmin = ;
	int thmax = ;
	int tvmax = ;
*/
#ifndef MYSCREENCAL_H
#define MYSCREENCAL_H

bool calOn = false;
uint8_t calStage = 0, hitcount = 0;
int scx1 = 0, scy1 = 0, scx2 = 0, scy2 = 0;
float slopex, slopey;
#define SC_HITCOUNT  5
#define SC_DIST	20		// distance of '+' from corners
#define SC_LINELEN 20	// length of '+' bars
#define SC_BG RGB565(0,20,15)
#define SC_BG2 RGB565(31,10,0)
#define SC_HIGH  0xffff

void calStop(void){
	calOn = false;
	dirtyScreen = true;
	//callStatus == CALL_IDLE;
}

int screenCal(int cmd) // cmd is ignored
{
/*
screenCals sc; // make this global when not testing 
*/	calOn = true;
	switch (calStage)
	{
		case 0:	// Starting - instructions and first + to target, move on after 5 hits 
			Serial.println("*** ScreenCal 0");
			// draw screen
			tft.fillRect(0,0, HMAX, VMAX, SC_BG);
			tft.drawLine(SC_DIST - SC_LINELEN/2, SC_DIST , 
					     SC_DIST + SC_LINELEN/2, SC_DIST, SC_HIGH); //"-"
			tft.drawLine(SC_DIST, SC_DIST - SC_LINELEN/2, 
						 SC_DIST, SC_DIST + SC_LINELEN/2, SC_HIGH); //"|"
			tft.setFont(NULL);
			tft.setTextColor(SC_HIGH);
			tft.setTextSize(2);
			tft.setCursor(HMAX/2 - 100, VMAX/2);
			tft.printf("Touch cross %i times", SC_HITCOUNT+1);
			if( hitcount > 0 && hitcount <= SC_HITCOUNT)
			{
				Serial.print("#");
				scx1 += p.x;
				scy1 += p.y;
			}
			hitcount++;
			
			// collect SC_HITCOUNT touches
			if (hitcount > SC_HITCOUNT) // calculate averages
			{
				scx1 /= SC_HITCOUNT;
				scy1 /= SC_HITCOUNT;
				hitcount = 0;
				calStage = 1;
			}			
			return CALL_PROC; // not done
			//break;
			
		case 1: // second + to target
			Serial.println("*** ScreenCal 1");
			tft.fillRect(0,0, HMAX, VMAX, SC_BG2);
			tft.drawLine(HMAX - SC_DIST - SC_LINELEN/2, VMAX - SC_DIST , 
					     HMAX - SC_DIST + SC_LINELEN/2, VMAX - SC_DIST, SC_HIGH); //"-"
			tft.drawLine(HMAX - SC_DIST, VMAX - SC_DIST - SC_LINELEN/2, 
						 HMAX - SC_DIST, VMAX - SC_DIST + SC_LINELEN/2, SC_HIGH); //"|"
			tft.setFont(NULL);
			tft.setTextColor(SC_HIGH);
			tft.setTextSize(2);
			tft.setCursor(HMAX/2 - 100, VMAX/2);
			if( hitcount > 0 && hitcount <= SC_HITCOUNT)
			{
				Serial.print("@");
				scx2 += p.x;
				scy2 += p.y;
			}
			hitcount++;
			tft.printf("Touch cross %i times", SC_HITCOUNT+1);
			// collect SC_HITCOUNT touches
			if (hitcount > SC_HITCOUNT) // calculate averages
			{	scx2 /= SC_HITCOUNT;
				scy2 /= SC_HITCOUNT;
				calStage = 2;				
			}		
			return CALL_PROC; // more processing
			//break;		
			
		case 2: // exit 
			Serial.printf("Screen touch %i, %i | %i, %i\n",scx1,scy1,scx2,scy2);
			// calculate touch unit values for pixels 0,0 and XMAX, YMAX
			slopex = 1.0 * (scx2 - scx1)/ (HMAX - 2 * SC_DIST);
			slopey = 1.0 * (scy2 - scx1)/ (VMAX - 2 * SC_DIST);
			sc.thmin = scx1 - slopex * SC_DIST; // in touch units
			sc.tvmin = scy1 - slopey * SC_DIST;
			
			sc.thmax = scx2 + slopex * SC_DIST;
			sc.tvmax = scy2 + slopey * SC_DIST;
			Serial.printf("Screen cal %i, %i | %i, %i\n",sc.thmin,sc.tvmin,sc.thmax,sc.tvmax);
			// save
			// reset for next use
			hitcount = 0;
			calStage = 0;
			scx1 = 0; scy1 = 0; scx2 = 0; scy2 = 0;
			calStop();
			valChanged(VAL_CHGD);
			return CALL_EX; // done
			break;
	}
	return CALL_ERR; // just in case
}
int tsRotate(int x){ // rotate touch screen 180 degrees
	sc.tsRot = (sc.tsRot + 2 ) % 4;
	valChanged(VAL_CHGD);
	ts.setRotation(sc.tsRot);
	return CALL_EX;
}


// *****************ADC calibration ********************
int calEntry(int dummy)
{
	calOff_I = 0.0; 
	calOff_V = 0.0; 
	return 0;
}
int calSave (int dummy)
{
	float temp;
#define CONSTRAIN (0.05)			// only allow changes of 5% or less	
	if (abs(calOff_V) > SMALL_DIFF)
	{		
		temp = calOff_V * myADC[VOUT].maxVal / myADC[VOUT].curVal; // scale up the current reading * offset to MAX
		Serial.printf("Volt max was: %5.3f, change by %5.3f to %5.3f\n", myADC[VOUT].maxVal, temp, myADC[VOUT].maxVal + temp);
		Serial.printf("Current voltage would be %5.3f\n", myADC[VOUT].curVal + calOff_V);
		if (abs(temp / myADC[VOUT].maxVal) < CONSTRAIN) // don't allow more than a small % change
		{ 		
			myADC[VOUT].maxVal += temp; 
			// make input voltage cal factor the same as the output one.
			myADC[VIN].maxVal = myADC[VOUT].maxVal;
		    // make input voltage cal factor the same as the output one.
			myADC[VIN].maxVal = myADC[VOUT].maxVal;
			Serial.println("V cal done");
		}
		
	}
	if (abs(calOff_I) > SMALL_DIFF)
	{
		temp = calOff_I * myADC[IOUT].maxVal / myADC[IOUT].curVal; 
		Serial.printf("Current max was: %5.3f, change by %5.3f to %5.3f\n", myADC[IOUT].maxVal, temp, myADC[IOUT].maxVal + temp);	
		if (abs(temp / myADC[IOUT].maxVal) < CONSTRAIN) // don't allow more than a small % change
		{
			myADC[IOUT].maxVal += temp; 
			Serial.println("I cal done");		
		}		
	}
	return 0;
}

int exitCal(int x){
// Nothing to do
	return 0;	
}
void currentZeroCal(void)
{
	  if(pSetA.outOn == false)
	  {
		myADC[IOUT].minVolts = myADC[IOUT].curVolts - IOUT_OFFSET;
		//Serial.printf("Current zero = %5.3f\n",  myADC[IOUT].minVolts);
	  }
}

/*
#define CAL_INCR (0.01)
int upCalVolts(int x){
	// calculate the calibration factors and update 
	myADC[VOUT].maxVal += CAL_INCR;
	myADC[VIN].maxVal = myADC[VOUT].maxVal;		// assume VIN has same calibration as VOUT	
	valChanged(VAL_CHGD); 
	//dirtyScreen = true;
	Serial.printf("V+ [%5.3f]\n", myADC[VOUT].maxVal);
}

int upCalAmps(int x){
	myADC[IOUT].maxVal += CAL_INCR;
	valChanged(VAL_CHGD);
	//dirtyScreen = true;
	Serial.printf("I+ [%5.3f]\n", myADC[IOUT].maxVal);
}
int dnCalVolts(int x){
	// calculate the calibration factors and update 
	myADC[VOUT].maxVal -= CAL_INCR;
	myADC[VIN].maxVal = myADC[VOUT].maxVal;		// assume VIN has same calibration as VOUT	
	valChanged(VAL_CHGD);
	//dirtyScreen = true;
	Serial.printf("V- [%5.3f]\n", myADC[VOUT].maxVal);
}
int dnCalAmps(int x){
	myADC[IOUT].maxVal -= CAL_INCR;
	valChanged(VAL_CHGD);
	//dirtyScreen = true;
	Serial.printf("I- [%5.3f]\n", myADC[IOUT].maxVal);
}

*/

#endif