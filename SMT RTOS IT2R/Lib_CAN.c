#include "main.h"

#include "Board_LED.h"                  // ::Board Support:LED
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN

extern   ARM_DRIVER_CAN         Driver_CAN2;

extern void myCAN2_callback(uint32_t obj_idx, uint32_t event);


void InitCan2 (void) {

	Driver_CAN2.Initialize(NULL,myCAN2_callback);
	Driver_CAN2.PowerControl(ARM_POWER_FULL);
	
	Driver_CAN2.SetMode(ARM_CAN_MODE_INITIALIZATION);
	Driver_CAN2.SetBitrate( ARM_CAN_BITRATE_NOMINAL,
													125000,
													ARM_CAN_BIT_PROP_SEG(5U)   |         // Set propagation segment to 5 time quanta
                          ARM_CAN_BIT_PHASE_SEG1(1U) |         // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                          ARM_CAN_BIT_PHASE_SEG2(1U) |         // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                          ARM_CAN_BIT_SJW(1U));                // Resynchronization jump width is same as phase segment 2
	// Mettre ici les filtres ID de réception sur objet 0
	Driver_CAN2.ObjectSetFilter(0,ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_STANDARD_ID(0x0b6),0);
	Driver_CAN2.ObjectSetFilter(0,ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_STANDARD_ID(0x101),0);
	
	Driver_CAN2.ObjectConfigure(0,ARM_CAN_OBJ_RX);				// Objet 0 du CAN2 pour réception
	Driver_CAN2.ObjectConfigure(2,ARM_CAN_OBJ_TX);				// Objet 2 du CAN2 pour émission
	
	Driver_CAN2.SetMode(ARM_CAN_MODE_NORMAL);					// fin init
}