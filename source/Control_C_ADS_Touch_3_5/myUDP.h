/* myUDP.h
Basic definitions for all instruments
scpi-raw on TCP port 5025 and UDP on UDP_PORT
*/
#ifndef MYUDP_H
#define MYUDP_H
void sprintIP(char *buf, IPAddress ip);

// max size not defined for ESP32 library
#define UDP_TX_PACKET_MAX_SIZE 1000 // must be less than (MTU - TCP header - UDP header) = (1500 - 24 - 8) = 1466
// do we need to record the UDP header information for responses? source/dest IP, source/dest Ports?
// UDP definitions
// UDP always broadcast to subnet, ignore all packets not for my tracking group, or global
// transmitted / received packet structure
struct SCPIpacket {
	char 		id[4];				// Always 'SCPI'
	uint32_t 	seq;				// Packet sequence number for logging streams. 0 otherwise
	uint16_t 	len;				// Content length <= 1024 bytes to avoid jumbo packets
	int8_t 		grp;				// Tracking group. -1 is global message (e.g. eStop all)
	char   		sName[NAMELEN];		// sender's instrument name (also ID'd by IP address)
	char   		content[SCPI_BUFLEN]; // content is always ASCII, CSV, NULL terminated
} SCpkt;							// we only need one as we'll satisfy each request before reading again

#define UDP_SIZE (sizeof(SCPIpacket))
#define UDP_HDR_SIZE (UDP_SIZE - SCPI_BUFLEN)

char UDPbuf[UDP_SIZE]; 		// buffer to hold  outgoing packet
uint16_t UDPsourcePort;			// sourcePort for return packet
IPAddress UDPsourceIP;			// source IP for return control packets (notification packets are broadcast)
WiFiUDP Udp;

bool UDPstart (void){	
uint8_t udps;
	udps = Udp.begin(WiFi.localIP(), UDP_PORT); 
	return (udps == 1) ? true : false;
}

// ignore UDP SCPI commands not tagged for my group
char * getUDP(){
  int packetSize = Udp.parsePacket();
  if (packetSize) 
  {
    //Serial.printf("Received packet of size %d from %s:%d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort() );

    // read the packet into packetBuffer
    int n = Udp.read((uint8_t*)&SCpkt, UDP_SIZE);	
	
	// test for a good packet that is for my tracking group
	if (n < (sizeof(UDP_HDR_SIZE) + 6)) // shortest legal command is "*IDN?\n" 
	{
		Serial.printf("UDP pkt to short = %i\n", n);
		return NULL; 
	}
	if(strncmp(SCpkt.id, "SCPI", 4)) // id chars not correct
	{
		Serial.printf("UDP pkt doesn't start with SCPI\n");
		return NULL; 
	}
	if (SCpkt.grp != pSetA.trackGrp && SCpkt.grp != -1) //  my tracking group or global message
	{
		Serial.printf("UDP pkt not for my [%i] group = %i\n", pSetA.trackGrp, SCpkt.grp);
		return NULL;
	}
	// good packet	
	UDPsourceIP = Udp.remoteIP();		// save the source info for response
	UDPsourcePort = Udp.remotePort();
	
	// make sure the input is null-terminated
	for(int i = 0; i < SCPI_BUFLEN; i++)
	{
		if (SCpkt.content[i] == '\n')
		{
			SCpkt.content[i] = '\0';
			break;
		}			
	}
    //Serial.printf("UDP In good: [%s]\n", SCpkt.content);
  
    // Serial.printf("| grp = %i\n", SCpkt.grp);
	return SCpkt.content;
  }
  return NULL;
}
bool putUDP(char * buf, IPAddress tIP, uint16_t port, int8_t grp, uint32_t seq = 0){
 
  short sent;
  char ipbuf[18];
      //Serial.println(counter);
	  // make up the SCPI packet
	  strncpy(SCpkt.id, "SCPI", 4);
	  SCpkt.seq = 0;
	  SCpkt.len = strlen(buf);
	  SCpkt.grp = grp;
	  strcpy(SCpkt.sName, myID.instName);
	  strncpy(SCpkt.content, buf, SCPI_BUFLEN);
      //sprintIP(IPBuf, WiFi.localIP());
      //sprintf(outBuffer, "Packet %i from %s", counter, IPBuf );
      //delay(100);
	  sprintIP(ipbuf, tIP);
      //Serial.printf("Sending UDP: [%s], grp = %i, port = %i, IP = %s", buf, grp, port, ipbuf);
      Udp.beginPacket(tIP, port);
		sent = Udp.write((uint8_t*)&SCpkt, UDP_HDR_SIZE + SCpkt.len); // transmit the terminating null
      Udp.endPacket();     

      //Serial.printf(", %i sent\n", sent);
	  return true;
}
// append IP address in char format to buffer 
void sprintIP(char *buf, IPAddress ip){
char localbuf[6];
  buf[0] = '\0';
  for (short i=0; i<4; i++)
  {
   sprintf(localbuf,"%i.", ip[i]);
   strcat(buf, localbuf);
  }
}

#endif /* MYUDP_H */