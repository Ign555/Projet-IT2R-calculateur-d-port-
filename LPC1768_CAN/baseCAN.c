/*---------------------------------------------------
* CAN 2 uniquement en TX 
* + réception CAN1 
* avec RTOS et utilisation des fonction CB
* pour test sur 1 carte -> relier CAN1 et CAN2
* 2017-04-02 - XM
---------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions

#include "LPC17xx.h"                    // Device header
#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include "cmsis_os.h"
#include "Board_ADC.h"                  // ::Board Support:A/D Converter

ARM_CAN_MSG_INFO   rx_msg_info;
uint8_t CANdata_buf[8];

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

osMutexId ID_mut_GLCD;
osMutexDef(mut_GLCD);

osThreadId ID_CANthreadR;
osThreadId ID_CANthreadT;
osThreadId ID_ADCtread;

void ADCtread(void const *argument);

extern   ARM_DRIVER_CAN         Driver_CAN1;
extern   ARM_DRIVER_CAN         Driver_CAN2;

osMailQId ID_CAN1;
osMailQDef(CAN1,16,char);

void InitCan1 (void);
void InitCan2 (void);
void myCAN1_callback(uint32_t obj_idx, uint32_t event);
void CANthreadT(void const *argument);
void CANthreadR(void const *argument);

osThreadDef(CANthreadR,osPriorityNormal, 1,0);
osThreadDef(CANthreadT,osPriorityNormal, 1,0);
osThreadDef(ADCtread,osPriorityNormal, 1,0);

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS

	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_DrawString(100, 100, "start");
	ADC_Initialize();
	
	InitCan1();//RX
	InitCan2();//TX

	ID_CANthreadR = osThreadCreate (osThread(CANthreadR), NULL);
	ID_CANthreadT = osThreadCreate (osThread(CANthreadT), NULL);
	ID_ADCtread = osThreadCreate (osThread(ADCtread), NULL);
	
	ID_mut_GLCD = osMutexCreate(osMutex(mut_GLCD));//mutex for LCD
	ID_CAN1 = osMailCreate(osMailQ(CAN1), NULL);//new mailbox

  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}
//--------------------------------------------------------------------------------------------------CAN2 CALLBACK
void myCAN1_callback(uint32_t obj_idx, uint32_t event)
{	

    switch (event)
    {
    case ARM_CAN_EVENT_RECEIVE:
        /*  Message was received successfully by the obj_idx object. */
				Driver_CAN1.MessageRead(0, &rx_msg_info, CANdata_buf, 1);
       osSignalSet(ID_CANthreadR, 0x01);

        break;
    }
}

//--------------------------------------------------------------------------------------------------CAN2 CALLBACK
void myCAN2_callback(uint32_t obj_idx, uint32_t event)
{
    switch (event)
    {
    case ARM_CAN_EVENT_SEND_COMPLETE:
        /* 	Message was sent successfully by the obj_idx object.  */
        osSignalSet(ID_CANthreadT, 0x02);
        break;
    }
}

//--------------------------------------------------------------------------------------------------CAN1 INIT RX
void InitCan1 (void) {
	Driver_CAN1.Initialize(NULL,myCAN1_callback);
	Driver_CAN1.PowerControl(ARM_POWER_FULL);
	
	Driver_CAN1.SetMode(ARM_CAN_MODE_INITIALIZATION);
	Driver_CAN1.SetBitrate( 
	ARM_CAN_BITRATE_NOMINAL,
	125000,
	ARM_CAN_BIT_PROP_SEG(5U)   |
  ARM_CAN_BIT_PHASE_SEG1(1U) |
  ARM_CAN_BIT_PHASE_SEG2(1U) |
  ARM_CAN_BIT_SJW(1U));
                          
	Driver_CAN1.ObjectSetFilter( 0, ARM_CAN_FILTER_ID_EXACT_ADD , ARM_CAN_STANDARD_ID(0x161), 0) ;
	Driver_CAN1.ObjectSetFilter( 0, ARM_CAN_FILTER_ID_EXACT_ADD , ARM_CAN_STANDARD_ID(0x0b6), 0) ;
		
	Driver_CAN1.ObjectConfigure(0,ARM_CAN_OBJ_RX);				// Objet 0 du CAN1 pour réception
	
	Driver_CAN1.SetMode(ARM_CAN_MODE_NORMAL);					// fin init
}

//--------------------------------------------------------------------------------------------------CAN2 INIT TX
void InitCan2 (void) {
	Driver_CAN2.Initialize(NULL,myCAN2_callback);
	
	Driver_CAN2.PowerControl(ARM_POWER_FULL);
	Driver_CAN2.SetMode(ARM_CAN_MODE_INITIALIZATION);
	Driver_CAN2.SetBitrate(

	ARM_CAN_BITRATE_NOMINAL, 125000, 
	ARM_CAN_BIT_PROP_SEG(5U) | 
	ARM_CAN_BIT_PHASE_SEG1(1U) | 
	ARM_CAN_BIT_PHASE_SEG2(1U) | 
	ARM_CAN_BIT_SJW(1U));

	Driver_CAN2.ObjectConfigure(1,ARM_CAN_OBJ_TX);
	Driver_CAN2.SetMode(ARM_CAN_MODE_NORMAL);
	
}
//--------------------------------------------------------------------------------------------------CAN TRANSMIT
void CANthreadT(void const *argument)
{
	ARM_CAN_MSG_INFO                tx_msg_info;
	uint8_t data_buf[8];
	char in, *inptr;
	osEvent GotADC;

	tx_msg_info.id = ARM_CAN_STANDARD_ID (0x0b6);
	tx_msg_info.rtr = 0; // 0 = trame DATA
	data_buf [0] = 0xAA; //init data to send
	
	while (1) {
		GotADC = osMailGet(ID_CAN1, osWaitForever);
		inptr = GotADC.value.p;
		in = *inptr;
		osMailFree(ID_CAN1, inptr);

		data_buf[0] = in;//apply value
		Driver_CAN2.MessageSend(1, &tx_msg_info, data_buf, 1);//send

		osSignalWait(0x02, osWaitForever);		// sommeil en attente fin emission
	}		
}
//--------------------------------------------------------------------------------------------------CAN RECEIVE
void CANthreadR(void const *argument)
{
	ARM_CAN_MSG_INFO   rx_msg_info;
	uint8_t data_buf[8];
	char texte[24], buff;
		
	while(1)
	{		
		osSignalWait(0x01, osWaitForever);		// sommeil en attente réception
		
		Driver_CAN1.MessageRead(0, &rx_msg_info, data_buf, 1);
		buff = data_buf[0];
		sprintf(texte,"ID: %d Rx: %d", 0x0b6, buff);//STM ID
		GLCD_DrawString(50,150,texte);

	}
}
//--------------------------------------------------------------------------------------------------ADC GET
void ADCtread(void const *argument)
{
	char *ptr, tab[16];
	short canVar;
	while(1)
	{
		ADC_StartConversion();
		if(!ADC_ConversionDone())
		{
			canVar = ADC_GetValue()>>4;
			ptr = osMailAlloc(ID_CAN1,100);
			
			sprintf(tab,"ADC %d", canVar);
			osMutexWait(ID_mut_GLCD, osWaitForever);
			GLCD_DrawString(100,50,tab);
			osMutexRelease(ID_mut_GLCD);
			
			*ptr = canVar;
			osMailPut(ID_CAN1, ptr);
		}
		osDelay(200);
	}
}