#include "stm32f4xx_hal_conf.h"         // Keil::Device:STM32Cube Framework:Classic
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "RTE_Components.h"             // Component selection
#include "Lib_ADC.h"

static ADC_HandleTypeDef hadc1;


void Lib_ADC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PA0 et PA1 en analog
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Init ADC1
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(&hadc1);
}

static int Lib_ADC_Read_Channel(int channel)
{
    ADC_ChannelConfTypeDef sConfig;
    uint32_t value;

    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return value;
}

int Lib_ADC_Read_Channel0(void)
{
    return Lib_ADC_Read_Channel(ADC_CHANNEL_0);
}

int Lib_ADC_Read_Channel1(void)
{
    return Lib_ADC_Read_Channel(ADC_CHANNEL_1);
}

