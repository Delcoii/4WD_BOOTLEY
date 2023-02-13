


#include "Bootley.h"

/* 수신기 ch1 계산 관련 변수 */
uint16_t u16_steering_capture1[2];
uint16_t u16_steering_capture2[2];
bool b_g_steering_ch1_done = false;
bool b_g_steering_ch2_done = false;
uint32_t u32_g_receiver_ch1_period;
uint32_t u32_g_steering_pw_us;
uint32_t u32_g_receiver_ch1_freq_Hz;
uint32_t u32_g_receiver_ch1_duty;


/* 수신기 ch2 계산 관련 변수 */
uint16_t u16_accel_capture1[2];			// dma를 통해 us 단위의 시간 저장됨
uint16_t u16_accel_capture2[2];			// dma를 통해 us 단위의 시간 저장됨
bool b_g_accel_ch3_done = false;
bool b_g_accel_ch4_done = false;
uint32_t u32_g_receiver_ch2_period;
uint32_t u32_g_accel_pw_us;
uint32_t u32_g_receiver_ch2_freq_Hz;
uint32_t u32_g_receiver_ch2_duty;

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

	if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		b_g_accel_ch3_done = true;
	}

	if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		b_g_accel_ch4_done = true;
	}
}

void check_IC()
{
	printf("check in : %d %d \r\n\n", b_g_steering_ch1_done, b_g_steering_ch2_done);
}



/*
 * HAL Library 이용 각종 init 함수
 * cubeMX로 생성되는 init 함수 이외의 것들을 별도로 작성
 */
void Bootley_Init()
{

	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)u16_steering_capture1, 2);
//	htim1.State = HAL_TIM_STATE_READY;
	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)u16_steering_capture2, 2);

	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_3, (uint32_t *)u16_accel_capture1, 2);
	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)u16_accel_capture2, 2);

}

/*
 * HAL_TIM_IC_CaptureCallback에서 채널 읽기 시작(true로 초기화)된 순간을 읽어
 * dma에 저장된 us 단위의 신호(TIM들 prescaler 이용 주파수 1Mhz)를 이용
 * freq, period, active(pulse width), duty 게산
 */
void Bootley_GetPulseWidth()
{
	/* 수신기 ch1 게산 부분 시작 */
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
	/* 수신기 ch1 계산 부분 끝 */

	/* 수신기 ch2 게산 부분 시작 */
	if(b_g_accel_ch3_done)
		{
			if(u16_accel_capture1[0] > u16_accel_capture1[1])
			{
				u32_g_receiver_ch2_period = htim1.Instance->ARR + u16_accel_capture1[1] - u16_accel_capture1[0];
			}
			else
			{
				u32_g_receiver_ch2_period = u16_accel_capture1[1] - u16_accel_capture1[0];
			}

			u32_g_receiver_ch2_freq_Hz = (HAL_RCC_GetPCLK1Freq() * 2) / (htim1.Instance->PSC + 1);  //84000000
			u32_g_receiver_ch2_freq_Hz = u32_g_receiver_ch2_freq_Hz / u32_g_receiver_ch2_period;

			b_g_accel_ch3_done = false;
		}

		if(b_g_accel_ch4_done)
		{
			if(u16_accel_capture2[0] >= u16_accel_capture1[0] && u16_accel_capture2[0] <= u16_accel_capture1[1])
			{
				u32_g_accel_pw_us = u16_accel_capture2[0] - u16_accel_capture1[0];
			}
			else if(u16_accel_capture2[1] >= u16_accel_capture1[0] && u16_accel_capture2[1] <= u16_accel_capture1[1])
			{
				u32_g_accel_pw_us = u16_accel_capture2[1] - u16_accel_capture1[0];
			}

			u32_g_receiver_ch2_duty = u32_g_accel_pw_us * 100 / u32_g_receiver_ch2_period;
			b_g_accel_ch4_done = false;
		}


	printf("%d %d\r\n", u32_g_steering_pw_us, u32_g_accel_pw_us);		// for debugging

}

