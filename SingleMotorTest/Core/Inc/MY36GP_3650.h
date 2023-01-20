/*
 * MY36GP_3650.h
 * 
 * Author       : Delcoii
 * Date         : 23.01.20
 * parameter    : TIM1, GPIO outputs
 * 
 * 건국대학교 팀카이 미니자율주행자동차 소모임
 * 행복찾아 멀리 가지 말자.
 * 다들 성공합시다
 * 
 * 
 * TIM1 이용, 목표 RPM에 따라 pulse신호 발생
 * CW / CCW 핀으로 모터 방향 신호 인가
 * 경우에 따라 BRAKE 핀에 신호 인가
 * 
 */

#ifndef __MY36GP_3650_H__
#define __MY36GP_3650_H__


#include "stm32f4xx_hal.h"

///////////////////////////////////////////////////
/* configuration define */

#define FL_SPEED_PORT       GPIOA
#define FL_SPEED_PIN        GPIO_PIN_8

#define FL_DIR_PORT         GPIOA
#define FL_DIR_PIN          GPIO_PIN_6

#define FL_BRAKE_PORT       GPIOB
#define FL_BRAKE_PIN        GPIO_PIN_10

///////////////////////////////////////////////////




#define CW          0
#define CCW         1

#define MAX_RPM     571



void FL_BrakeEnable(void);
void FL_BrakeDisable(void);
void FL_RunMotor(uint16_t rpm, uint8_t dir);
void motorVelocityCheck(uint16_t adc_value, uint8_t dir);




#endif  // __MY36GP_3650_H__