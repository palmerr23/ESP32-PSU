#ifndef gui_h
#define gui_h

#include "lcd.h"
#include "arial.h"

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
  {0,120,38,38,"1",49,0,1,1,BUTTON},
  {40,120,38,38,"2",50,0,1,1,BUTTON},
  {80,120,38,38,"3",51,0,1,1,BUTTON},
  {120,120,38,38,"4",52,0,1,1,BUTTON},
  {160,120,38,38,"5",53,0,1,1,BUTTON},
  {200,120,38,38,"6",54,0,1,1,BUTTON},
  {240,120,38,38,"7",55,0,1,1,BUTTON},
  {280,120,38,38,"8",56,0,1,1,BUTTON},
  {320,120,38,38,"9",57,0,1,1,BUTTON},
  {360,120,38,38,"0",48,0,1,1,BUTTON},
  {400,120,38,38,"-",45,0,1,1,BUTTON},
  {440,120,38,38,"=",61,0,1,1,BUTTON},
  {0,160,38,38,"q",113,0,1,1,BUTTON},
  {40,160,38,38,"w",119,0,1,1,BUTTON},
  {80,160,38,38,"e",101,0,1,1,BUTTON},
  {120,160,38,38,"r",114,0,1,1,BUTTON},
  {160,160,38,38,"t",116,0,1,1,BUTTON},
  {200,160,38,38,"y",121,0,1,1,BUTTON},
  {240,160,38,38,"u",117,0,1,1,BUTTON},
  {280,160,38,38,"i",105,0,1,1,BUTTON},
  {320,160,38,38,"o",111,0,1,1,BUTTON},
  {360,160,38,38,"p",112,0,1,1,BUTTON},
  {400,160,38,38,"[",91,0,1,1,BUTTON},
  {440,160,38,38,"]",93,0,1,1,BUTTON},
  {20,200,38,38,"a",97,0,1,1,BUTTON},
  {60,200,38,38,"s",115,0,1,1,BUTTON},
  {100,200,38,38,"d",100,0,1,1,BUTTON},
  {140,200,38,38,"f",102,0,1,1,BUTTON},
  {180,200,38,38,"g",103,0,1,1,BUTTON},
  {220,200,38,38,"h",104,0,1,1,BUTTON},
  {260,200,38,38,"j",106,0,1,1,BUTTON},
  {300,200,38,38,"k",107,0,1,1,BUTTON},
  {340,200,38,38,"l",108,0,1,1,BUTTON},
  {380,200,38,38,";",59,0,1,1,BUTTON},
  {420,200,38,38,"'",39,0,1,1,BUTTON},
  {40,240,38,38,"z",122,0,1,1,BUTTON},
  {80,240,38,38,"x",120,0,1,1,BUTTON},
  {120,240,38,38,"c",99,0,1,1,BUTTON},
  {160,240,38,38,"v",118,0,1,1,BUTTON},
  {200,240,38,38,"b",98,0,1,1,BUTTON},
  {240,240,38,38,"n",110,0,1,1,BUTTON},
  {280,240,38,38,"m",109,0,1,1,BUTTON},
  {320,240,38,38,",",44,0,1,1,BUTTON},
  {360,240,38,38,".",46,0,1,1,BUTTON},
  {400,240,38,38,"/",47,0,1,1,BUTTON},
  {440,240,38,38,"`",96,0,1,1,BUTTON},
  {0,240,38,38,"\\",92,0,1,1,BUTTON},
  {0,280,98,38,"SHIFT",-1,0,1,1,BUTTON},
  {100,280,198,38,"SPACE",32,0,1,1,BUTTON},
  {300,280,78,38,"<-",8,0,1,1,BUTTON},
  {380,280,98,38,"ENTER",13,0,1,1,BUTTON},
  {360,0,120,38,"CANCEL",27,0,1,1,BUTTON}
};

guiObject upperCase[KEYCOUNT]={
  {0,120,38,38,"!",33,0,1,1,BUTTON},
  {40,120,38,38,"@",64,0,1,1,BUTTON},
  {80,120,38,38,"#",35,0,1,1,BUTTON},
  {120,120,38,38,"$",36,0,1,1,BUTTON},
  {160,120,38,38,"%",37,0,1,1,BUTTON},
  {200,120,38,38,"^",94,0,1,1,BUTTON},
  {240,120,38,38,"&",38,0,1,1,BUTTON},
  {280,120,38,38,"*",42,0,1,1,BUTTON},
  {320,120,38,38,"(",40,0,1,1,BUTTON},
  {360,120,38,38,")",41,0,1,1,BUTTON},
  {400,120,38,38,"_",95,0,1,1,BUTTON},
  {440,120,38,38,"+",43,0,1,1,BUTTON},
  {0,160,38,38,"Q",81,0,1,1,BUTTON},
  {40,160,38,38,"W",87,0,1,1,BUTTON},
  {80,160,38,38,"E",69,0,1,1,BUTTON},
  {120,160,38,38,"R",82,0,1,1,BUTTON},
  {160,160,38,38,"T",84,0,1,1,BUTTON},
  {200,160,38,38,"Y",89,0,1,1,BUTTON},
  {240,160,38,38,"U",85,0,1,1,BUTTON},
  {280,160,38,38,"I",73,0,1,1,BUTTON},
  {320,160,38,38,"O",79,0,1,1,BUTTON},
  {360,160,38,38,"P",80,0,1,1,BUTTON},
  {400,160,38,38,"[",91,0,1,1,BUTTON},
  {440,160,38,38,"]",93,0,1,1,BUTTON},
  {20,200,38,38,"A",65,0,1,1,BUTTON},
  {60,200,38,38,"S",83,0,1,1,BUTTON},
  {100,200,38,38,"D",68,0,1,1,BUTTON},
  {140,200,38,38,"F",70,0,1,1,BUTTON},
  {180,200,38,38,"G",71,0,1,1,BUTTON},
  {220,200,38,38,"H",72,0,1,1,BUTTON},
  {260,200,38,38,"J",74,0,1,1,BUTTON},
  {300,200,38,38,"K",75,0,1,1,BUTTON},
  {340,200,38,38,"L",76,0,1,1,BUTTON},
  {380,200,38,38,":",58,0,1,1,BUTTON},
  {420,200,38,38,"\"",148,0,1,1,BUTTON},
  {40,240,38,38,"Z",90,0,1,1,BUTTON},
  {80,240,38,38,"X",88,0,1,1,BUTTON},
  {120,240,38,38,"C",67,0,1,1,BUTTON},
  {160,240,38,38,"V",86,0,1,1,BUTTON},
  {200,240,38,38,"B",66,0,1,1,BUTTON},
  {240,240,38,38,"N",78,0,1,1,BUTTON},
  {280,240,38,38,"M",77,0,1,1,BUTTON},
  {320,240,38,38,"<",60,0,1,1,BUTTON},
  {360,240,38,38,">",62,0,1,1,BUTTON},
  {400,240,38,38,"?",63,0,1,1,BUTTON},
  {440,240,38,38,"~",126,0,1,1,BUTTON},
  {0,240,38,38,"|",124,0,1,1,BUTTON},
  {0,280,98,38,"SHIFT",-1,1,1,1,BUTTON},    //note that shift is pressed for upper case
  {100,280,198,38,"SPACE",32,0,1,1,BUTTON},
  {300,280,78,38,"<-",8,0,1,1,BUTTON},
  {380,280,98,38,"ENTER",13,0,1,1,BUTTON},
  {360,0,120,38,"CANCEL",0,0,1,1,BUTTON}
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
  guiObject titleButton={16,0,strlen(t)*fontWidth(GUI_FONT)+10,38,t,0,1,1,1,BUTTON};
  guiObject textButton={16,54,n*fontWidth(GUI_FONT)+10,50,a,0,0,1,1,BUTTON};
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

