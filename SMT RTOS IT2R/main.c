/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_SPI.h"                 // ::CMSIS Driver:SPI
#include "Driver_CAN.h"                 // ::CMSIS Driver:CAN
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx.h"                  // Device header
#include "RTE_Components.h"             // Component selection
#include "stm32f4xx_hal_conf.h"         // Keil::Device:STM32Cube Framework:Classic
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Lib_ADC.h"
#include "LED_lib.h"
#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "Lib_Usons.h"  
#include "Lib_CAN.h" 

uint8_t trame[300];
char SLAVE_I2C_ADDR[5] = {0x70, 0x71, 0x72, 0x73, 0x74};
ARM_CAN_MSG_INFO   rx_msg_info;
uint8_t CANdata_buf[8];

void ADC_Init ( void );


void Led_Task( void const * argument );
void GPS_Task( void const * argument );
//void US_Task( void const * argument );
void CAN_Task_e( void const * argument );
void Lum_Task ( void const * argument );
void Humidity_Task ( void const * argument );
void sensor_Task (void const * argument);
void CAN_Task_r (void const * argument);

osThreadId ID_Led_Task, ID_GPS_Task,/*ID_US_Task,*/ID_CAN_Task_e, ID_Lum_Task, ID_Humidity_Task, ID_sensor_Task, ID_CAN_Task_r; 

osThreadDef ( Led_Task, osPriorityNormal, 1, 0);
osThreadDef ( GPS_Task, osPriorityNormal, 1, 0);
//osThreadDef ( US_Task, osPriorityNormal, 1, 0);
osThreadDef ( CAN_Task_e, osPriorityHigh, 1, 0);
osThreadDef ( Lum_Task, osPriorityNormal, 1, 0);
osThreadDef ( Humidity_Task, osPriorityNormal, 1, 0);
osThreadDef ( sensor_Task, osPriorityNormal, 1, 0);
osThreadDef ( CAN_Task_r, osPriorityHigh, 1,0);

osMailQId ID_LumDir, ID_ADC_CAN, ID_sensor, ID_CANr, ID_CANe;

osMailQDef ( LumDir, 2, char);
osMailQDef ( ADC_CAN, 1, int);
osMailQDef (sensor, 10, int);
osMailQDef (CANr, 10 , int);
osMailQDef (CANe, 10 , int);


/* Callback fonction */

void myCAN2_callback(uint32_t obj_idx, uint32_t event)
{
    switch (event)
    {
    case ARM_CAN_EVENT_RECEIVE:
        /*  Message was received successfully by the obj_idx object. */
				Driver_CAN2.MessageRead(0, &rx_msg_info, CANdata_buf, 1);
				osSignalSet(ID_CAN_Task_r, (1<<10));
        break;
		case ARM_CAN_EVENT_SEND_COMPLETE:
        /* 	Message was sent successfully by the obj_idx object.  */
        osSignalSet(ID_CAN_Task_e, (1<<11));
        break;
    }
}


/*
 * main: initialize and start the system
 */
 
 
 
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	Lib_ADC_Init();
	HAL_Init();
	
  // initialize peripherals here
  // create 'thread' functions that start executing,
	
	ID_Led_Task =osThreadCreate (osThread ( Led_Task), NULL);
	ID_GPS_Task =osThreadCreate (osThread ( GPS_Task), NULL);
  //ID_US_Task  =osThreadCreate (osThread (US_Task), NULL);	
	ID_CAN_Task_e =osThreadCreate (osThread ( CAN_Task_e), NULL);
	ID_Lum_Task = osThreadCreate ( osThread ( Lum_Task), NULL);
	ID_Humidity_Task = osThreadCreate ( osThread ( Humidity_Task), NULL);
  ID_sensor_Task = osThreadCreate ( osThread ( sensor_Task), NULL);	
  // example: tid_name = osThreadCreate (osThread(name), NULL);

  osKernelStart ();                         // start thread execution 
}


void Led_Task(void const*argument)
	{
		char * Reception;
		char type;
		int dir, lum;
		char ETAT;
		osEvent EV_LumDir; 
		
		char Tab[2][3] ={{0,1,2},{3,4,5}} ;
		
		ETAT= 0;

		switch (ETAT) {
			
				case 0: // tout est eteint
					
					Phares_Off(trame, 6, 15);
					while (1){
						EV_LumDir = osMailGet(ID_LumDir, 10);
						
						if ( EV_LumDir.status != osEventTimeout){
							
							Reception = EV_LumDir.value.p ;
							type = * Reception;
							osMailFree(ID_LumDir, Reception);
			
							if ( 'l' == type ) {
					
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								lum = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[lum][0];
							}
							if ( 'd' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								dir = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[0][dir];
							}
						
						break;
					
					}
						
				}break;
					
				case 1:  // cli_gauche on 
					
					Phares_Off(trame, 6, 15);
				
					while (1) {
						
						
						Clignotant_Gauche(trame, 20, 15);
					
						EV_LumDir = osMailGet(ID_LumDir, 10);
						
						if ( EV_LumDir.status != osEventTimeout){
				
							Reception = EV_LumDir.value.p ;
							type = * Reception;
							osMailFree(ID_LumDir, Reception);
			
							if ( 'l' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								lum = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[lum][1];

							}
							
							if ( 'd' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								dir = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[0][dir];
						
							}
						
							break;
						}
					
					}break;
					
					case 2 : // cli_d on 
						
						Phares_Off(trame, 6, 15);
						
						while (1){
							
							
							Clignotant_Droit(trame, 5, 0);
							
							EV_LumDir = osMailGet(ID_LumDir, 10);
						
							if ( EV_LumDir.status != osEventTimeout){
						
								Reception = EV_LumDir.value.p ;
								type = * Reception;
								osMailFree(ID_LumDir, Reception);
							
							
							
							if ( 'l' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								lum = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[lum][2];
							
							}
							
							if ( 'd' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								dir = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[0][dir];
					
							} 
							
						break ;
							
					}
							
				}break;
						
						
				case 3 : // phares on 
						
					phares(trame,6,15);
				
					while(1){

					
						EV_LumDir = osMailGet(ID_LumDir, 10);
						
						
						if ( EV_LumDir.status != osEventTimeout) {
							
						
							Reception = EV_LumDir.value.p ;
							type = * Reception;
							osMailFree(ID_LumDir, Reception);
			
							if ( 'l' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								lum = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[lum][0];
						}
						if ( 'd' == type ) {
				
							EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
							Reception = EV_LumDir.value.p ;
							dir = * Reception;
							osMailFree(ID_LumDir, Reception);
					
							ETAT = Tab[1][dir];
						} 
						break; 
					}
				}break;
					
				case 4 : // phares+cli_droit
					
					phares(trame,6,15);
				
					while (1){	
						
						Clignotant_Droit(trame, 5, 0);
						EV_LumDir = osMailGet(ID_LumDir, 10);
						
						if ( EV_LumDir.status != osEventTimeout){
						
							Reception = EV_LumDir.value.p ;
							type = * Reception;
							osMailFree(ID_LumDir, Reception);
			
							if ( 'l' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								lum = * Reception;
								osMailFree(ID_LumDir, Reception);
					
							
								ETAT = Tab[lum][2];
							
							}
							
							if ( 'd' == type ) {
				
								EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
								Reception = EV_LumDir.value.p ;
								dir = * Reception;
								osMailFree(ID_LumDir, Reception);
					
								ETAT = Tab[1][dir];
							
							} 
						
							break;
					
						}

					}break;
		
					case 5 : // phares+cli_gauche 
					
						phares(trame, 5,16);
					
						while (1){
							
							Clignotant_Gauche(trame, 20, 15);
							EV_LumDir = osMailGet(ID_LumDir, 10);
							
							if ( EV_LumDir.status != osEventTimeout){
							
								Reception = EV_LumDir.value.p ;
								type = * Reception;
								osMailFree(ID_LumDir, Reception);
			
								if ( 'l' == type ) {
				
									EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
									Reception = EV_LumDir.value.p ;
									lum = * Reception;
									osMailFree(ID_LumDir, Reception);
					
									ETAT = Tab[lum][1];
								}
							
								if ( 'd' == type ) {
					
									EV_LumDir = osMailGet(ID_LumDir, osWaitForever);
									Reception = EV_LumDir.value.p ;
									dir = * Reception;
									osMailFree(ID_LumDir, Reception);
					
						
									ETAT = Tab[1][dir];
					
							} 
						break; 
					}
			
				}
			break;
			}
		
		}

	void GPS_Task(void const*argument)
	{
		while(1)
		{
			
			osDelay(osWaitForever);
		}
	}
	
void sensor_Task(void const*argument)
	{
		int i ;
		char *ptr;
		short sensOut[10];
		while(1)
		{
				
		
			for(i=0;i<5;i++)//	This will scan through 5 sensors of defined @
		{

	//----------------------------------------Get one value
			write1byte(SLAVE_I2C_ADDR[i], 0x00, 0x51);
			osDelay(70);
			sensOut[i] = get1Sens(SLAVE_I2C_ADDR[i]);
			ptr= osMailAlloc(ID_sensor,osWaitForever); 
			*ptr=
			osMailPut(ID_sensor,ptr); 
			//----------------------------------------Get one value
		
			osDelay(70);
		}
		}
	}
	
	
	

void CAN_Task_e(void const*argument)
	{
		char *ptr; 
		ARM_CAN_MSG_INFO                tx_msg_info;
		uint8_t tx_data_buf[8];
		
		
	tx_msg_info.id = ARM_CAN_STANDARD_ID(0x161);   /* pour carburant   */
	tx_msg_info.rtr = 0;
	tx_data_buf[0] = 0xAA;
		
		while(1)
		{
			osSignalWait((1<<12), osWaitForever);//on receive
		
		Driver_CAN2.MessageSend(2, &tx_msg_info, tx_data_buf, 1);
		osSignalWait((1<<11), osWaitForever);
			
		}
	}
	
	void CAN_Task_r (void const * argument){
	
	while (1)
		{
		
		osSignalWait((1<<10), osWaitForever);
		if(rx_msg_info.id & 0x101)osSignalSet(ID_CAN_Task_e, (1<<12));
	}
	
	}
	
	void Lum_Task ( void const * argument ){
		
		int * Envoie_l ;
		int AD_last_l;
		
		char Etat = 0;
		
		switch ( Etat ) {
		
			case 0:
				
				while(1) {
	

					AD_last_l =Lib_ADC_Read_Channel0();
		 
					if (AD_last_l <= 2000){
					
						Envoie_l = osMailAlloc(ID_LumDir, osWaitForever);
						* Envoie_l = 1; // à modifier 
						osMailPut(ID_LumDir, Envoie_l);
						Etat = 1;
						break;
					
					}			
				
					osDelay(1000);
					
				}
		break; 
			case 1 :

				while(1) {

					AD_last_l =Lib_ADC_Read_Channel0();
		 
					if (AD_last_l >= 2000){
						
						Envoie_l = osMailAlloc(ID_LumDir, osWaitForever);
						* Envoie_l = 0; // à modifier 
						osMailPut(ID_LumDir, Envoie_l);		 
						Etat = 0;
						break;
					}
				 			
				osDelay(1000);
				
				}
			break;
		}
	
	}
	
	void Humidity_Task ( void const * argument ){
	
		int * Envoie_h ;
		int AD_last_h;
		
		while(1) {
		 

			AD_last_h = Lib_ADC_Read_Channel1();
			Envoie_h = osMailAlloc(ID_ADC_CAN, osWaitForever);
			* Envoie_h = AD_last_h; // à modifier 
			osMailPut(ID_ADC_CAN, Envoie_h);		 
			osDelay(1000);
		}	
	}
	