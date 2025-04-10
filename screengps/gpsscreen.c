#include "LPC17xx.h"                    // Device header
#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include "cmsis_os.h"



extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;




int main(){

	
	float lat = 4878.905, lon = 232.80685;
	char vallat, vallon;
	
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	
	
		GLCD_DrawRectangle(187,36,110,48);
		GLCD_DrawRectangle(62,10,110,48);
		GLCD_DrawRectangle(62,80,40,104);
		GLCD_DrawRectangle(98,210.,76,26);
	while(1){
		
		
		GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
		GLCD_DrawRectangle((int)lat, ((int)lon-240), 5, 5);
		
		//code to get lat & lon
		lat = 4878.6970;
		lon = 232.77;
		
		lat /= 4878.6970;
		lon /= 232.7755;
		
		vallat = lat;
		vallon = lon;
		
		
//		lat *= 0.0005866;
//		lon *= 0.0002604;
		
		GLCD_SetForegroundColor(GLCD_COLOR_RED);
		GLCD_DrawRectangle(vallat, vallon-235, 5, 5);
		GLCD_DrawRectangle(vallat+1, vallon-234, 3, 3);
		
		
	
		
		
	}
	
}


