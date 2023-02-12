
#ifndef __BOOTLEY_H__
#define __BOOTLEY_H__


#include "stm32f4xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <stdbool.h>


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

void Bootley_Init();
void Bootley_GetPulseWidth();













#endif	//__BOOTLEY_H__
