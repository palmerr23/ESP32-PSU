#ifndef  MYPSU_H
#define  MYPSU_H
/* Configuration information for the PSU (slave) control hardware
*/

#define MODEL "PSU-A"
#define HARD_VERSION 2

//#define ISESP8266
//#define ISNANO
#define ISESP32

// diagnostics
//#define CDEBUG2

#define CS_PIN       10
#define CS_PIN2      0  // not enabled

#ifdef ISESP32
  #define VPIN        35   // ADC1_6 Coarse control VO
  #define VPIN_S      34   // ADC1_7
  #define IPIN        32   // ADC1_4
  #define TPIN        33   // ADC1_5
  #define DACPIN      25   // FINE CONTROL VO
  #include "myESP32ADC.h"

#endif

#ifdef ISNANO
  #define VPIN        A0   // analog read main output voltage (SENSE_A = GPIO34 = ADC1_6) 
  #define VPIN_S      A1   // analog read SMPS output voltage (SENSE_B = GPIO35 = ADC1_7) 
  #define IPIN        A2   // analog read main output current (SENSE_C = GPIO32 = ADC1_4)
  #define TPIN        A3   // analog read heatsink temp (SENSE_C = GPIO32 = ADC1_4)
  #include "myNANOADC.h"
  // uncorrected ADC
  //inHAL ADC_HAL = {&readADCHAL, 0.0, 0.0, 4.74, 0, 0, 10, false};
#endif
/**/
#include "myADS1115.h"
#include "Adafruit_MCP4725.h"
// mutex variable for interrupt exclusion, starts open
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define ADC_CHANNELS 4
#define ADSMUX_START 0  // channel for first reading

#define NUMCHANNELS 2 // number of PSU channels 
uint8_t currentChannel = 0; // index value of current channel (local control) - may be different from SCIPchan

struct chPins{
	uint8_t onPin;
	uint8_t offPin;
} chanPins[NUMCHANNELS] = {
	{SW_ON, SW_OFF}
};

//I2C and limit settings
#define ADS_ADDR0 0x48	// default locations for ADS1115
#define ADS_ADDR1 0x49
//#define ADS_RATE ADS1115_REG_CONFIG_DR_128SPS
#define ADS_RATE ADS1115_REG_CONFIG_DR_250SPS // I2C commands take 500uS, so no point in very fast conversion
//#define ADS_RATE ADS1115_REG_CONFIG_DR_475SPS	// ~10mS for all 4 readings
//#define ADS_RATE ADS1115_REG_CONFIG_DR_860SPS // too fast for all the I2C traffic


#define ADS_GAIN GAIN_TWO // FSD: GAIN_ONE = 4.096V; GAIN_TWO = 2.048V 
#define ADS_MAXV 2.048
Adafruit_ADS1115 ads(ADS_ADDR0);  
Adafruit_MCP4725 dac;

#define DPOT_BASE    0x3D 	// MCP45 I2C address: base = 0x3C
#define DAC_BASE	0x60 	// MCP 4725

#define DAC_MASK 	0x0fff  // 12 bits [0..4096]
#define VOMIN			(0.0)
#define VOMAX		  (32.0)	// max output voltage setting (float)
#define IMIN			(0.01)
#define IMAX			(5)	// max set current
#define SHORTCIRCUIT (6.0)  // When to trip short circuit protection - saved in HAL profile
#define VDROPOUT     (3.6)  // dropout voltage of final reg (i.e. diff between final and SMPS target settings
                            // just enough to regulate well under load
                            // large values will increase final regulator thermal output
//#define DIGI_S      0x3C 	// NOT USED 							
//#define VMAXS		 (32.0)	// max SMPS voltage setting
//#define VREFS		  (1.2)	// reference value (control voltage divider target)
digiPot potV = {0, VOMAX, MCPX51, MCP45X, DPOT_BASE, 1};
//digiPot potS = {VDROPOUT, VMAXS, MCPX51, MCP45X, DIGI_S, 1}; 

// need to define ADS_GAIN  // FSD: GAIN_ONE = 4.096V; GAIN_TWO = 2.048V 
//                ADS_MAXV  // value from ADS GAIN
//= {count, minVolts, midVolts, MaxVolts, midcount, halAddr, adcbits, threepoint}
inHAL ADS_HAL = { 0, 0, 0, ADS_MAXV, 0, 0, 15, false}; // ADS1115: +/- 15 bits 

float  readADStherm(uint8_t signal); // declare before use in struct (ads1115bits.h)
float  readADSvolts1(uint8_t signal);

/*
	= { curval, curvolts, lowError, hiError,  
	    minVal, midVal, maxVal,
	    minVolts, midVolts, MaxVolts, units, iname, channel, threePoint, constrainVals}
*/ 
#define IOUT_OFFSET (0.00001)
#ifdef BOARD_D
	#define IOUT 0
	#define VOUT 1
	#define TEMP 2
	#define VIN  3

inADC myADC[NUMCHANNELS][ADC_CHANNELS] = {
	{
	  // current
			{0.50, 0.0, -0.1, SHORTCIRCUIT, // lowError: input is offset by Vdiode/2 * 4.125 = 0.145V - should not get to this value.
				0.00, 0.0, 10.0,  // maxVal: modified by CALibration
				0.144, 0.0, ADS_MAXV,   "A", "Iout", IPIN, false, false},
			// volts out 
			{12.5, 0.0, -0.3, 30.0,  // lowError: can't go below 0 counts
				0.0, 0.0, 15.32 * ADS_MAXV,  // maxVal: modified by CALibration
				0.00, 0, ADS_MAXV,  "V", "Vout", VPIN, false, false},		  
			// temp: standard curve 10k NTC thermistor , 10k to +3v3 (Vishay NTCALUG02A-103) - parameters for Steinberg equation
			{26.0, 0.0, 0.0, 80.0,  
				0.0, 10000.0, 25.0, // ( XX, R SERIES, NOMTEMP) therm data = 25, cal better at 30 (25-40 C)
				10000, 3984, 3.3,   // (NOM R, B COEFF, DIV VOLTS) 
				"C", "TEMP", TPIN, false, true},
			// volts in (voltsS)
			{8.0, 0.0, -0.1, 35.0, 
				0.0, 0.0, 15.32 * ADS_MAXV, 
				0.00, 0, ADS_MAXV,    "V", "Vin", VPIN_S, false, false} // 15.46 = 4.7K & 68k voltage divider; 11 = 4.7k 7 47k
			
	},{
		{0.50, 0.0, -0.1, SHORTCIRCUIT, // lowError: input is offset by Vdiode/2 * 4.125 = 0.145V - should not get to this value.
		  0.00, 0.0, 10.0,  // maxVal: modified by CALibration
		  0.144, 0.0, ADS_MAXV,   "A", "Iout", IPIN, false, false},
	  // volts out 
	  {12.5, 0.0, -0.3, 30.0,  // lowError: can't go below 0 counts
		  0.0, 0.0, 15.32 * ADS_MAXV,  // maxVal: modified by CALibration
		  0.00, 0, ADS_MAXV,  "V", "Vout", VPIN, false, false},		  
	  // temp: standard curve 10k NTC thermistor , 10k to +3v3 (Vishay NTCALUG02A-103) - parameters for Steinberg equation
	  {26.0, 0.0, 0.0, 80.0,  
		  0.0, 10000.0, 25.0, // ( XX, R SERIES, NOMTEMP) therm data = 25, cal better at 30 (25-40 C)
		  10000, 3984, 3.3,   // (NOM R, B COEFF, DIV VOLTS) 
		  "C", "TEMP", TPIN, false, true},
	  // volts in (voltsS)
	  {8.0, 0.0, -0.1, 35.0, 
		  0.0, 0.0, 15.32 * ADS_MAXV, 
		  0.00, 0, ADS_MAXV,    "V", "Vin", VPIN_S, false, false} // 15.46 = 4.7K & 68k voltage divider; 11 = 4.7k 7 47k
	}};

ADC_callbacks myADCcalls[ADC_CHANNELS] = {
	{&readADSvolts1, &convertADC},
	{&readADSvolts1, &convertADC},
	{&readADSvolts1, &convertTherm},
	{&readADSvolts1, &convertADC}
};

#else  // BOARD_A

	#define VIN  0
	#define VOUT 1
	#define IOUT 2
	#define TEMP 3
inADC myADC[0][ADC_CHANNELS] = {
	  // volts out
	  {8.0, 0.0, 0.0, 31.0, 
		  0.0, 0.0, 15.32 * ADS_MAXV, 
		  0, 0, ADS_MAXV,    "V", "Vin", VPIN_S, false, false}, // 15.46 = 4.7K & 68k voltage divider; 11 = 4.7k 7 47k
	  // volts in (voltsS)
	  {12.5, 0.0, 0.0, 28.0,  
		  0.0, 0.0, 15.32 * ADS_MAXV, 
		  0, 0, ADS_MAXV,  "V", "Vout", VPIN, false, false},
	  // current
	  {0.50, 0.0, -0.20, 5.5, 
		  0.00, 0.0, 10.0,  
		  0.00, 0, ADS_MAXV,   "A", "Iout", IPIN, false, false},
	  // temp: standard curve 10k NTC thermistor , 10k to +3v3 (Vishay NTCALUG02A-103)
	  // these values, or the conversion routine not working well
	  // see struct definition re: thermistor values
	  {30.0, 0.0, 0.0, 90.0,  
		  0.0, 10000.0, 45.0, // therm data = 25, cal better at 30 (25-40 C)
		  10000, 3984, 3.3,  "C", "TEMP", TPIN, false, true}
	};
	ADC_callbacks myADCcalls[ADC_CHANNELS] = {
		{&readADSvolts1, &convertADC},
		{&readADSvolts1, &convertADC},
		{&readADSvolts1, &convertADC},
		{&readADSvolts1, &convertTherm}
	};
#endif

// functions to allow callbacks to library functions
// position in code is significant - after myADC is defined
float readADSvolts1(uint8_t signal){
	return myADC[0][signal].curVolts;
}

// settings BEFORE saved values have been read from EEPROM
// move most of these to the control profile.

//#define SMPS_DAMP (0.9)		  // damping 
#define MCPMAXVAL 255

// ********** calculate these from circuit values
#define STEPS_VOLT     (7.88)   // number of coarse setting steps to change vReadingA by 1V (slightly conservative) 
#define VOLTS_STEP		(0.13)	// volts per coarse setting step
#define F_STEPS_VOLT   (11800)   	// number of fine setting steps to change vReadingA by 1V (slightly conservative) was 900
#define FPOL            (1)   	// polarity of fine control
#define MS_CYCLE		(16)	// how often control() is called in mS (1/4 of ADS_RATE)

// these alter behaviour of control() stop hunting, control dVdT
#define HYSTERESIS 		0       // (MCP steps) Voltage coarse control. 
#define BIGJUMP			10		// when to damp a coarse jump
#define DAMP_VOLT	    (0.8)	// damping factor for big jumps of coarse voltage
#define FHYSTERESIS 	8       // V fine control (slightly more than half the steps/mV value)
//#define SHYSTERESIS 1         // SMPS control
#define DAMP_ILIMIT     (0.8)   // damping factor for current limiting (to reduce hunting)
#define I_HYST			(0.003)	// hysteresis for iLimit
#define IND_OFF			0		// limiting disabled (indicator)
#define IND_EN			1		// limiting enabled
#define IND_ACT			2		// enabled and actively limiting

#define DVDT			(0.1)	// desired maximum rate of voltage change V per mS - used to stop overshoot with big voltage changes
#define DVDC			(DVDT * MS_CYCLE) // max DV per control cycle

// number of setting steps to change vReadingS by 1V, slightly conservative (damped response)
// for SMPS control, dV per step changes (relatively) inversely linearly over the scale. 
//#define S_DELTA			(3.0)	  // big jump needed if the step is greater than this, else increment current step
//#define STEPS_VOLT_SB   (120) // at the bottom  volt setting
//#define STEPS_VOLT_ST  (1)	  // at the top voltage setting

#define WSTART       (0x00)   // start_up (safe) value for main wiper (VO)
#define DSTART       (DAC_MASK/2)   // start fine control mid-range
//#define WSTART_S     (0x20)   // start_up (safe) value for SMPS wiper (polarity = -1 in this case)
#define VSTART       (0.0)    // start_up main voltage
//#define VSTART_S     (VSTART + VDROPOUT)    // SMPS start voltage (dropout)
//#define ISTART      (0.01)    // current reading - irrelevant

#define VINTERNAL_S   (7)     // SMPS voltage required to sustain internal supplies

#define SMALL_DIFF 0.001	// generic value for "significant" difference to floats
#define MEDIUM_DIFF 0.01	// 99%

//#define VCHANNEL    0    // Main
//#define ICHANNEL    1    // Main
//#define TCHANNEL    2    // Heatsink
//#define VCHANNEL_S  3    // SMPS
//#define ADCBITS     12
//#define MAXADC      ((1 << ADCBITS) - 1) // max adc count

// TRACKING MODES
#define TRA_IND	  0
#define TRA_LINK	1
#define TRA_SER	  2
#define TRA_PLL   3
#define TRA_NUM		4

char trackLegend[TRA_NUM][5] = {"Ind", "+/-","Ser","Pll"};
char trackLegendLong[TRA_NUM][16] = {"Independent", "+/-","Series","Parallel"};

/* not yet implemented
struct control {
} myC; 
*/
struct settings { // saved in Profile
	float   voltage;		// set point
	float   current;		// set point
	bool		outEn; 			// channel output is enabled

} pSet[NUMCHANNELS] = {{2,  0.5,  false}, {1,  0.6, false}};

struct mSettings { // saved in Profile
  bool    trackOn;		// group tracking is enabled			 
  uint8_t trackMode;		// unused - TBD (2 channel version - relays)
	bool 		trackSa;		// tracking - set current as group
	bool 		trackSv;		// tracking - set volts as group	
	short   temperature;	// set point for fan on
} mSet = {false, TRA_IND,false, false, 28};

	bool		outOn; 			// outputs are on
	float   eTrack;			// reduction ratio for group set by group (SCPI) TRAC:REDU X.X messages	
  bool    limitOn; 		// local current limiting 
  bool 		twoChans;		// two channels found		
	bool 		WifiConnected;
	
struct controls { // one per PSU board - not saved in Profile
	bool  chFound;			// channel detected
	bool 	limiting;  		// set by control() - MOVE THIS?
	int 	DPOTsetting, 			// startup setting for MCP
				DACsetting;  		// current voltage setting (DAC units)	
	float vSetpointX, 	// final value actually used
				vSetpointR, 		// remote tracking
				vSetpointI, 		// current limit
				vSetpointS,     	// nominal setting 
				localLimitRatio,
				vLast,vLastSet, watts;	
	bool iLimit, oldTrack;
	int8_t limitInd, trackInd; // easy indicators: 0==disabled; 1==enabled; 2==limiting/track-limiting
} pCont[NUMCHANNELS] = {{true,WSTART, DSTART, 
			0,0,0,0,1,0,0,0, 
			false, false,5,7},
			{true,WSTART, DSTART, 
			0,0,0,0,1,0,0,0, 
			false, false,5,7}};
			
#define chanBfound pCont[1].chFound	// master flag for single/dual channel operation

// calibration menu
float calOff_IA = 0.00, // channel A
	  calOff_VA = 0.0, // setting variables for offset calculation 
		calOff_IB = 0.0, // channel B
		calOff_VB = 0.0;
		
struct channelDef { // static or set up at detection
	settings *sp;		//
	inHAL    *hp;
	inADC    *ap;
	controls	 *cp;
	uint8_t ADCaddr,	// I2C addresses for hardware devices
					DACaddr,
					DPOTaddr;
	uint8_t potV;
	uint8_t potS;	
} chDef[NUMCHANNELS] = { 
		{&pSet[0], &ADS_HAL, &myADC[0][0], &pCont[0], ADS_ADDR0, 0, 0, DPOT_BASE, 0},
		{&pSet[1], &ADS_HAL, &myADC[1][0], &pCont[1], ADS_ADDR1, 0, 0, DPOT_BASE, 0}
	};

short  highButton = -1;	// currently selected screen button
bool fanIsOn; // fan on indicator

// SCPI identification 
#define IDN_MANUF "SiliconChip"
#define IDN_MODEL MODEL
#define IDN_SERIAL "2001"
#define IDN_FIRMWARE SOFT_VERSION
#define IDN_OPTS "NONE"

#endif
