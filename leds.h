#include "cmsis_os.h"  
#include "Driver_SPI.h"

#ifndef LEDS_H
#define LEDS_H
#define NB_LEDS	60

void _LEDS_trame_start(uint8_t *trame);
void _LEDS_trame_end(uint8_t *trame);

void LEDS_init(uint8_t *trame);
void LEDS_clear(uint8_t *trame);
void LEDS_set_single_led_color(uint8_t *trame, uint8_t led_number, uint8_t r, uint8_t g, uint8_t b, uint8_t intensity);
void chenillard (uint8_t *trame, int num_leds);
extern ARM_DRIVER_SPI Driver_SPI1;

#endif