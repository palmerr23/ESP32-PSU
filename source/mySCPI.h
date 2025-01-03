/*
SCPI definitions and service routines
Command tree is below
System errors
-100	Syntax error (could not parse command)
-200	Execution error (e.g. parameter outside range)
-300	Device error (e.g. overrange error state: short circuit protection, overvoltage on SMPS)

Device errors
108		not yet implemented

v2
-203	Command protected (needs password)
-210	Can't trigger
-310	System error
-400	Query error (e.g. no data to provide)

105		data not found
106 	measurement not found
122		Channel not found
*/
#ifndef  MYSCPI_H
#define  MYSCPI_H
#include "scpiparser.h"
short SCPIsendGrpMessage(uint8_t tgrp, char *buf);
//uint8_t streamIn[currentStreamIn].channel = 0;  // current channel set by :INST:CHAN (value is: CHAN -1)


struct scpi_parser_context ctx;
bool SCPIheartbeatSent;
char myError[64];
char valBuf[64];
float lastVset = 9999, lastCset = 9999;

// buffer status
#define SCPIBUF_FREE   (-1) // not active stream , may be re-used
#define SCPIBUF_IDLE    0   // ready for a new cycle
#define SCPIBUF_INPUT   1   // set with first char received
#define SCPIBUF_PROCESS 2   // EOL received, passed to processor, reset to free when response has been sent.

// command stream status/type - only one needed per type of connection as each command is 
#define SCPISTREAM_FREE (-1)	// duplicates function of SCPIBUF_FREE
#define SCPISTREAM_SER (-2)
#define SCPISTREAM_TEL (-3)
//#define SCPISTREAM_UDP  [0..255]

#define SCPI_INBUFS 6
short SCPIsource = SCPISTREAM_FREE;   // which type of stream did the last command come from?
short SCPIstreamID = SCPISTREAM_FREE; // stream within each source type. ip[3] for UDP
uint8_t currentStreamIn;		      // current input stream (index)   

char SCPIoutBuf[SCPI_BUFLEN]; 		// only one needed, as we only process one SCPI command at a time
struct scpiStrm {	
	char buf[SCPI_BUFLEN];  		// one per input stream - 1 x Serial; 1 x Telnet, n x UDP
	uint8_t len;					// content length
	unsigned short indx;    		// current buffer pointer (for serial - character stream, rest are block) == len????
	int16_t streamID;         		// type of stream - see defines above [0.255] for UDP, etc
	uint16_t remotePort;			// for UDP packet responses
	uint8_t channel = 0;		    // instrument channel last addressed
	int8_t status;					// status of this buffer/connection
	float eTrack;					// last tracking value sent on this stream (active value will be min of active UDP stream values and local value)
	long lastMessage;				// (Timer Seconds) last message received (will time out after different periods for different stream types)
} streamIn[SCPI_INBUFS];  

// mandatory
scpi_error_t system_error(struct scpi_parser_context* context, struct scpi_token* command);

scpi_error_t identify(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_voltage(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_voltage_i(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_current(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_temp(struct scpi_parser_context* context, struct scpi_token* command);

scpi_error_t set_voltage(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_current(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_protect(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_output(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_channel(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_estop(struct scpi_parser_context* context, struct scpi_token* command);

scpi_error_t get_set_voltage(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_current(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_protect(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_output(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_channel(struct scpi_parser_context* context, struct scpi_token* command);

// tracking settings
scpi_error_t set_track_group(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_track_group(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_track_enable(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_track_enable(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_track_v_ena(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_track_v_ena(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_track_c_ena(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_track_c_ena(struct scpi_parser_context* context, struct scpi_token* command);
// live control
scpi_error_t set_track_reduce(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_track_reduce(struct scpi_parser_context* context, struct scpi_token* command);
// Tracking group estop is the same function as SCPI command: set_estop() 
scpi_error_t set_track_volts(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_track_current(struct scpi_parser_context* context, struct scpi_token* command);


scpi_error_t set_ssid(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_ssid(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_pass(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_pass(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_connect(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_connect(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_hostName(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t get_set_hostName(struct scpi_parser_context* context, struct scpi_token* command);

void SCPI_setup(){	
  struct scpi_command* source;
  struct scpi_command* measure;
  struct scpi_command* instrument;
  struct scpi_command* track;
  struct scpi_command* system;
  struct scpi_command* error;
//  struct scpi_command* comms;

  /* First, initialise the parser. */
  scpi_init(&ctx);
  /*
  **** NOTE :SYSTEM:ERROR must be registered (also SYS  & ERR)
   * Command tree
   *  *IDN?          				-> identify (pre-registered in library code)
   *** *TST?						-> run self test (pre-registered in library code?)
   *  :INSTrument
   *    :NAME  		<string>      	-> set name
   *    :NAME?       				-> get name
   *    :CHANnel  	<CHx>   		-> set channel number
   *    :CHANnel?    				-> get current channel
   *	:ESTOP		 				-> turn all channels on this instrument OFF
   *  :SOURCce						-- Control values
   *    :VOLTage  	<float[V]>  	-> set_voltage
   *    :VOLTage?    				-> get set_voltage
   *    :CURRent   	<float[A]>  	-> set_current
   *    :CURRent?    				-> get set_current
   *    :OUTPut   	<ON/OFF>   		-> set output on/off 
   *    :OUTput?     				-> get output state   
   *	:PROTection	<ON/OFF>		-> set current limiting setting (on / off)
   *	:PROTection? 				-> get current limiting setting (on / off)
   * 	:TEMP?		 				-> get heatsink temperature
   *  :MEASure						-- Instrument readings
   *    :VOLTage?    				-> get_voltage
   *    :CURRent?    				-> get_current
   *    :IVOLtage?   				-> get_voltage_i
   *  :TRACk						-- Instrument GROUP Controls
   *    :GROUp     <int>   			-> set Tracking group
   *    :GROUp?       				-> get Tracking group
   *    :ENABle    <ON/OFF>   		-> enable/disable tracking
   *    :ENABle?      				-> ENABle status   
   *	:VENAble	 <ON/OFF>  		-> enable remote voltage setting
   *	:VENAble?
   *	:CENAble	 <ON/OFF>  		-> enable remote current setting
   *	:CENAble?
   *    :VOLTage  	<float[V]>  	-> set_voltage (group). Query not required as this command sets the instrument V.
   *    :CURRent   	<float[A]>  	-> set_current (group). Query not required.
   *    :REDUce?      				-> get current group output voltage ratio when tracking 
   *	:REDUce		<GRP:float>  	-> set group output voltage ratio (2 args: GRP_ID:RATIO passed as a string)
   *    :ESTOP        				-> turn all group's outputs off, when short circuit protection, etc occurs (arg GRP_ID)
   *  :SYSTem						-> system commands
   *    :SSID <string 8>		-> SSID of WiFi LAN
   *    :SSID?
   *	:PASSphrase <string 32> -> WiFi passphrase
   *	:PASSphrase?
   *	:AUTOconnect <ON/OFF>   -> Connect to WiFi
   *	:AUTOconnect?
   *	:HOSTname				-> Hostname (mDNS)
   *	:HOSTname?
To be done:
   ***    :MODE   <GRP:int>      	-> set tracking mode - see SIGLENT SPD3303. Mode 1: +/-, group set volts; 
																				Mode 2: Series, group set volts & current, also display total volts; 
																				Mode 3: Parallel, group set volts & current, also display total current.
   ***    :MODE?        			-> get tracking mode
  

   */
   	// mandatory registrations
	system = scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "SYSTEM", 6,  "SYST", 4, NULL);	
	
	error = scpi_register_command(system, SCPI_CL_CHILD, "ERROR", 5,"ERR", 3, NULL);									   
	scpi_register_command(system, SCPI_CL_CHILD, "ERROR?", 6, "ERR?", 4, system_error);	
	scpi_register_command(error, SCPI_CL_CHILD, "NEXT?", 5, "NEXT?", 5, system_error);
	
  
 //scpi_register_command(parent, location, long_name, long_name_length, short_name, short_name_length, callback);  
  scpi_register_command(ctx.command_tree, SCPI_CL_SAMELEVEL, "*IDN?", 5, "*IDN?", 5, identify);
  // tree roots
  source =     scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "SOURce",      6, "SOUR", 4, NULL);
  measure =    scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "MEASure",     7, "MEAS", 4, NULL);
  instrument = scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "INSTrument", 10, "INST", 4, NULL);
  track =      scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "TRACk",       5, "TRAC", 4, NULL);
  //systm =      scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "SYSTem",      6, "SYS", 3, NULL);
// comms =      scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "COMMs",      5, "COMM", 4, NULL);

  // SOURCE defs
  // channels
  scpi_register_command(source, SCPI_CL_CHILD, "CHANnel", 7,  "CHAN", 4, set_channel);
  scpi_register_command(source, SCPI_CL_CHILD, "CHANnel", 8, "CHAN?", 5, get_set_channel);
  // setpoints
  scpi_register_command(source, SCPI_CL_CHILD, "VOLTage", 7,     "VOLT", 4, set_voltage);
  scpi_register_command(source, SCPI_CL_CHILD, "CURRent", 7,     "CURR", 4, set_current);
  scpi_register_command(source, SCPI_CL_CHILD, "PROTection", 10, "PROT", 4, set_protect);
  scpi_register_command(source, SCPI_CL_CHILD, "OUTPut", 6,      "OUTP", 4, set_output);
  // query setpoints
  scpi_register_command(source, SCPI_CL_CHILD, "VOLTage?", 8,     "VOLT?", 5, get_set_voltage);
  scpi_register_command(source, SCPI_CL_CHILD, "CURRent?", 8,     "CURR?", 5, get_set_current);
  scpi_register_command(source, SCPI_CL_CHILD, "PROTection?", 11, "PROT?", 5, get_set_protect);
  scpi_register_command(source, SCPI_CL_CHILD, "OUTPut?", 7,      "OUTP?", 5, get_set_output);
  // MEASURE defs
  // get read values
  scpi_register_command(measure, SCPI_CL_CHILD, "VOLTage?", 8,      "VOLT?", 5, get_voltage);
  scpi_register_command(measure, SCPI_CL_CHILD, "CURRent?", 8,      "CURR?", 5, get_current);
  scpi_register_command(measure, SCPI_CL_CHILD, "IVOLtage?", 9,     "IVOL?", 5, get_voltage_i);
  scpi_register_command(measure, SCPI_CL_CHILD, "TEMPerature?", 12, "TEMP?", 5, get_temp);
  // INSTRUMENT defs
  scpi_register_command(instrument, SCPI_CL_CHILD, "CHANnel", 7,  "CHAN",   4, set_channel);
  scpi_register_command(instrument, SCPI_CL_CHILD, "CHANnel?", 8, "CHAN?",  5, get_set_channel);
  scpi_register_command(instrument, SCPI_CL_CHILD, "ESTOp", 5,    "ESTO",   4, set_estop);

  // TRACK defs
  scpi_register_command(track, SCPI_CL_CHILD, "GROUp", 5,      "GROU",  4, set_track_group);
  scpi_register_command(track, SCPI_CL_CHILD, "GROUp?", 6,     "GROU?", 5, get_set_track_group);
  scpi_register_command(track, SCPI_CL_CHILD, "ENABle", 6,     "ENAB",  4, set_track_enable);
  scpi_register_command(track, SCPI_CL_CHILD, "ENABle?", 7,    "ENAB?", 5, get_set_track_enable);
  scpi_register_command(track, SCPI_CL_CHILD, "VENAble", 7, "VENA",  4, set_track_v_ena);  
  scpi_register_command(track, SCPI_CL_CHILD, "VENAble?", 8, "VENA?",  5, get_set_track_v_ena);
  scpi_register_command(track, SCPI_CL_CHILD, "CENAble", 7, "CENA",  4, set_track_c_ena); 
  scpi_register_command(track, SCPI_CL_CHILD, "CENAble?", 8, "CENA?",  5, get_set_track_c_ena); 
  // // scpi_register_command(track, SCPI_CL_CHILD, "MODE", 4,    "MODE",  4, set_track_mode);  
  // // scpi_register_command(track, SCPI_CL_CHILD, "MODE?", 5,   "MODE?", 5, get_track_mode);
  // TRACK dynamic settings
  scpi_register_command(track, SCPI_CL_CHILD, "VOLTage", 7, "VOLT",  4, set_track_volts);  
  scpi_register_command(track, SCPI_CL_CHILD, "CURRent", 7, "CURR",  4, set_track_current); 
  scpi_register_command(track, SCPI_CL_CHILD, "REDUce?", 7,   "REDU?", 5, get_track_reduce); 
  scpi_register_command(track, SCPI_CL_CHILD, "REDUce", 6,    "REDU",  4, set_track_reduce); 
  scpi_register_command(track, SCPI_CL_CHILD, "ESTOP", 5,     "ESTO",  4, set_estop); // same as INSTrument version
  
  // SYSTEM (Comms & ID) defs
  scpi_register_command(system, SCPI_CL_CHILD, "SSID",4,           "SS",    2, set_ssid);
  scpi_register_command(system, SCPI_CL_CHILD, "SSID?", 5,         "SS?",   3, get_set_ssid);
  scpi_register_command(system, SCPI_CL_CHILD, "PASSphrase",10,    "PASS",  4, set_pass);
  scpi_register_command(system, SCPI_CL_CHILD, "PASSphrase?", 11,  "PASS?", 5, get_set_pass);
  scpi_register_command(system, SCPI_CL_CHILD, "AUTOconnect",11,   "AUTO",  4, set_connect);
  scpi_register_command(system, SCPI_CL_CHILD, "AUTOconnect?", 12, "AUTO?", 5, get_set_connect);
  scpi_register_command(system, SCPI_CL_CHILD, "HOSTname",8,       "HOST",  4, set_hostName);
  scpi_register_command(system, SCPI_CL_CHILD, "HOSTname?",9,      "HOST?", 5, get_set_hostName);

  
	for (int i = 0; i < SCPI_INBUFS; i++)
	{
		streamIn[i].indx = 0; // empty buffer
		streamIn[i].status = SCPIBUF_FREE;
		streamIn[i].streamID = SCPISTREAM_FREE;
	}
}
short getSCPIbuf(short stream)
{
	short i;
	short freeBuf = -1;
//Serial.printf("Getbuf: seeking %i. ", stream);
	// look for existing buffer, tag free ones along the way
	for (i = 0; i < SCPI_INBUFS; i++)
	{
		if (streamIn[i].streamID == stream)
		{
			//Serial.printf("Found existing %i ", i);
			streamIn[i].streamID = stream;
			streamIn[i].lastMessage = millis() / 1000;
			return i;		
		}

		if (streamIn[i].status == SCPIBUF_FREE) // will return the last free buf id
			freeBuf = i;
	}
	//Serial.printf("New %i ", freeBuf);
	if(freeBuf > 0)
	{
		streamIn[freeBuf].streamID = stream;
		streamIn[i].lastMessage = millis() / 1000;
	}
	return freeBuf;
}
// for now, just serial terminal
// handle one message each cycle, save who it's from
// returns buffer ID when a complete message has been received.
// ***************************** Needs to be non blocking code ******************
short serialBufID = SCPISTREAM_FREE;
short telnetBufID = SCPISTREAM_FREE;
short UDPBufID = SCPISTREAM_FREE;
bool directCommand = false; // do TRAC commands need to be re-broadcast?
short SCPIread(void){
	// no delayed commands allowed
	// read available chars from each stream, process first one to complete a line 
	// read available serial into buffer
	//short bufID;
	//Serial.print(":");
	if (Serial.available() > 0) 
	{
		//Serial.print("*");
		if(serialBufID == SCPISTREAM_FREE) // won't release this, if Serial is being used
		{
			serialBufID = getSCPIbuf(SCPISTREAM_SER); 
			streamIn[serialBufID].status = SCPIBUF_INPUT;
			streamIn[serialBufID].indx = 0;			
		}		
		
		while (Serial.available())
		{
			streamIn[serialBufID].buf[streamIn[serialBufID].indx] = Serial.read();
			//Serial.print(streamIn[serialBufID].buf[streamIn[serialBufID].indx]); //echo to serial
			// if EOL return the bufferID
			if(streamIn[serialBufID].buf[streamIn[serialBufID].indx++] =='\n')
			{
				streamIn[serialBufID].indx--;
				streamIn[serialBufID].buf[streamIn[serialBufID].indx] = '\0'; // kill the '\n'
				streamIn[serialBufID].status = SCPIBUF_PROCESS;
				streamIn[serialBufID].len = strlen(streamIn[serialBufID].buf);
				SCPIsource = SCPISTREAM_SER;
				currentStreamIn = serialBufID;
				directCommand = true;
				return currentStreamIn;
			}				
		}	
		
		streamIn[serialBufID].lastMessage = millis()/1000;
	}
	// process telnet
	char *iptr;
	iptr = getTelnet(); // '\n' already dealt with in here
	if (iptr)
	{
		if(telnetBufID == SCPISTREAM_FREE) // won't release this, if Telnet is being used
		{
			telnetBufID = getSCPIbuf(SCPISTREAM_TEL); 
			streamIn[telnetBufID].status = SCPIBUF_PROCESS;					
		}
		streamIn[telnetBufID].lastMessage = millis()/1000;		
		strcpy(streamIn[telnetBufID].buf, iptr);
		streamIn[telnetBufID].len = strlen(iptr);
		SCPIsource = SCPISTREAM_TEL;
		currentStreamIn = telnetBufID;
		directCommand = true;
		return currentStreamIn;		
	}
	// process UDP packets
	// search through buffer pile for active UDP buffers
	iptr = getUDP();
	if (iptr)
	{
		// separate streams for each remote host
		UDPBufID = getSCPIbuf(UDPsourceIP[3]); // may release this, if exceeds SCPIUDPTIMEOUT
		//Serial.printf("UDP pkt in [%s] to buffer %i from IP[%i]\n", iptr, UDPBufID,UDPsourceIP[3]);
		streamIn[UDPBufID].status = SCPIBUF_PROCESS;	
		streamIn[UDPBufID].remotePort =	UDPsourcePort;	
		strcpy(streamIn[UDPBufID].buf, iptr);
		streamIn[UDPBufID].len = strlen(iptr);
		SCPIsource = UDPsourceIP[3];
		currentStreamIn = UDPBufID;
		streamIn[UDPBufID].lastMessage = millis()/1000;
		directCommand = false;
		return currentStreamIn;	
	}
	
	// nothing waiting
	directCommand = false;
	return SCPISTREAM_FREE;
}

// call from main loop
short SCPIreadBufID; // used to echo group commands from Serial or Telnet
void  process_SCPI(void)
{

  short stat;
  
  SCPIreadBufID = SCPIread(); // we only process one command from one stream each time around
  
  if(SCPIreadBufID != SCPISTREAM_FREE)
  {
	//Serial.printf("SCPI_proc: buf [%s], len = %i\n", streamIn[SCPIreadBufID].buf, strlen(streamIn[SCPIreadBufID].buf));
    stat = scpi_execute_command(&ctx, streamIn[SCPIreadBufID].buf, strlen(streamIn[SCPIreadBufID].buf)); 
	if (stat < 0 && strcmp(streamIn[SCPIreadBufID].buf, HEART_MSG))
		Serial.printf("SCPI_proc: unknown cmd [%s]\n", streamIn[SCPIreadBufID].buf);
	streamIn[SCPIreadBufID].indx = 0;
	streamIn[SCPIreadBufID].status = SCPIBUF_IDLE;
  }
}

// send response to requestor
void SCPItrans(char * response)
{
	bool sentOK;
	// which is the current stream?
	if(SCPIsource == SCPISTREAM_SER)
	{
		Serial.print(response);
		return;
	}
	
	if (SCPIsource == SCPISTREAM_TEL)
	{
		//Serial.printf("SCPIOUT Telnet: %s\n", response); // for now!
		sentOK = putTelnet(response);
		if(!sentOK)
			Serial.printf("SCPIOUT Telnet: unable to send\n");
		return;
	}
		
	if (SCPIsource >= 0) // UDP streams
	{
		IPAddress sendIP = myIP;
		sendIP[3] = streamIn[currentStreamIn].streamID;
		//Serial.printf("SCPI UDP OUT [%i]: %s\n", SCPIsource, response); // for now!
		// not broadcast - point to point response ********* do we ignore all messages outside our track grp, or just broadcast ones??
		//putUDP(streamIn[currentStreamIn].buf, sendIP, streamIn[currentStreamIn].remotePort, pSet[0].trackGrp, 0);
		//streamIn[currentStreamIn].status = SCPIBUF_IDLE;
		return;
	}
}

// send response to a tracking group
// ********** NOT used ****** See SCIPgrpMsg
void SCPItransGrp(char * response, uint8_t groupID){
	// UDP broadcast
  //Serial.println(response);
}

/*
 * Respond to *IDN?
 */
scpi_error_t identify(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  scpi_free_tokens(command);
  sprintf(SCPIoutBuf,"%s,%s,%s,%i,%s\n", IDN_MANUF, IDN_MODEL, IDN_SERIAL,IDN_FIRMWARE,IDN_OPTS);
  SCPItrans(SCPIoutBuf);
 // Serial.println("OIC,Embedded SCPI Example,1,10");
  return SCPI_SUCCESS;
}

/******************** READ LIVE VALUES ***********************/
/* Read the output voltage */
scpi_error_t get_voltage(struct scpi_parser_context* context, struct scpi_token* command)
{
  float voltage;
  voltage = (chDef[streamIn[currentStreamIn].channel].ap + VOUT)->curVal;
  sprintf(SCPIoutBuf,"%8.4f\n", voltage);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

/** Read the output current */
scpi_error_t get_current(struct scpi_parser_context* context, struct scpi_token* command)
{
  float current;
  current = (chDef[streamIn[currentStreamIn].channel].ap + IOUT)->curVal;
  //Serial.printf("CCC %5.3f\n",current);
  sprintf(SCPIoutBuf,"%8.4f\n", current);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

/** Read the voltage from the pre-regulator */
scpi_error_t get_voltage_i(struct scpi_parser_context* context, struct scpi_token* command)
{
  float voltage;
  voltage = (chDef[streamIn[currentStreamIn].channel].ap + VIN)->curVal;
  sprintf(SCPIoutBuf,"%8.4f\n", voltage);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
/** Read the temperature of the heatsink */
scpi_error_t get_temp(struct scpi_parser_context* context, struct scpi_token* command)
{
  float temp;
  temp = (chDef[streamIn[currentStreamIn].channel].ap + TEMP)->curVal;
  sprintf(SCPIoutBuf,"%5.1f\n", temp);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
/******************** CHANGE SETTINGS ***********************/
/** Set the output voltage  */
scpi_error_t set_voltage(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  float output_value, output_valueS;
  //float maxV = 25;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 12, 0, VOMAX);
  if((output_numeric.length == 0 ||
    (output_numeric.length == 1 && output_numeric.unit[0] == 'V')) &&
	(output_numeric.value >= 0 && output_numeric.value <= VOMAX)
	)
  {
    output_value = constrain(output_numeric.value, 0, VOMAX);
	chDef[streamIn[currentStreamIn].channel].sp->voltage = output_value;
	//Serial.printf("S [%i] %f ", streamIn[currentStreamIn].channel, output_value);
	// also set input (SMPS) voltage
	//output_valueS = constrain(output_numeric.value + VDROPOUT , VSTART_S, VMAXS);
	//chDef[streamIn[currentStreamIn].channel].sp->voltageS = output_valueS;
	dirtyScreen = true;
	valChanged(VAL_CHGD);
  }
  else
  {
    scpi_error error;
    error.id = -200;
	 if( (output_numeric.value >= 0 && output_numeric.value <= VOMAX))
		error.description = (char *)"Command error; Invalid unit";
	else 
	{
		sprintf(myError, "Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    //error.length = 26;
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }
  scpi_free_tokens(command);
 // Serial.printf("Debug - set volts to %6.3f V\n", vSettingA);
  return SCPI_SUCCESS;
}

/**
 * Set the current value (:SOURCE:PROT X turns current limiting on/off)
 * add mA units?
 */
scpi_error_t set_current(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  float output_value, ov, temp;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 1, 0, 3);
  if((output_numeric.length == 0 ||
    (output_numeric.length == 1 && output_numeric.unit[0] == 'A')) &&
	(output_numeric.value >= 0 && output_numeric.value <= IMAX))
  {
//temp = (chDef[streamIn[currentStreamIn].channel].ap + IOUT)->maxVal; // maximum allowed current (off
//Serial.printf("SC: %5.2f\n", temp);
    ov = constrain(output_numeric.value, 0, IMAX);
    chDef[streamIn[currentStreamIn].channel].sp->current = ov;
    //iSetting = output_value;
	//Serial.print("S");
	valChanged(VAL_CHGD); 
	dirtyScreen = true;
  }
  else
  {
    scpi_error error;
    error.id = -200;
	if (output_numeric.value >= 0 && output_numeric.value <= IMAX)
		error.description = (char *)"Command error;Invalid unit";
	else 
	{
		sprintf(myError, "Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}
/*** Set the Current Limit on or off */
scpi_error_t set_protect(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  //struct scpi_numeric output_numeric;
  //unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  //Serial.printf("SET IPROT: [%s]\n", args->value);
  if (!strncmp(args->value,"ON",2))
  {
    // chDef[streamIn[currentStreamIn].channel].sp->limitOn = true;  
	 //Serial.print("S");
	 valChanged(VAL_CHGD);
	 dirtyScreen = true;
  }
  else 
    if(!strncmp(args->value,"OFF", 3))
	{		
    //  chDef[streamIn[currentStreamIn].channel].sp->limitOn = false;  
	  //Serial.print("S");
	  valChanged(VAL_CHGD); 
	  dirtyScreen = true;
	}
    else
    {
      scpi_error error;
      error.id = -200;
	  strncpy(valBuf, args->value, args->length);
	  valBuf[args->length] = '\0';
      sprintf(myError, "Command error; Invalid value '%s'", valBuf) ;
	  error.description = myError;
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);
      scpi_queue_error(&ctx, error);
      scpi_free_tokens(command);
      return SCPI_SUCCESS;
    }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
/*** Set the Tracking on or off */
scpi_error_t set_track_enable(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  //struct scpi_numeric output_numeric;
  //unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  if (!strncmp(args->value,"ON",2))
  {
   //  chDef[streamIn[currentStreamIn].channel].sp->trackOn = true;  
	//Serial.print("S:TE Y");
	valChanged(VAL_CHGD); 
	dirtyScreen = true;
  }
  else 
    if(!strncmp(args->value,"OFF", 3))
	{		
     // chDef[streamIn[currentStreamIn].channel].sp->trackOn = false;  
	//Serial.print("S:TE N");
	valChanged(VAL_CHGD);
	dirtyScreen = true;
	}
    else
    {
      scpi_error error;
      error.id = -200;
	  strncpy(valBuf, args->value, args->length);
	  valBuf[args->length] = '\0';
      sprintf(myError, "Command error; Invalid value '%s'", valBuf) ;
	  error.description = myError;
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);
      scpi_queue_error(&ctx, error);
      scpi_free_tokens(command);
      return SCPI_SUCCESS;
    }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
/*** Set Voltage Tracking on or off */
scpi_error_t set_track_v_ena(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  //struct scpi_numeric output_numeric;
  //unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  if (!strncmp(args->value,"ON",2))
  {
    // chDef[streamIn[currentStreamIn].channel].sp->trackSv = true;  
	//Serial.print("S: VE Y");
	valChanged(VAL_CHGD); 
	dirtyScreen = true;
  }
  else 
    if(!strncmp(args->value,"OFF", 3))
	{		
     // chDef[streamIn[currentStreamIn].channel].sp->trackSv = false;  
	//Serial.print("S: VE N");
	valChanged(VAL_CHGD);
	dirtyScreen = true;
	}
    else
    {
      scpi_error error;
      error.id = -200;
	  strncpy(valBuf, args->value, args->length);
	  valBuf[args->length] = '\0';
      sprintf(myError, "Command error; Invalid value '%s'", valBuf) ;
	  error.description = myError;
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);
      scpi_queue_error(&ctx, error);
      scpi_free_tokens(command);
      return SCPI_SUCCESS;
    }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

/*** Set Current Tracking on or off */
scpi_error_t set_track_c_ena(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  //struct scpi_numeric output_numeric;
  //unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  if (!strncmp(args->value,"ON",2))
  {
    // chDef[streamIn[currentStreamIn].channel].sp->trackSa = true;  
	//Serial.print("S:CE Y");
	valChanged(VAL_CHGD); 
	dirtyScreen = true;
  }
  else 
    if(!strncmp(args->value,"OFF", 3))
	{		
     // chDef[streamIn[currentStreamIn].channel].sp->trackSa = false;  
	//Serial.print("S:CE N");
	valChanged(VAL_CHGD);
	dirtyScreen = true;
	}
    else
    {
      scpi_error error;
      error.id = -200;
	  strncpy(valBuf, args->value, args->length);
	  valBuf[args->length] = '\0';
      sprintf(myError, "Command error; Invalid value '%s'", valBuf) ;
	  error.description = myError;
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);
      scpi_queue_error(&ctx, error);
      scpi_free_tokens(command);
      return SCPI_SUCCESS;
    }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

/** Tracking: Set the output voltage if tracking and voltage tracking both enabled */
scpi_error_t set_track_volts(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  float output_value, output_valueS;
  //float maxV = 25;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 12, 0, VOMAX);
  if((output_numeric.length == 0 ||
    (output_numeric.length == 1 && output_numeric.unit[0] == 'V')) &&
	(output_numeric.value >= 0 && output_numeric.value <= VOMAX)
	)
  {
    output_value = constrain(output_numeric.value, 0, VOMAX);
	if(0)//chDef[streamIn[currentStreamIn].channel].sp->trackOn && chDef[streamIn[currentStreamIn].channel].sp->trackSv) // only if tracking and Voltage tracking are both enabled
	{
		//chDef[streamIn[currentStreamIn].channel].sp->voltage = output_value;
		//Serial.printf("S: TV %5.2f\n", output_value);
		//Serial.printf("S [%i] %f ", streamIn[currentStreamIn].channel, output_value);
		// also set input (SMPS) voltage
		//output_valueS = constrain(output_numeric.value + VDROPOUT , VSTART_S, VMAXS);
		//chDef[streamIn[currentStreamIn].channel].sp->voltageS = output_valueS;
		dirtyScreen = true;
		valChanged(VAL_CHGD);
		if (directCommand)
		{
			// rebroadcast to group
			sprintf(valBuf,":TRAC:VOLT %3.6f\n",output_value);
			//Serial.print("#");
			//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
			SCPIheartbeatSent = true;
			//lastVset = output_value; //prevent rebroadcast to group
			changedLocal = false;
		}
	}
  }
  else
  {
    scpi_error error;
    error.id = -200;
	 if( (output_numeric.value >= 0 && output_numeric.value <= VOMAX))
		error.description = (char *)"Command error; Invalid unit";
	else 
	{
		sprintf(myError, "Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    //error.length = 26;
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }
  scpi_free_tokens(command);
 // Serial.printf("Debug - set volts to %6.3f V\n", vSettingA);
  return SCPI_SUCCESS;
}

/**
 * Set the current value (:SOURCE:PROT X) turns currentl limiting on/off 
 * add mA units?
 */
scpi_error_t set_track_current(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  float output_value, ov;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 1, 0, 3);
  if((output_numeric.length == 0 ||
    (output_numeric.length == 1 && output_numeric.unit[0] == 'A')) &&
	(output_numeric.value >= 0 && output_numeric.value <= IMAX))
  {
    ov = constrain(output_numeric.value, 0, 3);
	if(mSet.trackOn && mSet.trackSa) // only if tracking and current tracking are both enabled
	{
		//chDef[streamIn[currentStreamIn].channel].sp->current = ov;
		//iSetting = output_value;
		//Serial.printf("S: TC %5.2f\n", ov);
		valChanged(VAL_CHGD); 
		dirtyScreen = true;
		if (directCommand)
		{
			// rebroadcast to group
			sprintf(valBuf,":TRAC:CURR %3.6f\n",ov);
			//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
			SCPIheartbeatSent = true;
			//lastCset = ov;  //prevent rebroadcast to group
			changedLocal = false;
		}
	}
  }
  else
  {
    scpi_error error;
    error.id = -200;
	if (output_numeric.value >= 0 && output_numeric.value <= IMAX)
		error.description = (char *)"Command error;Invalid unit";
	else 
	{
		sprintf(myError, "Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/*** Set the SSID*/
scpi_error_t set_ssid(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  float output_value, output_valueS;
  //float maxV = 25;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  if(strlen(args->value) >= 2  && strlen(args->value) <= 32)
  {
		//Serial.printf("Change SSID to [%s]\n", args->value);
		strcpy(myID.local_ssid,args->value);
		valChanged(VAL_CHGD);
  }
  else
  {
    scpi_error error;
    error.id = -200;
	 if( (output_numeric.value >= 0 && output_numeric.value <= VOMAX))
		error.description = (char *)"Command error; Invalid unit";
	else 
	{
		sprintf(myError, "Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    //error.length = 26;
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }
  scpi_free_tokens(command);
 // Serial.printf("Debug - set volts to %6.3f V\n", vSettingA);
  return SCPI_SUCCESS;
}
/*** Set the WiFi PASSphrase*/
scpi_error_t set_pass(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  float output_value, output_valueS;
  //float maxV = 25;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }
  if(strlen(args->value) >= 8  && strlen(args->value) <= 63)
  {
		//Serial.printf("Change PASS to [%s]\n", args->value);
		strcpy(myID.local_pass,args->value);
		valChanged(VAL_CHGD);
  }
  else
  {
    scpi_error error;
    error.id = -200;
	 if( (output_numeric.value >= 0 && output_numeric.value <= VOMAX))
		error.description = (char *)"Command error; Invalid unit";
	else 
	{
		sprintf(myError, "Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    //error.length = 26;
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }
  scpi_free_tokens(command);
 // Serial.printf("Debug - set volts to %6.3f V\n", vSettingA);
  return SCPI_SUCCESS;
}
/**
 * Set the tracking reduction value 
*************** MORE LOGIC NEEDED HERE **********
(active value will be min of active UDP stream values and local value)
 */
scpi_error_t set_track_reduce(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_token* output_string;
  struct scpi_numeric output_numeric;
  float output_value, ov;
  short tg;
  char *gp, *tvalp;
  args = command;
//Serial.printf("Set track \n");
  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

 // assume command is for me (my group) UDP and serial messages are point to point, UDP messages for other groups are discard/* //chDef[streamIn[currentStreamIn].channel].sp->eTrack != chDef[streamIn[currentStreamIn].channel].sp->trackGrp;
 /*
 if(pSet[0].trackGrp != chDef[streamIn[currentStreamIn].channel].sp->trackGrp)
  {
	// no error, just meant for another tracking group
	scpi_free_tokens(command);
	return SCPI_SUCCESS;
  }
  */
  output_numeric = scpi_parse_numeric(args->value, args->length, 1, 0, 1); // constrain [0.. 1]
  ov = output_numeric.value;
//Serial.printf("Set track  %f\n",  ov);

  if(ov >= 0 && ov <= 999 + SMALL_DIFF)
  {   // set eTrack for selected channel
	ov = constrain(ov, 0.0, 1.0);
	//pSet[0].eTrack = ov;
    //chDef[streamIn[currentStreamIn].channel].sp->eTrack = ov; // more elegant

	//Serial.print("TRK\n");
	//valChanged(VAL_CHGD); // nothing changed in profile
	//dirtyScreen = true;  // let another process update the screen - Tracking group messages happen too quickly
	if (directCommand)
	{
		// rebroadcast to group
		sprintf(valBuf,":TRAC:REDU %3.6f\n",ov);
		//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
		SCPIheartbeatSent = true;
	}
  }
  else
  {
    scpi_error error;
    error.id = -200;	
	error.description = (char *)"Command error; Invalid unit";
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
/** Set the tracking group */
scpi_error_t set_track_group(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  float output_value, ov;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 1, 0, MAXTRACKGRP);
  if(output_numeric.value >= 0 && output_numeric.value <= MAXTRACKGRP)
  {    
   // chDef[streamIn[currentStreamIn].channel].sp->trackGrp = output_numeric.value;
	//Serial.print("S");
	valChanged(VAL_CHGD);
	dirtyScreen = true;
  }
  else
  {
    scpi_error error;
    error.id = -200;
	if (output_numeric.value > 0 && output_numeric.value <= IMAX)
		error.description = (char *)"Command error;Invalid unit";
	else 
	{
		sprintf(myError, "Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}
/*** Set the Output on or off */
scpi_error_t set_output(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  //struct scpi_numeric output_numeric;
  //unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }
  //Serial.printf("SET OUTP: [%s]\n", args->value);
  if (!strncmp(args->value,"ON",2))
  {
	onOff(streamIn[currentStreamIn].channel, true);
    //chDef[streamIn[currentStreamIn].channel].sp->outOn  = true; 
	//Serial.print("S+");
	//valChanged(VAL_CHGD); // nothing changed in profile
	dirtyScreen = true;
  }
   else 
    if(!strncmp(args->value,"OFF",3))
	{		
	  onOff(streamIn[currentStreamIn].channel, false);
      //chDef[streamIn[currentStreamIn].channel].sp->outOn  = false; 
	  //Serial.print("S-");
	// valChanged(VAL_CHGD); - nothing changed in profile
	  dirtyScreen = true;
	} 
    else
    {
      scpi_error error;
      error.id = -200;
	  strncpy(valBuf, args->value, args->length);
	  valBuf[args->length] = '\0';
      sprintf(myError, "Command error; Invalid value '%s'", valBuf) ;
	  error.description = myError;
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);  
      scpi_queue_error(&ctx, error);
      scpi_free_tokens(command);
      return SCPI_SUCCESS;
    }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
/*** set all channels off (emergency stop) */
scpi_error_t set_estop(struct scpi_parser_context* context, struct scpi_token* command)
{
	//chDef[i].sp->outOn  = false;
	onOff(-1, false);
  	//Serial.print("eS");
	valChanged(VAL_CHGD); // nothing changed in profile
	//dirtyScreen = true;
/*  do we want to register an error as well???
      scpi_error error;
      error.id = -200;
	  error.description = "EMERGENCY STOP";
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);  
      scpi_queue_error(&ctx, error);
 */
	// rebroadcast to group, if from serial or Telnet
	if (directCommand)
	{
		sprintf(valBuf,":TRAC:ESTO\n");
		//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
		SCPIheartbeatSent = true;
	}

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
// set the instrument channel
scpi_error_t set_channel(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  //struct scpi_numeric output_numeric;
  //unsigned char output_value;
  short i;
  args = command;
  bool found = false;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }
  strcpy(valBuf,"CHX"); 
  for (i = 0; i < NUMCHANNELS; i++)
  {
	  valBuf[2] = '1' + i; // insert the digit - OK for < 9 channels
	  if (!strncmp(args->value, valBuf, 3))
	  {
		streamIn[currentStreamIn].channel = i; 
		found = true;
		//Serial.print("S");
		//valChanged(VAL_CHGD); // nothing changed in overall profile
		//dirtyScreen = true;
		break;
	  }
  }
    if (!found)
    {
      scpi_error error;
      error.id = -200;
      error.description = (char *)"Command error; Invalid channel";
      error.length = 26;
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);  
      scpi_queue_error(&ctx, error);
      scpi_free_tokens(command);
      return SCPI_SUCCESS;
    }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}


/*** Set the WiFi autoConnect function on or off */
scpi_error_t set_connect(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  //struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }
  //Serial.printf("SET CONN: [%s]\n", args->value);
  if (!strncmp(args->value,"ON",2))
  {
	myID.autoConnect = true;    
	//Serial.print("AC+");
	valChanged(VAL_CHGD);
	//dirtyScreen = true;
  }
   else 
    if(!strncmp(args->value,"OFF",3))
	{		
	  myID.autoConnect = false;    
	 // Serial.print("AC-");
	  valChanged(VAL_CHGD) ;
	  dirtyScreen = true;
	} 
    else
    {
      scpi_error error;
      error.id = -200;
	  strncpy(valBuf, args->value, args->length);
	  valBuf[args->length] = '\0';
      sprintf(myError, "Command error; Invalid value '%s'", valBuf) ;
	  error.description = myError;
      sprintf(SCPIoutBuf,"%i,\"%s\"", error.id,error.description);
      SCPItrans(SCPIoutBuf);  
      scpi_queue_error(&ctx, error);
      scpi_free_tokens(command);
      return SCPI_SUCCESS;
    }

  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
scpi_error_t set_hostName(struct scpi_parser_context* context, struct scpi_token* command){
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  //float output_value, output_valueS;
  //float maxV = 25;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  if(strlen(args->value) >= 2  && strlen(args->value) <= 16)
  {
		//Serial.printf("Change HOST NAME to [%s]\n", args->value);
		strcpy(myID.instName, args->value);
		valChanged(VAL_CHGD);
  }
  else
  {
    scpi_error error;
    error.id = -200;
	 if( (output_numeric.value >= 0 && output_numeric.value <= VOMAX))
		error.description = (char *)"Command error; Invalid unit";
	else 
	{
		sprintf(myError, (char *)"Command error; Invalid value %5.2f", output_numeric.value) ;
		error.description = myError;
	}
    //error.length = 26;
    sprintf(SCPIoutBuf,"%i,\"%s\"", error.id, error.description);
    SCPItrans(SCPIoutBuf);    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }
  scpi_free_tokens(command);
 // Serial.printf("Debug - set volts to %6.3f V\n", vSettingA);
  return SCPI_SUCCESS;
}

/******************** GET INSTRUMENT SETTINGS ***********************/
// return the voltage setpoint
scpi_error_t get_set_voltage(struct scpi_parser_context* context, struct scpi_token* command)
{ 
//Serial.print("SCPI get volt\n");
  sprintf(SCPIoutBuf,"%8.4f\n", chDef[streamIn[currentStreamIn].channel].sp->voltage);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
// return the voltage setpoint
scpi_error_t get_set_current(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  sprintf(SCPIoutBuf,"%8.4f\n", chDef[streamIn[currentStreamIn].channel].sp->current);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

// return the protection setting 
scpi_error_t get_set_protect(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  //sprintf(SCPIoutBuf,"%i\n", chDef[streamIn[currentStreamIn].channel].sp->limitOn);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
// return the output on/off setting
scpi_error_t get_set_output(struct scpi_parser_context* context, struct scpi_token* command)
{ 
 // sprintf(SCPIoutBuf,"%i\n", chDef[streamIn[currentStreamIn].channel].sp->outOn);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
scpi_error_t get_set_channel(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  sprintf(SCPIoutBuf,"%i\n", streamIn[currentStreamIn].channel+1);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

// return the tracking enable setting
scpi_error_t get_set_track_enable(struct scpi_parser_context* context, struct scpi_token* command)
{ 
 // sprintf(SCPIoutBuf,"%i\n", chDef[streamIn[currentStreamIn].channel].sp->trackOn);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

// return the tracking group setting 
scpi_error_t get_set_track_group(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  //sprintf(SCPIoutBuf,"%i\n", chDef[streamIn[currentStreamIn].channel].sp->trackGrp);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

// tracking: return the group voltage (V) enable setting 
scpi_error_t get_set_track_v_ena(struct scpi_parser_context* context, struct scpi_token* command)
{ 
 // sprintf(SCPIoutBuf,"%i\n", chDef[streamIn[currentStreamIn].channel].sp->trackSv);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

// tracking: return the group current (A) enable setting 
scpi_error_t get_set_track_c_ena(struct scpi_parser_context* context, struct scpi_token* command)
{ 
 // sprintf(SCPIoutBuf,"%i\n", chDef[streamIn[currentStreamIn].channel].sp->trackSa); 
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
// return the currently tracking value (1.0 for not reducing || disabled 
scpi_error_t get_track_reduce(struct scpi_parser_context* context, struct scpi_token* command)
{ 
/*
  float track = 1.0;
  if(chDef[streamIn[currentStreamIn].channel].sp->trackOn && chDef[streamIn[currentStreamIn].channel].sp->eTrack < (1-SMALL_DIFF))
	  track = chDef[streamIn[currentStreamIn].channel].sp->eTrack;
  sprintf(SCPIoutBuf,"%5.3f\n", track);
  SCPItrans(SCPIoutBuf);
	*/
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

// return the "currently limiting" flag
scpi_error_t get_limiting(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  //sprintf(SCPIoutBuf,"%i\n", chDef[streamIn[currentStreamIn].channel].sp->limiting);
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
// return the WiFi pass phrase
scpi_error_t get_set_pass(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  sprintf(SCPIoutBuf,"%s\n", "WiFi password is not available remotely"); //myID.local_pass
//Serial.printf("SCPI PASS: %s\n", myID.local_pass);
  //sprintf(SCPIoutBuf,"%s\n", "SCPI PASS: RUBBISH");
// Serial.printf(""%s\n",SCPI PASS: RUBBISH\n");

  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
// return the SSID
scpi_error_t get_set_ssid(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  sprintf(SCPIoutBuf,"%s\n",myID.local_ssid);
  //Serial.printf("SCPI SSID: %s\n", myID.local_ssid);
  //sprintf(SCPIoutBuf,"%s\n", "SCPI SSID: RUBBISH");
//Serial.printf("%s\n","SCPI SSID: RUBBISH\n");
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

scpi_error_t get_set_hostName(struct scpi_parser_context* context, struct scpi_token* command){
  sprintf(SCPIoutBuf,"%s\n",myID.instName);
  //Serial.printf("SCPI HOST NAME: [%s]\n", myID.instName);
  //sprintf(SCPIoutBuf,"%s\n", "SCPI SSID: RUBBISH");
//Serial.printf("%s\n","SCPI SSID: RUBBISH\n");
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}
// Special error function - used by SCPI system
scpi_error_t
system_error(struct scpi_parser_context* ctx, struct scpi_token* command)
{
	struct scpi_error* error = scpi_pop_error(ctx);
	
	Serial.print(error->id);
        Serial.print(",\"");
        Serial.write((const uint8_t*)error->description, error->length);
	Serial.println("\"");

	scpi_free_tokens(command);
	return SCPI_SUCCESS;
}
// return the WiFi auto connect setting
scpi_error_t get_set_connect(struct scpi_parser_context* context, struct scpi_token* command)
{ 
  sprintf(SCPIoutBuf,"%s\n", (myID.autoConnect)?"ON":"OFF");
//Serial.printf("SCPI AUTO CONN: %s\n", (myID.autoConnect)?"ON":"OFF");
 // sprintf(SCPIoutBuf,"%s\n", "AUTO CONN: RUBBISH");
//Serial.printf("SCPI AUTO: %s\n", "RUBBISH");
  SCPItrans(SCPIoutBuf);
  scpi_free_tokens(command);
  return SCPI_SUCCESS;
}

// broadcast TRACKING messages to the group if tracking
// - Tracking reduction value
// - voltage set
// - Current set
// incoming messages handled via SCPI handler
short eTcntr = 99;
bool oneSent = false;

#define SEND_NOT_TRACK	1000 // send heartbeat occasionally if not Tracking
void SCPIgrpTrack(void)
{
	float tval;
	tval = pCont[0].localLimitRatio;
	// need to send an occasional "Don't reduce" message if tracking is off - as last one prior to tracking OFF may have been a "reduce to XX%" message
	if (mSet.trackOn || (tval > (1 - SMALL_DIFF))) // not tracking or track reduce value ~ 1.0, then send a "reduce = 1.0" occasionally
	{	  
		if(++eTcntr == SEND_NOT_TRACK)
		{		
			sprintf(valBuf,":TRAC:REDU %3.6f\n", 1.0);
			//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
			//Serial.printf("SCPI: TRAC:REDU %3.2f message\n", 1.0);
			SCPIheartbeatSent = true;
			eTcntr = 0;
		}			
	} else
	{
		// send out local current reduction ratio to the group
		sprintf(valBuf,":TRAC:REDU %3.6f\n",tval);
		//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
		//Serial.printf("SCPI: TRAC:REDU %3.2f message\n", tval);
		SCPIheartbeatSent = true;
		eTcntr = 0;
	}
	// send out vSet and iSet messages if Tracking and mode enabled and value changed *** LOCALLY ***.
	if(mSet.trackOn)
	{
		if(mSet.trackSv && abs(pSet[0].voltage - lastVset) > SMALL_DIFF && changedLocal)
		{
			sprintf(valBuf,":TRAC:VOLT %3.6f\n",pSet[0].voltage);
			//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
			//Serial.print("*");
			//Serial.printf("SCPI: TRAC:VOLT %3.2f message\n", pSet[0].voltage);
			SCPIheartbeatSent = true;
			lastVset = pSet[0].voltage;
			changedLocal = false;
			eTcntr = 0;
		}
		if(mSet.trackSa && abs(pSet[0].current - lastCset) > SMALL_DIFF && changedLocal)
		{
			sprintf(valBuf,":TRAC:CURR %3.6f\n",pSet[0].current);
			//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
			//Serial.printf("SCPI: TRAC:CURR %3.2f message\n", pSet[0].current);
			SCPIheartbeatSent = true;
			lastCset = pSet[0].current;
			changedLocal = false;
			eTcntr = 0;
		}		
	}
}


// check UDP streams for recent messages, disable those exceeding SCPIUDPTIMEOUT
// disp == false for production
uint8_t countSCPIstreams(bool disp)
{
	short i, numStreams = 0;
	//if(disp) Serial.printf("\nSTRM upd @ %i: ", (millis()/1000));
	for(i = 0; i < SCPI_INBUFS; i++)
	{
		/*
		if(disp) // diagnostic
		{
			Serial.printf("\nStrm = %i, ID = %i, stat = %i, lm = %i, ", i,streamIn[i].streamID, streamIn[i].status, streamIn[i].lastMessage);
		}
		*/
		if(streamIn[i].streamID >= 0 && streamIn[i].status >= 0) // UDP stream and not FREE
		{	
			if((streamIn[i].lastMessage + SCPIUDPTIMEOUT) > (millis() / 1000 ))
			{
				//if(disp) Serial.print("*");
				numStreams++;
			}
			else
			{
				//if(disp) Serial.print("Freeing");
				streamIn[i].streamID = SCPISTREAM_FREE; // free up the stream
				streamIn[i].status = SCPIBUF_FREE; // free up the buffer
			}
		}
		/*
		else
			if(disp) Serial.print("-");
		*/
		
	}
	return numStreams;
}

// broadcast a heartbeat message to the rest of the tracking group
// return number of active UDP streams 
uint8_t SCPIgroupHeartbeat(void)
{	
	
	// send our heartbeat message if nothing's been sent since last time
	if(SCPIheartbeatSent == false) // nothing else sent since we last sent a message
	{
		sprintf(valBuf,HEART_MSG); // body length needs to be >= "*IDN?" or it will be discarded
		//SCPIsendGrpMessage(pSet[0].trackGrp, valBuf);
		eTcntr = 0;
	}
	
	SCPIheartbeatSent = false; 
	
	// return number of active UDP streams
	return countSCPIstreams(true); // false for prod
}

// send message to group
// return number of active UDP streams 
short SCPIsendGrpMessage(uint8_t tgrp, char *buf)
{
short i, UDPcount = 0;
	// send message to my group
	putUDP(buf, myBroadcastIP, UDP_PORT,  tgrp, 0);
//Serial.printf("SCPI Grp [%i] Msg: %s\n", tgrp, buf);
	// reset heartbeat clock - any outgoing message will do
	SCPIheartbeatSent = true;	
		eTcntr = 0;
	return countSCPIstreams(false);;
}
///////////////////// UNUSED //////////////////////////
// group commands passed through to slave
// local control settings passed through to slave
void SCPIpassThru(char * command){
  // transmit to my slave channel
}
// responses to commands and SCPI messages from slave
void SCPIpassBack(char * command){
  // transmit to my master channel
  // process locally if required
}


#endif