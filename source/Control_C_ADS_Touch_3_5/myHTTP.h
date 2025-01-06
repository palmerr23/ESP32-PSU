/* myHTTP.h
Basic definitions for all instruments
- WebServer constructor
- 404 handler

*/
#ifndef MYHTTP_H
#define MYHTTP_H
String SendHTML();
String tableItem(short row, short col);
String clrSize(const GFXfont *font, uint16_t col);
void onOff(int8_t channel, bool status); // myControl.h
String HMS(long time);

#define HTTP_PORT 80 // standard port 

// 404 response 
WebServer server(HTTP_PORT);
#define HTTPMSGBUFLEN 256
char HTTPmessage[HTTPMSGBUFLEN];
#define MSGHDR ""

void HTTPinfo (char * msg){
  short i;
  String msgString;
  msgString = "URI: ";
  msgString += server.uri();
  msgString += "\nMethod: ";
  msgString += (server.method() == HTTP_GET) ? "GET" : "POST";
  msgString += "\nArguments: ";
  msgString += server.args();
  msgString += "\n";
  for ( i = 0; i < server.args(); i++) {
    msgString += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  // copy back to char[] form
  msgString.toCharArray(msg, msgString.length()) ; //HTTPMSGBUFLEN
}

void handleNotFound() { 
  //digitalWrite(led, 1);
  strcpy(HTTPmessage,"File Not Found\n");
  HTTPinfo( &HTTPmessage[strlen(HTTPmessage)] );
  server.send(404, "text/plain", HTTPmessage);
  //digitalWrite(led, 0);
   Serial.printf("HTTP 404 - Args [%i] %s\n", server.args(), HTTPmessage);
}

void handleRoot() {
  //digitalWrite(led, 1);
  server.send(200, "text/html", SendHTML()); 
  /*
  server.send(200, "text/plain", "Hello from ESP32! Pretty table with readings goes here refresh 10 secs");
  Serial.print("HTTP root\n");
  // digitalWrite(led, 0);
  HTTPmessage[0] = '\0';
  HTTPinfo( &HTTPmessage[strlen(HTTPmessage)] );
  Serial.printf("HTTP Args - root: %s", HTTPmessage);
  */
}
void handleStop() {
  onOff(0, false);	//turn off output
  
  //  simple refresh page -> back to root 
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">"; 
  ptr += "<meta http-equiv=\"refresh\" content=\"1; url=/\">";
  ptr += "<title>Emergency Stop</title><body><H1>Turning output off</H1><BR>";
  ptr += "If you are not redirected in a few seconds: <a href=\"../\">return</a></body></html>\n";
  
  server.send(200, "text/html", ptr); 
}
#define TAB_ROWS 4
#define TAB_COLS  3
struct tblCont {
	char 	type;		// s for setting; r for reading
	short	index;		// offset into setting or reading array
	char	legend[16];
}
tableH[TAB_ROWS][TAB_COLS] = {
	{{'r', 8, "Vin"},{'r', 0, "Output Volts"},{'s', 0, "Set Volts (V)"}},
	{{'x', 1, "Limit"},{'r', 1, "Output Amps"},{'s', 1, "Set Current (A)"}},	
	{{'x', 2, "Output"},{'r', 2, "Output Power"},{'r',7, "Temp"}},
	{{'x', 0, "Track"},{'r', 4, "Tracking Group"},{'r', 3, "Remote"}}
};

#define LXIND pContA.limitInd
#define TRXIND pContA.trackInd
#define OUTXIND pSetA.outOn
char DUMMS[1] = "";
valFmt localVals[]
= {
	 {&TRXIND,&FONT1, 1, 0,24, READX,TOPLINE+0*BVSPACE, 'Z', "",CYAN_L, 0,2,2} ,    // avoid "units" for type Z
	 {&LXIND, &FONT1, 1,0,1, READX+20,TOPLINE+2*BVSPACE, 'Z', "", CYAN_L,0,2,0} ,  
	 {&OUTXIND, &FONT1, 1,0,1, READX+20,TOPLINE+2*BVSPACE, 'Z', "", CYAN_L,0,2,0},  
	 {&DUMMS, &FONT1, 1,0,1, READX+20,TOPLINE+2*BVSPACE, 'A', "", CYAN_L,0,2,0}   // dummy entry (blank)
};
//long mycountrr;
#define BROWSER_REFRESH 5
String SendHTML(){
	short i,j;
	char buf[64];
	String ptr = "<!DOCTYPE html> <html>\n";
	ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">"; 
	ptr += "<meta http-equiv=\"refresh\", content=\"";
	ptr += itoa(BROWSER_REFRESH,buf, 10);
	ptr += "\">\n";
	ptr +="<title>";
	ptr += myID.instName;
	ptr +="</title>\n";
	ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
	ptr +="body{margin-top: 50px;} h1 {color: #222222;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
	ptr +=".button {display: inline-block;width: 35px;background-color: #3498db;border: none;color: white;padding: 5px 5px;text-decoration: none;font-weight: bold;font-size: 20px;margin: 0px auto 5px;cursor: pointer;border-radius: 4px;}\n";
	ptr +=".button-on {background-color: #00A000;}\n";
	ptr +=".button-act {background-color: #FF0000; animation: blinky 1.5s linear infinite;} @keyframes blinky {50% {opacity: 0; } }\n";
	ptr +=".button-off {background-color: #0000D0;}\n";
	ptr +=".button-off:active {background-color: #2c3e50;}\n";
	ptr +=".button-red {background-color: #FF0000; width: 100px}\n";
	ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
	ptr +="</style>\n";
	ptr +="<style> .blink {animation: blinker 1.5s linear infinite;} @keyframes blinker {50% {opacity: 0; } }</style>\n"; //color: #1c87c9; font-size: 20px; font-weight: bold;  font-family: sans-serif;    
	ptr+= "<style> table { width: 50%;  margin: 30px auto; border-collapse: collapse; }";
	ptr += " th, td { padding: 10px; border: 1px solid #666; } </style> ";
	ptr +="</head>\n";
	ptr +="<body>\n";
	ptr +="<h1>"; 
	ptr += myID.instName;
	ptr+= "</h1>\n";
	//ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
	ptr +="<table align=center>";
	
	for (i=0;i<TAB_ROWS;i++){
		ptr += "<TR>\n";
		for (j=0;j<TAB_COLS;j++)
		{
			ptr += "  <TD>";
			ptr += tableItem(i,j);
			//ptr += tableH[i][j].type;ptr += ", "; ptr += itoa(tableH[i][j].index, buf, 10);ptr += "<BR>"; 
			//ptr += itoa(i, buf, 10); ptr += ", "; ptr += itoa(j, buf, 10);
			ptr += "</TD>\n";
		}
		ptr += "</TR>\n";
		
	}
	ptr +="</TABLE>";
	ptr +="<a class=\"button button-red\" href=\"/eStop\">Turn off Output</A>\n"; 
 
	ptr += "<br>Uptime: ";
	ptr += HMS(millis()/1000);
	//ptr += " secs";
	// mycountrr+= BROWSER_REFRESH;
	//Serial.printf("LXIND %i, %i\n",LXIND, pContA.iLimit);

	ptr +="</body>\n";
	ptr +="</html>\n";
	return ptr;
}
String tableItem(short row, short col)
{
	String clrSiz;
	char fmt, typ;
	char buf[32];
	short ind;
	const  GFXfont *fptr;
	uint16_t colr;
	valFmt *ptr;
	void *vp; // value takes on several formats (defined by fmt)
	char *legptr;
	uint8_t testval = 123;
	typ  = tableH[row][col].type;
	ind = tableH[row][col].index;
	legptr = tableH[row][col].legend;
	// sprintf(buf, "x = %i, y=%i, t=%c, %i<BR>", row, col, typ, ind);
	String spt;

	switch (typ)
	{
		case 's' : // from myScreenDefs settings table
			ptr = &setVals[ind];
			break;
		
		case 'r' :  // from readings table
			ptr = &readVals[ind];
			break;
			
		case 'x' : // specials (in local table)
			ptr = &localVals[ind];
			//Serial.printf("HTTP: %i [%i]\n", *(int8_t*)localVals[ind].vp, ind);
			//spt += "<p class=\"body blink\" >";
			break;
	}
	fmt = ptr->fmt;
	vp = ptr->vp;
	fptr = ptr->font; // may be NULL - match these with known font pointers
	colr = ptr->clr;  // match these with known colours
	clrSiz = clrSize(fptr,colr);
	spt += legptr; spt += "<BR><span style=\"";
	spt += clrSiz;
	spt += "\">";
	// modified printfloatX from myScreenDraw
	switch (fmt)
	{
		case 'T' : // Toggle	  
		  spt += ((*(uint8_t*)vp > 0)? "On " : "Off");
		  break;
		case 'Z' : // Toggle	 button  
		//<a class=\"button button-on\"  href=\"\\\">T</a>"
		switch(*(uint8_t*)vp)
		{	
			case 0:		
				spt += "<span class=\"button button-off\">Off"; 
				break;
			case 1:			
				spt += "<span class=\"button button-on\">On"; 		
				break;				
			default:	// it's an error condition
			case 2:
				spt += "<span class=\"button button-act\">Active"; 
		}
	    spt += "</span>";
	    break;
	   // use SAFE 'A' format for all string variables
	   case 'A' : // SAFE string text (may have characters outside 0x20 .. 0x7e range)
		  strcpy(buf, (char*)vp); // substitute unprintable chars
		   //Serial.println(buf);
		  for (short i = 0; i < strlen(buf); i++)
			if (buf[i] < 0x20 || buf[i] > 0x7d)
			  buf[i] = '*';
		  //Serial.println(buf);
		  //delay(500);
		  spt += buf;
		  break;
		case 'S' :  // unvalidated string - really only for const strings      
		 spt += ((char*)vp);      
		 break;
		case 'B': // Bracketed
		  spt += "[";
			//cCount++;
		case 'I': // 8-bit Integer: convert value to float for display
		  spt += itoa( *(uint8_t*)vp, buf, 10);      
		  break;
		case 'F': // Float
		 sprintf(buf, "%5.2f", *(float*)vp );
		  spt += buf;
		 // Serial.printf("PFX: %6.4f [round %i]\n", val, post);
		 
		  break;
		case 'P': // Float as percentage
		sprintf(buf, "%5.2f&percnt;", *(float*)vp * 100);
		  spt += buf;
		  //Serial.printf(" %5.1f%\n", val);
		  break;
  }
	if (fmt == 'B')
			spt += "]";
	
	if (strlen(ptr->units))
	{
		spt += " "; 
		spt += ptr->units;
	}
	//if (typ != 'x')
	//else
		//spt += "</p>";
	spt += "</span>";	
	return spt;
}

void setup_http(){  
  server.on("/", handleRoot);
  server.on("/eStop", handleStop);

 /* server.on("/inline", []() {
    server.send(200, "text/plain", "This [/inline] works as well");
  });
*/
  server.onNotFound(handleNotFound);

  server.begin();
  //server.on("/", handleRoot); // just one page
}
String clrSize(const GFXfont *font, uint16_t col)
{
	String ss;
		
	ss = "font-size: 16px; ";	// px (default, i.e. == NULL)
	if (font == &FONT0)	
		ss = "font-size: 16px; ";	
	if (font == &FONT1)
		ss = "font-size: 18px; ";	
	if (font == &FONT2)
		ss =  "font-size: 25px; ";	
	if (font == &FONT3)
		ss = "font-size: 38px; font-weight: bold; ";	
	
	switch (col){
		case (CYAN_L):
			ss += "color: #2150BC; ";	
			break;
		case (GREEN_L):
			ss += "color: #22A022; ";	
			break;
		case (PINK_M):
			ss += "color: #FF00FF; ";	
			break;
		default:
			ss += "color: #000000; ";	
	}
	
	return ss;
}
String HMS(long time)
{
	String myTime;
	char buf[10];
	int h, m, s;
	h = (time/3600); 	
	m = (time -(3600*h))/60;	
	s = (time -(3600*h)-(m*60));
	
	sprintf(buf,"%i:%02i:%02i", h,m,s);
	myTime = buf;
	
	//myTime =  itoa(h,buf,10); myTime += ":";
	//myTime += itoa(m,buf,10); myTime += ":";
	//myTime += itoa(s,buf,10); 
	return myTime;
}
#endif /* MYHTTP_H */