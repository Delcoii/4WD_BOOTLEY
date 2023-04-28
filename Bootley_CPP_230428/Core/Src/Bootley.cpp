


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




Bootley::Bootley()
{
	// printf("Bootley Initiated \r\n\n");
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

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

	HAL_TIM_Base_Start_IT(&htim10);
	HAL_TIM_Base_Start_IT(&htim11);

}






void Bootley::SetCarState()
{
	GetPulseWidth();
	GetSteeringVal();
	GetAccelVal();
	SetAutoMode();
	SetDriveMode();
	
	if (b_auto_mode == MANUAL_MODE) {
		HAL_GPIO_WritePin(ASSI_MANUAL_GPIO_Port, ASSI_MANUAL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ASSI_AUTO_GPIO_Port, ASSI_AUTO_Pin, GPIO_PIN_RESET);
	}
	else if (b_auto_mode == AUTO_MODE) {
		HAL_GPIO_WritePin(ASSI_MANUAL_GPIO_Port, ASSI_MANUAL_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ASSI_AUTO_GPIO_Port, ASSI_AUTO_Pin, GPIO_PIN_SET);
	}
		
		
	if (u8_drive_mode == LOCK_MODE)
		HAL_GPIO_WritePin(ASSI_BRAKE_GPIO_Port, ASSI_BRAKE_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(ASSI_BRAKE_GPIO_Port, ASSI_BRAKE_Pin, GPIO_PIN_RESET);


}


void Bootley::Drive()
{
	float f_temp_FL_rpm, f_temp_FR_rpm, f_temp_RL_rpm, f_temp_RR_rpm;

	if (b_auto_mode == MANUAL_MODE) {
		if (u8_drive_mode == NORMAL_MODE)
		{
			NormalMode(f_accel_val, f_steering_val);


			f_temp_FL_rpm = FL_GetRPM();
			f_temp_FR_rpm = FR_GetRPM();
			f_temp_RL_rpm = RL_GetRPM();
			f_temp_RR_rpm = RR_GetRPM();


			/*
			if((f_temp_FL_rpm >= 0.) && (f_temp_FL_rpm <= 600.)) {
				f_FL_checked_rpm = FL_MovingAverage(f_temp_FL_rpm);
			}
			if((f_temp_FR_rpm >= 0.) && (f_temp_FR_rpm <= 600.)) {
				f_FR_checked_rpm = FR_MovingAverage(f_temp_FR_rpm);
			}
			if((f_temp_RL_rpm >= 0.) && (f_temp_RL_rpm <= 600.)) {
				f_RL_checked_rpm = RL_MovingAverage(f_temp_RL_rpm);
			}
			if((f_temp_RR_rpm >= 0.) && (f_temp_RR_rpm <= 600.)) {
				f_RR_checked_rpm = RR_MovingAverage(f_temp_RR_rpm);
			}*/


			// printf("rpm : %d\t%d\t%d\t%d\r\n", FL_edge_per_100ms, FR_edge_per_100ms, RL_edge_per_100ms, RR_edge_per_100ms);
		}

		else if (u8_drive_mode == SPINNING_MODE)
		{
			SpinningMode(f_steering_val);
			f_FL_checked_rpm = FL_GetRPM();
			f_FR_checked_rpm = FR_GetRPM();
			f_RL_checked_rpm = RL_GetRPM();
			f_RR_checked_rpm = RR_GetRPM();
		}	
		
		else	// if drive_mode == LOCK_MODE
		{
			EStopMode();
		}
	}

	else if (b_auto_mode == AUTO_MODE) {

		if (u8_drive_mode == LOCK_MODE) {
			EStopMode();
		}
			
		else {

			f_FL_checked_rpm = FL_GetRPM();
			f_FR_checked_rpm = FR_GetRPM();
			f_RL_checked_rpm = RL_GetRPM();
			f_RR_checked_rpm = RR_GetRPM();
		
			AutoDrive();
		}

		
	}
		
}



void Bootley::NormalMode(float accel, float steering)
{	
	GPIO_PinState FL_dir, FR_dir, RL_dir, RR_dir;
	float input_rpm, FL_rpm, FR_rpm, RL_rpm, RR_rpm;
	float steering_division = 1.;

	input_rpm = float_map(abs(accel), 0., ACCEL_MAX, RPM_MIN, RPM_MAX);

	// 왼쪽으로 선회할 때
	if ((steering >= IGNORING_STEERING_MAX) && (steering <= STEERING_MAX))
	{
		steering_division = float_map(steering, IGNORING_STEERING_MAX, STEERING_MAX, STEERING_DIVISION_MIN, STEERING_DIVISION_MAX);

		if (steering_division <= 1.)		steering_division = 1.;
		else if (steering_division >= 5.) 	steering_division = 5.;

		FL_rpm = input_rpm / steering_division;
		FR_rpm = input_rpm;
		RL_rpm = input_rpm / steering_division;
		RR_rpm = input_rpm;

	}
	// 직진
	else if ((steering >= IGNORING_STEERING_MIN) && (steering <= IGNORING_STEERING_MAX))
	{

		FL_rpm = input_rpm;
		FR_rpm = input_rpm;
		RL_rpm = input_rpm;
		RR_rpm = input_rpm;
	}
	// 오른쪽으로 선회
	else if ((steering >= STEERING_MIN) && steering <= IGNORING_STEERING_MIN)
	{
		steering_division = float_map(steering, STEERING_MIN, IGNORING_ACCEL_MIN, STEERING_DIVISION_MAX, STEERING_DIVISION_MIN);

		if (steering_division <= 1.)		steering_division = 1.;
		else if (steering_division >= 5.) 	steering_division = 5.;

		FL_rpm = input_rpm;
		FR_rpm = input_rpm / steering_division;
		RL_rpm = input_rpm;
		RR_rpm = input_rpm / steering_division;

	}


	// 신호가 중간값 부근일 땐 아무것도 하지 않음
	if ((accel >= IGNORING_ACCEL_MIN) && (accel <= IGNORING_ACCEL_MAX))
	{
		FL_RunMotor(0., CW);
		FR_RunMotor(0., CW);
		RL_RunMotor(0., CW);
		RR_RunMotor(0., CW);
	}
	// 신호가 범위 이내일 때 작동
	else if ((accel >= ACCEL_MIN) && (accel <= ACCEL_MAX))
	{
		if (accel >= 0.)		// 직진일 때
		{
			FL_dir = CCW;
			FR_dir = CW;
			RL_dir = CCW;
			RR_dir = CW;

			FL_RunMotor(FL_rpm, FL_dir);
			FR_RunMotor(FR_rpm, FR_dir);
			RL_RunMotor(RL_rpm, RL_dir);
			RR_RunMotor(RR_rpm, RR_dir);

		} else if (accel < 0.)
		{
			FL_dir = CW;
			FR_dir = CCW;
			RL_dir = CW;
			RR_dir = CCW;

			FL_RunMotor(FL_rpm, FL_dir);
			FR_RunMotor(FR_rpm, FR_dir);
			RL_RunMotor(RL_rpm, RL_dir);
			RR_RunMotor(RR_rpm, RR_dir);

		}
	}
	
}

void Bootley::SpinningMode(float steering)
{
	GPIO_PinState FL_dir = CW;
	GPIO_PinState FR_dir = CW;
	GPIO_PinState RL_dir = CW;
	GPIO_PinState RR_dir = CW;

	float rpm;

	// 왼쪽으로 빙빙돌아
	if ((steering >= IGNORING_SPINNING_MAX) && (steering <= STEERING_MAX))
	{
		FL_dir = CW;
		FR_dir = CW;
		RL_dir = CW;
		RR_dir = CW;

		rpm = float_map(steering, IGNORING_SPINNING_MAX, STEERING_MAX, SPINNING_RPM_MIN, SPINNING_RPM_MAX);
		FL_RunMotor(rpm, FL_dir);
		FR_RunMotor(rpm, FR_dir);
		RL_RunMotor(rpm, RL_dir);
		RR_RunMotor(rpm, RR_dir);
	}
	// 아무것도 안함
	else if ((steering >= IGNORING_SPINNING_MIN) && (steering <= IGNORING_SPINNING_MAX))
	{

		FL_RunMotor(0., FL_dir);
		FR_RunMotor(0., FR_dir);
		RL_RunMotor(0., RL_dir);
		RR_RunMotor(0., RR_dir);

	}

	// 오른쪽으로 빙빙돌아
	else if ((steering >= STEERING_MIN) && (steering <= IGNORING_SPINNING_MIN))
	{
		FL_dir = CCW;
		FR_dir = CCW;
		RL_dir = CCW;
		RR_dir = CCW;

		rpm = float_map(abs(steering), abs(IGNORING_SPINNING_MIN), abs(STEERING_MIN), SPINNING_RPM_MIN, SPINNING_RPM_MAX);
		FL_RunMotor(rpm, FL_dir);
		FR_RunMotor(rpm, FR_dir);
		RL_RunMotor(rpm, RL_dir);
		RR_RunMotor(rpm, RR_dir);
	}

}

void Bootley::EStopMode()
{
	FL_BrakeEnable();
	FR_BrakeEnable();
	RL_BrakeEnable();
	RR_BrakeEnable();
}

void Bootley::AutoDrive()
{
	/* 추후 ROS subscriber 이용 구동 */
	FL_BrakeEnable();
	FR_BrakeEnable();
	RL_BrakeEnable();
	RR_BrakeEnable();
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


}



void Bootley::GetSteeringVal()
{
	float pulse_width_us = (float)u32_steering_pw_us - STEERING_PW_ERROR_US;

	if(pulse_width_us < (float)RECEIVER_MIN_PW) 		pulse_width_us = (float)RECEIVER_MIN_PW;
	else if(pulse_width_us > (float)RECEIVER_MAX_PW)	pulse_width_us = (float)RECEIVER_MAX_PW;

	
	f_steering_val = float_map(pulse_width_us, RECEIVER_MIN_PW, RECEIVER_MAX_PW, LEFT_MAX_STEERING_VAL, RIGHT_MAX_STEERING_VAL);
	
}

void Bootley::GetAccelVal()
{
	float pulse_width_us = (float)u32_accel_pw_us - ACCEL_PW_ERROR_US;

	if(pulse_width_us < (float)RECEIVER_MIN_PW) 		pulse_width_us = (float)RECEIVER_MIN_PW;
	else if(pulse_width_us > (float)RECEIVER_MAX_PW)	pulse_width_us = (float)RECEIVER_MAX_PW;


	f_accel_val = float_map(pulse_width_us, RECEIVER_MIN_PW, RECEIVER_MAX_PW, BACKWARD_MAX_ACCEL_VAL, FORWARD_MAX_ACCEL_VAL);

}


void Bootley::SetAutoMode()
{
	if ((u32_auto_mode_pw_us > 900) && (u32_auto_mode_pw_us < 1500))		b_auto_mode = MANUAL_MODE;
	else if ((u32_auto_mode_pw_us > 1500) && u32_auto_mode_pw_us < 2200)	b_auto_mode = AUTO_MODE;

}


void Bootley::SetDriveMode()
{
	if ((u32_drive_mode_pw_us > 900) && (u32_drive_mode_pw_us < 1300))		u8_drive_mode = SPINNING_MODE;
	else if ((u32_drive_mode_pw_us > 1301) && u32_drive_mode_pw_us < 1700)	u8_drive_mode = NORMAL_MODE;
	else if ((u32_drive_mode_pw_us > 1701) && u32_drive_mode_pw_us < 2200)	u8_drive_mode = LOCK_MODE;

}







/*******************************************************************************/
// for debugging
void check_IC()
{
	printf("check in : %d %d \r\n\n", b_g_drive_mode_ch3_done, b_g_drive_mode_ch4_done);
}
