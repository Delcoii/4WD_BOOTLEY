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

#include "tim.h"            // TIM1 이 모터 속도값 출력에 쓰임
#include "MY36GP_3650.h"

#include <stdio.h>			// for debugging


float float_map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t uint32_map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float f_AvgFilter(float f_new_data)
{
	static float f_value[2] = {0., 0.};

	f_value[0] = f_value[1];
	f_value[1] = f_new_data;

	return (AVG_FILTERING_VAL * f_value[0]) + (1. - AVG_FILTERING_VAL) * f_value[1];
}

uint32_t u32_AvgFilter(uint32_t u32_new_data)
{
	static uint32_t u32_value[2] = {0, 0};

	u32_value[0] = u32_value[1];
	u32_value[1] = u32_new_data;

	return (AVG_FILTERING_VAL * u32_value[0]) + (1 - AVG_FILTERING_VAL) * u32_value[1];
}



////////////////////////////////////////////////////////////////////////////////////
void FL_BrakeEnable(void)
{
    HAL_GPIO_WritePin(FL_BRAKE_PORT, FL_BRAKE_PIN, GPIO_PIN_RESET);
}

void FL_BrakeDisable(void)
{
    HAL_GPIO_WritePin(FL_BRAKE_PORT, FL_BRAKE_PIN, GPIO_PIN_SET);
}

// CW 일 때 pin reset, CCW일 때 set
void FL_SetDir(uint8_t dir)
{
	HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
}
/////////////////////////////////////////////////////////////////////////////////////


/*
 * 모터 스펙
 *
 * RPM  : 정격 450, 최대 571
 * 데이터시트가 줫나 이상하게돼서 전압 범위는 확인을 해봐야하는데,
 * 일단은 0~5V 라고 가정해야지 뭐
 *
 * dir  : CW or CCW
 * parameter
 * RATED_SPEED_CCR, MIN_SPEED_CCR, MAX_RPM
*/
void FL_RunMotor(float rpm, uint8_t dir)
{
    float f_RPMtoCCR;
    uint32_t u32_input_CCR;

//    f_RPMtoCCR = MIN_SPEED_CCR - (rpm * ((float)MIN_SPEED_CCR/(float)MAX_RPM));
    f_RPMtoCCR = float_map(rpm, 0.0, RATED_RPM, MIN_SPEED_CCR, RATED_SPEED_CCR);

    if (f_RPMtoCCR > MIN_SPEED_CCR)
    	f_RPMtoCCR = MIN_SPEED_CCR;
    else if(f_RPMtoCCR < RATED_SPEED_CCR)
    	f_RPMtoCCR = RATED_SPEED_CCR;

    u32_input_CCR = (uint32_t)f_RPMtoCCR;

    // 존나 짜치네 진짜 중국산모터
    if (u32_input_CCR > 380)	u32_input_CCR = 500;

	FL_BrakeDisable();
    HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
    TIM1->CCR1 = u32_input_CCR;


    printf("\r\n\nCCR1 : %d\t", u32_input_CCR);	// for debugging
    printf("result CCR : %f \r\n\n", f_RPMtoCCR);
}


/*
 * us 단위의 주기를 RPM으로 환산
 *
 * 모터 ppr	: 6
 * 모터 기어비	: 14
 */
float Period2RPM(uint32_t u32_period_us)
{
	float f_RPM = (60 * 1000000) / (((float)u32_period_us * MOTOR_PPR) * GEAR_RATIO);
	return f_RPM;
}


/////////////////////////////////////////////////////////
/* 테스트용 함수 */
void motorVelocityCheck(uint16_t adc_val, uint8_t dir)
{
    FL_BrakeDisable();

    HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
    TIM1->CCR1 = uint32_map(adc_val, 0, ADC_MAX_VALUE, 0, PWM_MAX_VALUE);

}
