/* Screen and touch defs 
*/
#ifndef  MYSCREENDEFS_H
#define  MYSCREENDEFS_H

#include "SPI.h"
#include "Adafruit_GFX.h"

#include <XPT2046_Touchscreen.h>

#ifdef ILI9341
#define sx(x)	((short)(x))	// no scaling
#define sy(x)	((short)(x))	// no scaling
#include "Adafruit_ILI9341.h"
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

#ifdef ILI9488
// see also  myInst.h
#define sx(x)	((short)(1.5 * (x)))	  // scale by 480/320 for bigger display
#define sy(x)	((short)(1.333 * (x))) // 320/240
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_CLK /* SCK */, TFT_MOSI /* MOSI */, -1 /* MISO */, VSPI /* spi_num */);
Arduino_ILI9488_18bit tft =  Arduino_ILI9488_18bit(bus, TFT_RST, 0, false /* IPS */);
#endif

#ifdef ILI9488

	#include "myFonts/DejaVu_Sans_Mono_Bold_18.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_22.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_32.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_42.h"
	
	#define FONT0 DejaVu_Sans_Mono_Bold_18  // smallest
	#define FONT1 DejaVu_Sans_Mono_Bold_22  // small
	#define FONT2 DejaVu_Sans_Mono_Bold_32  // larger
	#define FONT3 DejaVu_Sans_Mono_Bold_42        // biggest
#else
	#include "myFonts/DejaVu_Sans_Mono_Bold_16.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_18.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_26.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_38.h"
	
	#define FONT0 DejaVu_Sans_Mono_Bold_16  // smallest
	#define FONT1 DejaVu_Sans_Mono_Bold_18  // small
	#define FONT2 DejaVu_Sans_Mono_Bold_26  // larger
	#define FONT3 DejaVu_Sans_Mono_Bold_38        // biggest
#endif

#define MY_DARKGREEN 0x03E0
#define MY_RED 0xF800
#define MY_WHITE 0xFFFF
#define MY_YELLOW 0xFFE0
#define MY_GREEN RGB565(0,36,0)
#define RGB565(R, G, B) (((R)<<11)|((G)<<5)|(B))  
#define LIGHTYELLOW RGB565(31,63,0)
#define YELLOW_VL RGB565(31,63,10)
#define CYAN_L RGB565(15,45,31)
#define CYAN_VL RGB565(21,63,31)
#define CYAN_M RGB565(8,32,24)
#define GREEN_L RGB565(5,63,5)
#define CYAN_M RGB565(8,32,24)
#define PINK_M RGB565(31,28,31)
// special colours
#define VOLT_COL LIGHTYELLOW
#define AMP_COL GREEN_L
#define WATT_COL PINK_M
// screen background
#define BGCOL 0 
#define BGHIGH RGB565(5,11,4)  
#define MIDBG RGB565(5,11,4) 
// value text
#define TEXTCOL  MY_YELLOW
#define HIGHLIGHTCOL MY_WHITE
//menu button items
#define BTEXTCOL RGB565(16,32,16) //light grey
#define BHIGHCOL 0xffff  //LIGHTYELLOW // MY_YELLOW // ILI9341_BLACK 
//menu button backgrounds
#define BBORDHIGH BHIGHCOL
#define BBORD BGCOL
#define BBGHIGH RGB565(0,10,22)
#define BBXHIGH RGB565(0,0,9)

// error screen backgrounds
#define ERR_BG_A MY_DARKGREEN
#define ERR_BG_B MY_RED

// indicator BG colours
#define ITRIG MY_RED 			// Action triggered
#define IENABLE	MY_DARKGREEN	// Function enabled
#define IND_ON MY_GREEN
#define IND_OFFC MY_RED
// screen calibration 
struct screenCals {
  int thmin; // tft.pixel (0,0)
  int tvmin;
  int thmax;// tft.pixel (HMAX,VMAX)
  int tvmax;
  int tsRot;	// screen rotation - as some touch screens are rotated 180%
} ;
screenCals sc = {400, 300, 3900, 3700, SCREENROT}; // useful starting values, overwritten by screen cal and EEPROM restore.

// touch units to screen pixel conversions
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

#define BVSPACE 50
#define BVSPACE2 30

#define LEGX 22
#define LEG2X 5
#define CIRCX 10
#define VALX 100
#define VAL2X 60
#define CALX 80
#define CAL2X 160
#define CALREADX 240
#define READX 185
#define TOPLINE 50
#define TOPLEG 17 // big enough for a FONT2 character
#define TOPLEGS 15 // big enough for a FONT1 character

#define IPSPACE 55
#define IPDOT 40
#define ROUNDERR 0.01    // float rounding may stop hi/low set values being attained.
#define UNITLEN 6
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
 uint8_t menu;         // which menu are these in?
 uint8_t pre, post;    // decimals before and after the point (post is ignored for fmt != 'F')
  bool *displayIf;		 // only display if TRUE
 };
 
// SETTINGS VARIABLES 
// current set/read vals. SET values will be overwritten on start-up from saved EEPROM values
// only float and int8_t allowed
#define vSetting 	pSetA.voltage
#define iSetting 	pSetA.current
#define iSLimit 	pSetA.limitOn
#define iSTrack 	pSetA.trackOn
#define iRLimit 	pSetA.eTrack
#define gSvolts 	pSetA.trackSv
#define gScurr 		pSetA.trackSa
#define gTrack 		pSetA.trackGrp
#define vTrack 		pSetA.trackSv
#define cTrack 		pSetA.trackSa
#define iHost 		myID.instName
#define tsRota		sc.tsRot
// READINGS VARIABLES 
#define vReading 	myADC[VOUT].curVal
#define vReadingS 	myADC[VIN].curVal
#define iReading 	myADC[IOUT].curVal
#define tReading 	myADC[TEMP].curVal
#define iMax 		myADC[IOUT].maxVal
#define vMAx 		myADC[VOUT].maxVal
#define wRreading 	pSetA.WifiConnected
#define wAutoConn 	myID.autoConnect
#define IPreading 	IPstring
#define HostName 	myHostName // FQDN (mDNS)
#define dummyRead	xDumVal
#define wattReading pContA.watts
int dummySteps;
#define stepSMPS dummySteps
//float ADCvRead = 3.2, ADCvReadS = 3.5; //vSetting = 5.0, vReading = 15.8,  vReadingS = 23.4,
//float  ADCiRead = 2.1;// iSetting = 2.0,iReading = 2.4,
 // uint8_t iRLimit = 100;iSLimit = 1,
//uint8_t vSTrack = 12;//, gTrack = 2;
char dummyS[64] = "ABC";
// hi [real, read]; lo [] ...
float dummyCal[12] = {24.1, 2.3, 1, .04, 
                      2.0, 1.4, 0.1, 0.05,
                      27.2, 2.35, 6, 0.8};

#define NUMSETTINGS 15
#define LSET 2 // index for LIMIT
#define TSET 3 // TRACK

 valFmt setVals[NUMSETTINGS] = {
   {&vSetting,&FONT2, 1, 0.01,29, sx(READX),sy(TOPLINE+0*BVSPACE), 'F', "",VOLT_COL, 0,2,2, NULL},// volts set
   {&iSetting,&FONT2, 1, 0,5, sx(READX),sy(TOPLINE+1*BVSPACE), 'F',"", AMP_COL, 0,2,2, NULL},     // amps limit set
   {&iSLimit, &FONT2, 1,0,1, sx(READX+20),sy(TOPLINE+2*BVSPACE), 'T', "", CYAN_L,0,2,0, NULL},    // limit on/off
   {&iSTrack, &FONT2, 1,0,1, sx(READX+20),sy(TOPLINE+3*BVSPACE), 'T', "", CYAN_L,0,2,0, NULL},    // track on/off
#define SET_COM 4
   //COMMS MENU - 1
   {&myID.local_ssid,   &FONT1, 1,0,255, sx(LEG2X),sy(TOPLINE+0*BVSPACE), 'A', "", CYAN_L,1,3,0, NULL},
   {&myID.local_pass,   &FONT1, 1,0,255, sx(LEG2X),sy(TOPLINE+0.75*BVSPACE+10), 'A', "", CYAN_L,1,3,0, NULL},   // long (32 chars) - offset Y, so under button
   {&wAutoConn, 		&FONT1, 1,0,1, sx(READX+40),sy(TOPLINE+1.5*BVSPACE), 'T', "", CYAN_L,1,2,0, NULL},  // auto connect
   {&iHost, 		    &FONT1, 1,0,1, sx(LEG2X),sy(TOPLINE+2.25*BVSPACE), 'A', "", CYAN_L,1,2,0, NULL},     // hostname
#define SET_TRA (SET_COM + 4)  
    // TRACK MENU - 2
   {&gSvolts,  &FONT2, 1, 0, 1,  sx(READX), sy(TOPLINE+0*BVSPACE), 'T', "", CYAN_L,2,2,0, NULL},    // track voltage setting
   {&gScurr,   &FONT2, 1, 0, 1,  sx(READX), sy(TOPLINE+1*BVSPACE), 'T', "", CYAN_L,2,2,0, NULL},    // track current setting 
   {&gTrack,   &FONT2, 1, 0, 255,  sx(READX), sy(TOPLINE+2*BVSPACE), 'I', "", CYAN_L,2,3,0, NULL},  // track group #
#define SET_CAL (SET_TRA + 3)
   //CAL MENU - 3
   {&calOff_V, &FONT2, 1,-2.0, 2.0, sx(READX),sy(TOPLINE+0*BVSPACE), 'F', "", VOLT_COL,3,1,3, NULL},     // Calibration (variable => 0.0 on entering Cal menu)
   {&calOff_I, &FONT2, 1,-0.5, 0.5, sx(READX),sy(TOPLINE+1*BVSPACE-10), 'F', "", AMP_COL,3,1,3, NULL},     

 #define SET_ID (SET_CAL + 2)
   // ID MENU  - NOT USED
   {&dummyS,  &FONT1, 1,0,255, sx(100),sy(TOPLINE), 'A', "kN",CYAN_L,9,12,0, NULL}, // test string edit
   {NULL, NULL, 1,0,0, 0,0, 'X', "",CYAN_L,0,0,0, NULL} // EOM: .vp == NULL
 };
#define NUMREADS 15
 valFmt readVals[NUMREADS]= { // max and min are hi/lo limits
   {&vReading, &FONT3, 1,-1,32.0,sx(LEGX), sy(TOPLINE+0*BVSPACE), 'F', "V", VOLT_COL,0,2,2, NULL},   // output volts
   {&iReading, &FONT3, 1,-1,5.0, sx(LEGX), sy(TOPLINE+1*BVSPACE), 'F', "A", AMP_COL,0,2,2, NULL},  // output amps
   {&wattReading, &FONT3, 1,-1,150.0, sx(LEGX), sy(TOPLINE+2*BVSPACE), 'F', "W", WATT_COL,0,2,2, NULL},  // output watts
   {&iRLimit,  &FONT2, 1, 0, 0,  sx(LEGX+20), sy(TOPLINE+3*BVSPACE), 'P', "", CYAN_L,0,3,0, &iSTrack},    // remote limit percent
   {&gTrack,   &FONT2, 1, 0, 0,  sx(VALX+25), sy(TOPLINE+3*BVSPACE), 'I', "", CYAN_L,0,2,0, &iSTrack},    // track group
   {&vTrack,   &FONT0, 1, 0, 0,  sx(LEG2X), sy(TOPLINE+2.5*BVSPACE), 'Q', "V ", CYAN_L,0,2,0, &iSTrack},    // track voltage set
   {&cTrack,   &FONT0, 1, 0, 0,  sx(LEG2X), sy(TOPLINE+3*BVSPACE), 'Q', "A ", CYAN_L,0,2,0, &iSTrack},    // track current set

   {&tReading, &FONT0, 1, 0, 0,  sx(VALX-10), sy(TOPLEGS), 'F', "C", CYAN_M,0,3,0, NULL},           // temperature
   {&vReadingS,&FONT0, 1, 0, 0,  sx(0), sy(TOPLEGS), 'F', "Vin", CYAN_M,0,2,1, NULL},               // SMPS volts
   // dummy for test - set to menu 9 when not needed
 //  {&dummyRead,&FONT1, 1, 0, 0, sx(LEGX), sy(TOPLINE+2*BVSPACE), 'P', "", CYAN_M,9,5,2, NULL},               // Dummy for test
   
   // COMMS Menu - 1
   {&IPreading, &FONT1, 1, 0, 0, sx(VALX-20), sy(TOPLINE+3*BVSPACE), 'A', "C", CYAN_M,1,3,0, NULL},           // IP (string format)
   {&HostName, &FONT1, 1, 0, 0,  sx(VALX-20), sy(TOPLINE+3.5*BVSPACE), 'A', "C", CYAN_M,1,3,0, NULL},         // fully qualified hostname (mDNS)
    // TRACK MENU - 2
	
   // CAL MENU - 3 these three readings are "ADC read volts"
   {&vReading, &FONT2, 1,-1,30.0,sx(LEG2X), sy(TOPLINE+0*BVSPACE), 'F', "", VOLT_COL, 3,2,3, NULL},  // output volts
   {&iReading, &FONT2, 1,-1,5.0, sx(LEG2X), sy(TOPLINE+1*BVSPACE-10), 'F', "", AMP_COL,3,2,3, NULL},	 // output amps
   {&tsRota,  &FONT2, 1,0,4, HMAX/2 - 10, VMAX/2 + sy(50), 'I', "", LIGHTYELLOW,3,1,0, NULL},  // screen rotation

   {NULL, NULL, 1,0,0,   0,0, 'X', "",0,0,0, NULL} // EOM: .vp == NULL
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
#define NUMLEGS 9	// also some static screen items in redrawScreen()
legend leg[NUMLEGS] = {
   {&FONT0,  1,sx(READX), sy(TOPLEGS), 0, "SET", HIGHLIGHTCOL, NULL},
   {&FONT0,  1,sx(LEGX+20), sy(TOPLINE+2.5*BVSPACE), 0, "Track", CYAN_L, &iSTrack},
   {&FONT0,  1,sx(VALX+25), sy(TOPLINE+2.5*BVSPACE), 0, "Grp", CYAN_L, &iSTrack},
   // COMMS MENU - 1
   {&FONT1,  1,sx(2), sy(TOPLEG+3), 1, "COMMS", HIGHLIGHTCOL, NULL},
   {&FONT1,  1,sx(2), sy(TOPLINE+3*BVSPACE), 1, "IP:", CYAN_M, NULL},
   {&FONT1,  1,sx(2), sy(TOPLINE+3.5*BVSPACE), 1, "HOST:", CYAN_M, NULL},
   //{&FONT1,  1,LEG2X, TOPLINE, 1, "IP", CYAN_L, NULL},
   //{&FONT2,  1,VAL2X+IPDOT, TOPLINE+5, 1, ".", CYAN_L, NULL}, // dots between IP, Y+5 avoids block erase on value edit
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
  #define BUTSIZ 55	
#else
  #define BUTSIZ 40    
#endif
#define BUTEDGETHICK 2
#define BUTROUND 6
#define TEXTDOWN 4
// location
#define BUTX 275
#define BUTBOTV 215
#define BVSPACEX BVSPACE
#define TOPLINEX (TOPLINE - 5)
#define BTOP 20
#define SBTOP (BTOP + BUTSIZ/2) // small buttons - align baseline
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
  uint8_t siz;     // val 0..7 - bit 0 == 0 :half height, bit 1 == 0 : half width; bit 2 = display button
  short xpos, ypos;
  bool border;
  char text[BUTTEXT];
  uint16_t selColour;	// some buttons are also indicators - so provide ability to change bg colours
  uint16_t unselColour;
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
int toggleLimit(int);
int toggleTrack(int);
int toggleTrackVolts(int);
int toggleTrackAmps(int);
int screenCal(int);
int exitCal(int);
int tsRotate(int);
int toggleWconn(int);
/*

int upCalVolts(int);
int upCalAmps(int);
int dnCalVolts(int);
int dnCalAmps(int);
*/
#define NUMBUTS 23
button but[NUMBUTS] = {
  // MAIN MENU - 0
  {NULL, 0, 0, 0, 7, sx(BUTX), sy(BTOP+0*BVSPACEX), false,"V", BBGHIGH, BBXHIGH  },
  {NULL,0, 0, 1, 7, sx(BUTX), sy(BTOP+1*BVSPACEX), false,"A", BBGHIGH, BBXHIGH   },
  {&toggleLimit,0, 0, 2, 7, sx(BUTX), sy(BTOP+2*BVSPACEX), false,"L", BBGHIGH, BBXHIGH }, // LBUT - implicit set value
  {&toggleTrack,0, 0, 3, 7, sx(BUTX), sy(BTOP+3*BVSPACEX), false,"T", BBGHIGH, BBXHIGH }, // TBUT
  {NULL,0, 1, NOSETVAL, 6, sx(10), sy(BUTBOTV), false,"COM", BBGHIGH, BBXHIGH   },
  {NULL,0, 2, NOSETVAL, 6, sx(130), sy(BUTBOTV), false,"TRA", BBGHIGH, BBXHIGH   }, // TRACKING
  {&calEntry,0, 3, NOSETVAL, 6, sx(70), sy(BUTBOTV), false,"CAL", BBGHIGH, BBXHIGH   },
  // COMMS MENU - 1
  /*
  {NULL,1, 1, SET_COM, 6, VAL2X+0*IPSPACE,TOPLINE+10, false,"IP1", BBGHIGH, BBXHIGH  },
  {NULL,1, 1, SET_COM + 1, 6, VAL2X+1*IPSPACE,TOPLINE+10, false,"IP2", BBGHIGH, BBXHIGH  }, 
  {NULL,1, 1, SET_COM + 2, 6, VAL2X+2*IPSPACE,TOPLINE+10, false,"IP3", BBGHIGH, BBXHIGH  }, 
  {NULL,1, 1, SET_COM + 3, 6, VAL2X+3*IPSPACE,TOPLINE+10, false,"IP4", BBGHIGH, BBXHIGH  }, 
  */
  {NULL,1, 1, SET_COM, 	   			6, sx(BUTX), SBTOP + sy(0*BVSPACEX), false,"SS", BBGHIGH, BBXHIGH  }, // SSID
  {NULL,1, 1, SET_COM + 1, 			6, sx(BUTX), SBTOP + sy(0.75*BVSPACEX), false,"PA", BBGHIGH, BBXHIGH  }, //WiFi Pass
  {&toggleWconn,1, 1, SET_COM + 2,  6, sx(BUTX), SBTOP + sy(1.5*BVSPACEX), false,"AC", BBGHIGH, BBXHIGH }, // Toggle WiFi AutoConnect
  {NULL,1, 1, SET_COM + 3, 			6, sx(BUTX), SBTOP + sy(2.25*BVSPACEX), false,"HO", BBGHIGH, BBXHIGH }, // Hostname
  
  // TRACK MENU - 2
  {&toggleTrackVolts,2, 2, SET_TRA 	 , 7, sx(BUTX), sy(BTOP+0*BVSPACE), false,"VOL", BBGHIGH, BBXHIGH   },  
  {&toggleTrackAmps,2, 2, SET_TRA + 1, 7, sx(BUTX), sy(BTOP+1*BVSPACE), false,"AMP", BBGHIGH, BBXHIGH   },  
  {NULL,2, 2, SET_TRA + 2, 7, sx(BUTX), sy(BTOP+2*BVSPACE), false,"GRP", BBGHIGH, BBXHIGH   },  
  
  // CAL MENU - 3  
  {&screenCal,3, 3, NOSETVAL, 6, sx(20), sy(BUTBOTV), false,"TCH", BBGHIGH, BBXHIGH }, 
  {NULL,3, 3, SET_CAL, 7, sx(BUTX), sy(BTOP+0*BVSPACEX), false,"V" , BBGHIGH, BBXHIGH  }, 
  {NULL,3, 3, SET_CAL + 1, 7, sx(BUTX), sy(BTOP+1*BVSPACEX), false,"A" , BBGHIGH, BBXHIGH  }, 
  {&calSave,3, 0, NOSETVAL, 7, sx(BUTX), sy(BTOP+2.5*BVSPACEX), false,"SAVE" , BBGHIGH, BBXHIGH  }, // save and exit
  {&tsRotate,3, 3, NOSETVAL, 7, (HMAX - BUTSIZ)/2, (VMAX - BUTSIZ)/2, false,"ROT" , BBGHIGH, BBXHIGH  }, // in middle of screen - as rotation mat be reversed


  // exit buttons (all menus except MAIN)
  {NULL,1, 0, NOSETVAL, 7, sx(BUTX), sy(BTOP+3.5*BVSPACEX), false,"X", BBGHIGH, BBXHIGH   }, // COMMS exit
  {NULL,2, 0, NOSETVAL, 7, sx(BUTX), sy(BTOP+3.5*BVSPACEX), false,"X", BBGHIGH, BBXHIGH   }, // ID  exit
  {&exitCal,3, 0, NOSETVAL, 7,  sx(BUTX), sy(BTOP+3.5*BVSPACEX), false,"X", BBGHIGH, BBXHIGH   },  // CAL exit
  {NULL,-1, 0, NOSETVAL, 0, 0, 0, false,"", BBGHIGH, BBXHIGH  } // EOM
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
