/* Screen COLOURS AND FONTS
*/
#ifndef  MYCOLOURFONT_H
#define  MYCOLOURFONT_H

#ifdef ILI9488
	#include "myFonts/DejaVu_Sans_Mono_Bold_18.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_22.h"
	//#include "myFonts/DejaVu_Sans_Mono_Bold_32.h"
		#include "myFonts/DejaVu_Sans_Mono_Bold_28.h"
		#include "myFonts/DejaVu_Sans_Mono_Bold_38.h"
	//#include "myFonts/DejaVu_Sans_Mono_Bold_42.h"
	
	#define FONT0 DejaVu_Sans_Mono_Bold_18  // smallest
	#define FONT1 DejaVu_Sans_Mono_Bold_22  // small
	#define FONT2 DejaVu_Sans_Mono_Bold_28 // larger
	#define FONT3 DejaVu_Sans_Mono_Bold_38        // biggest
	
	#define FH0 11	// 1/2 font size +2
	#define FH1 13
	#define FH2 16
	#define FH3 21
#else // ILI9341
	#include "myFonts/DejaVu_Sans_Mono_Bold_16.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_18.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_26.h"
	#include "myFonts/DejaVu_Sans_Mono_Bold_38.h"
	
	#define FONT0 DejaVu_Sans_Mono_Bold_16  // smallest
	#define FONT1 DejaVu_Sans_Mono_Bold_18  // small
	#define FONT2 DejaVu_Sans_Mono_Bold_26  // larger
	#define FONT3 DejaVu_Sans_Mono_Bold_38        // biggest
	
	#define FH0 10 // 1/2 font size +2
	#define FH1 11
	#define FH2 15
	#define FH3 21
#endif

#define MY_DARKGREEN 0x03E0
#define MY_RED 0xF800
#define MY_WHITE 0xFFFF
#define MY_YELLOW 0xFFE0
#define MY_GREEN RGB565(0,36,0)
#define DK_GREEN RGB565(0,12,0)
#define RGB565(R, G, B) (((R)<<11)|((G)<<5)|(B))  
#define LIGHTYELLOW RGB565(31,63,0)
#define YELLOW_VL RGB565(31,63,10)
#define CYAN_L RGB565(15,45,31)
#define CYAN_VL RGB565(21,63,31)
#define CYAN_M RGB565(8,32,24)
#define GREEN_L RGB565(5,63,5)
#define CYAN_M RGB565(8,32,24)
#define PINK_M RGB565(31,28,31)
#define GREY_DK RGB565(8,16,8)
#define GREY_LT RGB565(16,32,16)
#define BLUE_DK RGB565(0,0,16)
// special colours
#define VOLT_COL LIGHTYELLOW
#define AMP_COL GREEN_L
#define WATT_COL PINK_M
#define MY_DARKBLUE RGB565(0,0,12)
#define MY_DARKRED RGB565(12,0,0)
// screen background
#define BGCOL 0 
#define BGCOLA MY_DARKBLUE 
#define BGCOLB MY_DARKRED
#define BGCOLC DK_GREEN
#define BGHIGH RGB565(5,11,4)  
#define MIDBG RGB565(5,11,4) 
#define RECTOUT GREY_DK 
// value text
#define TEXTCOL  MY_YELLOW
#define HIGHLIGHTCOL MY_WHITE
//menu button items
#define BTEXTCOL RGB565(16,32,16) //light grey
#define BHIGHCOL 0xffff  //LIGHTYELLOW // MY_YELLOW // ILI9341_BLACK 
//menu button backgrounds
#define BBORDHIGH BHIGHCOL
#define BBORD BGCOL
#define BBGHIGH RGB565(0,10,22)
#define BBXHIGH RGB565(0,0,9)
#define BBGHIGH2 RGB565(31,40,0)
#define BBXHIGH2 RGB565(16,20,0)
#define BBGHIGH3 MY_GREEN
#define BBXHIGH3 MY_DARKRED

// error screen backgrounds
#define ERR_BG_A MY_DARKGREEN
#define ERR_BG_B MY_RED

// indicator BG colours
#define ITRIG MY_RED 			// Action triggered
#define IENABLE	MY_DARKGREEN	// Function enabled
#define IND_ON MY_GREEN
#define IND_OFFC MY_RED

#endif
