// No longer used
// **************** BOARD_D_E have FANPIN on 26 CONTROL 14. 
/* REMEMBER TO SET BOARD AND DISPLAY TYPE  BEFORE COMPILING!!!!
 *  HAL - separation of Counts -> V; V -> Real World value (IN and OUT)
 *    getVolts() in each HAL file (e.g MCP41.h, ADS1115) - can be interrupt or poll driven
 *    convert() translates Volts to real world values
 *    setVolts (etc) 
 *  control() measures inputs, sets outputs to desired setpoint, handling current limiting and tracking
 *  ADS1115: interrput driven, converts each used input in turn
*/
// select a display type 
//#define  ILI9488 // not working yet
#define  ILI9341 
#define BOARD_D   // note BOARD_E has FANPIN on 26 CONTROL 14. 
#define SOFT_VERSION 1 // change this number if any of the myProfile.h structures are altered - incl REALNET.
#define REALNET     // connect to my REAL or dummy WiFi LAN. OFF for PRODUCTION (change SOFT_VERSION to write this to EEPROM)

//#define C_DEBUG // slow down control execution and enable serial diagnostics, for testing
bool startUp = true;
//bool numtest = false;
#include "myMCP45.h"
#include "myInst.h"
#include "myPSU.h"
#include "myHelpers.h"
#include "myScreenDefs.h"
#include "mySwitches.h"
#include "myADS1115.h"
#include "ADS1115bits.h"
#include "myPOST.h"

//#define SPLASH // turn on for production
//inHAL ADC_HAL = {&readADCHAL, 0, 0, 3.3, 0, 0, 16, false}; // ADS1115
void printfloat(char fmt[], float val, uint16_t highlight, uint16_t x, short y, uint16_t txtcol, uint16_t highcol, uint16_t bgcol);
//long loopcntr;
bool myBlink = false; // set by timer at same time as regular redraw
#include "myScreenDraw.h"
#include "myWifi.h"
#include "myTelnet.h"
#include "myUDP.h"
#include "myHTTP.h"

#include "myControl.h"
#include "scpiparser.h"
#include "mySCPI.h"
#include "myProfile.h"
//float vTarget = VSTART;
//float iTarget = 0.08;
//float eTrack = 1.0;
//int setting = WSTART;
#define TESTPIN FANPIN

long lTimer, mTimer, fTimer, vTimer, cTimer; // various processing frequency timers for loop()
void setup() {   

//pinMode(TESTPIN, OUTPUT);
//digitalWrite(TESTPIN, HIGH);
 //diagPin(HIGH);
  newPosition = 0; 
  Serial.begin(115200);
  delay(1000);
  Serial.println("*********** Starting PSU Control *************");
  /******** TFT & TOUCH *****************/
  screenBegin();
#ifdef ILI9341
  //ili9341Diag();
#endif
#ifdef SPLASH
  splashScreen();
#endif
  //Serial.print(pSetA.voltage); Serial.print(" Vout = "); Serial.print(myADC[VOUT].maxVolts); Serial.println(myADC[VOUT].units);
  //randomSeed(micros());
  
  if (!post())
    screenError(postBuf, MY_RED, 10, false); // 3 x I2C for single (7 for dual);
    
   EEPROM.begin(EESIZE + 5 ); // maybe a few additional bytes will stop the kernel panic restart on save
   if (!setupEE()) // read values from EEPROM
   {
      screenError("Bad EEPROM read\nFailed factory reset\nAborting", MY_RED, 10, false);  
      // should probably shut things down
   }
   ts.setRotation(sc.tsRot); // some displays have a rotated touch screen (CAL menu)
   pSetA.eTrack = 1.0;
  //  testEEprofile(); 

  // set up control pins, ISR and other hardware-related initialisation
  // adsTest();
  control_setup();
  
  // remote command handling - Telnet, UDP, SCPI
  if(myID.autoConnect)
  {
    wifiBegin();
    setupTelnet();
    setup_http();
    UDPstart();// Serial.printf("UDP start %s\n",(UDPstart())?"OK":"BAD");
  }
  SCPI_setup();
//roundtest(2);

  redrawScreen();
  Serial.println("******** Done setup *******");
  Serial.println("SCPI Command?");
  delay(50);
  
  lTimer =  mTimer = fTimer = vTimer = cTimer = millis();
}
int count = 0;

//float floatval = 123.47;

#include "myTouchProcess.h"
#define PROCESS_EVERY_F 5     //   1 mS - encoder and buttons (debounce)
#define PROCESS_EVERY_CTRL 20 //  20 mS - control loop update
#define PROCESS_EVERY_M 100    //  50 mS - touch screen, on-off buttons, group messages
#define PROCESS_EVERY_L 750   // 750 mS - display and readings; control group heartbeat messages
#define PROCESS_EVERY_VL 10000  // 10  S - are control group members still present?
#define STEP_DELAY 5000         // 5  S - for debug only
#define CYCLES_STEP 8
uint16_t potval = 0;
float vCont;
bool updateZero;
short UDPcnt;
long lastADS = 100;
bool dp;
void loop() {

    /* fast stuff */
    adsProcess();
    if(adsSetready) 
    {
      getReadings();        
#ifndef C_DEBUG // slow down control for debug (see myControl.h)
      vCont = control(&pSetA, &pContA);
#endif
      adsSetready = false;
    }
    enc.loop();
    
    yield(); // let ESP32 background tasks run
     
    if(millis() > fTimer + PROCESS_EVERY_F)
    {   
      fTimer = millis();  
     // Serial.print("F"); delay(1);
      // but.loop(); // encoder switch not used
      butL.loop(); // L & R switches
      butR.loop();
      processOnOffSw();
    }
    yield(); // let ESP32 background tasks run
    
    if(millis() - cTimer > PROCESS_EVERY_CTRL)
    { 
      cTimer = millis();
  //Serial.print("C");
      process_SCPI(); // do this faster than we send out messages - or deadly embrace can occur.
     /* loopcntr++;
      if (loopcntr % CYCLES_STEP == 0) // allow several cycles to reach the set point
       { 
          // for voltage step testing
      }
      */
   }
   yield(); // let ESP32 background tasks run
   
   if(millis() > mTimer + PROCESS_EVERY_M)
   {   
 //Serial.print("M");
    mTimer = millis();  
    // detect lost interrupt on adsReady (adSProcessCount should update every 16mS)
    if(adSProcessCount <= lastADS)
    {
       adsReady = true;
    }
    lastADS = adSProcessCount;
 //    Serial.print("1");
     // control inputs: encoder, buttons and touch    
     processTouchSwEnc(); 
  //   Serial.print("2");
     if (dirtyScreen)
       redrawScreen();
    //  Serial.print("3");
     drawReadings(); // only one per cycle
     //    Serial.print("4");
     SCPIgrpTrack();  // send out control group updates 
 //Serial.print("5");
     server.handleClient(); // http
     count++;
 //diagPin(LOW);
  
  }
  yield(); // let ESP32 background tasks run
  
  // lower priority  - screen redraw
  if(millis() > lTimer + PROCESS_EVERY_L)
  {
    //Test(1);
#ifdef C_DEBUG // slow down control for debug
      vCont = control(&pSetA, &pContA);
#endif
     startUp = false; // startUp concluded first time this is called
     lTimer = millis();   
     drawIndicators(currentMenu);
     drawLegends(currentMenu);
     myBlink = !myBlink;  
     if(pSetA.outOn == false && updateZero) // re-calibrate zero current offset
     {
        currentZeroCal();
        updateZero = false;
     }
    
     //valTest(); // test display of values - not for prod
/* // won't need this later - in prod, or stepsSMPS[] in profile.
    // caibrate once the main supply comes on
      if (myADC[VIN].curVal > VDROPOUT && stepsSMPS[SMPS_STEPS/2].volts < VDROPOUT)
        calSMPS();
*/
  } 
  yield(); // let ESP32 background tasks run
  
     // lower priority  - screen redraw
  if(millis() > vTimer + PROCESS_EVERY_VL)
  {
     vTimer = millis();          
      // belongs in M
      //SCPIextTrack(.95); // ************ real value => localLimitRatio
     // heartbeat check on control group 
     // ads count changed since last time
     // release Tracking if no one left in group    
     // save values to EEPROM
     UDPcnt = SCPIgroupHeartbeat();
     saveEE(); 
     updateZero = true; // Current zero offset calibration at next "L" cycle
     //Serial.printf("Active SCPI streams %i, LLR %5.2f\n",UDPcnt, localLimitRatio);
     //Serial.printf("Limit:  %i [%i], Track: %i [%i], Output: %i\n", pContA.limitInd, pSetA.limitOn, pContA.trackInd, pSetA.trackOn, pSetA.outOn);
    //Serial.printf("Setpoint Nom: %5.2F / %5.2F, act: %5.2F\n", pSetA.voltage, pContA.vSetpointR, pContA.vSetpointX);
    //Serial.printf("Fanpin %i, Fan on %i, tRead %3.f\n", FANPIN, fanIsOn, myADC[TEMP].curVal);
  }
  yield(); // let ESP32 background tasks run
}
bool hiLo;
float vxq = 1.0;
void Test(short tnum)
{
/*
  short bd = 2;
  float incr = 0.01;
  tft.fillRect(0, 150,150,60,BGHIGH);
   numtest = true;
 printfloatX((void*)&vxq, "", &FONT1, 1 ,2, 2, bd, 5,  188,  'F', LIGHTYELLOW, LIGHTYELLOW, BGCOL);
 //vxq -= incr;
 //printfloatX((void*)&vxq, "", &FONT1, 1 ,2, 2, bd, 5,  170,  'F', LIGHTYELLOW, LIGHTYELLOW, BGCOL);
 vxq += incr ;
 // printfloatX((void*)&vxq, "", &FONT1, 1 ,2, 2, bd, 5,  206,  'F', LIGHTYELLOW, LIGHTYELLOW, BGCOL);
  numtest = false;
  */
}
void diagPin(bool val)
{
  digitalWrite(TESTPIN, val);
  dp = val;
}
