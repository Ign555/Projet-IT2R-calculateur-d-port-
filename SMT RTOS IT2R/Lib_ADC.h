
#include "stm32f4xx_hal.h"

void Lib_ADC_Init(void);

static int Lib_ADC_Read_Channel(int channel);

int Lib_ADC_Read_Channel0(void);

int Lib_ADC_Read_Channel1(void);
