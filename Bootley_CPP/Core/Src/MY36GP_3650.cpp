/*
 *
 * MY36GP_3650.c
 *
 * Author       : Delcoii
 * Date         : 23.01.20
 * parameter    : TIM1, GPIO outputs
 *
 *
*/
#include <stdio.h>
#include <stdbool.h>

#include "tim.h"            // TIM1 이 모터 속도값 출력에 쓰임
#include "gpio.h"

#include "MY36GP_3650.hpp"



uint32_t RR_capture[2];
bool RR_saved_index = FIRST_INDEX;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if(GPIO_Pin == GPIO_PIN_4)	// RR
	{

//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);		// for test

		if (RR_saved_index)
		{
			RR_capture[0] = TIM10->CNT;
			RR_saved_index = FIRST_INDEX;
		}
		else
		{
			RR_capture[1] = TIM10->CNT;
			RR_saved_index = SECOND_INDEX;
		}
	}
}


float RR_GetRPM()
{
	uint32_t period_us;
	if(RR_saved_index)
	{
//		period_us = (RR_capture[1] - RR_capture[0]) * 2;
		period_us = (RR_capture[1] - RR_capture[0]);
	}
	else
	{
//		period_us = (RR_capture[0] - RR_capture[1]) * 2;
		period_us = (RR_capture[0] - RR_capture[1]) * 2;

	}

	return (60*1000000) / (float)(period_us * MOTOR_PPR * GEAR_RATIO);
}


void FL_BrakeEnable(void) { HAL_GPIO_WritePin(FL_BRAKE_PORT, FL_BRAKE_PIN, GPIO_PIN_RESET); }
void FL_BrakeDisable(void) { HAL_GPIO_WritePin(FL_BRAKE_PORT, FL_BRAKE_PIN, GPIO_PIN_SET); }
void FL_SetDir(GPIO_PinState dir) { HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir); }	// CW 일 때 pin reset, CCW일 때 set

void FR_BrakeEnable(void) { HAL_GPIO_WritePin(FR_BRAKE_PORT, FR_BRAKE_PIN, GPIO_PIN_RESET); }
void FR_BrakeDisable(void) { HAL_GPIO_WritePin(FR_BRAKE_PORT, FR_BRAKE_PIN, GPIO_PIN_SET); }
void FR_SetDir(GPIO_PinState dir) { HAL_GPIO_WritePin(FR_DIR_PORT, FR_DIR_PIN, dir); }	// CW 일 때 pin reset, CCW일 때 set

void RL_BrakeEnable(void) { HAL_GPIO_WritePin(RL_BRAKE_PORT, RL_BRAKE_PIN, GPIO_PIN_RESET); }
void RL_BrakeDisable(void) { HAL_GPIO_WritePin(RL_BRAKE_PORT, RL_BRAKE_PIN, GPIO_PIN_SET); }
void RL_SetDir(GPIO_PinState dir) { HAL_GPIO_WritePin(RL_DIR_PORT, RL_DIR_PIN, dir); }	// CW 일 때 pin reset, CCW일 때 set

void RR_BrakeEnable(void) { HAL_GPIO_WritePin(RR_BRAKE_PORT, RR_BRAKE_PIN, GPIO_PIN_RESET); }
void RR_BrakeDisable(void) { HAL_GPIO_WritePin(RR_BRAKE_PORT, RR_BRAKE_PIN, GPIO_PIN_SET); }
void RR_SetDir(GPIO_PinState dir) { HAL_GPIO_WritePin(RR_DIR_PORT, RR_DIR_PIN, dir); }	// CW 일 때 pin reset, CCW일 때 set




/*
 * 모터 스펙
 *
 * RPM  : 정격 450, 최대 571
 * 데이터시트 상으로 0.5V일 때 정격 RPM, 2.5V일 때 최소인데 막 정확하진 않음
 * 
 * TIM2의 ARR 800일 때
 * 최대 RPM -> CCR = 140
 * 최소 RPM -> CCR = 750
 * dir  : CW or CCW
*/
void FL_RunMotor(float rpm, GPIO_PinState dir)
{
    float f_RPMtoCCR;
    uint32_t u32_input_CCR;

    f_RPMtoCCR = float_map(rpm, 0.0, FL_RATED_RPM, FL_MIN_SPEED_CCR, FL_RATED_SPEED_CCR);

	// constrain
    if (f_RPMtoCCR > FL_MIN_SPEED_CCR)    		f_RPMtoCCR = FL_MIN_SPEED_CCR;
    else if(f_RPMtoCCR < FL_RATED_SPEED_CCR) 	f_RPMtoCCR = FL_RATED_SPEED_CCR;

	// 모터가 전원만 받고 안돌아가면 꺼지더라구요 -> 최소 출력 설정
    if (f_RPMtoCCR < FL_MOTOR_STOP_CCR) 	u32_input_CCR = (uint32_t)f_RPMtoCCR;
    else 									u32_input_CCR = OUTPUT_HIGH;


	FL_BrakeDisable();
    FL_SetDir(dir);
    TIM2->CCR1 = u32_input_CCR;

//    printf("\r\nFL CCR : %d\t", u32_input_CCR);	// for debugging

}

void FR_RunMotor(float rpm, GPIO_PinState dir)
{
    float f_RPMtoCCR;
    uint32_t u32_input_CCR;

    f_RPMtoCCR = float_map(rpm, 0.0, FR_RATED_RPM, FR_MIN_SPEED_CCR, FR_RATED_SPEED_CCR);

	// constrain
    if (f_RPMtoCCR > FR_MIN_SPEED_CCR)    		f_RPMtoCCR = FR_MIN_SPEED_CCR;
    else if(f_RPMtoCCR < FR_RATED_SPEED_CCR) 	f_RPMtoCCR = FR_RATED_SPEED_CCR;

	// 모터가 전원만 받고 안돌아가면 꺼지더라구요 -> 최소 출력 설정
    if (f_RPMtoCCR < FR_MOTOR_STOP_CCR) 	u32_input_CCR = (uint32_t)f_RPMtoCCR;
    else 									u32_input_CCR = OUTPUT_HIGH;


	FR_BrakeDisable();
    FR_SetDir(dir);
    TIM2->CCR2 = u32_input_CCR;

//    printf("FR CCR : %d\t", u32_input_CCR);	// for debugging

}

void RL_RunMotor(float rpm, GPIO_PinState dir)
{
    float f_RPMtoCCR;
    uint32_t u32_input_CCR;

    f_RPMtoCCR = float_map(rpm, 0.0, RL_RATED_RPM, RL_MIN_SPEED_CCR, RL_RATED_SPEED_CCR);

	// constrain
    if (f_RPMtoCCR > RL_MIN_SPEED_CCR)    		f_RPMtoCCR = RL_MIN_SPEED_CCR;
    else if(f_RPMtoCCR < RL_RATED_SPEED_CCR) 	f_RPMtoCCR = RL_RATED_SPEED_CCR;

	// 모터가 전원만 받고 안돌아가면 꺼지더라구요 -> 최소 출력 설정
    if (f_RPMtoCCR < RL_MOTOR_STOP_CCR) 	u32_input_CCR = (uint32_t)f_RPMtoCCR;
    else 									u32_input_CCR = OUTPUT_HIGH;


	RL_BrakeDisable();
    RL_SetDir(dir);
    TIM2->CCR3 = u32_input_CCR;

//    printf("RL CCR : %d\t", u32_input_CCR);	// for debugging

}

void RR_RunMotor(float rpm, GPIO_PinState dir)
{
    float f_RPMtoCCR;
    uint32_t u32_input_CCR;

    f_RPMtoCCR = float_map(rpm, 0.0, RR_RATED_RPM, RR_MIN_SPEED_CCR, RR_RATED_SPEED_CCR);

	// constrain
    if (f_RPMtoCCR > RR_MIN_SPEED_CCR)    		f_RPMtoCCR = RR_MIN_SPEED_CCR;
    else if(f_RPMtoCCR < RR_RATED_SPEED_CCR) 	f_RPMtoCCR = RR_RATED_SPEED_CCR;

	// 모터가 전원만 받고 안돌아가면 꺼지더라구요 -> 최소 출력 설정
    if (f_RPMtoCCR < RR_MOTOR_STOP_CCR) 	u32_input_CCR = (uint32_t)f_RPMtoCCR;
    else 									u32_input_CCR = OUTPUT_HIGH;


	RR_BrakeDisable();
    RR_SetDir(dir);
    TIM2->CCR4 = u32_input_CCR;

//    printf("RR CCR : %d\r\n", u32_input_CCR);	// for debugging

}


void RR_FeebackMotorControl(float f_input_rpm, float f_meas_rpm, GPIO_PinState dir)
{
	uint32_t u32_input_CCR;

	float f_ff_RPMtoCCR = 0.;

    float f_error;
    float f_p_control_value;
    float f_p_controled_RPMtoCCR = 0.;

//    printf("\r\n\n");


    f_ff_RPMtoCCR = float_map(f_input_rpm, 0.0, RR_RATED_RPM, RR_MIN_SPEED_CCR, RR_RATED_SPEED_CCR);	//개루프제어
//    printf("ff CCR : %f\r\n", f_ff_RPMtoCCR);

    f_error = f_input_rpm - f_meas_rpm;
    f_p_control_value = (-1.) * f_error * RR_P_GAIN;
//    printf("p val : %f \r\n", f_p_control_value);


//    u32_input_CCR = (uint32_t)f_RPMtoCCR;		// 개루프만 할 때
    u32_input_CCR = (uint32_t)(f_ff_RPMtoCCR + f_p_control_value);


	// constrain
    if (u32_input_CCR > RR_MIN_SPEED_CCR)    		u32_input_CCR = RR_MIN_SPEED_CCR;
    else if(u32_input_CCR < RR_RATED_SPEED_CCR) 	u32_input_CCR = RR_RATED_SPEED_CCR;

	// 모터가 전원만 받고 안돌아가면 꺼지더라구요 -> 최소 출력 설정
    if (u32_input_CCR < RR_MOTOR_STOP_CCR) 	u32_input_CCR = (uint32_t)u32_input_CCR;
    else 									u32_input_CCR = OUTPUT_HIGH;

//    printf("CCR : %d\r\n\n", u32_input_CCR);


	RR_BrakeDisable();
    RR_SetDir(dir);
    TIM2->CCR4 = u32_input_CCR;


//    printf("\r\n\nCCR1 : %d\t", u32_input_CCR);	// for debugging
//    printf("result CCR : %f \r\n\n", f_RPMtoCCR);
}



/*
 * 모터 ppr 6
 * 기어비 14
 */
float Period2RPM(uint32_t u32_period_us)
{
	float f_RPM = (60 * 1000000) / (((float)u32_period_us * MOTOR_PPR) * GEAR_RATIO);
	return f_RPM;
}




float FL_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
//	  printf("sum : %d \r\nindex : %d", u32_sum, index);

	f_data[filter_index] = f_input;
//	  printf("index : %d\tdata[index] : %d \r\n", index, u32_data[index]);

	for(int i = 0; i < WINDOW_SIZE; i++)
	{
		f_sum += f_data[i];
	}
//	  printf("f_sum : %f \r\n", f_sum);

	filter_index = (filter_index+1) % WINDOW_SIZE;
//	  printf("index : %d \r\n", index);

	f_result = f_sum / (float)WINDOW_SIZE;
//	  printf("result : %d \r\n\n", u32_result);

	return f_result;
}

float FR_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
//	  printf("sum : %d \r\nindex : %d", u32_sum, index);

	f_data[filter_index] = f_input;
//	  printf("index : %d\tdata[index] : %d \r\n", index, u32_data[index]);

	for(int i = 0; i < WINDOW_SIZE; i++)
	{
		f_sum += f_data[i];
	}
//	  printf("f_sum : %f \r\n", f_sum);

	filter_index = (filter_index+1) % WINDOW_SIZE;
//	  printf("index : %d \r\n", index);

	f_result = f_sum / (float)WINDOW_SIZE;
//	  printf("result : %d \r\n\n", u32_result);

	return f_result;
}

float RL_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
//	  printf("sum : %d \r\nindex : %d", u32_sum, index);

	f_data[filter_index] = f_input;
//	  printf("index : %d\tdata[index] : %d \r\n", index, u32_data[index]);

	for(int i = 0; i < WINDOW_SIZE; i++)
	{
		f_sum += f_data[i];
	}
//	  printf("f_sum : %f \r\n", f_sum);

	filter_index = (filter_index+1) % WINDOW_SIZE;
//	  printf("index : %d \r\n", index);

	f_result = f_sum / (float)WINDOW_SIZE;
//	  printf("result : %d \r\n\n", u32_result);

	return f_result;
}

float RR_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
//	  printf("sum : %d \r\nindex : %d", u32_sum, index);

	f_data[filter_index] = f_input;
//	  printf("index : %d\tdata[index] : %d \r\n", index, u32_data[index]);

	for(int i = 0; i < WINDOW_SIZE; i++)
	{
		f_sum += f_data[i];
	}
//	  printf("f_sum : %f \r\n", f_sum);

	filter_index = (filter_index+1) % WINDOW_SIZE;
//	  printf("index : %d \r\n", index);

	f_result = f_sum / (float)WINDOW_SIZE;
//	  printf("result : %d \r\n\n", u32_result);

	return f_result;
}




float float_map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t uint32_map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/////////////////////////////////////////////////////////
/* 테스트용 함수 */

#define ADC_MAX_VALUE	4095
#define PWM_MAX_VALUE	820

void motorVelocityCheck(uint16_t adc_val, GPIO_PinState dir)
{
    FL_BrakeDisable();

    HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
    TIM1->CCR1 = uint32_map(adc_val, 0, ADC_MAX_VALUE, 0, PWM_MAX_VALUE);

}



