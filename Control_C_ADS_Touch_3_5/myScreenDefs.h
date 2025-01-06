/* Screen and touch defs 
*/
#ifndef  MYSCREENDEFS_H
#define  MYSCREENDEFS_H

#include "SPI.h"
#include "Adafruit_GFX.h"

#include <XPT2046_Touchscreen.h>
#include "myColourFont.h"

// all dimensions assume smaller display (320 x 240) 
#ifdef ILI9341
#define sx(x)	((short)(x))	// no scaling
#define sy(x)	((short)(x))	// no scaling
#include "Adafruit_ILI9341.h"
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

#ifdef ILI9488
// see also  myInst.h
#define sx(x)	((short)(1.333 * (x)))	  // 320/240
#define sy(x)	((short)(1.5 * (x)))  // scale by 480/320 for bigger display
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_CLK /* SCK */, TFT_MOSI /* MOSI */, -1 /* MISO */, VSPI /* spi_num */);
Arduino_ILI9488_18bit tft =  Arduino_ILI9488_18bit(bus, TFT_RST, 0, false /* IPS */);
#endif

// screen calibration 
struct screenCals {
  int thmin; // tft.pixel (0,0)
  int tvmin;
  int thmax;// tft.pixel (HMAX,VMAX)
  int tvmax;
  int tsRot;	// screen rotation - as some touch screens are rotated 180%
} ;
screenCals sc = {400, 300, 3900, 3700, SCREENROT}; // useful starting values, overwritten by screen cal and EEPROM restore.

// touch units to screen pixel conversions, with calibration
int16_t pxh (int16_t px){ 
  int x = ((px-sc.thmin)*HMAX)/(sc.thmax-sc.thmin);
  if (x < 0) return 0;
  if (x > HMAX) return HMAX;
  return x;
}
int16_t pxv (int16_t py){ 
    int y = ((py-sc.thmin)*VMAX)/(sc.tvmax-sc.tvmin);
  if (y < 0) return 0;
  if (y > VMAX) return VMAX;
  return y;  
}

XPT2046_Touchscreen ts(TFT_TOUCH_CS);
TS_Point p;
//XPT2046_Touchscreen ts(TFT_TOUCH_CS, TFT_TOUCH_IRQ);
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define BVSPACE 50	// determines BUTSIZ
#define BVSPACE2 35
#define BVSPACEX BVSPACE

// main screen values
#define RVX 8
#define RIX 120
#define RPX 80
#define F2F3OFF 13


#define LEGX 22
#define LEG2X 8
#define CIRCX 10
#define VALX 100
#define VAL2X 60
#define CALX 80
#define CAL2X 160
#define CALREADX 180
#define READX 135
#define TOPLINE 70
#define TOPLEG 17 // big enough for a FONT2 character
#define TOPLEGS 15 // big enough for a FONT1 character

#define IPSPACE 55
#define IPDOT 40
#define ROUNDERR 0.01    // float rounding may stop hi/low set values being attained.
#define UNITLEN 6

// big screen blocks
#define BBXSTART 5
#define BBXWID (HXMAX - 2* BBXSTART) // always in 
#define BBYSTART 40
#define BBYHT	(BVSPACE2 *3)

#define BBYGAP 5
#define BBROUND	6
#define BBYHT2 (VYMAX - 2 *(BBYHT + BBYGAP) - BBYSTART) // rest of page after 2 panels
#define K1			10

// turn these into a struct, include format string?
struct  valFmt {
 void * vp;            // pointer to value - type is determined by fmt
 const  GFXfont *font;  // font - NULL for GLCD font (wider character set)
 uint8_t textSize;     // textSize
 float minVal, maxVal; // limits for settings; hi-low error points for readings (float is OK for most things)
 short xpos, ypos;     // screen location
 char  fmt;             // format and value type - see printfloatX() - 'T' gets special treatment when touched
 char  units[UNITLEN]; // units to print on screen
 uint16_t clr;        // colour of text
 uint16_t bclr;        // colour of background
 uint8_t menu;         // which menu are these in?
 uint8_t pre, post;    // decimals before and after the point (post is ignored for fmt != 'F')
 bool * displayIf;		 // only display if TRUE or NULL
 };
 
// SETTINGS VARIABLES 
// current set/read vals. SET values will be overwritten on start-up from saved EEPROM values
bool dummyB;
uint8_t dummyU;
float dummyF;
#define vSettingA 	pSet[0].voltage
#define iSettingA 	pSet[0].current
#define vSettingB 	pSet[1].voltage
#define iSettingB 	pSet[1].current
#define iSLimit 	dummyB
#define iSTrack 	mSet.trackOn
#define iRLimit 	eTrack
#define gSvolts 	dummyF
#define vTrack 		dummyB
#define gScurr 		dummyF
#define gTrack 		dummyU
#define cTrack 		dummyB
#define iHost 		myID.instName
#define tsRota		sc.tsRot

// READINGS VARIABLES 
#define vReadingA 	myADC[0][VOUT].curVal
#define vReadingS 	myADC[0][VIN].curVal		// no longer needed
#define iReadingA 	myADC[0][IOUT].curVal
#define tReadingA 	myADC[0][TEMP].curVal
#define wattReadingA pCont[0].watts
#define vReadingB 	myADC[0][VOUT].curVal

#define iReadingB 	myADC[0][IOUT].curVal
#define tReadingB 	myADC[0][TEMP].curVal
#define wattReadingB pCont[0].watts

#define ImAX 		myADC[0][IOUT].maxVal	// SHARED setting for both channels
#define vMAx 		myADC[0][VOUT].maxVal

#define wRreading 	WifiConnected
#define wAutoConn 	myID.autoConnect
#define IPreading 	IPstring
#define HostName 	myHostName // FQDN (mDNS)
#define dummyRead	xDumVal

//int dummySteps;
//#define stepSMPS dummySteps
//float ADCvRead = 3.2, ADCvReadS = 3.5; //vSettingA = 5.0, vReadingA = 15.8,  vReadingS = 23.4,
//float  ADCiRead = 2.1;// iSettingA = 2.0,iReadingA = 2.4,
 // uint8_t iRLimit = 100;iSLimit = 1,
//uint8_t vSTrack = 12;//, gTrack = 2;
char dummyS[64] = "ABC";
// hi [real, read]; lo [] ...
/*
float dummyCal[12] = {24.1, 2.3, 1, .04, 
                      2.0, 1.4, 0.1, 0.05,
                      27.2, 2.35, 6, 0.8};
*/
#define NUMSETTINGS 17
//#define LSET 2 // index for LIMIT
//#define TSET 3 // TRACK

 valFmt setVals[NUMSETTINGS] = {
   {&vSettingA,&FONT2, 1, VOMIN, VOMAX, 		sx(RVX+F2F3OFF), sy(TOPLINE+0*BVSPACE2+K1), 'F', "",VOLT_COL, BGCOLA, 0,2,2, NULL},// volts set A
   {&iSettingA,&FONT2, 1, IMIN,  IMAX, 			sx(RIX+F2F3OFF), sy(TOPLINE+0*BVSPACE2+K1), 'F',"", AMP_COL,  BGCOLA, 0,2,2, NULL},     // amps limit set
	 {&vSettingB,&FONT2, 1, VOMIN, VOMAX, 		sx(RVX+F2F3OFF), sy(TOPLINE+3*BVSPACE2+BBYGAP+K1), 'F', "",VOLT_COL, BGCOLB, 0,2,2, &chanBfound},// volts set B
	 {&iSettingB,&FONT2, 1, IMIN,  IMAX,  		sx(RIX+F2F3OFF), sy(TOPLINE+3*BVSPACE2+BBYGAP+K1), 'F',"", AMP_COL,  BGCOLB, 0,2,2, &chanBfound},     // amps limit set
   //{&iSLimit, &FONT2, 1,0,1, sx(READX+20),sy(TOPLINE+2*BVSPACE), 'T', "", CYAN_L,BGCOL, 0,2,0, NULL},    // limit on/off
   //{&iSTrack, &FONT2, 1,0,1, sx(READX+20),sy(TOPLINE+3*BVSPACE), 'T', "", CYAN_L,BGCOL, 0,2,0, NULL},    // track on/off
#define SET_COM 4
   //COMMS MENU - 1
   {&myID.local_ssid,   &FONT1, 1,0,255, sx(LEG2X), sy(TOPLINE+0*BVSPACE2), 				'A', "", CYAN_L, BGCOL, 1,3,0, NULL},
   {&myID.local_pass,   &FONT1, 1,0,255, sx(LEG2X), sy(TOPLINE+0.75*BVSPACE2+10), 	'A', "", CYAN_L, BGCOL, 1,3,0, NULL},   // long (32 chars) - offset Y, so under button
   {&wAutoConn, 				&FONT1, 1,0,1, 	 sx(READX+40), sy(TOPLINE+1.5*BVSPACE2), 	'T', "", CYAN_L, BGCOL, 1,2,0, NULL},  // auto connect
   {&iHost, 		    		&FONT1, 1,0,1, 	 sx(LEG2X), sy(TOPLINE+2.25*BVSPACE2), 		'A', "", CYAN_L, BGCOL, 1,2,0, NULL},     // hostname
#define SET_TRA (SET_COM + 4)  
    // TRACK MENU - 2
   {&gSvolts,  &FONT2, 1, 0, 1,  		sx(READX), sy(TOPLINE+0*BVSPACE2), 'T', "", CYAN_L, BGCOL, 2,2,0, NULL},    // track voltage setting
   {&gScurr,   &FONT2, 1, 0, 1,  		sx(READX), sy(TOPLINE+1*BVSPACE2), 'T', "", CYAN_L, BGCOL, 2,2,0, NULL},    // track current setting 
   {&gTrack,   &FONT2, 1, 0, 255,  sx(READX), sy(TOPLINE+2*BVSPACE2), 	'I', "", CYAN_L, BGCOL, 2,3,0, NULL},  // track group #
#define SET_CAL (SET_TRA + 3)
   //CAL MENU - 3
   {&calOff_VA, &FONT2, 1,-2.0, 2.0, sx(READX), sy(TOPLINE+0*BVSPACE2), 		'F', "", VOLT_COL, BGCOL, 3,1,3, NULL},     // Calibration (variable => 0.0 on entering Cal menu)
   {&calOff_IA, &FONT2, 1,-0.5, 0.5, sx(READX), sy(TOPLINE+1*BVSPACE2-10), 'F', "", AMP_COL,  BGCOL, 3,1,3, NULL},    
   {&calOff_VB, &FONT2, 1,-2.0, 2.0, sx(READX), sy(TOPLINE+2*BVSPACE2), 		'F', "", VOLT_COL, BGCOL, 3,1,3, NULL},     
   {&calOff_IB, &FONT2, 1,-0.5, 0.5, sx(READX), sy(TOPLINE+3*BVSPACE2-10), 'F', "", AMP_COL,  BGCOL, 3,1,3, NULL},   	 

 #define SET_ID (SET_CAL + 4)
   // ID MENU  - NOT USED
   {&dummyS,  &FONT1, 1,0,255, 			sx(100), 		sy(TOPLINE), 							'A', "kN", CYAN_L, BGCOL, 9,12,0, NULL}, // test string edit
   {NULL, NULL, 			1,0,0, 				0,0, 																	'X', "",	 CYAN_L, BGCOL, 0,0,0, NULL} // EOM: .vp == NULL
 };
#define NUMREADS 13
 valFmt readVals[NUMREADS]= { // max and min are hi/lo limits
   {&vReadingA, &FONT3, 1,-1,32.0,			sx(RVX), sy(TOPLINE+1*BVSPACE2+K1), 'F', "V", VOLT_COL,BGCOLA, 0,2,2, NULL},   // output volts A
	 {&iReadingA, &FONT3, 1,-1,5.0, 			sx(RIX), sy(TOPLINE+1*BVSPACE2+K1), 'F', "A", AMP_COL,BGCOLA,  0,2,2, NULL},  // output amps
   {&wattReadingA, &FONT2, 1,-1,150.0, 	sx(RPX), sy(TOPLINE+2*BVSPACE2), 'F', "W", WATT_COL,BGCOLA, 0,2,2, NULL},  // output 
	 {&vReadingB, &FONT3, 1,-1,32.0,			sx(RVX), sy(TOPLINE+4*BVSPACE2+BBYGAP+K1), 'F', "V", VOLT_COL,BGCOLB, 0,2,2, &chanBfound},   // output volts B
   {&iReadingB, &FONT3, 1,-1,5.0, 			sx(RIX), sy(TOPLINE+4*BVSPACE2+BBYGAP+K1), 'F', "A", AMP_COL,BGCOLB,  0,2,2, &chanBfound},  // output amps
	 {&wattReadingB, &FONT2, 1,-1,150.0, 	sx(RPX), sy(TOPLINE+5*BVSPACE2+BBYGAP), 'F', "W", WATT_COL,BGCOLB, 0,2,2, &chanBfound},  // output watts
   //{&iRLimit,  &FONT2, 1, 0, 0,  sx(LEGX+20), sy(TOPLINE+3*BVSPACE), 'P', "", CYAN_L,BGCOL, 0,3,0, &iSTrack},    // remote limit percent
  // {&gTrack,   &FONT2, 1, 0, 0,  sx(VALX+25), sy(TOPLINE+3*BVSPACE), 'I', "", CYAN_L,BGCOL, 0,2,0, &iSTrack},    // track group
  // {&vTrack,   &FONT0, 1, 0, 0,  sx(LEG2X), sy(TOPLINE+2.5*BVSPACE), 'Q', "V ", CYAN_L,BGCOL, 0,2,0, &iSTrack},    // track voltage set
   //{&cTrack,   &FONT0, 1, 0, 0,  sx(LEG2X), sy(TOPLINE+3*BVSPACE), 'Q', "A ", CYAN_L,BGCOL, 0,2,0, &iSTrack},    // track current set

   {&tReadingA, &FONT0, 1, 0, 0,  sx(VALX-40), sy(TOPLEGS), 'F', "C", CYAN_M,BGCOL, 0,3,0, NULL},           // temperature
  // {&vReadingS,&FONT0, 1, 0, 0,  sx(0), sy(TOPLEGS), 'F', "Vin", CYAN_M,BGCOL, 0,2,1, NULL},               // SMPS volts
   // dummy for test - set to menu 9 when not needed
 //  {&dummyRead,&FONT1, 1, 0, 0, sx(LEGX), sy(TOPLINE+2*BVSPACE), 'P', "", CYAN_M,BGCOL, 9,5,2, NULL},               // Dummy for test
   
   // COMMS Menu - 1
   {&IPreading, &FONT1, 1, 0, 0, sx(VALX-20), sy(TOPLINE+3*BVSPACE2), 	 'A', "C", CYAN_M,BGCOL, 1,3,0, NULL},           // IP (string format)
   {&HostName, &FONT1, 1, 0, 0,  sx(VALX-20), sy(TOPLINE+3.5*BVSPACE2), 'A', "C", CYAN_M,BGCOL, 1,3,0, NULL},         // fully qualified hostname (mDNS)
    // TRACK MENU - 2
	
   // CAL MENU - 3 these three readings are "ADC read volts"
   {&vReadingA, &FONT2, 1,-1,30.0,sx(LEG2X), sy(TOPLINE+0*BVSPACE2), 	 'F', "", VOLT_COL,BGCOL,  		3,2,3, NULL},  // output volts
   {&iReadingA, &FONT2, 1,-1,5.0, sx(LEG2X), sy(TOPLINE+1*BVSPACE2-10), 'F', "", AMP_COL,BGCOL,   		3,2,3, NULL},	 // output amps
   {&tsRota,  &FONT2, 1,0,4, HMAX/2 - 10, VMAX/2 + sy(50), 						 'I', "", LIGHTYELLOW,BGCOL, 	3,1,0, NULL},  // screen rotation

   {NULL, NULL, 1,0,0,   0,0, 'X', "",0,0,0,0,0, NULL} // EOM: .vp == NULL
 };   
 #define LEGMAX 18
struct legend {
   const GFXfont *font;  // font
   uint8_t textSize;     // textSize
   short xpos, ypos;     // screen location
   int8_t menu;          // which menu are these in?
   char text[LEGMAX];    // text to write
   uint16_t clr;         // colour of text
   bool *displayIf;		 // only display if TRUE
};
#define NUMLEGS 8	// also some static screen items in redrawScreen()
legend leg[NUMLEGS] = {
  // {&FONT0,  1,sx(READX), sy(TOPLEGS), 0, "SET", HIGHLIGHTCOL, NULL},
  // {&FONT0,  1,sx(LEGX+20), sy(TOPLINE+2.5*BVSPACE), 0, "Track", CYAN_L, &iSTrack},
  // {&FONT0,  1,sx(VALX+25), sy(TOPLINE+2.5*BVSPACE), 0, "Grp", CYAN_L, &iSTrack},
   // COMMS MENU - 1
   {&FONT1,  1,sx(2), sy(TOPLEG+3), 1, "COMMS", HIGHLIGHTCOL, NULL},
   {&FONT1,  1,sx(2), sy(TOPLINE+3*BVSPACE2), 1, "IP:", CYAN_M, NULL},
   {&FONT1,  1,sx(2), sy(TOPLINE+3.5*BVSPACE2), 1, "HOST:", CYAN_M, NULL},
   {&FONT1,  1,sx(LEG2X), sy(BBYSTART + FH1), 0, "CHA", MY_WHITE, NULL},
   {&FONT1,  1,sx(LEG2X), sy(BBYSTART + FH1 + BBYHT + BBYGAP), 0, "CHB", MY_WHITE, &chanBfound}, 
  // {&FONT2,  1,VAL2X+IPDOT+IPSPACE, TOPLINE+5, 1, ".", CYAN_L, NULL},
  // {&FONT2,  1,VAL2X+IPDOT+2*IPSPACE, TOPLINE+5, 1, ".", CYAN_L, NULL},
   // TRACK MENU - 2
   {&FONT1,  1,sx(2), sy(TOPLEG+3), 2, "TRACKING", HIGHLIGHTCOL, NULL},
   // CAL MENU -3
   {&FONT1,  1,sx(2), sy(TOPLEG+3), 3, "CALIBRATION", HIGHLIGHTCOL, NULL},
  // {&FONT0,  1,LEG2X, TOPLEG+BVSPACE2, 3, "Instrument", CYAN_L, NULL},
   //{&FONT0,  1,CAL2X+10, TOPLEG+BVSPACE2, 3, "Ext reading", CYAN_L, NULL},
  // {&FONT2,  1,LEG2X, TOPLINE+0*BVSPACE2+BVSPACE2/2, 3, "V", CYAN_L, NULL},
  // {&FONT0,  1,LEG2X+30, TOPLINE+0*BVSPACE2, 3, "Hi", CYAN_L, NULL},
  // {&FONT0,  1,LEG2X+30, TOPLINE+1*BVSPACE2, 3, "Lo", CYAN_L, NULL},
   //{&FONT2,  1,LEG2X, TOPLINE+2*BVSPACE2+BVSPACE2/2, 3, "A", CYAN_L, NULL},
  // {&FONT0,  1,LEG2X+30, TOPLINE+2*BVSPACE2, 3, "Hi", CYAN_L, NULL},
  // {&FONT0,  1,LEG2X+30, TOPLINE+3*BVSPACE2, 3, "Lo", CYAN_L, NULL},
//{&FONT0,  1,LEG2X, TOPLINE+4*BVSPACE2, 3, "In Hi", CYAN_L, NULL},
  // {&FONT0,  1,LEG2X, TOPLINE+5*BVSPACE2, 3, "V  Lo", CYAN_L, NULL},
   {NULL,  0,0, 0, -1, "0", 0, NULL} // EOM: .menu == -1
};

// TOUCH SCREEN BUTTONS
//  sizing
#ifdef ILI9488
  #define BUTSIZ (BVSPACE - 2) // button needs to be smaller than spacing!
#else
  #define BUTSIZ (BVSPACE - 10)    
#endif
#define BUTEDGETHICK 2
#define BUTROUND 6
#define TEXTDOWN 4
// location
#define BUTX 200
#define BUTBOTV 275

#define TOPLINEX (TOPLINE - 5)
#define BTOP 20
#define SBTOP (BTOP + BUTSIZ/2) // small buttons - align baseline
#define BOFFI	20
//

// callback routines  should contain non-blocking code (e.g. state machines)
// if blocking code is required, then yield() should be issued regularly to allow background (e.g. WiFi) tasks to process

//  callback routine COMMAND codes
#define CALL_CMD_START  1 // start cmd (first call)
#define CALL_CMD_PROC   0 // continue processing
#define CALL_CMD_STOP  -1 // cease processing

// callback routine RETURN codes - see ScreenCal and OSK for examples
#define CALL_PROC -1  	// Still processing
#define CALL_ERR  -10 	// error exit return any value <= -10
// Ok exit may be any value >= CALL_EX
#define CALL_EX    0
#define CALL_NOEX  1

// callback processing STATUS codes
#define CALL_ACTIVE 1 	// still processing (call me again)
#define CALL_DONE   2	// completed
#define CALL_START  3	// intialising - not yet called
#define CALL_IDLE   0 	// no call in progress

uint8_t callStatus = CALL_IDLE;
boolean wastouched = false;
// also used by switches
bool swPressed = false; 
short butDigit = 0; // start at 10^0
short butDir = 0;

uint8_t currentMenu = 0; // which menu are we in?

// callbacks here
#include "myScreenCal.h" 
#include "myOSK.h"
// also myTouchProcess.h

#define BUTTEXT 6 // 5 chars at most
struct button {
  int (* callback)(int cmd); // see above
  int8_t menu;
  uint8_t nextMenu;   // same as menu, unless changing screens 
  uint8_t sv;     // index to the SetVal item to be edited
	// OLD: val 0..7 - bit 0 == 0 :half height, bit 1 == 0 : half width; bit 2 = display button
  uint16_t siz;     // bits 0..3 = height (1/2 .. 8); bits 4..7 width (same fmt). Bit 8 = visible. Bit 9 enabled.
  short xpos, ypos;
  bool border;
  char text[BUTTEXT];
  uint16_t selColour;	// some buttons are also indicators - so provide ability to change bg colours
  uint16_t unselColour;
	 bool * enabledIf;		 // only enabled if TRUE or NULL
};

#define CHOFFY 15 // offsets for invisible buttons
#define CHOFFX (10)
// some setVal indices - used in tMode
#define LBUT 2
#define TBUT 3
#define WBUT 6
#define GSVBUT SET_TRA 
#define GSABUT (SET_TRA +1)
#define NOSETVAL 255

#define ENA_BUT 0x0200 // can be touched
#define DRAW_BUT 0x100 // is visible


int toggleLimit(int);
int toggleTrack(int);
int toggleTrackVolts(int);
int toggleTrackAmps(int);
int screenCal(int);
int exiREADXtCal(int);
int tsRotate(int);
int toggleWconn(int);
/*

int upCalVolts(int);
int upCalAmps(int);
int dnCalVolts(int);
int dnCalAmps(int);
*/
#define NUMBUTS 25
button but[NUMBUTS] = {
  // MAIN MENU - 0
  {NULL, 0, 0, 0, 0x0245, sx(RVX+F2F3OFF), sy(TOPLINE+0*BVSPACE2-BOFFI), false,"VA", BBGHIGH, BBXHIGH, NULL}, // ch A
  {NULL, 0, 0, 1, 0x0245, sx(RIX+F2F3OFF), sy(TOPLINE+0*BVSPACE2-BOFFI), false,"AA", BBGHIGH, BBXHIGH, NULL},
	{NULL, 0, 0, 2, 0x0245, sx(RVX+F2F3OFF), sy(TOPLINE+3*BVSPACE2-BOFFI+BBYGAP), false,"VB", BBGHIGH, BBXHIGH, &chanBfound}, // ch B
  {NULL, 0, 0, 3, 0x0245, sx(RIX+F2F3OFF), sy(TOPLINE+3*BVSPACE2-BOFFI+BBYGAP), false,"AB", BBGHIGH, BBXHIGH, &chanBfound},
 // {&toggleLimit,0, 0, 2, 0x0311, sx(BUTX), sy(BTOP+2*BVSPACEX), false,"L", BBGHIGH, BBXHIGH }, // LBUT - implicit set value
//  {&toggleTrack,0, 0, 3, 0x0311, sx(BUTX), sy(BTOP+3*BVSPACEX), false,"T", BBGHIGH, BBXHIGH }, // TBUT
  {&calEntry,0, 3, NOSETVAL, 0x0311, sx(3), sy(3), false,"CAL", BBGHIGH2, BBXHIGH2, NULL},
  {NULL,0, 1, NOSETVAL, 0x0311, sx(200), sy(3), false,"COM", BBGHIGH2, BBXHIGH2, NULL},
	{NULL,0, 2, NOSETVAL, 0x0311, sx(10), sy(BUTBOTV), false,"TRA", BBGHIGH3, BBXHIGH3, &chanBfound}, // TRACKING
  // COMMS MENU - 1
  /*
  {NULL,1, 1, SET_COM, 6, VAL2X+0*IPSPACE,TOPLINE+10, false,"IP1", BBGHIGH, BBXHIGH  },
  {NULL,1, 1, SET_COM + 1, 6, VAL2X+1*IPSPACE,TOPLINE+10, false,"IP2", BBGHIGH, BBXHIGH  }, 
  {NULL,1, 1, SET_COM + 2, 6, VAL2X+2*IPSPACE,TOPLINE+10, false,"IP3", BBGHIGH, BBXHIGH  }, 
  {NULL,1, 1, SET_COM + 3, 6, VAL2X+3*IPSPACE,TOPLINE+10, false,"IP4", BBGHIGH, BBXHIGH  }, 
  */
  {NULL,1, 1, SET_COM, 	   			0x0301, sx(BUTX), SBTOP + sy(0*BVSPACEX), false,"SS", BBGHIGH, BBXHIGH, NULL}, // SSID
  {NULL,1, 1, SET_COM + 1, 			0x0301, sx(BUTX), SBTOP + sy(0.75*BVSPACEX), false,"PA", BBGHIGH, BBXHIGH, NULL}, //WiFi Pass
  {&toggleWconn,1, 1, SET_COM + 2,  0x0301, sx(BUTX), SBTOP + sy(1.5*BVSPACEX), false,"AC", BBGHIGH, BBXHIGH, NULL}, // Toggle WiFi AutoConnect
  {NULL,1, 1, SET_COM + 3, 			0x0301, sx(BUTX), SBTOP + sy(2.25*BVSPACEX), false,"HO", BBGHIGH, BBXHIGH, NULL}, // Hostname
  
  // TRACK MENU - 2
  {&toggleTrackVolts,2, 2, SET_TRA 	 , 0x0311, sx(BUTX), sy(BTOP+0*BVSPACE), false,"VOL", BBGHIGH, BBXHIGH, NULL},  
  {&toggleTrackAmps,2, 2, SET_TRA + 1, 0x0311, sx(BUTX), sy(BTOP+1*BVSPACE), false,"AMP", BBGHIGH, BBXHIGH, NULL},  
  {NULL,2, 2, SET_TRA + 2, 0x0311, sx(BUTX), sy(BTOP+2*BVSPACE), false,"GRP", BBGHIGH, BBXHIGH, NULL},  
  
  // CAL MENU - 3  
  {&screenCal,3, 3, NOSETVAL, 0x0311, sx(20), sy(BUTBOTV), false,"TCH", BBGHIGH, BBXHIGH, NULL}, 
  {NULL,3, 3, SET_CAL, 0x0311, sx(BUTX), sy(BTOP+0*BVSPACEX), false,"V" , BBGHIGH, BBXHIGH, NULL}, // A chan
  {NULL,3, 3, SET_CAL + 1, 0x0311, sx(BUTX), sy(BTOP+1*BVSPACEX), false,"A" , BBGHIGH, BBXHIGH, NULL}, 
	{NULL,3, 3, SET_CAL + 2, 0x0311, sx(BUTX), sy(BTOP+2*BVSPACEX), false,"VB" , BBGHIGH, BBXHIGH, NULL}, // B chan
  {NULL,3, 3, SET_CAL + 3, 0x0311, sx(BUTX), sy(BTOP+3*BVSPACEX), false,"AB" , BBGHIGH, BBXHIGH, NULL}, 
  {&calSave,3, 0, NOSETVAL, 0x0311, sx(BUTX), sy(BTOP+5*BVSPACEX), false,"SAVE" , BBGHIGH, BBXHIGH, NULL}, // save and exit
  {&tsRotate,3, 3, NOSETVAL, 0x0311, (HMAX - BUTSIZ)/2, (VMAX - BUTSIZ)/2, false,"ROT" , BBGHIGH, BBXHIGH, NULL}, // in middle of screen - as rotation mat be reversed


  // exit buttons (all menus except MAIN)
  {NULL,1, 0, NOSETVAL, 0x0311, sx(BUTX), sy(BTOP+3.5*BVSPACEX), false,"X", BBGHIGH, BBXHIGH, NULL}, // COMMS exit
  {NULL,2, 0, NOSETVAL, 0x0311, sx(BUTX), sy(BTOP+3.5*BVSPACEX), false,"X", BBGHIGH, BBXHIGH, NULL}, // ID  exit
  {&exitCal,3, 0, NOSETVAL, 0x0311,  sx(BUTX), sy(BTOP+7*BVSPACEX), false,"X", BBGHIGH, BBXHIGH, NULL},  // CAL exit
  {NULL,-1, 0, NOSETVAL, 0, 0, 0x0, false,"", BBGHIGH, BBXHIGH, NULL} // EOM
};

/*
Colour tables for Toggle buttons and Indicators
Modes 
	Off [0] 
	On [1]: Inactive, e.g. not limiting or tracking = 100%
	On [2]: Active, e.g. limiting or tracking < 100%
for indicators, Off is BGCOLOR, rather than tColours[0]
*/
#define BBOFF 0 // track and limit (disabled)
#define BBINACT 1 // function enabled, but not active
#define BBACT 2 // function enabled, and active
#define WIFIACT 1 // green (autoconnect + connected)
#define WIFIOFF 0 // blue  (autoconnect off = idle)
#define WIFIINACT 2 // red (autoconnect + not connected = error)
uint16_t tColours[3] = {BBXHIGH, MY_DARKGREEN, MY_RED};


#endif
