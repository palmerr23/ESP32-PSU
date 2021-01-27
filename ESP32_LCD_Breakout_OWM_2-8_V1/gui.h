#ifndef gui_h
#define gui_h

#include "lcd.h"

#ifdef F1
#include "arial.h"
#else
#include "bigfont.h"
#endif

enum guiObjectType{
  BUTTON=1
};

struct guiObject{
  int x;
  int y;
  int w;
  int h;
  char* text;
  int value;
  bool pressed;
  bool visible;
  bool enabled;
  guiObjectType type;
};

#define KEYCOUNT 52
#define SHIFTKEY 47

guiObject lowerCase[KEYCOUNT]={
  {4,90,24,  24,"1",49,0,1,1,BUTTON},
  {30,90,24, 24,"2",50,0,1,1,BUTTON},
  {56,90,24, 24,"3",51,0,1,1,BUTTON},
  {82,90,24, 24,"4",52,0,1,1,BUTTON},
  {108,90,24,24,"5",53,0,1,1,BUTTON},
  {134,90,24,24,"6",54,0,1,1,BUTTON},
  {160,90,24,24,"7",55,0,1,1,BUTTON},
  {186,90,24,24,"8",56,0,1,1,BUTTON},
  {212,90,24,24,"9",57,0,1,1,BUTTON},
  {238,90,24,24,"0",48,0,1,1,BUTTON},
  {264,90,24,24,"-",45,0,1,1,BUTTON},
  {290,90,24,24,"=",61,0,1,1,BUTTON},
  
  {4,116,24,  24,"q",113,0,1,1,BUTTON},
  {30,116,24 ,24,"w",119,0,1,1,BUTTON},
  {56,116,24, 24,"e",101,0,1,1,BUTTON},
  {82,116,24, 24,"r",114,0,1,1,BUTTON},
  {108,116,24,24,"t",116,0,1,1,BUTTON},
  {134,116,24,24,"y",121,0,1,1,BUTTON},
  {160,116,24,24,"u",117,0,1,1,BUTTON},
  {186,116,24,24,"i",105,0,1,1,BUTTON},
  {212,116,24,24,"o",111,0,1,1,BUTTON},
  {238,116,24,24,"p",112,0,1,1,BUTTON},
  {264,116,24,24,"[",91,0,1,1,BUTTON},
  {290,116,24,24,"]",93,0,1,1,BUTTON},
  
  {4,142,24,   24,"a",97,0,1,1,BUTTON},
  {30,142,24,  24,"s",115,0,1,1,BUTTON},
  {56,142,24,  24,"d",100,0,1,1,BUTTON},
  {82,142,24,  24,"f",102,0,1,1,BUTTON},
  {108,142,24, 24,"g",103,0,1,1,BUTTON},
  {134,142,24, 24,"h",104,0,1,1,BUTTON},
  {160,142,24, 24,"j",106,0,1,1,BUTTON},
  {186,142,24, 24,"k",107,0,1,1,BUTTON},
  {212,142,24, 24,"l",108,0,1,1,BUTTON},
  {238,142,24, 24,";",59,0,1,1,BUTTON},
  {264,142,24, 24,"'",39,0,1,1,BUTTON}, // short row
  
  {30,168,24,  24,"z",122,0,1,1,BUTTON}, //short row
  {56,168,24,  24,"x",120,0,1,1,BUTTON},
  {82,168,24,  24,"c",99,0,1,1,BUTTON},
  {108,168,24, 24,"v",118,0,1,1,BUTTON},
  {134,168,24, 24,"b",98,0,1,1,BUTTON},
  {160,168,24, 24,"n",110,0,1,1,BUTTON},
  {186,168,24, 24,"m",109,0,1,1,BUTTON},
  {212,168,24, 24,",",44,0,1,1,BUTTON},
  {238,168,24, 24,".",46,0,1,1,BUTTON},
  {264,168,24, 24,"/",47,0,1,1,BUTTON},
  {290,168,24, 24,"`",96,0,1,1,BUTTON},
  
  {4,194, 24, 24,"\\",92,0,1,1,BUTTON},
  {30,194,76, 24,"SHFT",-1,0,1,1,BUTTON},
  {108,194,90,24,"SPACE",32,0,1,1,BUTTON},
  {202,194,40,24,"<-",8,0,1,1,BUTTON},
  {244,194,70,24,"ENT",13,0,1,1,BUTTON},
  
  {220,6,96, 24,"CANCEL",27,0,1,1,BUTTON}
};

guiObject upperCase[KEYCOUNT]={
  {4,90,  24, 24,"!",33,0,1,1,BUTTON},
  {30,90, 24, 24,"@",64,0,1,1,BUTTON},
  {56,90 ,24, 24,"#",35,0,1,1,BUTTON},
  {82,90, 24, 24,"$",36,0,1,1,BUTTON},
  {108,90,24, 24,"%",37,0,1,1,BUTTON},
  {134,90,24, 24,"^",94,0,1,1,BUTTON},
  {108,90,24, 24,"&",38,0,1,1,BUTTON},
  {186,90,24, 24,"*",42,0,1,1,BUTTON},
  {212,90,24, 24,"(",40,0,1,1,BUTTON},
  {238,90,24, 24,")",41,0,1,1,BUTTON},
  {264,90,24, 24,"_",95,0,1,1,BUTTON},
  {290,90,24, 24,"+",43,0,1,1,BUTTON},
  
  {4,116,  24, 24,"Q",81,0,1,1,BUTTON},
  {30,116, 24, 24,"W",87,0,1,1,BUTTON},
  {56,116, 24, 24,"E",69,0,1,1,BUTTON},
  {82,116, 24, 24,"R",82,0,1,1,BUTTON},
  {108,116,24, 24,"T",84,0,1,1,BUTTON},
  {134,116,24, 24,"Y",89,0,1,1,BUTTON},
  {108,116,24, 24,"U",85,0,1,1,BUTTON},
  {186,116,24, 24,"I",73,0,1,1,BUTTON},
  {212,116,24, 24,"O",79,0,1,1,BUTTON},
  {238,116,24, 24,"P",80,0,1,1,BUTTON},
  {264,116,24, 24,"[",91,0,1,1,BUTTON},
  {290,116,24, 24,"]",93,0,1,1,BUTTON},
  
  {4,142,24,   24,"A",65,0,1,1,BUTTON},
  {30,142,24,  24,"S",83,0,1,1,BUTTON},
  {56,142,24,  24,"D",68,0,1,1,BUTTON},
  {82,142,24,  24,"F",70,0,1,1,BUTTON},
  {108,142,24, 24,"G",71,0,1,1,BUTTON},
  {134,142,24, 24,"H",72,0,1,1,BUTTON},
  {160,142,24, 24,"J",74,0,1,1,BUTTON},
  {186,142,24, 24,"K",75,0,1,1,BUTTON},
  {212,142,24, 24,"L",76,0,1,1,BUTTON},
  {238,142,24, 24,":",58,0,1,1,BUTTON},
  {264,142,24, 24,"\"",148,0,1,1,BUTTON}, // short row
  
  {30,168,24,  24,"Z",90,0,1,1,BUTTON}, // short row
  {56,168,24,  24,"X",88,0,1,1,BUTTON},
  {82,168,24,  24,"C",67,0,1,1,BUTTON},
  {108,168,24, 24,"V",86,0,1,1,BUTTON},
  {134,168,24, 24,"B",66,0,1,1,BUTTON},
  {108,168,24, 24,"N",78,0,1,1,BUTTON},
  {186,168,24, 24,"M",77,0,1,1,BUTTON},
  {212,168,24, 24,"<",60,0,1,1,BUTTON},
  {238,168,24, 24,">",62,0,1,1,BUTTON},
  {264,168,24, 24,"?",63,0,1,1,BUTTON},
  {290,168,24, 24,"~",126,0,1,1,BUTTON},
  
  {4,194,24,  24,"|",124,0,1,1,BUTTON},
  {30,194,76, 24,"SHFT",-1,1,1,1,BUTTON}, //note that shift is pressed for upper case
  {108,194,90,24,"SPACE",32,0,1,1,BUTTON},
  {202,194,40,24,"<-",8,0,1,1,BUTTON},
  {244,194,70,24,"ENT",13,0,1,1,BUTTON},
  
  {220,6,96,  24,"CANCEL",27,0,1,1,BUTTON}
};
 


void guiDraw(guiObject g){
  unsigned long f,b,t,i;
  i=strlen(g.text);
  f=GUI_FOREGROUND_COLOUR;
  b=GUI_BACKGROUND_COLOUR;
  t=GUI_TEXT_COLOUR;
  if(g.pressed){
    f=GUI_BACKGROUND_COLOUR;
    b=GUI_FOREGROUND_COLOUR;    
    t=GUI_BACKGROUND_COLOUR;
  }
  if(g.enabled==false){
    f=GUI_DISABLE_COLOUR;
    b=GUI_BACKGROUND_COLOUR;
    t=GUI_DISABLE_COLOUR;    
  }
  if(g.visible==false){
    return;               //don't do anything
    f=GUI_BACKGROUND_COLOUR;
    b=GUI_BACKGROUND_COLOUR;
    t=GUI_BACKGROUND_COLOUR;
  }
  if(g.type==BUTTON){
    //drawbox(g.x,g.y,g.x+g.w,g.y+g.h,b);   //rough, does overdraw
    int x0,y0,x1,x2,y1,y2;      //1:top left of text box, 2: top left of bottom right rectangle
    x0=g.x;
    y0=g.y;
    x1=g.x+(g.w-i*fontWidth(GUI_FONT))/2;
    x2=x1+i*fontWidth(GUI_FONT);
    y1=g.y+(g.h-fontHeight(GUI_FONT))/2;
    y2=y1+fontHeight(GUI_FONT);
    x0=x0+1;
    y0=y0+1;
    characterarray(x1,y1,g.text,t,b,GUI_FONT);  
    drawbox(x0,g.y+1,g.x+g.w-1,y1-1,b); //above text
    drawbox(x0,y2,g.x+g.w-1,g.y+g.h-1,b);//below text
    drawbox(x0,y1,x1-1,y2-1,b);         //left of text
    drawbox(x2,y1,g.x+g.w-1,y2-1,b);    //right of text    
    hline(g.x,g.y,g.x+g.w-1,f);
    hline(g.x,g.y+g.h,g.x+g.w-1,f);
    vline(g.x,g.y+1,g.y+g.h-1,f);
    vline(g.x+g.w,g.y,g.y+g.h,f);
  }
}

int findKey(boolean c,int key){       //find index from keycode
  int r=-1;
  int i;
  if(c){
    for(i=0;i<KEYCOUNT;i++){
      if(upperCase[i].value==key){r=i;}
    }
  }else{
    for(i=0;i<KEYCOUNT;i++){
      if(lowerCase[i].value==key){r=i;}
    }    
  }
  return r;
}

int drawKeyboard(boolean c){   //0 for lower, 1 for upper, caller should set pressed buttons, except shift
  int i;
  if(c){
    guiDraw(upperCase[SHIFTKEY]);   //so keyboard appears to respond
    for(i=0;i<KEYCOUNT;i++){
      guiDraw(upperCase[i]);
      yield();
    }
  }else{    
    guiDraw(lowerCase[SHIFTKEY]);   //so keyboard appears to respond
    for(i=0;i<KEYCOUNT;i++){
      guiDraw(lowerCase[i]);
      yield();
    }
  }
}

boolean isPressed(guiObject* g){
  int x,y;
  x=touchx();
  y=touchy();
  if(x<0){return 0;}
  if(y<0){return 0;}
  if(x<(g->x)){return 0;}
  if(x>(g->x+g->w)){return 0;}
  if(y<(g->y)){return 0;}
  if(y>(g->y+g->h)){return 0;}
  return 1;
}

boolean pressWait(guiObject* g){
  if(isPressed(g)){
    g->pressed=1;
    guiDraw(*g);     
    delay(5);                       //some debouncing
    while(isPressed(g)){delay(1);}  //wait for release
    g->pressed=0;
    guiDraw(*g);
    delay(5);
    return 1;
  }
  return 0;
}

int scanKeyboard(boolean c){        //c=case, returns key or null
  int i;
  if(c){
    for(i=0;i<KEYCOUNT;i++){
      if(isPressed(&upperCase[i])){return upperCase[i].value;}
      yield();
    }
  }else{    
    for(i=0;i<KEYCOUNT;i++){
      if(isPressed(&lowerCase[i])){return lowerCase[i].value;}
      yield();
    }
  }
  return 0;
}
  
int keyAnimate(boolean c){   //scans and animates
  static int lastkey=0;
  int key=0;
  int i;
  key=scanKeyboard(c);
  if(key){
    i=findKey(c,key);
    if(i>-1){
      if(c){
        upperCase[i].pressed=1;
        guiDraw(upperCase[i]);    
        upperCase[i].pressed=0;   //change back so we don't get lost
      }else{
        lowerCase[i].pressed=1;
        guiDraw(lowerCase[i]);    
        lowerCase[i].pressed=0;   //change back so we don't get lost
      }      
    }
  }
  if(lastkey&&(key!=lastkey)){                    //erase last
    i=findKey(c,lastkey);
    if(i>-1){
      if(c){
        guiDraw(upperCase[i]);    
      }else{
        guiDraw(lowerCase[i]);            
      }
    }
  }
  if(key!=lastkey){
    lastkey=key;
    return key;
  }
  return 0;
}

int softKeyboard(char* t,char* a, int n){ //title, destination, max length
  boolean done=0;
  static bool cs=0;
  int i,c;          //pointer, character
  guiObject titleButton={4,0,strlen(t)*fontWidth(GUI_FONT)+10,38,t,0,1,1,1,BUTTON};
  guiObject textButton={4,44,n*fontWidth(GUI_FONT)+10,30,a,0,0,1,1,BUTTON};
  for(i=0;i<n;i++){a[i]=32;}        //fill with spaces
  a[n-1]=0;                         //null term
  i=0;
  guiDraw(titleButton);
  drawKeyboard(cs);
  while(!done){
    c=keyAnimate(cs);
    if(c){
      if((c>31)&&(c<128)){    //ASCII
        a[i]=c;
        i=i+1;
        if(i>(n-2)){i=n-2;}
      }
      if(c==8){if(i>0){a[i]=32;i=i-1;}}
      if(c==13){a[i]=0;return 1;}
      if(c==27){for(i=0;i<n;i++){a[n]=0;}return 0;} //fill with null and return 0
      if(c==-1){cs=!cs;drawKeyboard(cs);}   //toggle keyboard      
      //Serial.println(c);
      //delay(20);          //smoothen
    }
    if(millis()&512){a[i]='_';}else{a[i]=' ';}      //arbitrary cursor flash
    guiDraw(textButton);
    //characterarray(20,67,a,GUI_FOREGROUND_COLOUR,GUI_BACKGROUND_COLOUR,GUI_FONT);
    yield();
  }  
}

#endif
