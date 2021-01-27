// Demo for SC WiFi Enabled ESP32 LCD Breakout - 3.5" version
// gets time from NTP server; set here below: udp.beginPacket("pool.ntp.org",123);
// gets weather data from https://openweathermap.org/
// this needs a free account and API key, set below at OWM_API_KEY
// you can also change units to F by setting temp_units below (to 'F')

#include "lcd.h"
#include "arial.h"
#include "small_font.h"
#include "gui.h"
#include "graphics.h"
#include "EEPROM.h"
#include "icons.h"
#include <WiFi.h>//#include <ESP8266WiFi.h>
#include <HTTPClient.h>//#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
HTTPClient http;        //to keep alive
WiFiUDP udp;            //for NTP
String payload;         //to handle returned payload
#define REFRESH_DELAY (600000UL)
#define OWM_API_KEY "f952b1d901054c3f4064574483a4cf24"
#define NTP_TIMESTAMP_DELTA  2208988800ull

char k[40]="";
char temp_units='C';    //in lieu of anything else, let's save if possible

struct wifinet{
  int ssidLen;
  int bssidLen;
  char ssid[36] = "ABCDEF";  //word align
  char bssid[36]; //word align
};

#define LOCATION_LENGTH 80

struct location{
  int length;
  char name[LOCATION_LENGTH];
};

wifinet currentWifi;
location currentLocation;

unsigned long UTCzero=0;  //corresponds to millis()==0, for RTC, zero=> invalid
long TZoffset=-1;         //in seconds for adjustment of local time, -1 for not set
unsigned long NTPtimeout=0;//don't refresh as often as weather
#define NTP_UPDATE_INTERVAL (360000UL)
boolean NTPstatus=0;      //1 for validly set

void setup() {
  int r;
  unsigned long t0;
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);
  Serial.begin(115200);  
  while(!Serial && (millis() < 2000))
    ;
  //load and validate EEPROM, write back if modified    
  EEPROM.begin(512);
  EEPROM.get(0,currentWifi);
  EEPROM.get(80,currentLocation);   //sizeof(wifinet)=80 
  Serial.println("Silicon Chip weather app");
  // Serial.println(currentWifi.ssidLen);
  //Serial.println(currentWifi.bssidLen);
  Serial.println(currentWifi.ssid);
  //Serial.println(strlen(currentWifi.bssid));
  if((currentWifi.ssidLen<0)||(currentWifi.ssidLen>32)){currentWifi.ssidLen=0;}
  if((currentWifi.bssidLen<0)||(currentWifi.bssidLen>32)){currentWifi.bssidLen=0;}
  currentWifi.ssid[currentWifi.ssidLen]=0;      //null term
  currentWifi.bssid[currentWifi.bssidLen]=0;      //null term
  if((currentLocation.length<0)||(currentLocation.length>(LOCATION_LENGTH-2))){currentLocation.length=0;}
  currentLocation.name[currentLocation.length]=0;   //null term
  Serial.print("Location: "); 
  Serial.print(currentLocation.name); Serial.print(" ["); Serial.print(currentLocation.length);Serial.println("]");
  EEPROM.put(0,currentWifi);  
  EEPROM.put(80,currentLocation);   //sizeof(wifinet)=80   
  EEPROM.commit();
  //display
  displaySetup();
  setrotation(3);     //change this and image will adjust
  Serial.println(sizeof(wifinet));
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(currentWifi.ssid,currentWifi.bssid);
  if(currentLocation.length==0){pickLocation(k,27);}         //on boot, if data empty, prompt
  if(currentWifi.ssidLen==0){pickWifi(k,36);}         //on boot, if data empty, prompt
  udp.begin(2390);
}

void loop() {  
  mainScreen();
}

void getWebData(){
  long n;
  int i;
  char d[32];
  Serial.println("Sending Get Request to Server.......");
  http.begin(String("http://api.openweathermap.org/data/2.5/weather?q=")+String(currentLocation.name)+String("&appid=" OWM_API_KEY));
  http.setReuse(0);
  int httpCode = http.GET();
  //Serial.println(httpCode);
  if(httpCode!=200){
    Serial.println("Error retrieving data.");
    return;
  }
  Serial.print(http.getSize()); Serial.println(" bytes received");
  payload = http.getString();
  http.end();  
  Serial.println(payload);
  //roughly parse JSON result
  float temp, temp_min, temp_max;
  long UTC_time, UTC_TZ,sunrise,sunset;    //Unix time, offset in seconds
  //numeric returns:
  temp=getFloat("\"temp\":",&payload);
  if(temp<10){Serial.println("Parsing error.");return;}
  temp_min=getFloat("\"temp_min\":",&payload);
  if(temp_min<10){Serial.println("Parsing error.");return;}
  temp_max=getFloat("\"temp_max\":",&payload);
  if(temp_max<10){Serial.println("Parsing error.");return;}
  UTC_time=getLong("\"dt\":",&payload);
  if(UTC_time<10){Serial.println("Parsing error.");return;}
  UTC_TZ=getLong("\"timezone\":",&payload);
  sunrise=getLong("\"sunrise\":",&payload);
  if(sunrise<10){Serial.println("Parsing error.");return;}
  sunset=getLong("\"sunset\":",&payload);  
  if(sunset<10){Serial.println("Parsing error.");return;}
  TZoffset=UTC_TZ;          //update
  //string returns
  char icon_id[10]="";    //has surrounding quotes
  const uint8_t* current_icon;
  getChar("\"icon\":",&payload,icon_id,8);
  Serial.println(icon_id);
  current_icon=matchIcon(icon_id);
  if(current_icon != NULL){
   eightBitBitmap(10,50,current_icon);
  }
  //main
  char main_desc[15]="";      //we'll use up to around 12 characters, 1-12
  getChar("\"main\":",&payload,main_desc,14);
  padFix(main_desc+1,12);
  characterarray(120,50,main_desc+1,GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  //now temp
  characterarray(120,75,"Now ",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  drawTemp(184,75,temp);
  //min temp
  characterarray(120,100,"Min ",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  drawTemp(184,100,temp_min);
  //max temp
  characterarray(120,125,"Max ",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  drawTemp(184,125,temp_max);
  //long

  //full description
  char full_desc[40]="";      //
  getChar("\"description\":",&payload,full_desc,40);
  padFix(full_desc+1,36);
  characterarray(10,150,full_desc+1,GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,myke_font);
  //other
  eightBitBitmap(10,160,icon_sunrise);  
  characterarray(70,172,"Sunrise: ",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  eightBitBitmap(10,210,icon_sunset);
  characterarray(70,222,"Sunset:  ",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  characterarray(10,263,"Updated at:  ",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  drawTime(214,172,sunrise,GUI_FOREGROUND_COLOUR);
  drawTime(214,222,sunset,GUI_FOREGROUND_COLOUR);
  drawTime(214,262,UTC_time,GUI_FOREGROUND_COLOUR);
  Serial.print("Curr temp: "); Serial.println(temp-273.15);
  Serial.print("Min temp:  "); Serial.println(temp_min-273.15);
  Serial.print("Max temp:  "); Serial.println(temp_max-273.15);
  Serial.print("UTC time:  "); Serial.println(UTC_time);
  Serial.print("UTC time:  "); Serial.println(UTC_TZ);
  Serial.print("Sunrise:   "); Serial.println(sunrise);
  Serial.print("Sunset:    "); Serial.println(sunset);
  yield();
}

void drawTemp(int x, int y, float t){   //note t in K, convert to temp_units, 8 characters wide
  long tt;
  boolean flag=0;
  if(temp_units=='C'){
    t=t-273.15;
  }else{
    t=(t*1.8)-459.67;   //assume Fahrenheit
  }
  tt=t*10.0;  //simplify comparisons, now in tenths of  a degree
  if(tt<0){
    tt=-tt;
    x=x+character(x,y,'-',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
  }else{
    x=x+character(x,y,' ',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);    
  }
  if(tt>9999){tt=9999;}
  if(tt>999){
    flag=1;
    x=x+character(x,y,tt/1000+'0',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);    
  }else{
    x=x+character(x,y,tt/1000+' ',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);        
  }
  tt=tt%1000;
  if((tt>99)||(flag)){
    flag=1;
    x=x+character(x,y,tt/100+'0',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);    
  }else{
    x=x+character(x,y,tt/100+' ',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);        
  }
  tt=tt%100;
  x=x+character(x,y,tt/10+'0',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);    
  x=x+character(x,y,'.',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);    
  x=x+character(x,y,tt%10+'0',GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);      
  x=x+character(x,y,0x7F,GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);    
  x=x+character(x,y,temp_units,GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);    
  
}

void padFix(char* a, int n){  //remove trailing quotes and pad with spaces up til n (to achieve strlen==n)
  int i=0;
  boolean flag=0;   //set on first quote
  while(i<n){
    if(a[i]=='"'){flag=1;}
    if(flag){a[i]=' ';}
    i++;
  }
  a[i]=0;     //null term
}

const uint8_t* matchIcon(char* id){
  const uint8_t* retval = icon_err;    //if no match
  int i;
  for(i=0;i<ICON_LIST_COUNT;i++){
    if((icon_names[i][0]==id[1])&&(icon_names[i][1]==id[2])&&(icon_names[i][2]==id[3])){    //offset by one due to leading quote mark
      retval=icons[i];
    }
  }
  return retval;
}

float getFloat(char* name, String* source){   //find name in source and return corresponding value (as float)
  char d[32];  
  if(getChar(name,source,d,30)){
    return atof(d);
  }
  return 0.0;     //nothing to return    
}

long getLong(char* name, String* source){   //find name in source and return corresponding value (as long)
  char d[32];  
  if(getChar(name,source,d,30)){
    return atol(d);
  }
  return 0;     //nothing to return  
}

int getChar(char* name, String* source, char* dest, int n){   //find name in source and copy value to dest
  int p=0;
  int s=0;
  int f;
  while(p < source->length()){
    if(name[s]==source->charAt(p)){
      s++;
    }else{
      s=0;    //reset on non-match
    }
    p++;
    if(name[s]==0){   //we've hit the trailing null, so it's a match
      f=0;            //reset and use for destination
      while((--n)&&(p < source->length())){
        dest[f]=source->charAt(p);
        p++;
        f++;
      }
      dest[f]=0;      //null term
      return f;
    }
  }
  return 0;
}

boolean doNTPupdate(){
  Serial.println("Updating NTP");
  delay(10);
  byte NTPpacket[48]="";
  unsigned long long ts1,ts2,ts3,ts4,newtime; //UTP uses four timestamps
  int i;
  for(i=0;i<48;i++){NTPpacket[i]=0;}     //clear packet contents 
  NTPpacket[0] = 0xE3;    //set default values for outgoing packet to server
  NTPpacket[2] = 0x06;
  NTPpacket[3] = 0xEC;
  NTPpacket[12]  = 0x31;
  NTPpacket[13]  = 0x4E;
  NTPpacket[14]  = 0x31;
  NTPpacket[15]  = 0x34;
  //dumpNTP(NTPpacket);
  udp.beginPacket("pool.ntp.org",123);    //NTP requests are to port 123
  udp.write(NTPpacket,48);      //buffer 48 bytes
  udp.endPacket();              //send 48 bytes
  ts1=millis64();               //outgoing timestamp
  while((millis64()-ts1<500UL)&(!udp.parsePacket())){yield();}      //wait up to a 0.5s for packet to return
  ts2=millis64();               //return timestamp
  //Serial.println(udp.available());
  udp.read(NTPpacket,48);       //save the returned packet
  //dumpNTP(NTPpacket);
  ts3=(((unsigned long long)NTPpacket[32])<<56)|(((unsigned long long)NTPpacket[33])<<48)|(((unsigned long long)NTPpacket[34])<<40)|(((unsigned long long)NTPpacket[35])<<32)|(((unsigned long long)NTPpacket[36])<<24)|(((unsigned long long)NTPpacket[37])<<16)|(((unsigned long long)NTPpacket[38])<<8)|(((unsigned long long)NTPpacket[39]));
  ts4=(((unsigned long long)NTPpacket[40])<<56)|(((unsigned long long)NTPpacket[41])<<48)|(((unsigned long long)NTPpacket[42])<<40)|(((unsigned long long)NTPpacket[43])<<32)|(((unsigned long long)NTPpacket[44])<<24)|(((unsigned long long)NTPpacket[45])<<16)|(((unsigned long long)NTPpacket[46])<<8)|(((unsigned long long)NTPpacket[47]));
  if((ts3!=0)&&(ts4!=0)){                           //if valid packet returned
    newtime=ts3-NTPfromms(ts1/2+ts2/2);             //time returned by NTP less millis() since boot
    newtime=(newtime>>32)-NTP_TIMESTAMP_DELTA;
    UTCzero=newtime;      //seconds
    Serial.print("Success:");
    Serial.println(UTCzero);
    return 1;
  }
  Serial.println("Fail");
  return 0;    
}

void dumpNTP(byte* p){
  int i;
  char hex[]="0123456789ABCDEF";
  for(i=0;i<48;i++){
    Serial.write(hex[(p[i]>>4)&0xF]);
    Serial.write(hex[(p[i]>>0)&0xF]);
    Serial.write(' ');
    if((i&3)==3){Serial.println();}    
  }
}

void drawTimeSeconds(int x, int y, unsigned long t, unsigned long c){  //convert t to local time and draw
  unsigned long h,m,s;
  char ss[10]="";
  t=t+TZoffset;
  t=t%86400;        //truncate to seconds
  s=t%60;
  t=t/60;
  m=t%60;
  h=t/60;
  ss[0]=(h/10)+'0';
  ss[1]=h%10+'0';
  ss[2]=':';
  ss[3]=(m/10)+'0';
  ss[4]=m%10+'0';
  ss[5]=':';
  ss[6]=(s/10)+'0';
  ss[7]=s%10+'0';
  ss[8]=0;
  characterarray(x,y,ss,c,GUI_BACKGROUND_COLOUR,GUI_FONT);  
}


void drawTime(int x, int y, unsigned long t, unsigned long c){  //convert t to local time and draw
  unsigned long h,m,s;
  char ss[10]="";
  t=t+TZoffset;
  t=t%86400;        //truncate to seconds
  s=t%60;
  t=t/60;
  m=t%60;
  h=t/60;
  ss[0]=(h/10)+'0';
  ss[1]=h%10+'0';
  ss[2]=':';
  ss[3]=(m/10)+'0';
  ss[4]=m%10+'0';
  ss[5]=0;
  characterarray(x,y,ss,c,GUI_BACKGROUND_COLOUR,GUI_FONT);  
}

void mainScreen(){
  unsigned long tmout=0;
  boolean done=0;
  char ipAdd[20]="Test";
  IPAddress ip;
  guiObject setupButton={380,0,100,30,"WiFi",0,0,1,1,BUTTON};  //x,y,w,h,text,value,pressed,visible,enabled,type
  guiObject locButton={0,0,320,40,currentLocation.name,0,0,1,1,BUTTON};
  clear(GUI_BACKGROUND_COLOUR);
  guiDraw(setupButton);
  guiDraw(locButton);
  while(!done){
    delay(10); 
    // Serial.println("MainScreen");
    if(WiFi.status()==WL_CONNECTED){
      if((NTPtimeout==0)||((millis()-NTPtimeout)>NTP_UPDATE_INTERVAL)){
        NTPstatus=doNTPupdate();
        Serial.print("NTP status  = "); Serial.print(NTPstatus ? "Success" : "fail");
        Serial.print(" @ T = ");
        Serial.println((float)millis()/1000);
        if(NTPstatus){
          NTPtimeout=millis();
        }else{
          Serial.println("Retrying NTP in 60 seconds");
          NTPtimeout=millis()+(60000UL)-NTP_UPDATE_INTERVAL;    //retry a minute later if it hasn't worked        
        }
      }
      if((tmout==0)||((millis()-tmout)>REFRESH_DELAY)){
        tmout=millis();
        Serial.print("Periodic refresh at ");   
        Serial.println((float)millis()/1000);         
        getWebData();
      }
      monoBitmap(340,0,GREEN,GUI_BACKGROUND_COLOUR,wifi);
      //Serial.println(WiFi.localIP());
      ip = WiFi.localIP();
      WiFi.localIP().toString().toCharArray(ipAdd,17);    //i know...
      while(strlen(ipAdd)<16){
        ipAdd[strlen(ipAdd)+1]=0;        //ensure next character is nul
        ipAdd[strlen(ipAdd)]=' ';        //pad with spaces
      }
    }else{
      monoBitmap(340,0,RED,GUI_BACKGROUND_COLOUR,wifi);      
      strCopy("---.---.---.--- ",ipAdd,17);
    }
    characterarray(352,32,ipAdd,GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,myke_font);
    if(UTCzero&&(TZoffset!=-1)){      //UTCzero <=> time has been set from NTP at some point, TZ has been set
      drawTimeSeconds(186,293,UTCzero+millis()/1000,(NTPstatus?GUI_FOREGROUND_COLOUR:GUI_DISABLE_COLOUR));      
    }else{
      characterarray(186,293,"--:--:--",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,arial_font);
    }
    characterarray(10,293,"Local time:",GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,arial_font);
    if(pressWait(&setupButton)){
      pickWifi(k,36);
      clear(GUI_BACKGROUND_COLOUR);
      guiDraw(setupButton);
      guiDraw(locButton);
      tmout=0;      //force refresh
    }
    if(pressWait(&locButton)){
      pickLocation(k,27);
      clear(GUI_BACKGROUND_COLOUR);
      guiDraw(setupButton);
      guiDraw(locButton);
      tmout=0;      //force refresh
    }
  }  
}

int pickLocation(char* a,int n){
  clear(GUI_BACKGROUND_COLOUR);
  if(softKeyboard("Enter location",a,n)){
    String(k).toCharArray(currentLocation.name, 32);
    currentLocation.length=strlen(currentLocation.name);
    Serial.println(currentLocation.length);
    Serial.println(currentLocation.name);
    EEPROM.put(80,currentLocation);   //sizeof(wifinet)=80   
    EEPROM.commit();
  }else{
    Serial.println("CANCELLED");
  }
}

int pickWifi(char* a, int n){ //destination, max length
  boolean done=0;
  guiObject cancelButton={380,0,100,30,"Cancel",0,0,1,1,BUTTON};  //x,y,w,h,text,value,pressed,visible,enabled,type  
  char ssidList[9][36];
  guiObject ssidButton[9]={
    {0,31,320,30,ssidList[0],0,0,1,1,BUTTON},
    {0,63,320,30,ssidList[1],0,0,1,1,BUTTON},
    {0,95,320,30,ssidList[2],0,0,1,1,BUTTON},
    {0,127,320,30,ssidList[3],0,0,1,1,BUTTON},
    {0,159,320,30,ssidList[4],0,0,1,1,BUTTON},
    {0,191,320,30,ssidList[5],0,0,1,1,BUTTON},
    {0,223,320,30,ssidList[6],0,0,1,1,BUTTON},
    {0,255,320,30,ssidList[7],0,0,1,1,BUTTON},
    {0,287,320,30,ssidList[8],0,0,1,1,BUTTON}
  };
  int i,c;      //index, count of ssid's
  clear(GUI_BACKGROUND_COLOUR);
  guiDraw(cancelButton);
  ssidButton[4].text="Scanning...";
  ssidButton[4].pressed=1;
  guiDraw(ssidButton[4]);     //use this cos it's in a good spot
  ssidButton[4].text=ssidList[4];
  ssidButton[4].pressed=0;
  //c=WiFi.scanNetworks();    //async, if problems with sync version below
  WiFi.scanNetworks(true);
  while(WiFi.scanComplete()<0){
    yield();
    if(pressWait(&cancelButton)){
      Serial.println("cancel");
      return 0;
    }    
  }
  c=WiFi.scanComplete();
  Serial.print(c);
  Serial.println(" networks found");
  for(i=0;i<9;i++){
    if(i<c){
      WiFi.SSID(i).toCharArray(ssidList[i], 36);
      ssidButton[i].enabled=1;
      if(strlen(ssidList[i])>18){ssidList[i][14]='.';ssidList[i][15]='.';ssidList[i][16]='.';ssidList[i][17]=0;}   //truncate with ...
      Serial.println(WiFi.SSID(i).c_str());  
    }else{
      ssidList[i][0]=0;  //blank
      ssidButton[i].enabled=0;
    }
    guiDraw(ssidButton[i]);
  }
  while(!done){
    yield();
    if(pressWait(&cancelButton)){
      Serial.println("cancel");
      done=1;
    }
    for(i=0;i<c;i++){
      if(pressWait(&ssidButton[i])){
        Serial.println(i);
        Serial.println(" selected");
        if(getPassword(k,28)){
           WiFi.SSID(i).toCharArray(currentWifi.ssid, 32);
           String(k).toCharArray(currentWifi.bssid, 32);
           currentWifi.ssidLen=strlen(currentWifi.ssid);
           currentWifi.bssidLen=strlen(currentWifi.bssid);
          if((currentWifi.ssidLen<0)||(currentWifi.ssidLen>32)){currentWifi.ssidLen=0;}
          if((currentWifi.bssidLen<0)||(currentWifi.bssidLen>32)){currentWifi.bssidLen=0;}
          currentWifi.ssid[currentWifi.ssidLen]=0;      //null term
          currentWifi.bssid[currentWifi.bssidLen]=0;      //null term
          EEPROM.put(0,currentWifi);  
          EEPROM.commit();
          WiFi.disconnect();
          WiFi.begin(currentWifi.ssid,currentWifi.bssid);
          done=1;                    
        }else{
          done=1;   //bail
        }
      }      
    }
  }
  return 0;
}

int getLocation(char* a, int n){    //thin wrapper
}

int getPassword(char* a, int n){    //thin wrapper
  int r;
  clear(GUI_BACKGROUND_COLOUR);
  r=softKeyboard("Enter password",a,n);
  if(r){
    Serial.println(k);
  }else{
    Serial.println("CANCELLED");
  }
  return r;
}

boolean strCopy(char* src, char* dst, int n){
  int a,i;
  boolean retval=1;
  a=strlen(src)+1;
  if(a>n){a=n;retval=0;}    //copy incomplete
  for(i=0;i<a;i++){
    dst[i]=src[i];
  }
  dst[n-1]=0;               //null term
  return retval;
}

unsigned long long millis64(){      //looks for rollover; as long as it's called every 50 days, it'll be fine
  static unsigned long lo,hi;   //hi and lo 32 bits
  unsigned long newlo=millis(); //check millis()
  if(newlo<lo){hi++;}           //rollover has happened
  lo=newlo;                     //save for next check
  return ((unsigned long long)(hi) << 32) | ((unsigned long long)(lo));  //return 64 bit result
}

unsigned long long NTPfromms(unsigned long long t){      //converts a time in ms to time in 64 bit NTP, max precision, will rollover after 194 years
  return ((t<<22)/1000)<<10;
}

unsigned long msfromNTP(unsigned long long t){      //converts a 64 bit NTP time into ms
  return ((t*1000)>>32);                            //resolution will be lost however we do this
}
