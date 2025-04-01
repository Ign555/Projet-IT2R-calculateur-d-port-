
#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_sdram.h"
#include "RTE_Components.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_Touch.h"                // ::Board Support:Touchscreen
#include "GUI.h"
#include "stdio.h"
#include "DIALOG.h"
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN


#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) { 
  return os_time; 
}
#endif


/*********************************************************************
* *
Externals
* **********************************************************************
*/
WM_HWIN CreateWindow1(void);
WM_HWIN CreateWindow2(void);

int var=0;
char windowInstance=0;

/*----------------------------------------------------------------------------
 *      GUIThread: GUI Thread for Single-Task Execution Model
 *---------------------------------------------------------------------------*/

osMailQId ID_BAL1;
osMailQDef(BAL1,32,char);

extern ARM_DRIVER_CAN Driver_CAN1;//into the library !!
void myCAN1_callback(uint32_t obj_idx, uint32_t event);
void InitCan1 (void);

void GUIThread (void const *argument);
void switchWindow (void const *argument);
void myTime (void const *argument);
void CANthread(void const *argument);

osThreadId tid_GUIThread;
osThreadId switchWindow_ID;
osThreadId myTime_ID;
osThreadId CANthread_ID;

osThreadDef(GUIThread, osPriorityNormal, 1, 2048);
osThreadDef(switchWindow,osPriorityNormal, 1,0);
osThreadDef(myTime,osPriorityNormal, 1,0);
osThreadDef(CANthread,osPriorityNormal, 1,0);

int Init_GUIThread (void) {

  tid_GUIThread = osThreadCreate (osThread(GUIThread), NULL);
  if (!tid_GUIThread) return(-1);
  
  return(0);
}


/**
  * System Clock Configuration
  *   System Clock source            = PLL (HSE)
  *   SYSCLK(Hz)                     = 200000000
  *   HCLK(Hz)                       = 200000000
  *   AHB Prescaler                  = 1
  *   APB1 Prescaler                 = 4
  *   APB2 Prescaler                 = 2
  *   HSE Frequency(Hz)              = 25000000
  *   PLL_M                          = 25
  *   PLL_N                          = 400
  *   PLL_P                          = 2
  *   PLL_Q                          = 8
  *   VDD(V)                         = 3.3
  *   Main regulator output voltage  = Scale1 mode
  *   Flash Latency(WS)              = 6
  */
static void SystemClock_Config (void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  HAL_PWREx_EnableOverDrive();
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
}



/**
  * Configure the MPU attributes
  */
static void MPU_Config (void) {
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xC0200000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_2MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


/**
  * CPU L1-Cache enable
  */
static void CPU_CACHE_Enable (void) {

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/*********************************************************************
*
*       Main
*/
int main (void) {
	
	osKernelInitialize ();                    // initialize CMSIS-RTOS
  // initialize peripherals here
	Init_GUIThread();// verifier ordre de initialisation (horloge)
	//LED_Initialize();
	Buttons_Initialize();
	InitCan1();
	
	ID_BAL1 = osMailCreate(osMailQ(BAL1), NULL);
	
  // create 'thread' functions that start executing,
	switchWindow_ID = osThreadCreate (osThread(switchWindow), NULL);
	//myTime_ID = osThreadCreate (osThread(myTime), NULL);
	CANthread_ID = osThreadCreate (osThread(CANthread), NULL);
	
	
	
	
	
  osKernelStart ();                         // start thread execution 
	
  osDelay(osWaitForever);
}

/*************************** End of file ****************************/
//--------------------------------------------------------------------------------------------------CAN INIT
void InitCan1 (void) {

	Driver_CAN1.Initialize(NULL, myCAN1_callback);
	Driver_CAN1.PowerControl(ARM_POWER_FULL);
	
	Driver_CAN1.SetMode(ARM_CAN_MODE_INITIALIZATION);
	Driver_CAN1.SetBitrate( ARM_CAN_BITRATE_NOMINAL,
													125000,
													ARM_CAN_BIT_PROP_SEG(5U)   |         // Set propagation segment to 5 time quanta
                          ARM_CAN_BIT_PHASE_SEG1(1U) |         // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                          ARM_CAN_BIT_PHASE_SEG2(1U) |         // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                          ARM_CAN_BIT_SJW(1U));                // Resynchronization jump width is same as phase segment 2
                          
	// Mettre ici les filtres ID de réception sur objet 0
	//Driver_CAN2.ObjectSetFilter( 0, ARM_CAN_FILTER_ID_EXACT_ADD , ARM_CAN_STANDARD_ID(0x161), 0) ;

	Driver_CAN1.ObjectSetFilter( 0, ARM_CAN_FILTER_ID_EXACT_ADD , ARM_CAN_STANDARD_ID(0x0b6), 0) ;//KEIL LPC ID

	//....................................................
		
	Driver_CAN1.ObjectConfigure(0,ARM_CAN_OBJ_RX);				// Objet 0 du CAN1 pour réception, 2 pour la transmission
	
	Driver_CAN1.SetMode(ARM_CAN_MODE_NORMAL);					// fin init
}
//--------------------------------------------------------------------------------------------------CAN CALLBACK
void myCAN1_callback(uint32_t obj_idx, uint32_t event)
{	

    switch (event)
    {
    case ARM_CAN_EVENT_RECEIVE:
        /*  Message was received successfully by the obj_idx object. */
       osSignalSet(CANthread_ID, (1<<10));

        break;
    }
}
//--------------------------------------------------------------------------------------------------SWITCH WINDOW
void switchWindow(void const *argument)
{
	WM_HWIN hDlg;
	char in, *inptr;
	osEvent EVBAL;
	while(1)
	{
		EVBAL = osMailGet(ID_BAL1, osWaitForever);
		inptr = EVBAL.value.p;
		in = *inptr;
		osMailFree(ID_BAL1, inptr);
		
		if(in)
			{
				hDlg = CreateWindow1();
			}
			else
			{
				hDlg = CreateWindow2();
			}
	}
}
//--------------------------------------------------------------------------------------------------RANDOM
void myTime(void const *argument)
{
	while(1)
	{
	}
}
//--------------------------------------------------------------------------------------------------CAN TREAT
void CANthread(void const *argument)
{
	
	ARM_CAN_MSG_INFO   rx_msg_info;
	uint8_t data_buf[8];
	
	while(1)
	{
		osSignalWait((1<<10),osWaitForever);
		Driver_CAN1.MessageRead(0, &rx_msg_info, data_buf, 1);
		var = data_buf[0];
		//osDelay(100);
	}
}
//--------------------------------------------------------------------------------------------------GUI THREAD
void GUIThread (void const *argument) {
	char out, *ptr;
	char user=1, userOld=0, state=0;
	WM_HWIN hDlg;
	
	
	MPU_Config ();
	CPU_CACHE_Enable();                       /* Enable the CPU Cache           */
  HAL_Init();                               /* Initialize the HAL Library     */
  BSP_SDRAM_Init();                         /* Initialize BSP SDRAM           */
  SystemClock_Config();                     /* Configure the System Clock     */

  GUI_Init();
	Touch_Initialize();

	/* Add GUI setup code here */
	hDlg = CreateWindow2();

  while (1) {

    /* All GUI related activities might only be called from here */
		user = Buttons_GetState();
		if(user && !userOld)
		{
			state=!state;
			ptr = osMailAlloc(ID_BAL1,100);
			out=state;
			*ptr = out;
			osMailPut(ID_BAL1, ptr);
		}
		userOld=user;
		if(windowInstance)
		{
			windowInstance=0;
			state=!state;
			ptr = osMailAlloc(ID_BAL1,100);
			out=state;
			*ptr = out;
			osMailPut(ID_BAL1, ptr);
		}
		// mises à jour affichage
		GUI_Exec();
		GUI_Delay(10);
		GUI_X_ExecIdle();             /* Nothing left to do for the moment ... Idle processing */
  }
}