#ifndef lcd_h
#define lcd_h

#include <SPI.h>
void setrotation(byte r);
//colours are 24bit/18bit
// the ul is because we need 24 bits to store the colours
#define BLACK 0x000000ul
#define BLUE 0x0000FFul
#define RED 0xFF0000ul
#define GREEN 0x00FF00ul
#define CYAN 0x00FFFFul
#define MAGENTA 0xFF00FFul
#define YELLOW 0xFFFF00ul  
#define WHITE 0xFFFFFFul
#define GREY 0x808080ul

//colour scheme

#define GUI_BACKGROUND_COLOUR BLACK
//#define GUI_BACKGROUND_COLOUR BLUE
#define GUI_FOREGROUND_COLOUR WHITE
#define GUI_TEXT_COLOUR WHITE
#define GUI_DISABLE_COLOUR GREY

// for breakout
#define RSPIN (13) //(DC pin)
#define CSPIN (5) 
#define TOUCH_CS_PIN (15) 
#define SD_CS_PIN (255) 

#define Z_TOUCH_THRESHOLD 120
#define TOUCH_X0 110
#define TOUCH_X1 1850
#define TOUCH_Y0 1880
#define TOUCH_Y1 76
#define TOUCH_OVERSAMPLE 16

#define LCD_SPI_SPEED  (40000000UL) //(10000000UL) //
#define TOUCH_SPI_SPEED  (2000000UL) //(4000000UL) //

int width,height,rotation,rotation_t;

void data8(byte d){
  SPI.transfer(d);
}

void cmd8(byte d){
  digitalWrite(RSPIN,LOW);
  data8(d);
  digitalWrite(RSPIN,HIGH);  
}
uint8_t readcommand8(uint8_t commandByte)
{ uint8_t result;
  digitalWrite(CSPIN,LOW); 
    digitalWrite(RSPIN,LOW);
      SPI.transfer(commandByte);
    digitalWrite(RSPIN,HIGH);  
    result = SPI.transfer(0);
  digitalWrite(CSPIN,HIGH); 
  return result;
}

void setarea(int x0,int y0,int x1,int y1){
  int t;
  if(x1<x0){int t=x0;x0=x1;x1=t;}   //sort x
  if(y1<y0){int t=y0;y0=y1;y1=t;}   //sort y
  digitalWrite(CSPIN,LOW);
  cmd8(0x2A);   //set column start/end
  data8(x0>>8); 
  data8(x0);    //start
  data8(x1>>8);
  data8(x1);    //end
  cmd8(0x2B);   //set row start/end
  data8(y0>>8);
  data8(y0);    //start
  data8(y1>>8);
  data8(y1);    //end
  cmd8(0x2C);   //draw!
  digitalWrite(CSPIN,HIGH);
}

void displaySetup(){
  int i;
  width=320;  // 2.8" screen
  height=240;
  rotation=SCREENROT;
  rotation_t = TOUCHROT; // screen and touch panel rotations may be different
  SPI.begin();
  SPI.setFrequency(LCD_SPI_SPEED);
  //SPI.beginTransaction(SPISettings(60000000, MSBFIRST, SPI_MODE3));  //assume no-one else is using SPI
  pinMode(CSPIN,OUTPUT);
  pinMode(RSPIN,OUTPUT);
  pinMode(TOUCH_CS_PIN,OUTPUT);  
  digitalWrite(CSPIN,HIGH);
  digitalWrite(RSPIN,HIGH);
  digitalWrite(TOUCH_CS_PIN,HIGH);    
  delay(100);    //no reset pin, allow some time to come up
  digitalWrite(CSPIN,LOW);
  cmd8(0x1);    //sw reset
  delay(120);
  cmd8(0x11);   //Sleep out
  delay(120);
  cmd8(0x13);   //normal
  cmd8(0x20);   //no inversion
  cmd8(0x28);   //display off
  cmd8(0x38);   //idle mode off
  cmd8(0xC0);   //power control 1
  data8(0x17);
  data8(0x15);
  cmd8(0xC1);   //power control 2
  data8(0x41);
  cmd8(0xC5);   //VCOM control
  data8(0x0e);
  data8(0x0e);
  cmd8(0x36);   //Memory Access control
  data8(88);    //suits rotation 0
  cmd8(0x3A);   //Pixel interface format
  data8(0x66);
  cmd8(0xB4);   //inversion control
  data8(0x2);
  cmd8(0xB6);   //Function control
  data8(0x2);
  data8(0x2);
  data8(0x3B);
  cmd8(0x29);   //display on
  cmd8(0x2A);   //set column
  data8(0x0);
  data8(0x0);
  data8(0x1);
  data8(0x3F);
  cmd8(0x2B);   //set row
  data8(0x0);
  data8(0x0);
  data8(0x1);
  data8(0xDF);
  cmd8(0x2C);   //draw
  digitalWrite(CSPIN,HIGH);
  delay(10);
  setrotation(rotation);
}

void setrotation(byte r){
  digitalWrite(CSPIN,LOW);
  switch(r){
    case 0:
      rotation=0;     //portrait 1
      width=240;
      height=320;
      cmd8(0x36);   //Memory access control
      data8(88);   //sets orientation/scan direction/flip
      setarea(0,0,width-1,height-1);
      break;
    case 1:
      rotation=1;     //landscape 1
      width=320;
      height=240;
      cmd8(0x36);   //Memory access control
      data8(56);   //sets orientation/scan direction/flip
      setarea(0,0,width-1,height-1);
      break;
    case 2:
      rotation=2;     //portrait reversed
      width=240;
      height=320;
      cmd8(0x36);   //Memory access control
      data8(152);   //sets orientation/scan direction/flip
      setarea(0,0,width-1,height-1);
      break;
    case 3:
      rotation=3;     //landscape reversed
      width=320;
      height=240;
      cmd8(0x36);   //Memory access control
      data8(248);   //sets orientation/scan direction/flip
      setarea(0,0,width-1,height-1);
      break;
  }
  digitalWrite(CSPIN,HIGH);
}

void drawbox(int x0,int y0,int x1,int y1,unsigned long c){
  int t,x,y;
  if(x1<x0){int t=x0;x0=x1;x1=t;}   //sort x
  if(y1<y0){int t=y0;y0=y1;y1=t;}   //sort y
  setarea(x0,y0,x1,y1);
  x1++;
  y1++;
  digitalWrite(CSPIN,LOW);
  for(y=y0;y<y1;y++){
    for(x=x0;x<x1;x++){
      data8(c>>16);
      data8(c>>8);
      data8(c);      
    }
  }
  digitalWrite(CSPIN,HIGH);
}

void clear(unsigned long c){
  int x,y;
  setarea(0,0,width-1,height-1);
  digitalWrite(CSPIN,LOW);
  for(x=0;x<width;x++){
    for(y=0;y<height;y++){
      data8(c>>16);
      data8(c>>8);
      data8(c);
    }
    yield();
  }
  digitalWrite(CSPIN,HIGH);
}

char fontWidth(const uint8_t* font){
  return font[0]; 
}

char fontHeight(const uint8_t* font){
  return font[1];
}

int character(int x, int y, char c, unsigned long f, unsigned long b, const uint8_t* font){
  unsigned int w,h,c0,c1,p;
  unsigned int p0;
  char d;
  w=font[0];
  h=font[1];
  c0=font[2];
  c1=font[3]+c0;
  if(c<c0){return 0;}   //out of range
  if(c>c1){return 0;}   //out of range
  p0=4+(w*h*(c-c0))/8;
  //Serial.println(w);
  //Serial.println(h);
  //Serial.println(c0);
  //Serial.println(c1);
  setarea(x,y,x+w-1,y+h-1);    //set area
  digitalWrite(CSPIN,LOW);
  for(p=0;p<w*h;p++){
    if((p&7)==0){
      d=font[p0+(p/8)];
    }
    if(d&128){
      data8(f>>16);data8(f>>8); data8(f);
    }else{
      data8(b>>16);data8(b>>8); data8(b);        
    }
    d=d<<1;
  }
  digitalWrite(CSPIN,HIGH);
  return w;             //allows arrays to be printed in a row
}

void characterarray(int x, int y, char *c, unsigned long f, unsigned long b, const uint8_t* font){
  while(*c){
    x=x+character(x,y,*c++,f,b,font);
    if(x>width){x=0;}      //wrap around (will probably look ugly, but better than nothing)
  }    
}

void point(int x, int y, unsigned long c){
  setarea(x,y,x,y);
  digitalWrite(CSPIN,LOW);
  data8(c>>16);
  data8(c>>8);
  data8(c);      
  digitalWrite(CSPIN,HIGH);
}

int touchxraw(){ 
  long n=0;
  int k;
  SPI.setFrequency(TOUCH_SPI_SPEED);
  digitalWrite(TOUCH_CS_PIN,LOW);  
  SPI.transfer(0x91);
  n=SPI.transfer16(0x91);   //ignore first
  for(k=0;k<TOUCH_OVERSAMPLE-1;k++){
    n=n+SPI.transfer16(0x91);
  }
  digitalWrite(TOUCH_CS_PIN,HIGH);  
  SPI.setFrequency(LCD_SPI_SPEED);
  return (n/TOUCH_OVERSAMPLE)>>4;      //only 12 bits resolution
}

int touchyraw(){
  long n=0;
  int k;
  SPI.setFrequency(TOUCH_SPI_SPEED);
  digitalWrite(TOUCH_CS_PIN,LOW);  
  SPI.transfer(0xD1);
  n=SPI.transfer16(0xD1);   //ignore first
  for(k=0;k<TOUCH_OVERSAMPLE-1;k++){
    n=n+SPI.transfer16(0xD1);
  }
  digitalWrite(TOUCH_CS_PIN,HIGH);  
  SPI.setFrequency(LCD_SPI_SPEED);
  return (n/TOUCH_OVERSAMPLE)>>4;      //only 12 bits resolution
}

int touchzraw(){    //needed to validate press
  int n;
  SPI.setFrequency(TOUCH_SPI_SPEED);
  digitalWrite(TOUCH_CS_PIN,LOW);  
  SPI.transfer(0xB1);
  n=SPI.transfer16(0xB1);   //ignore first
  n=SPI.transfer16(0xB1);
  n=SPI.transfer16(0xB0);
  digitalWrite(TOUCH_CS_PIN,HIGH);  
  return n>>4;      //only 12 bits resolution
}

int touchx(){
  int n,p,z;
  z=touchzraw();
  if(z<Z_TOUCH_THRESHOLD){return -1;} //no touch
  switch(rotation_t){
    case 0:
      n=touchyraw();
      p=map(n,TOUCH_Y1,TOUCH_Y0,0,width-1);
      if(p<0){return -1;}
      if(p>width-1){return -1;}
      return p;
      break;
    case 1:
      n=touchxraw();
      p=map(n,TOUCH_X1,TOUCH_X0,0,width-1);
     Serial.printf("Touched x1: %i (%i), ",p, n);
      if(p<0){return -1;}
      if(p>width-1){return -1;}
      return p;
      break;
    case 2:
      n=touchyraw();
      p=map(n,TOUCH_Y0,TOUCH_Y1,0,width-1);
      if(p<0){return -1;}
      if(p>width-1){return -1;}
      return p;
      break;
    case 3:
      n=touchxraw();
      p=map(n,TOUCH_X0,TOUCH_X1,0,width-1);
     Serial.printf("Touched x3: %i (%i), ",p, n);
      if(p<0){return -1;}
      if(p>width-1){return -1;}
      return p;
      break;
    default:
      return -1;
  }  
}

int touchy(){
  int n,p,z;
  z=touchzraw();
  if(z<Z_TOUCH_THRESHOLD){return -1;} //no touch
  switch(rotation_t){
    case 0:
      n=touchxraw();
      p=map(n,TOUCH_X1,TOUCH_X0,0,height-1);
      if(p<0){return -1;}
      if(p>height-1){return -1;}
      return p;
      break;
    case 1:
      n=touchyraw();
      p=map(n,TOUCH_Y0,TOUCH_Y1,0,height-1);
  Serial.printf("y1: %i (%i)\n",p, n);
      if(p<0){return -1;}
      if(p>height-1){return -1;}
      return p;
      break;
    case 2:
      n=touchxraw();
      p=map(n,TOUCH_X0,TOUCH_X1,0,height-1);
      if(p<0){return -1;}
      if(p>height-1){return -1;}
      return p;
      break;
    case 3:
      n=touchyraw();
      p=map(n,TOUCH_Y1,TOUCH_Y0,0,height-1);
    Serial.printf("y3: %i (%i)\n",p, n);
      if(p<0){return -1;}
      if(p>height-1){return -1;}
      return p;
      break;
    default:
      return -1;
  }  
}

void line(int x1,int y1,int x2,int y2, unsigned long c){
  int steps,stepsx,stepsy,xinc,yinc,x,y,d;
  stepsx=abs(x1-x2);
  stepsy=abs(y1-y2);
  steps=max(stepsx,stepsy)+1;   //if start and end are the same, there's still 1 point
  xinc=constrain(x2-x1,-1,1);
  yinc=constrain(y2-y1,-1,1);
  x=x1;
  y=y1;  
  if(stepsx>stepsy){
    d=stepsx/2;
    for(int i=0;i<steps;i++){
      point(x,y,c);
      x=x+xinc;
      d=d+stepsy;
      if(d>stepsx){d=d-stepsx;y=y+yinc;}
    }
  }else{
    d=stepsy/2;
    for(int i=0;i<steps;i++){
      point(x,y,c);
      y=y+yinc;
      d=d+stepsx;
      if(d>stepsy){d=d-stepsy;x=x+xinc;}
    } 
  }  
}

//better variants of these
void hline(int x1,int y1,int x2, unsigned long c){
  //line(x1,y1,x2,y1,c);      //can be optimised
  int x;
  setarea(x1,y1,x2,y1);
  digitalWrite(CSPIN,LOW);
  for(x=0;x<(x2-x1+1);x++){
    data8(c>>16);
    data8(c>>8);
    data8(c);
  }
  digitalWrite(CSPIN,HIGH);
  
}

void vline(int x1,int y1,int y2, unsigned long c){
  //line(x1,y1,x1,y2,c);      //can be optimised
  int x;
  setarea(x1,y1,x1,y2);
  digitalWrite(CSPIN,LOW);
  for(x=0;x<(y2-y1+1);x++){
    data8(c>>16);
    data8(c>>8);
    data8(c);
  }
  digitalWrite(CSPIN,HIGH);
}

void monoBitmap(int x, int y, unsigned long f, unsigned long b, const uint8_t* bitmap){
  int w,h;
  long p;
  uint8_t d;
  w=bitmap[0];
  h=bitmap[1];
  setarea(x,y,x+w-1,y+h-1);    //set area
  digitalWrite(CSPIN,LOW);
  for(p=0;p<w*h;p++){
    if((p&7)==0){
      d=bitmap[2+(p/8)];
    }
    if(d&128){
      data8(f>>16);data8(f>>8); data8(f);
    }else{
      data8(b>>16);data8(b>>8); data8(b);        
    }
    d=d<<1;
  }
  digitalWrite(CSPIN,HIGH);
}

void eightBitBitmap(int x, int y, const uint8_t* bitmap){    //needs to be 32bit to be word aligned (might as well be 24bit, but no tools to convert)
  int w,h;
  long p;
  uint8_t d;
  w=bitmap[0];
  h=bitmap[1];
  setarea(x,y,x+w-1,y+h-1);    //set area
  digitalWrite(CSPIN,LOW);
  for(p=0;p<w*h;p++){
    d=bitmap[2+p];  
    if(d==0){         //replace black with background colour
      data8(GUI_BACKGROUND_COLOUR>>16);
      data8(GUI_BACKGROUND_COLOUR>>8);
      data8(GUI_BACKGROUND_COLOUR);    
    }else{
      data8(d&224);       //red
      data8((d&28)<<3);   //green
      data8((d&3)<<6);    //blue    
    }
  }  
  digitalWrite(CSPIN,HIGH);
}

#endif
