

#include "Board_LED.h"                  // ::Board Support:LED
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN

extern   ARM_DRIVER_CAN         Driver_CAN2;

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

void InitCan2 (void) ;


#endif



