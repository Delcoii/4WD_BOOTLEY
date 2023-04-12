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
#include "tim.h"            // TIM10 이 모터 속도값 출력에 쓰임
#include "gpio.h"			// 외부인터럽트 선언 필요

#include "MY36GP_3650.hpp"


// both rising and falling edge detection
uint16_t FL_edge_count = 0;
uint16_t FR_edge_count = 0;
uint16_t RL_edge_count = 0;
uint16_t RR_edge_count = 0;

uint16_t FL_edge_per_100ms = 0;
uint16_t FR_edge_per_100ms = 0;
uint16_t RL_edge_per_100ms = 0;
uint16_t RR_edge_per_100ms = 0;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	// FL FG signal edge detected
	if(GPIO_Pin == GPIO_PIN_2) {
		FL_edge_count++;
	}

	// FR FG signal edge detected
	else if(GPIO_Pin == GPIO_PIN_5)
	{
		FR_edge_count++;
	}

	// RL FG signal edge detected
	else if(GPIO_Pin == GPIO_PIN_3) {
		RL_edge_count++;
	}

	// RR FG signal edge detected
	else if(GPIO_Pin == GPIO_PIN_4) {
		RR_edge_count++;
	}
	
}


/*	FCLK 			84Mhz	
 * 	prescaler 		840
 * 	counter period	10000
 * 	따라서 0.1초마다 호출됨
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim10)
	{
		FL_edge_per_100ms = FL_edge_count;
		FR_edge_per_100ms = FR_edge_count;
		RL_edge_per_100ms = RL_edge_count;
		RR_edge_per_100ms = RR_edge_count;

		FL_edge_count = 0;
		FR_edge_count = 0;
		RL_edge_count = 0;
		RR_edge_count = 0;
	}
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


float FL_GetRPM() {
	return (FL_edge_per_100ms * 600) / ((MOTOR_PPR * 2) * GEAR_RATIO);

}

float FR_GetRPM() {
	return (FR_edge_per_100ms * 600) / ((MOTOR_PPR * 2) * GEAR_RATIO);

}

float RL_GetRPM() {
	return (RL_edge_per_100ms * 600) / ((MOTOR_PPR * 2) * GEAR_RATIO);

}

float RR_GetRPM() {
	return (RR_edge_per_100ms * 600) / ((MOTOR_PPR * 2) * GEAR_RATIO);

}



float FL_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
	f_data[filter_index] = f_input;

	for(int i = 0; i < WINDOW_SIZE; i++) {
		f_sum += f_data[i];
	}

	filter_index = (filter_index+1) % WINDOW_SIZE;
	f_result = f_sum / (float)WINDOW_SIZE;

	return f_result;
}


float FR_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
	f_data[filter_index] = f_input;

	for(int i = 0; i < WINDOW_SIZE; i++) {
		f_sum += f_data[i];
	}

	filter_index = (filter_index+1) % WINDOW_SIZE;
	f_result = f_sum / (float)WINDOW_SIZE;

	return f_result;
}

float RL_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
	f_data[filter_index] = f_input;

	for(int i = 0; i < WINDOW_SIZE; i++) {
		f_sum += f_data[i];
	}

	filter_index = (filter_index+1) % WINDOW_SIZE;
	f_result = f_sum / (float)WINDOW_SIZE;

	return f_result;
}

float RR_MovingAverage(float f_input)
{
  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., };

	float f_result;

	f_sum = 0.;
	f_data[filter_index] = f_input;

	for(int i = 0; i < WINDOW_SIZE; i++) {
		f_sum += f_data[i];
	}

	filter_index = (filter_index+1) % WINDOW_SIZE;
	f_result = f_sum / (float)WINDOW_SIZE;

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



