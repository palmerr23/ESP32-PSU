/* On Screen Keyboard (OSK) ADC

Need to define HMAX = screen width in pixels
*/
#ifndef MYOSK_H
#define MYOSK_H

#define KCOLS 14
#define KROWS 4          // buffer 
#define KGRID (HMAX/KCOLS -3) // 320/14 = 22. "-3" = Leave some space for right menu buttons.
#define KLEFT 3 //((HMAX - KGRID * KCOLS)/2) // left offset to center keyboard
#define KROUND 3
#define KCOL 0xfff8
#define KRCOL 0x1111
#define FITS_ON_LINE 22

void oskStart(char * buf, uint16_t ypos);
void oskStop (void);
void drawOSKEditLine(void);
void drawOSK(void);
int oskProcess(int cmd);

struct keybd {
  char c, sc;         // char and shift_char to display/return
  uint8_t row, pos;   // keyboard layout on a grid position
  char act;           // action A = add; B = backspace; S = shift; R = return, X = no action
};
// slightly modified US keyboard (Space in bottom row right)
keybd kb[KCOLS * KROWS] = { // only chars 0x20 [SP] to 0x7e ['}']
  {'`','~',0,0,'A'}, // no tilde in GFX font!
  {'1','!',0,1,'A'},
  {'2','@',0,2,'A'},
  {'3','#',0,3,'A'},
  {'4','$',0,4,'A'},
  {'5','%',0,5,'A'},
  {'6','^',0,6,'A'},
  {'7','&',0,7,'A'},
  {'8','*',0,8,'A'},
  {'9','(',0,9,'A'},
  {'0',')',0,10,'A'},
  {'-','_',0,11,'A'},
  {'=','+',0,12,'A'},
  {0x1b,0x1b,0,13,'B'}, // bksp 0x1b
  
  {'q','Q',1,0,'A'},
  {'w','W',1,1,'A'},
  {'e','E',1,2,'A'},
  {'r','R',1,3,'A'},
  {'t','T',1,4,'A'},
  {'y','Y',1,5,'A'},
  {'u','U',1,6,'A'},
  {'i','I',1,7,'A'},
  {'o','O',1,8,'A'},
  {'p','P',1,9,'A'},
  {'[','{',1,10,'A'},
  {']','}',1,11,'A'},
  {'\\','|',1,12,'A'},
  {' ',' ',1,13,'X'}, // spaces for 'nothing' keys (not printed)
  
  {'a','A',2,0,'A'},
  {'s','S',2,1,'A'},
  {'d','D',2,2,'A'},
  {'f','F',2,3,'A'},
  {'g','G',2,4,'A'},
  {'h','H',2,5,'A'},
  {'j','J',2,6,'A'},
  {'k','K',2,7,'A'},
  {'l','L',2,8,'A'},
  {';',':',2,9,'A'},
  {'\'','"',2,10,'A'},
  {' ',' ',2,11,'X'},
  {' ',' ',2,12,'X'},
  {0x19,0x19,2,13,'R'}, // return 0x19
    
  {0x18,0x18,3,0,'S'},// Shift 0x18
  {'z','Z',3,1,'A'},
  {'x','X',3,2,'A'},
  {'c','C',3,3,'A'},
  {'v','V',3,4,'A'},
  {'b','B',3,5,'A'},
  {'n','N',3,6,'A'},
  {'m','M',3,7,'A'},
  {',','<',3,8,'A'},
  {'.','>',3,9,'A'},
  {'/','?',3,10,'A'},
  {' ',' ',3,11,'P'},	// space
  {0x18,0x18,3,12,'S'}, // Shift 0x18 (sticky)

  {' ',' ',3,13,'X'}  
};
// ypos = top of keyboard in pixels 
// buf = source/result pointer

uint16_t oskY;
char * oskBuf = NULL;
uint16_t oskBCnt = 0;
bool oskShift = false;
bool oskActive(){
	return oskOn;
}

void oskStart(char * buf, uint16_t ypos) 
{	
  if (!buf)
  {
	Serial.println("oskBuf pointer is NULL");
	oskOn = false;   
	return;
  }
  if (oskOn)
		return; // already active
  oskY = ypos;
  oskBuf = buf;   // mainstream code needs to display the updated buffer
  oskBCnt = strlen(buf);
  oskShift = false;  
  oskOn = true;
  
  // display keyboard
  drawOSK();
}
void oskStop (void){
   oskOn = false;   
   // erase OSK on exit
   dirtyScreen = true;
   callStatus == CALL_IDLE;
   tft.fillRect(KLEFT , oskY , KCOLS*KGRID, (KROWS+1)*KGRID, BGCOL);     
}
#define XOFF 4
#define YOFF 2
#ifdef ILI9488
	#define BIGTEXT 3
	#define SMALLTEXT 2
#else
	#define BIGTEXT 23
	#define SMALLTEXT 1
#endif
void drawOSKEditLine(void){
  if (!oskBuf)
  {
	Serial.println("oskBuf is NULL");
	return;
  }
  tft.setFont(NULL); // revert to GLCD font for larger character range
  if(oskBCnt <= FITS_ON_LINE) 
	  tft.setTextSize(BIGTEXT);
  else
	   tft.setTextSize(SMALLTEXT);
//Serial.printf("oskBuf is %i\n", oskBCnt);
  tft.setTextColor(BHIGHCOL);
  tft.fillRect(KLEFT , oskY, KCOLS * KGRID, KGRID, MIDBG);
  tft.setCursor(KLEFT + XOFF , oskY + YOFF);
  tft.print(oskBuf);
  tft.setTextSize(1);
}

void drawOSK(void){
  uint16_t xpos, ypos, yp, i, j, kIndex, kBcol;
  uint16_t  cwid, cht; 
  int16_t  xdum, ydum;
  char ch;
  tft.setFont(NULL); // revert to GLCD font for larger character range
  //tft.setTextSize(2);
  //cwid = 8; cht = 7;
  tft.setTextColor(BHIGHCOL);
 // tft.getTextBounds("A", 0,0, &xdum, &ydum, &cwid, &cht); 
  // clear blocks for string display and keyboard
  //tft.fillRect(KLEFT , oskY, KCOLS*KGRID, KGRID, MIDBG);
  tft.fillRect(KLEFT , oskY + KGRID, KCOLS*KGRID, KROWS*KGRID, BGCOL); 
  //tft.setCursor(KLEFT + XOFF , oskY + YOFF);
  drawOSKEditLine();
  //tft.print(oskBuf);
  for (j=0; j < KROWS ; j++)
  {
    ypos = oskY + (j+1) * KGRID;
    for (i = 0; i < KCOLS; i++)
    {
      xpos = KLEFT + i * KGRID;
      kIndex = i + j * KCOLS;   
      if (kb[kIndex].act != 'X') // ignore 'no action' keys (blank placeholders)
      {   
        // draw outline and character
		if(kb[kIndex].act != 'A' ) // different BGCOL for shift, space, bksp and return
		{
			kBcol = KRCOL;
			tft.fillRoundRect(xpos, ypos, KGRID, KGRID,  KROUND, KRCOL);   //   return button 
		}
		else
			kBcol = KCOL;
        tft.drawRoundRect(xpos, ypos, KGRID, KGRID,  KROUND, KCOL);   //            
        //tft.setCursor(KLEFT + i * KGRID + cwid/2 + 1, oskY + j * KGRID + (KGRID+cht)/2);
        ch = (oskShift)? kb[kIndex].sc : kb[kIndex].c;
        yp = (ch == '_')? ypos - 2 : ypos; // underscore needs a lift

        // revert to default GLCD font for bigger character set
        //tft.drawChar(KLEFT + i* KGRID + cwid/2 , oskY + j * KGRID + (KGRID+cht)/2,  kb[kIndex].c, KCOL, BGCOL, 1);
		#ifdef ILI9488
		   tft.setTextSize(2);
		   tft.drawChar(xpos + XOFF, ypos + YOFF,  ch, KCOL, kBcol);
		#else
           tft.drawChar(xpos + XOFF, ypos + YOFF,  ch, KCOL, kBcol, 2);
	    #endif
        //tft.print((oskShift)? kb[kIndex].sc :kb[kIndex].c);     
        //Serial.print((oskShift)? kb[kIndex].sc : kb[kIndex].c);
      }       
    }
  }  
 //tft.setTextSize(1);
}
// include in touch screen process loop - should over-ride other touch points until 'return' is touched
// all editing occurs at the tail of the string
int oskProcess(int cmd){
  uint16_t xpos, ypos, i, j, chIndex;
  bool lastShift, foundOne = false;
  // start command ignored - oskStart() needs additional parameters, and must be called explicitly.
  if (!oskBuf) //check  that oskStart() has been called correctly 
  { 
	  Serial.println("oskBuf is NULL");
	  oskStop();
	  return CALL_ERR;
  }
  
  if (cmd == CALL_CMD_STOP)  // time to exit
  {
	Serial.println("Ending OSK");
	oskStop();
	return CALL_EX; // code-driven exit
  }
  
  drawOSK();

  lastShift = oskShift;
  // scan matrix to see which which button has been pressed
  for (j=0; j < KROWS ; j++)
  {
    ypos = oskY + (j+1) * KGRID;
    for (i = 0; i < KCOLS; i++)
    { 
      xpos = KLEFT + i * KGRID;
      // 1 pixel buffer around cells
      if (     pxh(p.x) > xpos && pxh(p.x) < (xpos + KGRID) 
            && pxv(p.y) > ypos && pxv(p.y) < (ypos + KGRID)
          )
      {    
		foundOne = true;
        chIndex = i + j * KCOLS;
        //Serial.print("Touched ");
        //Serial.print(kb[chIndex].c); 
        switch (kb[chIndex].act)
        {
         case 'A': // normal char
		 case 'P': // space
            oskBuf[oskBCnt++] = (oskShift)? kb[chIndex].sc : kb[chIndex].c;
            break; 
         case 'B': // backspace		 
            oskBuf[(--oskBCnt > 0) ? oskBCnt : 0] = '\0'; // don't backspace past the start of the string
            break; 
        case 'R': // return - processing below
             break; 
        case 'S': // shift
            oskShift = !oskShift;
            //Serial.print("Shift "); Serial.print(oskShift);
            break; 
        case 'X': // do nothing (empty key)
            break; 
        }         
      }
	  if (foundOne) break;
    }
	if (foundOne) break;
  }
  if (!foundOne) 
	  return CALL_PROC; // still processing
 // Serial.print("O");
  valChanged(VAL_CHGD);  
  
  if(kb[chIndex].act == 'R') 
  {
	oskStop();
	return CALL_EX; // user-driven exit
  }
  else // redraw as required
  {
    if (oskShift != lastShift) // redraw if SHIFT status changed
    {
      drawOSK(); // probably not needed as redrawn every cycle (above)
      oskShift != lastShift;
    }      
    drawOSKEditLine(); // update edited line in OSK (real value screen redraw by core code)
  }
  return CALL_PROC; // still processing
}

#endif
