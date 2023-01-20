/*
 * Author   : Delcoii
 * Date     : 23.01.20
 * 
 * 건국대학교 팀카이 미니자율주행자동차 소모임
 * 행복찾아 멀리 가지 말자.
 * 다들 성공합시다
 * 
 * adc, 타이머(pwm), 핀 번호 define
 * 
 * 수신기에서 6채널의 pwm신호를 lpf를 통해 analog값으로 변환한 후
 * adc1 모듈을 통해 multi-channel single-conversion 구현
 * 
*/

#include "tim.h"            // TIM1 이 모터 속도값 출력에 쓰임
#include "MY36GP_3650.h"


/*
extern ADC_HandleTypeDef hadc1;     // rf통신 수신기에서 pwm신호를 adc로 읽음 
extern TIM_HandleTypeDef htim1;     // 모터 속도 신호를 pwm으로 보냄
*/


long map(long x, long in_min, long in_max, long out_min, long out_max)
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


/*
 * 모터 스펙
 *
 * RPM  : 정격 450, 최대 571
 * 데이터시트가 줫나 이상하게돼서 전압 범위는 확인을 해봐야하는데,
 * 일단은 0~5V 라고 가정해야지 뭐
 * 
 * dir  : CW or CCW
*/
void FL_RunMotor(uint16_t rpm, uint8_t dir)
{
    FL_BrakeDisable();
    
    HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
    TIM1->CCR1 = map(rpm, 0, MAX_RPM, 0, 999);           // 0 ~ 3.3V의 전압을 0~571RPM으로 매핑

}



/////////////////////////////////////////////////////////
/* 테스트용 함수 */
void motorVelocityCheck(uint16_t adc_value, uint8_t dir)
{
    FL_BrakeDisable();

    HAL_GPIO_WritePin(FL_DIR_PORT, FL_DIR_PIN, dir);
    TIM1->CCR1 = map(adc_value, 0, MAX_RPM, 0, 999);

}