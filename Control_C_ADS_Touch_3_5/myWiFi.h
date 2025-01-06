/* myWiFi.h
basic WiFi for all instruments
- setup Wifi
*/
#ifndef MYWIFI_H
#define MYWIFI_H

#include <WiFi.h>
//#include <WiFiMulti.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiServer.h>
#include <ESPmDNS.h>
void setHostNameIP (void);

IPAddress nullIP = {0,0,0,0};

char wifiStatus[7][20] = {
   // WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    "WL_IDLE_STATUS"  ,
    "WL_NO_SSID_AVAIL"   ,
    "WL_SCAN_COMPLETED",
    "WL_CONNECTED",
    "WL_CONNECT_FAILED",
    "WL_CONNECTION_LOST",
	"WL_DISCONNECTED"};
#define CONNWAIT 10
bool wifiBegin(){ 
   char buf[256];
   short i;
   bool res;
   //if(WiFi.status() == WL_CONNECTED) // WiFi.begin() should disconnect
	   
   WifiConnected = false;
   SDEBUG1.println("**** Configuring WiFi *****");
   IamAP = false;
   // try connecting to nominated network
   SDEBUG1.printf("Trying existing WiFi network: %s\n", myID.local_ssid);
   sprintf(buf, "Connecting to \nSSID = %s\n", myID.local_ssid);
   screenError(buf, ERR_BG_A, 1, false);
   WiFi.begin(myID.local_ssid, myID.local_pass);
   i = 0;
   while ((WiFi.status() != WL_CONNECTED) && (i++ < CONNWAIT)) 
   {
      delay(1000); 
	  strcat(buf, ">");
	  screenError(buf, ERR_BG_A, 0, false);
      SDEBUG1.print(">");
   }
 
   //SDEBUG1.println(WiFi.localIP());   
   if (WiFi.status() == WL_CONNECTED) 
   {
      SDEBUG1.printf(" Connected to existing WiFi network: SSID %s\n",myID.local_ssid);
      myIP = WiFi.localIP();
	  myBroadcastIP = WiFi.broadcastIP();
	  mySubnetMask = WiFi.subnetMask();
	  MDNS.begin(myID.instName);
	  setHostNameIP ();
	  WifiConnected = true;
	  return true;
   }
   
   // Failed: Try to connect to ESP net as station    
   SDEBUG1.printf(" Failed to find Home AP, trying SSID %s as Station.\n", myID.esp_ssid);
   sprintf(buf, "Connecting to ESPNET\nSSID = %s\n", myID.esp_ssid);
   screenError(buf, ERR_BG_A, 0, false);
   WiFi.begin(myID.esp_ssid, myID.esp_pass);
   i = 0;
   while ((WiFi.status() != WL_CONNECTED) && (i++ < CONNWAIT)) 
   {
      delay(1000);
	  strcat(buf, "<");
	  screenError(buf, ERR_BG_A, 0, false);
      SDEBUG1.print("<");
   }
   delay(2000);

  if (WiFi.status() == WL_CONNECTED) 
  {
    SDEBUG1.printf(" Connected to existing SSID %s AP\n", myID.esp_ssid);
    SDEBUG1.println(WiFi.localIP());
    myIP = WiFi.localIP();
	myBroadcastIP = WiFi.broadcastIP();
	mySubnetMask = WiFi.subnetMask();
	MDNS.begin(myID.instName);
	setHostNameIP ();
	WifiConnected = true;
    return true;
  }
  // Failed: Launch ESPnet as AP
  SDEBUG1.printf(" Failed connecting to %s. Launching as AP+STA\n", myID.esp_ssid);
  sprintf(buf, "Can't connect to\nSSID = %s\nLaunching AP", myID.esp_ssid);
  screenError(buf, ERR_BG_A, 5, false);
  WiFi.config(nullIP,nullIP, nullIP); // cause WiFi to forget previous networks

  if (!WiFi.softAP(myID.esp_ssid, myID.esp_pass))
  {
    SDEBUG2.println("Failed to launch AP");
    sprintf(buf, "Wifi - failed to launch ESPNET AP\n SSID = %s", myID.esp_ssid);
    screenError(buf, ERR_BG_B, 5, false);
    WiFi.printDiag(SDEBUG1);
    SDEBUG1.println(WiFi.softAPIP());
    return false;
  }
  
  // set up IP range for AP network
  delay(1000);  
  if (WiFi.softAPConfig(myID.prefIP, myID.gateIP, myID.netmask))
  {
    SDEBUG1.println("AP Config OK");
    WiFi.printDiag(SDEBUG1);
    SDEBUG1.println(WiFi.softAPIP());
    myIP = WiFi.softAPIP();
	myBroadcastIP = WiFi.softAPBroadcastIP();
	mySubnetMask = cidr2netmask(WiFi.softAPSubnetCIDR());
	WiFi.softAPsetHostname(myID.instName);
	IamAP = true;
	MDNS.begin(myID.instName);
	setHostNameIP ();
	WifiConnected = true;
    return true;
  }
  
  SDEBUG2.println("AP IP config bad");
  WiFi.printDiag(SDEBUG1);
  SDEBUG1.println(WiFi.softAPIP());
  return false;
}
void wifiEnd(bool wifioff = false){
	bool ret;
		Serial.printf("Wifi end\n");
	ret = WiFi.softAPdisconnect(wifioff);
	Serial.printf("Soft AP disconnect %i\n", ret);
	ret = WiFi.disconnect(wifioff);
	Serial.printf("STA disconnect %i\n", ret);
	WifiConnected = false;
}

// set hostname and IP in string format
void setHostNameIP (void)
{
	char buf[HOSTLEN];
	strcpy(myHostName, myID.instName);
	strcat(myHostName, ".local");
		
	sprintf(IPstring, "%i.%i.%i.%i", myIP[0], myIP[1], myIP[2], myIP[3]);
	
	Serial.printf("Hostname: %s\n",myHostName);
	Serial.printf("IP: %s\n",IPstring);
}

/*
void setup_wifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);  // starting with no SSID, pw will reconnect to last network.

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
  }
  delay(500);

MDNS.begin(dns_id);
}
*/
#endif /* MYWIFI_H */