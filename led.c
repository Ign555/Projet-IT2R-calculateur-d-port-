// Utilisation Event UART en emission-reception

#define osObjectsPublic                     // define objects in main module
#include "Driver_SPI.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "stm32f4xx_hal.h" 
#include "Board_LED.h"   
#include "cmsis_os.h"                   // CMSIS RTOS header file




#define NB_LEDS	4
#define cli_g
#define cli_d
extern ARM_DRIVER_SPI Driver_SPI1;

void configure_GPIO(void);


void mySPI_Thread (void const *argument);                             // thread tache principale


osThreadId tid_mySPI_Thread;// thread ids

osThreadDef (mySPI_Thread, osPriorityNormal, 1, 0);                   // thread object




//fonction de CB lancee si Event T ou R
void mySPI_callback(uint32_t event)
{
	switch (event) {
		
		
		case ARM_SPI_EVENT_TRANSFER_COMPLETE  : 	 osSignalSet(tid_mySPI_Thread, 0x01);
																							break;
		
		default : break;
	}
}

void Init_SPI(void){
	Driver_SPI1.Initialize(mySPI_callback);
	Driver_SPI1.PowerControl(ARM_POWER_FULL);
	Driver_SPI1.Control(ARM_SPI_MODE_MASTER | 
											ARM_SPI_CPOL1_CPHA1 |   // Choisir en fonction datasheet
//											ARM_SPI_MSB_LSB | 
											ARM_SPI_SS_MASTER_UNUSED |
											ARM_SPI_DATA_BITS(8), 1000000); // 1Mhz
	Driver_SPI1.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}
/***************************Fonction bibliothèque ***********************/
void phare (void)
{

int 	i; 
char start_frame[300];
for( i = 0; i < 4; i++){ // 4 octects de start 
	start_frame[i] = 0x00;
}
for( i = 1; i <NB_LEDS+1 ; i++){
	start_frame[i*4] = 0xFF; // valeur globale
	start_frame[1+i*4] = 0x00;// Bleu
	start_frame[2+i*4] = 0x00;// Vert
	start_frame[3+i*4] = 0xFF;// Rouge
}
for( i = ((NB_LEDS+1)*4); i < ((NB_LEDS*4)+4); i++)// portion du bandeau led (deplacement de 4 en 4)
{ 
	start_frame[i] = 0xff;
}
Driver_SPI1.Send(start_frame,((NB_LEDS*4)+4));
}

void clignotant_g()
{

int 	i; 
char start_frame[300];
for( i = 0; i < 4; i++){ // 4 octects de start 
	start_frame[i] = 0x00;
}
for( i = 7; i <7+1 ; i++){
	start_frame[i*4] = 0xFF; // valeur globale
	start_frame[7+i*4] = 0x00;// Bleu
	start_frame[8+i*4] = 0xFF;// Vert
	start_frame[9+i*4] = 0x00;// Rouge
}
for( i = ((7+1)*4); i < ((7*4)+4); i++)// portion du bandeau led (deplacement de 4 en 4)
{ 
	start_frame[i] = 0xff;
}
Driver_SPI1.Send(start_frame,((7*4)+4));
}




/***************************Programme ***********************/
int main (void){
	
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// initialize peripherals here 
	
	Init_SPI();
	NVIC_SetPriority(SPI1_IRQn,2);
	LED_Initialize(); 
	tid_mySPI_Thread = osThreadCreate (osThread(mySPI_Thread), NULL);           // crea tâche
	
	osKernelStart ();                         // start thread execution 
	
	LED_On(1); 
	LED_On(2);
	LED_On(3);
	LED_On(4);
	osDelay(osWaitForever);
	
}

void mySPI_Thread (void const *argument)
{


while(1)
{	
	//clignotant_g();
	 phare(); 
	
	osDelay(100);
}	
}

