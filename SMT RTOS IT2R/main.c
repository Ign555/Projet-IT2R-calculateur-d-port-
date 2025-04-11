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
void CAN_Task_Send( void const * argument );
void Lum_Task ( void const * argument );
void Humidity_Task ( void const * argument );
void sensor_Task (void const * argument);
void CAN_Task_Recive (void const * argument);

osThreadId ID_Led_Task, ID_GPS_Task,/*ID_US_Task,*/ID_CAN_Task_Send, ID_Lum_Task, ID_Humidity_Task, ID_sensor_Task, ID_CAN_Task_Recive; 

osThreadDef ( Led_Task, osPriorityNormal, 1, 0);
osThreadDef ( GPS_Task, osPriorityNormal, 1, 0);
//osThreadDef ( US_Task, osPriorityNormal, 1, 0);
osThreadDef ( CAN_Task_Send, osPriorityHigh, 1, 0);
osThreadDef ( Lum_Task, osPriorityNormal, 1, 0);
osThreadDef ( Humidity_Task, osPriorityNormal, 1, 0);
osThreadDef ( sensor_Task, osPriorityNormal, 1, 0);
osThreadDef ( CAN_Task_Recive, osPriorityHigh, 1,0);

osMailQId ID_LumDir, ID_ADC_CAN, ID_sensor;

osMailQDef ( LumDir, 2, char);
osMailQDef ( ADC_CAN, 1, int);
osMailQDef (sensor, 10, int);



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
	ID_CAN_Task_Send =osThreadCreate (osThread ( CAN_Task_Send), NULL);
	ID_Lum_Task = osThreadCreate ( osThread ( Lum_Task), NULL);
	ID_Humidity_Task = osThreadCreate ( osThread ( Humidity_Task), NULL);
  	ID_sensor_Task = osThreadCreate ( osThread ( sensor_Task), NULL);	
  	// example: tid_name = osThreadCreate (osThread(name), NULL);
	ID_CAN_Task_Recive = osThreadCreat ( osThread ( CAN_Task_Recive), NULL);
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
	
	
	

void CAN_Task_Send(void const* argument) {
    ARM_CAN_MSG_INFO tx_msg_info;
    uint8_t tx_data_buf[8];  // Buffer to hold the data to be sent
    int *sensor_value;       // Pointer to the value received from sensor_Task
    int *humidity_value;     // Pointer to the value received from Humidity_Task

    tx_msg_info.id = ARM_CAN_STANDARD_ID(0x161);   /* ID for carburant & US */
    tx_msg_info.rtr = 0;  // Not using RTR (Remote Transmission Request)

    while (1) {
        // Wait for the humidity value
        osEvent humidity_event = osMailGet(ID_ADC_CAN, osWaitForever);
        if (humidity_event.status == osEventMail) {
            humidity_value = humidity_event.value.p;
            // Copy humidity value into tx_data_buf (for example, you can use the first byte for humidity)
            tx_data_buf[0] = *humidity_value;
            osMailFree(ID_ADC_CAN, humidity_value);  // Free the memory for the humidity value

            // Send humidity data
            Driver_CAN2.MessageSend(2, &tx_msg_info, tx_data_buf, 1);  // Send 1 byte of humidity data
            osSignalWait((1<<11), osWaitForever);  // Wait for send completion signal
        }

        // Wait for the sensor value
        osEvent sensor_event = osMailGet(ID_sensor, osWaitForever);
        if (sensor_event.status == osEventMail) {
            sensor_value = sensor_event.value.p;
            // Copy sensor value into tx_data_buf (for example, use first 4 bytes for sensor data)
            for (int i = 0; i < 4; i++) {
                tx_data_buf[i] = sensor_value[i];
            }
            osMailFree(ID_sensor, sensor_value);  // Free the memory for the sensor value

            // Send sensor data
            Driver_CAN2.MessageSend(2, &tx_msg_info, tx_data_buf, 4);  // Send 4 bytes of sensor data
            osSignalWait((1<<11), osWaitForever);  // Wait for send completion signal
        }
    }
}
	
	void CAN_Task_Receive(void const* argument) {
    ARM_CAN_MSG_INFO rx_msg_info;
    uint8_t rx_data_buf[8];  // Buffer to store the received data
    int *led_data;           // Pointer to hold the received data for LED

    rx_msg_info.id = ARM_CAN_STANDARD_ID(0x401);  // CAN ID 0x401 for the message filter
    rx_msg_info.rtr = 0;  // Not using RTR (Remote Transmission Request)

    // Enable CAN1 reception (assuming CAN1 is properly configured for receiving messages)
    Driver_CAN1.MessageReceive(1, &rx_msg_info, rx_data_buf, 8);  // Receive up to 8 bytes of data from CAN1

    while (1) {
        // Wait for a CAN message on the CAN1 bus
        osEvent can_event = osSignalWait((1 << 0), osWaitForever);  // Assume signal is set when message is received

        if (can_event.status == osEventSignal) {
            // Process received CAN message if ID matches 0x401
            if (rx_msg_info.id == 0x401) {
                // Copy the received data to led_data
                led_data = osMailAlloc(LumDir, osWaitForever);  // Allocate memory for LED data from LumDir mailbox
                if (led_data != NULL) {
                    // Here, assume that the data corresponds to controlling the LED
                    // Copy received data into the allocated mailbox object
                    for (int i = 0; i < 8; i++) {
                        led_data[i] = rx_data_buf[i];  // Copy the received message into the led_data
                    }

                    // Send the data to LED_Task through the LumDir mailbox
                    osMailPut(LumDir, led_data);  // Send the LED data to the LED task's mailbox
                }
            }
        }

        // Wait again for the next message
        osSignalWait((1 << 0), osWaitForever);
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
	
