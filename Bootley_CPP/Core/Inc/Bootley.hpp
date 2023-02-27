
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


#define RECEIVER_MAX_PW     2000.
#define RECEIVER_MIN_PW     1000.

#define STEERING_PW_ERROR_US    7.
#define LEFT_MAX_STEERING_VAL   500.
#define RIGHT_MAX_STEERING_VAL  -500.

#define ACCEL_PW_ERROR_US       7.
#define FORWARD_MAX_ACCEL_VAL   500.
#define BACKWARD_MAX_ACCEL_VAL  -500.

#define MANUAL_MODE             false
#define AUTO_MODE               true

#define SPINNING_MODE           0
#define NORMAL_MODE             1
#define LOCK_MODE               2


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

    
    float f_steering_val;
    float f_accel_val;
    bool b_auto_mode;
    uint8_t u8_drive_mode;

public:
    Bootley();

    void SetCarState();

    void InitModule();
    void GetPulseWidth();
    void GetSteeringVal();
    void GetAccelVal();
    void SetAutoMode();
    void SetDriveMode();

};










#endif	//__BOOTLEY_H__
