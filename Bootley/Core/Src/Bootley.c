


#include "Bootley.h"


uint16_t u16_steering_capture1[2];
uint16_t u16_steering_capture2[2];
bool b_g_steering_ch1_done = false;
bool b_g_steering_ch2_done = false;
uint32_t u32_g_receiver_ch1_period;
uint32_t u32_g_steering_pw_us;
uint32_t u32_g_receiver_ch1_freq_Hz;
uint32_t u32_g_receiver_ch1_duty;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		b_g_steering_ch1_done = true;
	}

	if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		b_g_steering_ch2_done = true;
	}
}


void Bootley_Init()
{
	  HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_1, (uint16_t *)u16_steering_capture1, 2);
	  htim1.State = HAL_TIM_STATE_READY;
	  HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_2, (uint16_t *)u16_steering_capture2, 2);
}


void Bootley_GetPulseWidth()
{
	if(b_g_steering_ch1_done)
	{
		if(u16_steering_capture1[0] > u16_steering_capture1[1])
		{
			u32_g_receiver_ch1_period = htim1.Instance->ARR + u16_steering_capture1[1] - u16_steering_capture1[0];
		}
		else
		{
			u32_g_receiver_ch1_period = u16_steering_capture1[1] - u16_steering_capture1[0];
		}

		u32_g_receiver_ch1_freq_Hz = (HAL_RCC_GetPCLK1Freq() * 2) / (htim1.Instance->PSC + 1);  //84000000
		u32_g_receiver_ch1_freq_Hz = u32_g_receiver_ch1_freq_Hz / u32_g_receiver_ch1_period;

		b_g_steering_ch1_done = false;
	}

	if(b_g_steering_ch2_done)
	{
		if(u16_steering_capture2[0] >= u16_steering_capture1[0] && u16_steering_capture2[0] <= u16_steering_capture1[1])
		{
			u32_g_steering_pw_us = u16_steering_capture2[0] - u16_steering_capture1[0];
		}
		else if(u16_steering_capture2[1] >= u16_steering_capture1[0] && u16_steering_capture2[1] <= u16_steering_capture1[1])
		{
			u32_g_steering_pw_us = u16_steering_capture2[1] - u16_steering_capture1[0];
		}

		u32_g_receiver_ch1_duty = u32_g_steering_pw_us * 100 / u32_g_receiver_ch1_period;
		b_g_steering_ch2_done = false;
	}

	printf("%d %d %d \r\n",
			u32_g_receiver_ch1_freq_Hz, u32_g_receiver_ch1_duty, u32_g_steering_pw_us);
}

