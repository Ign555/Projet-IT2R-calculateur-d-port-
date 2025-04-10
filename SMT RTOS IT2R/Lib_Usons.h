/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C

#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons

extern   ARM_DRIVER_CAN         Driver_CAN2;

extern void myCAN2_callback(uint32_t obj_idx, uint32_t event);

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2

void Init_I2C(void);
void write1byte(uint8_t composant, uint8_t registre, uint8_t valeur);
unsigned char read1byte(unsigned char composant, unsigned char registre);
char I2C_ReadByte(uint8_t dev_addr, uint8_t reg_addr);
char get1Sens(char addr);


#endif


