#include "Driver_SPI.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "stm32f4xx_hal.h" 
#include "Board_LED.h"   
#include "cmsis_os.h"                   // CMSIS RTOS header file


void Clignotant_g (void);
void phare (void);