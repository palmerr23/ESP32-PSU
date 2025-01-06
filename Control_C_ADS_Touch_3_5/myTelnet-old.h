/* myHTTP.h
Basic definitions for all instruments
- WebServer constructor
- 404 handler
- scpi-raw on TCP port 5025 and UDP port 5025
*/
#ifndef QQMYTELNET_H
#define QQMYTELNET_H
#define TELNET_PORT 5025 // SCPI std
#define MAX_SRV_CLIENTS 5 // how many clients should be able to telnet to this host?

WiFiServer tServer(TELNET_PORT);
WiFiClient tServerClients[MAX_SRV_CLIENTS];

#define MSGBUFLEN 256
char telMsg[MSGBUFLEN];

IPAddress telnetClients[MAX_SRV_CLIENTS]; 

// ********** change input to non-blocking code....
void handleTelnet() 
{
	char sbuf[256], telBuf[256], *iptr;
	int i;
	if (tServer.hasClient())
	{
	
      for(i = 0; i < MAX_SRV_CLIENTS; i++)
	  {
        //find free/disconnected spot
        if (!tServerClients[i] || !tServerClients[i].connected())
		{
          if(tServerClients[i]) 
			  tServerClients[i].stop();
          tServerClients[i] = tServer.available();
          if (!tServerClients[i]) 
			  Serial.println("available broken");
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(tServerClients[i].remoteIP());
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        //no free/disconnected spot so reject
        tServer.available().stop();
      }
    }
    //check clients for data
    for(i = 0; i < MAX_SRV_CLIENTS; i++)
	{
      if (tServerClients[i] && tServerClients[i].connected())
	  {
		telnetClients[i] = tServerClients[i].remoteIP();
        if(tServerClients[i].available())
		{
          //get data from the telnet client and push it to the UART
Serial.printf("Incoming message from telnet %i\n",i);
			  iptr = telBuf; 
			  while(tServerClients[i].available()) 
					*(iptr++) = tServerClients[i].read();
				*iptr = '\0';

				//echo to Serial
				Serial.println(telBuf);
	// write a standard response - will be written back by SCPI
				strcpy(sbuf, "Platypus,PSU01,S00001,V1.01XX\n");
				tServerClients[i].write(sbuf, strlen(sbuf));
				delay(1);				

		}
		else 
		{
			if (tServerClients[i]) 
			{
			 // tServerClients[i].stop();
			}
		  }
		}
	}
}

void setupTelnet(){  
  tServer.begin();
  tServer.setNoDelay(true);
  Serial.printf("Telnet (SCPI) - %i to connect\n", TELNET_PORT);
}


#endif /* MYHTTP_H */