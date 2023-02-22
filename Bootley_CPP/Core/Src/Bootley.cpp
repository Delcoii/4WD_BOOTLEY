


#include "Bootley.hpp"

/* 수신기 ch1 계산 관련 변수
 * 조향 스위치 (ch1) 읽어서 추후에 조향 수치 정함
 */
uint16_t u16_steering_capture1[2];		// dma를 통해 us 단위의 시간 저장됨
uint16_t u16_steering_capture2[2];		// dma를 통해 us 단위의 시간 저장됨
bool b_g_steering_ch1_done = false;
bool b_g_steering_ch2_done = false;



/* 수신기 ch2 계산 관련 변수
 * 엑셀 스위치 (ch2) 읽어서 추후에 엑셀(전진, 후진) 수치 정함
 */
uint16_t u16_accel_capture1[2];			// dma를 통해 us 단위의 시간 저장됨
uint16_t u16_accel_capture2[2];			// dma를 통해 us 단위의 시간 저장됨
bool b_g_accel_ch3_done = false;
bool b_g_accel_ch4_done = false;



/* 수신기 ch3 계산 관련 변수
 * 잠금 스위치 (ch3) 읽어서 추후에 수동주행, 자율주행(ROS subscribe) 모드 변경
 */
uint16_t u16_auto_mode_capture1[2];			// dma를 통해 us 단위의 시간 저장됨
uint16_t u16_auto_mode_capture2[2];			// dma를 통해 us 단위의 시간 저장됨
bool b_g_auto_mode_ch1_done = false;
bool b_g_auto_mode_ch2_done = false;



/* 수신기 ch4 계산 관련 변수
 * 측면 3단 스위치 (ch4) 읽어서 추후에 잠금, 직진, 제자리돌기 모드 변경
 */
uint16_t u16_drive_mode_capture1[2];			// dma를 통해 us 단위의 시간 저장됨
uint16_t u16_drive_mode_capture2[2];			// dma를 통해 us 단위의 시간 저장됨
bool b_g_drive_mode_ch3_done = false;
bool b_g_drive_mode_ch4_done = false;





void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		b_g_steering_ch1_done = true;
	}
	else if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		b_g_steering_ch2_done = true;
	}

	else if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		b_g_accel_ch3_done = true;
	}
	else if(htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		b_g_accel_ch4_done = true;
	}

	else if(htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		b_g_auto_mode_ch1_done = true;
	}
	else if(htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		b_g_auto_mode_ch2_done = true;
	}

	else if(htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		b_g_drive_mode_ch3_done = true;
	}
	else if(htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		b_g_drive_mode_ch4_done = true;
	}
}

void check_IC()
{
	printf("check in : %d %d \r\n\n", b_g_drive_mode_ch3_done, b_g_drive_mode_ch4_done);
}


Bootley::Bootley()
{
	printf("Bootley Initiated \r\n\n");
}

/*
 * HAL Library 이용 각종 init 함수
 * cubeMX로 생성되는 init 함수 이외의 것들을 별도로 작성
 */
void Bootley::InitModule()
{

	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)u16_steering_capture1, 2);
//	htim1.State = HAL_TIM_STATE_READY;
	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)u16_steering_capture2, 2);

	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_3, (uint32_t *)u16_accel_capture1, 2);
	HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)u16_accel_capture2, 2);

	HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)u16_auto_mode_capture1, 2);
	HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)u16_auto_mode_capture2, 2);

	HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_3, (uint32_t *)u16_drive_mode_capture1, 2);
	HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_4, (uint32_t *)u16_drive_mode_capture2, 2);

	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_4);


}

/*
 * HAL_TIM_IC_CaptureCallback에서 채널 읽기 시작(true로 초기화)된 순간을 읽어
 * dma에 저장된 us 단위의 신호(TIM들 prescaler 이용 주파수 1Mhz)를 이용
 * freq, period, active(pulse width), duty 게산
 */
void Bootley::GetPulseWidth()
{
	/* 수신기 ch1 계산 부분 시작 */
	if(b_g_steering_ch1_done)
	{
		if(u16_steering_capture1[0] > u16_steering_capture1[1])
		{
			u32_receiver_ch1_period = htim1.Instance->ARR + u16_steering_capture1[1] - u16_steering_capture1[0];
		}
		else
		{
			u32_receiver_ch1_period = u16_steering_capture1[1] - u16_steering_capture1[0];
		}

		u32_receiver_ch1_freq_Hz = (HAL_RCC_GetPCLK1Freq() * 2) / (htim1.Instance->PSC + 1);  //84000000
		u32_receiver_ch1_freq_Hz = u32_receiver_ch1_freq_Hz / u32_receiver_ch1_period;

		b_g_steering_ch1_done = false;
	}
	if(b_g_steering_ch2_done)
	{
		if(u16_steering_capture2[0] >= u16_steering_capture1[0] && u16_steering_capture2[0] <= u16_steering_capture1[1])
		{
			u32_steering_pw_us = u16_steering_capture2[0] - u16_steering_capture1[0];
		}
		else if(u16_steering_capture2[1] >= u16_steering_capture1[0] && u16_steering_capture2[1] <= u16_steering_capture1[1])
		{
			u32_steering_pw_us = u16_steering_capture2[1] - u16_steering_capture1[0];
		}

		u32_receiver_ch1_duty = u32_steering_pw_us * 100 / u32_receiver_ch1_period;
		b_g_steering_ch2_done = false;
	}
	/* 수신기 ch1 계산 부분 끝 */

	/* 수신기 ch2 계산 부분 시작 */
	if(b_g_accel_ch3_done)
		{
			if(u16_accel_capture1[0] > u16_accel_capture1[1])
			{
				u32_receiver_ch2_period = htim1.Instance->ARR + u16_accel_capture1[1] - u16_accel_capture1[0];
			}
			else
			{
				u32_receiver_ch2_period = u16_accel_capture1[1] - u16_accel_capture1[0];
			}

			u32_receiver_ch2_freq_Hz = (HAL_RCC_GetPCLK1Freq() * 2) / (htim1.Instance->PSC + 1);  //84000000
			u32_receiver_ch2_freq_Hz = u32_receiver_ch2_freq_Hz / u32_receiver_ch2_period;

			b_g_accel_ch3_done = false;
		}
	if(b_g_accel_ch4_done)
	{
		if(u16_accel_capture2[0] >= u16_accel_capture1[0] && u16_accel_capture2[0] <= u16_accel_capture1[1])
		{
			u32_accel_pw_us = u16_accel_capture2[0] - u16_accel_capture1[0];
		}
		else if(u16_accel_capture2[1] >= u16_accel_capture1[0] && u16_accel_capture2[1] <= u16_accel_capture1[1])
		{
			u32_accel_pw_us = u16_accel_capture2[1] - u16_accel_capture1[0];
		}

		u32_receiver_ch2_duty = u32_accel_pw_us * 100 / u32_receiver_ch2_period;
		b_g_accel_ch4_done = false;
	}
	/* 수신기 ch2 계산 부분 끝 */

	/* 수신기 ch3 계산 부분 시작 */
	if(b_g_auto_mode_ch1_done)
	{
		if(u16_auto_mode_capture1[0] > u16_auto_mode_capture1[1])
		{
			u32_receiver_ch3_period = htim3.Instance->ARR + u16_auto_mode_capture1[1] - u16_auto_mode_capture1[0];
		}
		else
		{
			u32_receiver_ch2_period = u16_auto_mode_capture1[1] - u16_auto_mode_capture1[0];
		}

		u32_receiver_ch3_freq_Hz = (HAL_RCC_GetPCLK1Freq() * 2) / (htim3.Instance->PSC + 1);  //84000000
		u32_receiver_ch3_freq_Hz = u32_receiver_ch3_freq_Hz / u32_receiver_ch3_period;

		b_g_auto_mode_ch1_done = false;
	}
	if(b_g_auto_mode_ch2_done)
	{
		if(u16_auto_mode_capture2[0] >= u16_auto_mode_capture1[0] && u16_auto_mode_capture2[0] <= u16_auto_mode_capture1[1])
		{
			u32_auto_mode_pw_us = u16_auto_mode_capture2[0] - u16_auto_mode_capture1[0];
		}
		else if(u16_auto_mode_capture2[1] >= u16_auto_mode_capture1[0] && u16_auto_mode_capture2[1] <= u16_auto_mode_capture1[1])
		{
			u32_auto_mode_pw_us = u16_auto_mode_capture2[1] - u16_auto_mode_capture1[0];
		}

		u32_receiver_ch3_duty = u32_auto_mode_pw_us * 100 / u32_receiver_ch3_period;
		b_g_auto_mode_ch2_done = false;
	}
	/* 수신기 ch3 계산 부분 끝 */

	/* 수신기 ch4 계산 부분 시작 */
	if(b_g_drive_mode_ch3_done)
	{
		if(u16_drive_mode_capture1[0] > u16_drive_mode_capture1[1])
		{
			u32_receiver_ch4_period = htim3.Instance->ARR + u16_drive_mode_capture1[1] - u16_drive_mode_capture1[0];
		}
		else
		{
			u32_receiver_ch4_period = u16_drive_mode_capture1[1] - u16_drive_mode_capture1[0];
		}

		u32_receiver_ch4_freq_Hz = (HAL_RCC_GetPCLK1Freq() * 2) / (htim3.Instance->PSC + 1);  //84000000
		u32_receiver_ch4_freq_Hz = u32_receiver_ch4_freq_Hz / u32_receiver_ch4_period;

		b_g_drive_mode_ch3_done = false;
	}
	if(b_g_drive_mode_ch4_done)
	{
		if(u16_drive_mode_capture2[0] >= u16_drive_mode_capture1[0] && u16_drive_mode_capture2[0] <= u16_drive_mode_capture1[1])
		{
			u32_drive_mode_pw_us = u16_drive_mode_capture2[0] - u16_drive_mode_capture1[0];
		}
		else if(u16_drive_mode_capture2[1] >= u16_drive_mode_capture1[0] && u16_drive_mode_capture2[1] <= u16_drive_mode_capture1[1])
		{
			u32_drive_mode_pw_us = u16_drive_mode_capture2[1] - u16_drive_mode_capture1[0];
		}

		u32_receiver_ch4_duty = u32_drive_mode_pw_us * 100 / u32_receiver_ch4_period;
		b_g_drive_mode_ch4_done = false;
	}
	/* 수신기 ch4 계산 부분 끝 */



	// for debugging
	printf("%d %d %d ", u32_steering_pw_us, u32_accel_pw_us, u32_auto_mode_pw_us);
	printf("%d \r\n", u32_drive_mode_pw_us);

}



void Bootley::GetSteeringVal()
{
	float pulse_width_us = (float)u32_steering_pw_us - STEERING_PW_ERROR_US;

	if(pulse_width_us < RECEIVER_MIN_PW) 		pulse_width_us = RECEIVER_MIN_PW;
	else if(pulse_width_us > RECEIVER_MAX_PW)	pulse_width_us = RECEIVER_MAX_PW;

	else	printf("receiver ch1 detect error");
	
	f_steering_val = float_map(pulse_width_us, RECEIVER_MIN_PW, RECEIVER_MAX_PW, RIGHT_MAX_STEERING_VAL, LEFT_MAX_STEERING_VAL);
	
}

void Bootley::GetAccelVal()
{
	float pulse_width_us = (float)u32_accel_pw_us - ACCEL_PW_ERROR_US;

	if(pulse_width_us < RECEIVER_MIN_PW) 		pulse_width_us = RECEIVER_MIN_PW;
	else if(pulse_width_us > RECEIVER_MAX_PW)	pulse_width_us = RECEIVER_MAX_PW;

	else	printf("receiver ch2 detect error");

	f_accel_val = float_map(pulse_width_us, RECEIVER_MIN_PW, RECEIVER_MAX_PW, FORWARD_MAX_ACCEL_VAL, BACKWARD_MAX_ACCEL_VAL);

}


void Bootley::SetAutoMode()
{
	if ((u32_auto_mode_pw_us > 900) && (u32_auto_mode_pw_us < 1500))		b_auto_mode = MANUAL_MODE;
	else if ((u32_auto_mode_pw_us > 1500) && u32_auto_mode_pw_us < 2200)	b_auto_mode = AUTO_MODE;

	else	printf("receiver ch3 detect error");
}


void Bootley::SetDriveMode()
{
	if ((u32_auto_mode_pw_us > 900) && (u32_auto_mode_pw_us < 1300))		u8_drive_mode = SPINNING_MODE;
	else if ((u32_auto_mode_pw_us > 1301) && u32_auto_mode_pw_us < 1700)	u8_drive_mode = NORMAL_MODE;
	else if ((u32_auto_mode_pw_us > 1701) && u32_auto_mode_pw_us < 2200)	u8_drive_mode = LOCK_MODE;

	else	printf("receiver ch4 detect error");
}