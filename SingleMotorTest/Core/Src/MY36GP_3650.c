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


/*
extern ADC_HandleTypeDef hadc1;     // rf통신 수신기에서 pwm신호를 adc로 읽음
extern TIM_HandleTypeDef htim1;     // 모터 속도 신호를 pwm으로 보냄
*/


float float_map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t uint32_map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


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


float Period2RPM(uint32_t u32_period_us)
{
	float f_RPM = (60 * 1000000) / (((float)u32_period_us * MOTOR_PPR) * GEAR_RATIO);
	return f_RPM;
}


/*
 * 모터 스펙
 *
 * RPM  : 정격 450, 최대 571
 * 데이터시트가 줫나 이상하게돼서 전압 범위는 확인을 해봐야하는데,
 * 일단은 0~5V 라고 가정해야지 뭐
 *
 * dir  : CW or CCW
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

    if (f_RPMtoCCR < MOTOR_STOP_CCR)
    	u32_input_CCR = (uint32_t)f_RPMtoCCR;
    else
    	u32_input_CCR = 9999;


	FL_BrakeDisable();
    HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
    TIM1->CCR1 = u32_input_CCR;


    printf("\r\n\nCCR1 : %d\t", u32_input_CCR);	// for debugging
//    printf("result CCR : %f \r\n\n", f_RPMtoCCR);
}


float f_MovingAverage(float f_input)
{

  	static uint8_t filter_index = 0;
  	static float f_sum = 0;
  	static float f_data[WINDOW_SIZE] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

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

/////////////////////////////////////////////////////////
/* 테스트용 함수 */
void motorVelocityCheck(uint16_t adc_val, uint8_t dir)
{
    FL_BrakeDisable();

    HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
    TIM1->CCR1 = uint32_map(adc_val, 0, ADC_MAX_VALUE, 0, PWM_MAX_VALUE);

}
