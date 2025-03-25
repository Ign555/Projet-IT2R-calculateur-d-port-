#include "leds.h"
void _Init_SPI(void){
	Driver_SPI1.Initialize(NULL);
	Driver_SPI1.PowerControl(ARM_POWER_FULL);
	Driver_SPI1.Control(ARM_SPI_MODE_MASTER | 
											ARM_SPI_CPOL1_CPHA1 |   // Choisir en fonction datasheet
												ARM_SPI_MSB_LSB | 
											ARM_SPI_SS_MASTER_UNUSED |
											ARM_SPI_DATA_BITS(8), 1000000); // 1Mhz
	Driver_SPI1.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}
void LEDS_init(uint8_t *trame){
	_Init_SPI();
	LEDS_clear(trame);
}
void LEDS_clear(uint8_t *trame){
	
	uint8_t i;
	
	_LEDS_trame_start(trame);
	for( i = 1; i < NB_LEDS+1 ; i++){
			trame[i*4] = 0xFF; // valeur globale
			trame[1+i*4] = 0;// Bleu
			trame[2+i*4] = 0;// Vert
			trame[3+i*4] = 0;// Rouge
	}
	_LEDS_trame_end(trame);
	Driver_SPI1.Send(trame,((NB_LEDS*4)+8));
}
void LEDS_set_single_led_color(uint8_t *trame, uint8_t led_number, uint8_t r, uint8_t g, uint8_t b, uint8_t intensity){
	
	uint8_t i;
	
	_LEDS_trame_start(trame);
	
	for( i = 1; i < NB_LEDS+1 ; i++){
		if(i == led_number){
			trame[i*4] = 0xFF; // valeur globale
			trame[1+i*4] = b;// Bleu
			trame[2+i*4] = g;// Vert
			trame[3+i*4] = r;// Rouge
		}
	}
	
	_LEDS_trame_end(trame);
	
	Driver_SPI1.Send(trame,((NB_LEDS*4)+8));
}


void chenillard (uint8_t *trame, int num_leds)
{
	int i,m;
	for(i = 0; i < num_leds; i++){
	LEDS_set_single_led_color(trame, i+1,255,0,0,255);
	osDelay(100);
	}
	for (m= num_leds; m>0;i--){
	void LEDS_clear(uint8_t *trame);
	}
	
}



void _LEDS_trame_start(uint8_t *trame){
	int i;
	for( i = 0; i < 4; i++){ // 4 octects de start 
		trame[i] = 0x00;
	}
}

void _LEDS_trame_end(uint8_t *trame){
	int i;
	for( i = ((NB_LEDS+1)*4); i < ((NB_LEDS+1)*4+5); i++)// portion du bandeau led (deplacement de 4 en 4)
	{ 
		trame[i] = 0xFF;
	}
}