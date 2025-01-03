/* myInst.h
Basic definitions for all instruments and the Control Module
*/
#ifndef MYINST_H
#define MYINST_H


#ifdef ILI9488
// need to include this before the pin (re)definitions
#include <Arduino_GFX_Library.h>
#endif

#define ESP_32

#include <EEPROM.h>
#include <WiFi.h>

#define EEAUTOSAVE 1	// 1 for save to EEPROM (production), 0 (!= 1) for no save
#define SAVE_EE_AFTER 6	// VL cycles - 6 for production

struct NullStream : public Stream{ // bit bucket for production debug info
 NullStream( void ) { return; }
 int available( void ) { return 0; }
 void flush( void ) { return; }
 int peek( void ) { return -1; }
 int read( void ){ return -1; };
 size_t write( uint8_t u_Data ){ return u_Data, 0x01; }
};
NullStream noDebug;

// switch Serial debugging on and off
#define SDEBUG1 Serial	// testing - information
//#define SDEBUG1 noDebug // production - information

#define SDEBUG2 Serial	// testing - errors
//#define SDEBUG2 noDebug // production - errors

#define IDLEN 16
char dns_id[IDLEN] = "ESP32_5";
#ifdef BOARD_E // may not need this one - same as D?
// BOARD REV E - Control v13 onwards
#define ENC_A 		32  // Scrubber control encoder 
#define ENC_B 		33  // Note: Encoder switch is NC
#define CNTRL_R 	14	// Scrubber buttons 
#define CNTRL_L 	27
#define TFT_DC 		13
//#define TFT_IRQ 	255  // jumper is open
#define TFT_RST 	255 //-1
#define TFT_CS       5
#define TFT_MOSI	23
#define TFT_CLK 	18
#define TFT_MISO 	19
#define RXD2 		16 
#define TXD2 		17
#define ADSRDY 		2
#define DACPIN 		25
#define FANPIN		26  // GPIO 16/17 are NC on WROVER - REvC boards need to patch FAN from CONTROL Pin 10 to pin 14.
#define SW_ON 		4 	// hi is active 
#define SW_OFF 		12  // hi is active 
#define TFT_TOUCH_CS 15
#define TFT_TOUCH_IRQ 255 // not used - jumper is open

#elif defined(BOARD_D)
// BOARD REV D - Control v13 onwards
#define ENC_A 		32  // Scrubber control encoder 
#define ENC_B 		33  // Note: Encoder switch is NC
#define CNTRL_R 	14	// Scrubber buttons 
#define CNTRL_L 	27
#define TFT_DC 		13
//#define TFT_IRQ 	255  // jumper is open
#define TFT_RST 	255 //-1
#define TFT_CS       5
#define TFT_MOSI	23
#define TFT_CLK 	18
#define TFT_MISO 	19
#define RXD2 		16 
#define TXD2 		17
#define ADSRDY 		2
#define DACPIN 		25
#define FANPIN		26  // GPIO 16/17 are NC on WROVER  - REvC boards need to patch FAN from CONTROL Pin 10 to pin 14.
#define SW_ON 		4 	// hi is active 
#define SW_OFF 		12  // hi is active 
#define TFT_TOUCH_CS 15
#define TFT_TOUCH_IRQ 255 // not used - jumper is open

#elif defined(BOARD_B)
// BOARD REV B - Control v10 onwards
#define ENC_A 		32  // Scrubber control encoder 
#define ENC_B 		33  // Note: Encoder switch is NC
#define CNTRL_R 	14	// Scrubber buttons 
#define CNTRL_L 	27
#define TFT_DC 		13
//#define TFT_IRQ 	255  // jumper is open
#define TFT_RST 	255 //-1
#define TFT_CS       5
#define TFT_MOSI	23
#define TFT_CLK 	18
#define TFT_MISO 	19
#define RXD2 		16 
#define TXD2 		17
#define ADSRDY 		2
#define DACPIN 		25
#define FANPIN		16
#define SW_ON 		4 	// hi is active 
#define SW_OFF 		12  // hi is active 
#define TFT_TOUCH_CS 15
#define TFT_TOUCH_IRQ 255 // not used - jumper is open

#else // BOARD_A - first prototype Control to v6
// BOARD REV A
#define ENC_A 		13	// Scrubber control encoder 
#define ENC_B 		26	// Note: Encoder switch is NC
#define CNTRL_R 	12	// Scrubber buttons 
#define CNTRL_L 	14
#define TFT_DC 		16
#define TFT_RST 	17  
#define TFT_IRQ 	2   //Jumper is NC
#define TFT_CS 		5
#define TFT_MOSI 	23
#define TFT_CLK 	18
#define TFT_MISO 	19
#define RXD2 		255 // not available on BOARD_A
#define TXD2 		255
#define ADSRDY 		27
#define DACPIN 		25
#define FANPIN		255
#define SW_ON 		4	// hi is active 
#define SW_OFF 		0	// hi is active 

#define TFT_TOUCH_CS 15
#define TFT_TOUCH_IRQ 255 // not used
#endif

#define L_BUT  CNTRL_L
#define R_BUT  CNTRL_R


// Generics
#define SENSE_A 	34 // GPIO or ADC
#define SENSE_B 	35
#define SENSE_C 	32
#define SENSE_D 	33


#define TFT_SS 		5

#define SPI_SCK 	18
#define MISO 		19
#define MOSI 		23
#define T_IRQ 		2
#define T_CS 		15

#define SDA 		21
#define SCL 		22

#define OPTO_IN 	3 	// RXD
#define OPTO_OUT 	1 	// TXD

#define I2C_SPEED	400000

// ***** rotary encoder and L-R switches
//#define ENCODER_DO_NOT_USE_INTERRUPTS 
#include "ESPRotary.h"
#include "Button2.h"
#define DEBOUNCE 10
#define ENC_CLIX 2
//Button2 but  = Button2(ENC_SW); // not used
Button2 butL = Button2(L_BUT, INPUT_PULLDOWN, false, DEBOUNCE);
Button2 butR = Button2(R_BUT, INPUT_PULLDOWN, false, DEBOUNCE);
ESPRotary enc = ESPRotary(ENC_B, ENC_A, ENC_CLIX);
#define ADSBITS 15

// Hardware abstraction for ADCs (HAL) aids Counts to Volts translation
// One record per DEVICE (if all channels have similar characteristics).
// HAL definitions are hidden from user code - simply accessed by calling the appropriate readHALXXX() function.
// each HAL needs to implement two functions:
// 		beginXXX(); which is called from setup, initialising the hardware
// 		float readHALXXX(uint16_t devAddr, uint8_t ADCchannel); which is called each time a reading is required, read value and return as volts.
 // float (* readHAL)(uint16_t devAddr, uint8_t ADCchannel);  // service routine to 
// 		  devAddr is the device address (if required, e.g. for I2C, or the CS pin number for SPI)
//        ADCchannel (for multi channel devices, or pin numbers for AnalogRead) is a reference to the individual input within the device and is inADC.ADCchannel
// **************** readHAL(, ADCchannel, ) is REDUNDANT - has same info as hp->ADCchannel
struct inHAL {
  volatile int curCount; // last reading
  float    minVolts;     // values are for scaling, real values do not need to reach 0 or full count
  float    midVolts;     // three point curve, if non-linear translation. Ignored if threePoint == false. 
  float    maxVolts;     // volts at maximum count. I.e. @ count = (1 << ADCbits) - 1
  uint16_t midCount;     // Ignore00d if threePoint == false.
  uint16_t HALaddr;      // hardware address for ADC (e.g. I2C address); inbuilt ADC pins should use inADC.ADCchannel
  uint8_t  ADCbits;      // e.g. 10, 12, 15, 16, 24
  bool     threePoint;   // three point curve?  
};

// One record per ADC input
// Contains setpoints and current value of input
// Points to HAL for interface with hardware
// Aids conversion from Volts to Real-world values for processing
// Each ADC needs to implement one function: 		
// 		convert(float volts); which is called each time a conversion from volts to real world values is required
// often this will be called as realWorld = convert(readHAL(devAddr, ADCchannel));
struct inADC {
  float curVal;   // last reading in real-world units
  float curVolts; // last reading in volts
  // setPoint belongs in a control struct, not a convert struct - see struct settings
 // float   setPoint;		   // control target in real-world units
  float   lowError;        // trigger error action - value too low
  float   hiError;		   // trigger error action (turn things off) - e.g. overvoltage, short circuit, over temp
  // conversion factors - xxxVal results from  xxxVolts
  float   minVal;          // value in real-world units at minVolts  [ignored for thermistors]
  float   midVal;          // used for 3 point linearity correction. Ignored if threePoint == false. [RSERIES for thermistors]
  float   maxVal;          // value in real-world units at maxVolts [TEMPERATURENOMINAL for thermistors]
 
  float   minVolts;        // min measured value in volts  [THERMISTORNOMINAL R for thermistors]
  float   midVolts;        // used for 3 point linearity correction. Ignored if threePoint == false. [BCOEFFICIENT for thermistors]
  float   maxVolts;        // max measured(V) at [vMax = VCC for thermistors]
 // inHAL *HALptr;         // pointer to HAL structure for service routines, etc
  char    units[4];        // name of the unit e.g. "mA" or "V"
  char    iname[6];		   // name of the control (e.g. "Vout")
  uint8_t ADCchannel;      // channel within device or ADC pin
  bool    threePoint;      // turn on three-point curve processing (used for ESP32 - non-linear ADC)
  bool    constrainVals;   // constrain values: force minVolts <= readVolts <= maxVolts?  
 // bool	  dummy[3];				// bring it up to a 32 bit boundary
};

struct ADC_callbacks {  // removed from inADC, so that pointers are not stored in EEPROM
  float (* readVolts)(uint8_t channel);  // service routine to convert read from ADC and covert to volts read
  float (* convert)(float volts, inADC * ap);  // service routine to convert volts read to real world value
};

/*
	contains all stored identification data - name, network, etc
*/
#define SSIDLEN 34		// > 32 + null
#define NETPASSLEN 64	// > 63 + null
#define NAMELEN 16		// displayed name, used to identify
uint8_t netCIDR = 24;	// default Class C network - to calculate netmask 

struct netParams { // general network spec
  IPAddress local, gateway, subnet;
  bool autoconnect; // maybe not relevant, other than to enable AP mode?
};

struct wifiParams { // Wifi credentials
  char ssid[SSIDLEN],
       pass[NETPASSLEN];
  bool autoconnect;  
};

#ifdef REALNET
	#define LOCAL_SS "PalHome"
	#define LOCAL_PASS "AF18181F9C4516EE36AF9D79D3"
#else
	#define LOCAL_SS "XP"
	#define LOCAL_PASS "E36AF9D79D3"
#endif	

//wifiParams homeNet = {"PalHome", "AF18181F9C4516EE36AF9D79D3", true};
//wifiParams homeNet = {"XP", "E36AF9D79D3", true};
struct instID {
	IPAddress prefIP;	// preferred IP
	IPAddress gateIP;	
	IPAddress netmask;	
    char local_ssid[SSIDLEN];
	char local_pass[NETPASSLEN];	// always assumes DHCP on WiFi
	char esp_ssid[SSIDLEN];
	char esp_pass[NETPASSLEN];	// always assumes DHCP on WiFi
	bool autoConnect;			// maybe "enabled"?
	char instName[NAMELEN];
};

instID myID = {{192,168,50,1}, {192,168,50,1},{192,168,50,0}, LOCAL_SS, LOCAL_PASS, "ESPINST", "ESPPW99X", true, "MYPSU"};
bool IamAP = false;	// am I the AP?

// screen pixel limits
// Portrait mode
// some sx() and sy() use thes max values
#define HXMAX 240
#define VYMAX 320
#ifdef ILI9341
	#define HMAX 240
	#define VMAX 320
	#define SCREENROT 4 
	#define TOUCHROT 2
#else // ILI9488
	#define HMAX 320
	#define VMAX 480
	#define SCREENROT 2 
	#define TOUCHROT 2
#endif

// various global defs
#define UDP_PORT 8888 
#define TELNET_PORT 5025 // SCPI std
#define MAXTRACKGRP 127
#define HOSTLEN 48
IPAddress myIP 			= {192,168,1,200};
IPAddress myBroadcastIP = {192,168,1,255};
IPAddress mySubnetMask  = {255,255,255,0};
char myHostName[HOSTLEN];
char IPstring[16];
uint8_t myNetCDIR = 24;

bool dirtyScreen = true; // any routine can cause the screen to be redrawn
//bool valChanged(VAL_CHGD);
volatile bool changedVal_scrn = false, changedVal_EE = false, changedVal_SCPI = false; // changed settings values need to be addressed by several modules
bool needToSaveEE = false;

// on screen keyboard active - disable various functions
bool oskOn = false;


#define SCPI_BUFLEN 256
#define SCPI_SHORTCMD 4
// SCPI stream timeouts (Seconds) -1 == never
#define SCPIUDPTIMEOUT 		10	// (S)
#define SCPISERIALTIMEOUT 	-1	// (S)
#define SCPITELNETTIMEOUT 	-1	// (S) 
#define HEART_MSG "HEARTBEAT"
/* explicit includes in main prog
#if defined(ESP_32) || defined(ESP_8266)
#include "myWiFi.h"
//#include "myHTTP.h"
//#include "myUDP.h"
#endif
*/
// test values - not for production
float xDumVal = 0.0;
#endif /* MYINST_H */