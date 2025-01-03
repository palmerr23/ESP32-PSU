/* myHTTP.h
Basic definitions for all instruments
- WebServer constructor
- 404 handler
- scpi-raw on TCP port 5025 and UDP port 5025
*/
#ifndef MYTELNET_H
#define MYTELNET_H

// single client version

WiFiServer tServer(TELNET_PORT);
WiFiClient tServerClient;
IPAddress telnetClient; 

#define MSGBUFLEN 256
//char telMsg[MSGBUFLEN];
//char sbuf[MSGBUFLEN];
char telObuf[MSGBUFLEN]; // the command returned for SCPI processing
char telIbuf[MSGBUFLEN] = ""; // incoming message buffer. May contain more than one SCPI command.

char * getTelnet(void);
bool putTelnet(char * sbuf);
bool TELBUFMT = true;
char * telptr = telIbuf; // start of next command in incoming buffer


// ********** non-blocking code
// if anything is left over from the last read, then SCPI process it first.
// read everything that's available and return the first line, saving the start point for the next command.
char * getTelnet(void)
{
	short rembuf, i = 0;
	char * cptr, * optr;
	rembuf = strlen(telptr); // how many characters still in the buffer?
//Serial.printf("TEL: remaining %i\n", rembuf); 
	if(rembuf >= SCPI_SHORTCMD ) // some commands still left in the buffer
	{
		cptr = telptr;	// start of unused commands
		optr = telObuf; // beginning of output buffer
		//scan through for newline or end of buffer
		while( (i < rembuf) & (*cptr != '\n') & (*cptr != '\0'))
		{
			*optr++ = *cptr++;
			i++;
		}
		*optr =  '\0';
		telptr = ++cptr;	// next command starts here
		// return the next command, runt commands will be ignored.
		//Serial.printf("TEL: [%s],\n", telObuf);
		return telObuf;		
	}
	
    // else reset to empty buffer
	telptr = telIbuf;
	telIbuf[0] = '\0';
			

	if (tServer.hasClient())
	{      
        //check for free/disconnected 
        if (!tServerClient || !tServerClient.connected())
		{
          if(tServerClient) // disconnected
			  tServerClient.stop();
          tServerClient = tServer.available();
          if (!tServerClient) 
		  {
			  Serial.println("Telnet 'client available' is broken");
			  return NULL;
		  }
          Serial.print("New client: ");
          Serial.print(' ');
          Serial.println(tServerClient.remoteIP());          
        }           
    }
    //check client for data  
	if (tServerClient && tServerClient.connected())
	{
		telnetClient = tServerClient.remoteIP();
		if(tServerClient.available())
		{		  
		  Serial.print("Incoming message queue from telnet: ");		 
		  while(tServerClient.available()) 
				*(telptr++) = tServerClient.read();
		  *telptr = '\0';
		  Serial.printf("%s\n", telIbuf);
		  telptr = telIbuf; // set pointer back to start of buffer for processing
// process the buffer next time around
		  // kill trailing '\n'
		  /*
		  if(*(telptr - 1) == '\n') 
				*(telptr - 1) = '\0';
		  Serial.println(telIbuf);
		  return telIbuf;	
*/		  
		}	
	}	
	return NULL;
}
bool putTelnet(char * sbuf)
{
	if (tServer.hasClient())
	{      
        //check for free/disconnected 
        if (!tServerClient || !tServerClient.connected())
		{
          if(tServerClient) // disconnected - try to reconnect
			  tServerClient.stop();			  
          tServerClient = tServer.available();
          if (!tServerClient) 
		  {
			  Serial.println("Can't reconnect to Telnet client.");
			  return false;
		  }              
        }           
    }
    //client good to go?
	if (tServerClient && tServerClient.connected())
	{
		tServerClient.write(sbuf, strlen(sbuf));
		Serial.printf("Sending to Telnet: [%s]\n",sbuf);
		return true;
	}
	return false;
}

void setupTelnet(){  
  tServer.begin();
  tServer.setNoDelay(true);
  Serial.printf("Telnet (SCPI): %i to connect\n", TELNET_PORT);
}
/*
// ********** change input to non-blocking code....
void handleTelnetx() 
{
	char *telptr;
	int i;
	if (tServer.hasClient())
	{      
        //check for free/disconnected 
        if (!tServerClient || !tServerClient.connected())
		{
          if(tServerClient) // disconnected
			  tServerClient.stop();
          tServerClient = tServer.available();
          if (!tServerClient) 
			  Serial.println("available broken");
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(tServerClient.remoteIP());          
        }           
    }
    //check client for data  
	if (tServerClient && tServerClient.connected())
	{
		telnetClient = tServerClient.remoteIP();
		if(tServerClient.available())
		{
		  //get data from the telnet client and push it to the UART
			  Serial.printf("Incoming message from telnet\n");
			  telptr = telIbuf; 
			  while(tServerClient.available()) 
					*(telptr++) = tServerClient.read();
				*telptr = '\0';

				//echo to Serial
				Serial.println(telIbuf);
		// write a standard response - will be written back by SCPI
				strcpy(sbuf, "Platypus,PSU01,S00001,V1.01XX\n");
				tServerClient.write(sbuf, strlen(sbuf));
				delay(1);		
		}	
	}		
}

void handleTelnet()
{
	char * telptr;
	bool sentOK;
	if ((telptr =  getTelnet()) != NULL)
	{
			Serial.printf("Telnet cmd[%s]\n", telptr);
			strcpy(sbuf, "Platypus,PSU01,S00001,V1.01XX\n");
			sentOK = putTelnet(sbuf);
			if (!sentOK)
				Serial.printf("Telnet response not sent\n");
	}
}
*/

#endif /* MYHTTP_H */