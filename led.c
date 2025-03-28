// Utilisation Event UART en emission-reception

#define osObjectsPublic                     // define objects in main module
#include "Driver_SPI.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "stm32f4xx_hal.h" 
#include "Board_LED.h"   
#include "cmsis_os.h"                   // CMSIS RTOS header file
#include "leds.h"




#define cli_g 2
//#define cli_d 2
extern ARM_DRIVER_SPI Driver_SPI1;

void configure_GPIO(void);

/************partie RTOS***************************************/
void mySPI_Thread (void const *argument);                             // thread tache principale
void clignotant_g (void const *argument);
//void clignotant_d (void const *argument);	

osThreadId tid_mySPI_Thread;// thread ids
osThreadId tid_clignotant_g;
osThreadId tid_clignotant_d;

osThreadDef (mySPI_Thread, osPriorityNormal, 1, 0);                   // thread object
osThreadDef (clignotant_g, osPriorityNormal, 1, 0);
//osThreadDef (clignotant_d, osPriorityNormal, 1, 0);
osSemaphoreId Id_Sema; 
osSemaphoreDef(MonSema);


//fonction de CB lancee si Event T ou R
void mySPI_callback(uint32_t event)
{
	switch (event) {
		
		
		case ARM_SPI_EVENT_TRANSFER_COMPLETE  : 	 osSignalSet(tid_mySPI_Thread, 0x01);
																							break;
		
		default : break;
	}
}

/***************************Fonction bibliothèque ***********************/
void phare (void)
{

int 	i; 
char start_frame[300];
for( i = 0; i < 4; i++){ // 4 octects de start 
	start_frame[i] = 0x00;
}

for( i = ((NB_LEDS+1)*4); i < ((NB_LEDS*4)+4); i++)// portion du bandeau led (deplacement de 4 en 4)
{ 
	start_frame[i] = 0x00;
}
Driver_SPI1.Send(start_frame,((NB_LEDS*4)+4));
}

void clignotant_gauche()
{

int 	i; 
char start_frame[300];
for( i = 1; i < 5; i++){ // 4 octects de start 
	start_frame[i] = 0x00;
}
for( i = 1; i <cli_g+1 ; i++){
	start_frame[i*4] = 0xFF; // intensité globale
	start_frame[1+i*4] = 0x00;// Bleu
	start_frame[2+i*4] = 0xFF;// Vert
	start_frame[3+i*4] = 0x00;// Rouge
}
for( i = ((cli_g+1)*4); i < ((cli_g*4)+4); i++)// portion du bandeau led (deplacement de 4 en 4)
{ 
	start_frame[i] = 0x00;
}
Driver_SPI1.Send(start_frame,((cli_g*4)+4));
}




/***************************Programme ***********************/
int main (void){
	
	uint8_t trame [300];

	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// initialize peripherals here 
	
	Id_Sema = osSemaphoreCreate(osSemaphore(MonSema),2);
	NVIC_SetPriority(SPI1_IRQn,2);
	LED_Initialize(); 
	tid_mySPI_Thread = osThreadCreate (osThread(mySPI_Thread), NULL);           // crea tâche
	tid_clignotant_g= osThreadCreate  (osThread(clignotant_g),NULL); 
//	tid_clignotant_d=osThreadCreate  (osThread(clignotant_d),NULL); 
	osKernelStart ();                         // start thread execution 
	
	LED_On(1); 
	LED_On(2);
	LED_On(3);
	LED_On(4);
	osDelay(osWaitForever);	


}

void mySPI_Thread (void const *argument)
{

	
uint8_t trame[300];
LEDS_init(trame);
		while(1)
					{	
						//osSemaphoreRelease(Id_Sema);
						chenillard (trame,5);	
						phares(trame,6,15);
						gauche(trame,20,15); 
					
						osDelay(100);
					}	
}

void clignotant_g (void const *argument)
{
	uint8_t trame [300];
	LEDS_init (trame); 
	
	while (1)
		
					{
						
						//gauche(trame,20); 
						osDelay(100);
						//osSemaphoreWait(Id_Sema,osWaitForever);
					}
}


 
