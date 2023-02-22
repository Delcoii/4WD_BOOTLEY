
#ifndef __BOOTLEY_H__
#define __BOOTLEY_H__


#include "stm32f4xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <stdbool.h>

#include "MY36GP_3650.hpp"


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void check_IC();


class Bootley {
private:

    uint32_t u32_receiver_ch1_period;
    uint32_t u32_steering_pw_us;
    uint32_t u32_receiver_ch1_freq_Hz;
    uint32_t u32_receiver_ch1_duty;

    uint32_t u32_receiver_ch2_period;
    uint32_t u32_accel_pw_us;
    uint32_t u32_receiver_ch2_freq_Hz;
    uint32_t u32_receiver_ch2_duty;

    uint32_t u32_receiver_ch3_period;
    uint32_t u32_auto_mode_pw_us;
    uint32_t u32_receiver_ch3_freq_Hz;
    uint32_t u32_receiver_ch3_duty;

    uint32_t u32_receiver_ch4_period;
    uint32_t u32_drive_mode_pw_us;
    uint32_t u32_receiver_ch4_freq_Hz;
    uint32_t u32_receiver_ch4_duty;

public:
    Bootley();

    void InitModule();
    void GetPulseWidth();

    

};










#endif	//__BOOTLEY_H__
