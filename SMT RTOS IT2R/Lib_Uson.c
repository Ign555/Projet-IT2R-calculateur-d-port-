#include "main.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C

#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

static volatile uint32_t I2C_Event;

extern ARM_DRIVER_I2C Driver_I2C1;


void write1byte(uint8_t composant, uint8_t registre, uint8_t valeur)//	To write a single byte
{
	unsigned char tab[2];
	tab[0] = registre;
	tab[1] = valeur;
	Driver_I2C1.MasterTransmit (composant, tab, 2, false);
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
}


unsigned char read1byte(unsigned char composant, unsigned char registre)//	To read a single byte
{
	unsigned char maValeur;
	unsigned char tab[1];
	tab[0] = registre;
	
	Driver_I2C1.MasterTransmit (composant, tab, 1, true);
	
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
	
	Driver_I2C1.MasterReceive (composant, &maValeur, 1, false);
	
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin 
	return maValeur;
}


char get1Sens(char addr)//	Only for reading
{
	  char msb, lsb;
		msb = read1byte(addr, 0x02);
		lsb = read1byte(addr, 0x03);
		return (msb << 8) | lsb;
}

void Init_I2C(void)//	Initialisation
	{
		Driver_I2C1.Initialize(NULL);
		Driver_I2C1.PowerControl(ARM_POWER_FULL);
		Driver_I2C1.Control(	ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
		//Driver_I2C1.Control(	ARM_I2C_BUS_CLEAR, 0 );
  }
	
//	static void SystemClock_Config(void)
//{
//  RCC_ClkInitTypeDef RCC_ClkInitStruct;
//  RCC_OscInitTypeDef RCC_OscInitStruct;

//  /* Enable Power Control clock */
//  __HAL_RCC_PWR_CLK_ENABLE();

//  /* The voltage scaling allows optimizing the power consumption when the device is 
//     clocked below the maximum system frequency, to update the voltage scaling value 
//     regarding system frequency refer to product datasheet.  */
//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

//  /* Enable HSE Oscillator and activate PLL with HSE as source */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//  RCC_OscInitStruct.PLL.PLLM = 8;
//  RCC_OscInitStruct.PLL.PLLN = 336;
//  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//  RCC_OscInitStruct.PLL.PLLQ = 7;
//  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    /* Initialization Error */
//  }

//  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
//     clocks dividers */
//  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
//  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
//  {
//    /* Initialization Error */

//  }

//  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
//  if (HAL_GetREVID() == 0x1001)
//  {
//    /* Enable the Flash prefetch */
//    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
//  }
//}

///**
//  * @brief  This function is executed in case of error occurrence.
//  * @param  None
//  * @retval None
//  */
