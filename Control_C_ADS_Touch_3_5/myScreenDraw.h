/* Screen Drawing routines 

Need to define HMAX/VMAX = screen width/height in pixels
SCREENROT - which orientation to use (also used by Touch)
*/
#ifndef MYSCREENDRAW_H
#define MYSCREENDRAW_H

//#include "myScreenDefs.h"


void printfloatX(void * vp, char* v2, const GFXfont *fp,  uint8_t textSize, short pre, short post, short highlight, uint16_t x, short y, char fmt, uint16_t txtcol, uint16_t highcol, uint16_t bgcol);
void drawTerm(uint16_t x0, uint16_t y0, uint16_t diam, uint16_t fcol, uint16_t bcol);
void DrawAngledLine(int x, int y, int x1, int y1, int size, int color);
void screenBegin(void)
{
	tft.begin();
	tft.fillScreen(BGCOL);
	tft.setRotation(SCREENROT);
	// ili9341Diag();
	ts.begin();
	ts.setRotation(TOUCHROT); // assume they're the same. Fixed later if required.
}

// calculate the correct mode for toggles
short tMode(short sv){
	
	// Limit
	if (sv == LBUT)
	{
		/*
	  if (pSet[0].limitOn)
	  {
		if(limiting)
			return BBACT;
		else
			return BBINACT;
	  }
	  else // disabled	
*/		
		  return BBOFF;
	}
	// Track
	if (sv == TBUT)
	{
	  if (mSet.trackOn)
	  {
		if(eTrack <= 1.0 - SMALL_DIFF)
			return BBACT;
		else
			return BBINACT;
	  }
	  else // disabled	  
		 return BBOFF;
	}
	// WiFi
	if (sv == WBUT)
	{
	  if (myID.autoConnect)
	  {
		if(WifiConnected)
		{
			//Serial.println("WBUT Act");
			return WIFIACT;
		}
		else
		{
			//Serial.println("WBUT InAct");
			return WIFIINACT;
		}
	  }
	  else // disabled	  
	  {
		   //Serial.println("WBUT Off");
		   return WIFIOFF;
	  }
	}
	
	if (sv == GSVBUT) // group track volts
	{
		if(mSet.trackSv)
			return BBINACT;
		else
			return BBOFF;
	}
	if (sv == GSABUT) // group track current
	{
		/*
		if(mSet.trackSa)
			return BBINACT;
		else
			*/
			return BBOFF;
	 }
	return -1; // sv not found
}


// colour for button and indicator background - toggles
uint16_t tColour(short sv, char mode){
	// indicators as buttons except "off" is screen background colour
	short tm;
	tm = tMode(sv);
	if (tm <= 0)
	{ 
	  if (mode == 'I')
	    return BGCOL;
	  else
		return BBXHIGH;
	}
	return tColours[tm];
}	
#define ABUT (BUTSIZ/2)
#define ABUTR (BUTROUND/2)
#define ABUTX 2
#define ABUTOFF 5
void drawIndicators(short aMenu)
{
//return; /**************** DISABLED */
	int8_t tm; 
	uint16_t xcol, xcolb;
	if (oskOn || calOn)
		return;
	tft.setFont(&FONT0);
	// every screen indicators
	
	// wireless connected
	//tft.fillRoundRect(VALX+40, 0, ABUT, ABUT,  ABUTR, (WifiConnected)?IENABLE:ITRIG); 
	tft.setCursor(sx(READX), sy(TOPLEGS));
	tft.setTextColor((WifiConnected)?IND_ON:IND_OFFC);
	tft.print("W");
	
	// save to EE pending
	if (needToSaveEE) // save to EE indicator
		tft.setTextColor(MY_YELLOW);
	else
		tft.setTextColor(BGCOL);
		
	tft.setCursor(sx(READX + 15), sy(15));
	tft.print("[E]");

	tft.setTextColor(BHIGHCOL);

  switch (aMenu)
  {
    case 0:		// main menu
		// Limit indicator
		/*
		tm = tMode(LBUT);
		xcol = tColour(LBUT, 'I');
		if (tm == BBACT)
			xcol = (myBlink)? xcol : BGCOL;			
		tft.fillRoundRect(sx(ABUTX), sy(2* BVSPACE) - ABUT, ABUT, ABUT,  ABUTR, xcol ); //BGCOL/
		if(tm > 0) // active or inactive
		{
		  	tft.setCursor(sx(ABUTX + ABUTOFF), sy(2*BVSPACE - ABUTOFF));
			tft.print("L");
		}
*/
		// Track indicator - flashing
		/*
		tm = tMode(TBUT) ;
		xcol = tColour(TBUT, 'I');
		if (tm == BBACT)
			xcol = (myBlink)? xcol : BGCOL;	
	    tft.fillRoundRect(sx(ABUTX), sy(4.5*BVSPACE) - ABUT, ABUT, ABUT,  ABUTR, xcol); 
		if(tm > 0)
		{
			tft.setCursor(sx(ABUTX + ABUTOFF), sy(4.5*BVSPACE - ABUTOFF));
			tft.print("T");
		}		
		*/
		// fan on
		xcol =(fanIsOn) ? IND_ON : BGCOL;	
		tft.fillRoundRect(sx(VALX+20), sy(6), sx(8), sx(8),  3, xcol); 
		// mode display
		#define MOD_X0	90
		#define MOD_Y0	275		
		#define MOD_DOUT	10 // diam outer: even number
		#define MOD_DIN	4	// diam inner: even number
		#define MOD_LINE	8	// line width:  even number
		#define MOD_XGAP  60
		#define MOD_YGAP  30
		#define MOD_TEXTX	(MOD_X0+MOD_XGAP+MOD_DOUT)
		#define MOD_TEXTY	(MOD_Y0+MOD_YGAP/2)
		if(chanBfound)
		{
			xcol  = (mSet.trackMode == TRA_SER) ? MY_DARKRED : MY_RED; // dim for Series mode
			xcolb = (mSet.trackMode == TRA_SER) ? GREY_DK : MY_WHITE; // dim for Series mode			
				drawTerm(MOD_X0,MOD_Y0, MOD_DOUT, xcol, xcolb);
			
			drawTerm(MOD_X0+MOD_XGAP,MOD_Y0, MOD_DOUT, BLUE, MY_WHITE);
			drawTerm(MOD_X0,MOD_Y0+MOD_YGAP, MOD_DOUT,  GREY_DK, MY_WHITE);
			
			xcol  = (mSet.trackMode == TRA_SER) ? BLACK : GREY_DK; // dim for Series mode
			xcolb = (mSet.trackMode == TRA_SER) ? GREY_DK : MY_WHITE; // dim for Series mode	
				drawTerm(MOD_X0+MOD_XGAP,MOD_Y0+MOD_YGAP, MOD_DOUT, xcol, xcolb);
			
			 tft.setFont(&FONT2);// set font and text size first, or cursor baseline will be wrong 
			 tft.setTextSize(1);	
			 tft.setCursor(sx(MOD_TEXTX+10), sy(MOD_TEXTY+10));
			 tft.setTextColor(MY_WHITE);
			 tft.print(trackLegend[mSet.trackMode]);
			//tft.drawLine(MOD_X0+MOD_DOUT, MOD_Y0+MOD_DOUT, MOD_X0+MOD_XGAP-MOD_DOUT, MOD_Y0-MOD_YGAP-MOD_DOUT, MY_GREEN);
			// Serial: slant
			if(mSet.trackMode == TRA_SER || mSet.trackMode == TRA_LINK)
			{
				DrawAngledLine(sx(MOD_X0+MOD_DOUT), sy(MOD_Y0+0.66*MOD_DOUT), sx(MOD_X0+MOD_XGAP-MOD_DOUT), sy(MOD_Y0+MOD_YGAP-0.66*MOD_DOUT), sx(MOD_LINE), MY_GREEN);
			
			}
			//never: vert
			//DrawAngledLine(sx(MOD_X0),sy(MOD_Y0+MOD_DOUT), sx(MOD_X0), sy(MOD_Y0+MOD_YGAP-MOD_DOUT), sx(MOD_LINE), MY_GREEN);
			//DrawAngledLine(sx(MOD_XGAP+MOD_X0),sy(MOD_Y0+MOD_DOUT), sx(MOD_XGAP+MOD_X0), sy(MOD_Y0+MOD_YGAP-MOD_DOUT), sx(MOD_LINE), MY_GREEN);
			//parallel: horz
				if(mSet.trackMode == TRA_PLL)
				{
					DrawAngledLine(sx(MOD_X0+MOD_DOUT), sy(MOD_Y0), sx(MOD_X0+MOD_XGAP-MOD_DOUT), sy(MOD_Y0), sx(MOD_LINE), MY_GREEN);
					DrawAngledLine(sx(MOD_X0+MOD_DOUT), sy(MOD_Y0+MOD_YGAP), sx(MOD_X0+MOD_XGAP-MOD_DOUT), sy(MOD_Y0+MOD_YGAP), sx(MOD_LINE), MY_GREEN);
				}
				// linked
				if(mSet.trackMode == 999)
					DrawAngledLine(sx(MOD_X0+MOD_DOUT), sy(MOD_Y0+MOD_YGAP/2), sx(MOD_X0+MOD_XGAP-MOD_DOUT), sy(MOD_Y0+MOD_YGAP/2), sx(MOD_LINE/2), MY_DARKRED);
			// back slant
			//DrawAngledLine(sx(MOD_X0+MOD_XGAP-MOD_DOUT) , sy(MOD_Y0+MOD_DOUT/2),  sx(MOD_X0+MOD_DOUT/2), sy(MOD_Y0+MOD_YGAP-MOD_DOUT), sx(MOD_LINE), MY_GREEN);
			//Serial.println();
		}
		break;
  
    case 1: // comms
		break;
  
    case 2: // track
		break;
	
	  case 3: //cal
		break;
  }
}
void drawTerm(uint16_t x0, uint16_t y0, uint16_t diam, uint16_t fcol, uint16_t bcol)
{	
	tft.fillCircle(sx(x0),sy(y0), sx(diam), fcol);
	tft.fillCircle(sx(x0),sy(y0), sx(diam/2), bcol);
	tft.drawCircle(sx(x0),sy(y0), sx(diam), bcol);
}

void DrawAngledLine(int x, int y, int x1, int y1, int size, int color) {
  float dx = (size / 2.0) * (y - y1) / sqrt(sq(x - x1) + sq(y - y1));
  float dy = (size / 2.0) * (x - x1) / sqrt(sq(x - x1) + sq(y - y1));
  tft.fillTriangle(x + dx, y - dy, x - dx,  y + dy,  x1 + dx, y1 - dy, color);
  tft.fillTriangle(x - dx, y + dy, x1 - dx, y1 + dy, x1 + dx, y1 - dy, color);
}
// draw legends on the menu screen
void drawLegends(short cMenu){
  short i = 0;
  uint16_t tempClr;
  if (oskOn || calOn)
		return;
  //for (short i =0; i< NUMLEGS; i++)
  while (leg[i].menu >= 0) // terminating item has -1 flag
  {
    if (leg[i].menu == cMenu)	  
	{
  	   // check for conditional display - print in Background colour to erase
	   if(leg[i].displayIf == NULL || *(leg[i].displayIf)) 
		 tempClr = leg[i].clr;
	   else
		 tempClr = BGCOL;

	   tft.setFont(leg[i].font);// set font and text size first, or cursor baseline will be wrong 
	   tft.setTextSize(leg[i].textSize);	
	   tft.setCursor(leg[i].xpos, leg[i].ypos);
	   tft.setTextColor(tempClr);
	   tft.print(leg[i].text);
	}    
    i++;
  }
}

// bits 0..3 = height (1/2 .. 8); bits 4..7 width (same fmt). Bit 8 = visible. Bit 9 enabled.
void drawButtons(short highlight)   
{
  uint16_t  cwid, cht; 
  int16_t  xdum, ydum, xht, yht, brnd;
  uint16_t clr, col;
  
   if (oskOn || calOn)
		return;
	
   for (int i = 0; i<NUMBUTS; i++)
     if((but[i].menu == currentMenu) && (but[i].siz & DRAW_BUT) && ((but[i].enabledIf == NULL) || *but[i].enabledIf))
     {                
       xht = (((but[i].siz & 0x0f) + 1) * BUTSIZ)/2; // bitmapped size variable - only two sizes supported
       yht = ((((but[i].siz & 0xf0) >> 4) + 1) * BUTSIZ)/2;
       brnd = (!(but[i].siz & 0xf) + 1) * BUTROUND/2; // either dimension is small: less rounding
        
        if(i == highlight)
        {
          tft.setTextColor(BHIGHCOL);
          col = BBORDHIGH;
		  clr = but[i].selColour;              
        }
        else
			  {
          tft.setTextColor(BTEXTCOL);
          col = BBORD;
					clr = but[i].unselColour;
        }
		
				// special colour treatment for Toggle buttons
				short tsv = but[i].sv;	
				if (tsv != NOSETVAL) // don't combine ifs - could cause pointer error
					if(setVals[tsv].fmt == 'T')
						clr = tColour(tsv, 'B');

				
				//if(oldHigh != highlight) // context sensitive buttons get redrawn each time
				tft.fillRoundRect(but[i].xpos, but[i].ypos, xht, yht,  brnd, clr); 
						
				if(but[i].border)
          for (short j = 0; j < BUTEDGETHICK; j++) // extra thickness on outside of basic outline
            tft.drawRoundRect(but[i].xpos-j,but[i].ypos-j, xht+2*j, yht+2*j,  brnd, col);      
        switch (strlen(but[i].text))
        {
          case 1:
		  case 0:
            tft.setFont(&FONT3);
            break;
          case 2:
            tft.setFont(&FONT2);
            break;
          default:
            tft.setFont(&FONT1);
        }           
        tft.getTextBounds(but[i].text, 0,0, &xdum, &ydum, &cwid, &cht); 
        tft.setCursor(but[i].xpos + (xht-cwid)/2, but[i].ypos + (yht + cht)/2);
        tft.print(but[i].text);     
     } // end if, for
     oldHigh = highlight;
}

short readCntr = 0;
void drawReadings(void) {
  // non blocking code - one reading per call
  uint16_t tempClr;
  short i = 0;
	long mstart = millis();
  if (oskOn || calOn)
		return;
	
  while(readVals[readCntr].menu != currentMenu && readVals[readCntr].vp) // skip ones not on this menu
	{
		readCntr++;
		//Serial.println("^");
	}
	
	if (!readVals[readCntr].vp)
	  readCntr = 0;	// NULL at list end 
	
 // for (short i =0; i< NUMLABELS; i++)
  //{
		
    mstart = millis();
  if(readVals[readCntr].menu == currentMenu &&(readVals[readCntr].displayIf == NULL || *(readVals[readCntr].displayIf)))
		{
				tempClr = readVals[readCntr].clr;
			// check for conditional display - print in Background colour to erase
			/*
			if() 
				tempClr = readVals[readCntr].clr;
			else
				tempClr = readVals[readCntr].bclr;		
			*/

			printfloatX(readVals[readCntr].vp, readVals[readCntr].units, readVals[readCntr].font, readVals[readCntr].textSize, readVals[readCntr].pre, readVals[readCntr].post, butDigit, readVals[readCntr].xpos,  readVals[readCntr].ypos,  readVals[readCntr].fmt, tempClr, tempClr, readVals[readCntr].bclr);
			//Serial.println("$");
		}
		//Serial.printf("[Q %i], ", millis() - mstart);
    readCntr++;
  //}
  /*********** debug
 */ 
#ifdef CDEBUG2
  uint8_t tempx = pCont[0].DPOTsetting;
  printfloatX((void *)&tempx, "", &FONT1, 1, 3,0,0,5, 160, 'I', TEXTCOL, TEXTCOL, BGCOL);
  float tempy = pCont[0].DACsetting;
  printfloatX((void *)&tempy, "", &FONT1, 1, 6,0,0,80, 160, 'F', TEXTCOL, TEXTCOL, BGCOL);
#endif

}

// selected is the index to setVals[]
// highlight digit for editing
void drawSettings(short selected, short highDigit, bool all) {
  short i = 0;
  if (oskOn || calOn)
		return;
	
  changedVal_scrn = false;	
  //Serial.printf("Sets sel = %i, hd = %i, all = %i\n", selected, highDigit, all); delay(20);
  while (setVals[i].vp)
  {
    if(setVals[i].menu == currentMenu && (setVals[i].displayIf == NULL || *setVals[i].displayIf)) 
    {
      if (i == selected)
			{
        //Serial.print(" Sel ");
        printfloatX(setVals[i].vp, setVals[i].units, setVals[i].font, setVals[i].textSize, setVals[i].pre, setVals[i].post, highDigit, setVals[i].xpos,  setVals[i].ypos,  setVals[i].fmt, setVals[i].clr, HIGHLIGHTCOL, BGHIGH);
      }
      else // don't always redraw out of focus settings
        if (all)
          printfloatX(setVals[i].vp, setVals[i].units, setVals[i].font, setVals[i].textSize, setVals[i].pre, setVals[i].post, highDigit, setVals[i].xpos,  setVals[i].ypos, setVals[i].fmt, setVals[i].clr, setVals[i].clr, setVals[i].bclr);
    }
    i++;
  }
}

void redrawScreen(void){
  short lsv; 
  
  if (oskOn || calOn)
		return;
  
  tft.setRotation(SCREENROT);
  tft.fillScreen(BGCOL);  
	if(currentMenu == 0) // main screen blocks
	{
		tft.fillRoundRect(sx(BBXSTART), sy(BBYSTART), sx(BBXWID), sy(BBYHT),  sx(BBROUND), BGCOLA); 
		tft.drawRoundRect(sx(BBXSTART), sy(BBYSTART), sx(BBXWID), sy(BBYHT),  sx(BBROUND), RECTOUT); 
		if(chanBfound)
		{
			tft.fillRoundRect(sx(BBXSTART), sy(BBYSTART + BBYHT + BBYGAP), sx(BBXWID), sy(BBYHT),  sx(BBROUND), BGCOLB); 
			tft.fillRoundRect(sx(BBXSTART), sy(BBYSTART + 2 * (BBYHT + BBYGAP)), sx(BBXWID), sy(BBYHT2),  sx(BBROUND), BGCOLC); 
			tft.drawRoundRect(sx(BBXSTART), sy(BBYSTART + BBYHT + BBYGAP), sx(BBXWID), sy(BBYHT),  sx(BBROUND), RECTOUT); 
			tft.drawRoundRect(sx(BBXSTART), sy(BBYSTART + 2 * (BBYHT + BBYGAP)), sx(BBXWID), sy(BBYHT2),  sx(BBROUND), RECTOUT); 
		}
	}
  tft.setTextColor(TEXTCOL);
  drawLegends(currentMenu);

  if(highButton < 0 || highButton > NUMBUTS)
	  lsv = -1;
  else
	  lsv = but[highButton].sv;
//Serial.printf("3: %i ", lsv);
//Serial.print("D");
  drawSettings(lsv, butDigit, true); 
  drawButtons(highButton);
  // readings and indicators redrawn each cycle in main code

  // bits and pieces of screen drawing
  switch (currentMenu)
  {
    case 0:
		/*
      tft.setFont(&FONT1);
      tft.setCursor(sx(200), sy(235));
      tft.print(myID.instName);
      tft.drawFastVLine(sx(175), sy(0),  sy(210), CYAN_M); // vertical line between readings and settings
			*/
      break;
  
    case 1:
	  tft.drawFastHLine(sx(2), sy(180),  sx(260), CYAN_M); // horz line separating panels
      break;
  
    case 2: 
      break;
	  
	case 3: 
	 // tft.drawFastHLine(LEG2X, TOPLINE+1*BVSPACE2+BVSPACE2/4,  HMAX -  2 * LEG2X, MY_RED);
	//  tft.drawFastHLine(LEG2X, TOPLINE+3*BVSPACE2+BVSPACE2/4,  HMAX -  2 * LEG2X, MY_RED);
	  // tft.drawLine(LEG2X, TOPLINE+1*BVSPACE2+BVSPACE2/4, HMAX - LEG2X, TOPLINE+1*BVSPACE2+BVSPACE2/4,MY_RED);
	  //tft.drawLine(LEG2X, TOPLINE+3*BVSPACE2+BVSPACE2/4, HMAX - LEG2X, TOPLINE+3*BVSPACE2+BVSPACE2/4,MY_RED);
	//  tft.drawFastVLine(CAL2X-5, 0,  TOPLINE+5*BVSPACE2, MY_RED);
	  //tft.drawLine(CAL2X , 0, CAL2X , TOPLINE+5*BVSPACE2,MY_RED);
      break;
  }
  dirtyScreen = false;
  changedVal_scrn = false;	
}

#define BORDER_HL 3 // extra space beyond char footprint of highlight
// Fancy print - with highlighting for currently editing value.
// Assumes fixed width font, of width "N".
// pre == digits before "."; post == after; highlight [pre .. -post]
// prints leading spaces to pad to desired size (align decimals)
// fmt: 'F' print as float; 'I' - Int (decimal justified); 'P' - float as percentage, 'B' - bracketed Int; 'T' = On/off (1/0; T > 0.1); 'S' = String; 'A' = safe string (no unprintable chars)
// v2 is a "units" string
void printfloatX(void * vp, char* v2, const GFXfont *fp,  uint8_t textSize, short pre, short post, short highlight, uint16_t x, short y, char fmt, uint16_t txtcol, uint16_t highcol, uint16_t bgcol){
  char ch, buf[128];
  uint16_t xcursor;
  uint16_t  cwid, cht; 
  int16_t  xdum = 100, ydum = 100;
  short digit, powVal;
  float val, pownum, tempVal;
  uint8_t cCount = 0;
  short i;
  xcursor = x;
  powVal = pre - 1;
  bool printedDot = false;
  bool leadZero = false; // true == zeros, false == spaces. Should be a parameter
    //Serial.print("H = ");Serial.println(highlight);
  // find the size of the "en" character block - assumes mono-spaced font
  tft.setFont(fp);
  tft.setTextSize(textSize);
  if (fp) // GFX font
  {
    tft.getTextBounds("MMMMMMMMMM", 0,0, &xdum, &ydum, &cwid, &cht); // only use cwid, cht
    cwid = cwid / 10 + 1; // allow for the additional space between characters
  }
  else // GLCD font
  {
    cwid = 5 * textSize; // 8 or 7?
    cht = 8 * textSize;
  }
  // assumes  fmt in "%x.yf"

//Serial.print("Decimal ");Serial.println(decimal);
  // shift to array index value and skip "."
  if (highlight <= pre)
    highlight--;
    
  // wipe out previous text: n characters + decimal (BG highlight for value being edited)
  //tft.fillRect(x - BORDER_HL, y - cht - BORDER_HL , (pre+post+2)* cwid + BORDER_HL * 2, cht + BORDER_HL * 2, bgcol ); 
  
  tft.setCursor(x, y);
  // ignore decimals for anything but Float format
  short nChars = (fmt == 'F')?(pre + post) : pre;
     //Serial.print(fmt);Serial.print(" nChars ");Serial.println(nChars);
  
  switch (fmt){
    case 'T' : // Toggle
      tft.fillRect(x  + cwid * cCount, y - cht - BORDER_HL , cwid*3 , cht + BORDER_HL * 2, bgcol); //bgcol
	  tft.setTextColor(txtcol);
      tft.print((*(uint8_t*)vp > 0)? "On " : "Off");
      return;
	case 'Q' :  // boolean print the first char in v2 string for T second for F
	  i = (*(bool*)vp) ? 0 : 1;
//	  Serial.printf("X: %i %i %i %i %i\n", i, mSet.trackSv, mSet.trackSa, vTrack, cTrack);
      tft.setTextColor(txtcol);
	  tft.print((char*)(v2 + i));
      tft.print(" "); // rubbish erase!!!
      return;
   // use SAFE 'A' format for all string variables
   case 'A' : // SAFE string text (may have characters outside 0x20 .. 0x7e range)
      strcpy(buf, (char*)vp); // substitute unprintable chars
       //Serial.println(buf);
      for (i = 0; i < strlen(buf); i++)
        if (buf[i] < 0x20 || buf[i] > 0x7d)
          buf[i] = '*';
      //Serial.println(buf);
      //delay(500);
	  tft.setTextColor(txtcol);
      tft.print(buf);
      tft.print(" "); // rubbish erase!!!
      return;
    case 'S' :  // unvalidated string - really only for const strings
      tft.setTextColor(txtcol);
	  tft.print((char*)vp);
      tft.print(" "); // rubbish erase!!!
      return;
    case 'B': // Bracketed 
      tft.setTextColor(txtcol);
	  tft.print("[");
      cCount++;
    case 'I': // 8-bit Integer: convert value to float for display
      val = *(uint8_t*)vp;      
      break;
    case 'F': // Float
      val = *(float*)vp;
	 // Serial.printf("PFX: %6.4f [round %i]\n", val, post);
	  val = myRound(val, post); // round to appropriate number of digits for display (stop the 2.99 .. 3.00 flashing issue	  
      break;
    case 'P': // Float as percentage
      val = *(float*)vp * 100;
	  //Serial.printf(" %5.1f%\n", val);
      break;
  }

  if (val < 0) // sign
  {
	  val = -val;
	  tft.fillRect(x  + cwid * cCount, y - cht - BORDER_HL , cwid , cht + BORDER_HL * 2, bgcol); //erase extra block
	  tft.setTextColor(txtcol);
      tft.print("-");
      cCount++;
	  nChars--; // reduce the post decimal width by 1
  }
  
//if(numtest)  Serial.printf("\nPFX %5.3f \n", val);
	// print the value one character at a time, highlight digit if indicated
  for (short i = 0; i < nChars ; i++)
  {
	// ************* Because this takes a long time, check for interrupt post-processing between characters ****************
	adsProcess();
	
	// y is baseline, erase block as each char is redrawn
    if(powVal == -1) //decimal point precedes numeral
    {
      tft.setTextColor(txtcol);
	  tft.fillRect(x  + cwid * cCount, y - cht - BORDER_HL , cwid , cht + BORDER_HL * 2, bgcol); //erase extra block
      tft.print(".");
      cCount++;
      printedDot = true; // no spaces after decimal
    }
	
	tft.fillRect(x  + cwid * cCount, y - cht - BORDER_HL , cwid , cht + BORDER_HL * 2, bgcol); // erase block
	
    if (powVal == highlight+1) // highlighted digit
      tft.setTextColor(highcol);
    else
      tft.setTextColor(txtcol);

    // from most signif digit; extract the digit; subtract its value from the number (i.e. calc remainder)
    pownum = pow(10, powVal);
//if(numtest) Serial.printf("PN %6.3f | ", pownum);
    tempVal = val/pownum; // myRound(pownum, -powVal);
//if(numtest) Serial.printf("TV %8.5f | ", tempVal);
//if(numtest && powVal == -post) Serial.print("*");
	//if(i == nChars -1)
	   //digit =  myRound(tempVal + SMALL_DIFF,1); //  truncate to single digit integer (round ***final*** digit to one decimal place to avoid trunc issue)
    //else
	digit = tempVal + SMALL_DIFF;  // float to integer truncate, avoid rounding errors 
    val = val - digit * pownum;    
//if(numtest) Serial.printf("D %i | RV %6.3f\n", digit, val);
  
    // print the digit, or a leading space
    itoa(digit, buf, 10);
    // print leading spaces when leading zeros are not required.
    // leading zeros for Bracketed format 
    if(digit == 0 && !leadZero && !printedDot && fmt != 'B')
    {
       if (powVal == 0) // single zero before decimal
	   {
        tft.print(buf);
		//if(numtest) Serial.printf("[%s]", buf);
	   }
       else
	   {
		//   if(numtest) Serial.print("[ ]");
        tft.print(" ");
	   }
    }
    else    
    {
      tft.print(buf);
	  //if(numtest) Serial.printf("[%s]", buf);
      printedDot = true;
    }
    powVal--;
    cCount++;
  } // end character loop
  tft.print(v2);
  if (fmt == 'P')
  {
    tft.print("%");
  }

  if (fmt == 'B')
    tft.print("]");
}  

#define HLEFT_E (HMAX/10)
#define WIDTH_E (HMAX*4/5)
#define VTOP_E (VMAX/10)
#define HEIGHT_E (VMAX*4/5)
#define LINESP_E 20
#define TEXTSIZ_E 2
#define BORDER_E 2
#define MARGIN_E 10
#define ROUND_E 10
// minimal reliance on fancy fonts, etc
#include "platylogo.h"
void screenError(char * message, uint16_t bgcol, uint16_t onTime, bool logo)
{
int i, ycursr = VTOP_E + LINESP_E;
     tft.setRotation(SCREENROT);
	 tft.fillRoundRect(HLEFT_E, VTOP_E, WIDTH_E, HEIGHT_E, ROUND_E, bgcol);
	 for(i = 0; i < BORDER_E; i++)
		tft.drawRoundRect(HLEFT_E + i, VTOP_E + i, WIDTH_E - 2 * i, HEIGHT_E - 2 * i, ROUND_E, HIGHLIGHTCOL);
	 tft.setTextColor(HIGHLIGHTCOL);
	 tft.setFont(NULL); // turn off fancy fonts
	 tft.setTextSize(TEXTSIZ_E);
	 tft.setCursor(HLEFT_E + MARGIN_E, ycursr );
	 for (i = 0; i < strlen(message); i++)
	 {
		 if (message[i] == '\n' || message[i] == '\r' )
		 { 
			ycursr += LINESP_E;
			tft.setCursor(HLEFT_E + MARGIN_E, ycursr);
		 } else
			 tft.print(message[i]);
	 }
	 if(logo) // 87w x 64h pixels
#ifdef ILI9488 // needs FG colour as well as BG
		tft.drawBitmap(HMAX/2-87/2, VTOP_E + HEIGHT_E - 64 - MARGIN_E, pLogo, 87, 64, WHITE, BGCOL);
#else
		tft.drawBitmap(HMAX/2-87/2, VTOP_E + HEIGHT_E - 64 - MARGIN_E, pLogo, 87, 64, BGCOL);
#endif
		
	 for (i = 0; i < onTime * 10; i++) // long delay, so allow at least WiFi processing to proceed
	 {
		delay(100); 
		yield();
	 }
	 dirtyScreen = true;
}
void splashScreen(void)
{
	char buf[256];
	sprintf(buf, "\n%s\nHardware version %i\nSoftware version %i\n", MODEL, HARD_VERSION, SOFT_VERSION);
	screenError(buf, ERR_BG_A ,5, true);
}
#ifdef ILI9341
void ili9341Diag(void)
{
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
} 
#endif
#endif