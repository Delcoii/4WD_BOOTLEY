/*
 * MY36GP_3650.h
 *
 * Author       : Delcoii
 * Date         : 23.01.20
 * parameter    : TIM1, GPIO outputs
 *
 ******************************************
 * 건국대학교 팀카이 미니자율주행자동차 소모임
 * 행복찾아 멀리 가지 말자.
 * 다들 성공합시다
 *
 ******************************************
 *
 * TIM1 이용, 목표 RPM에 따라 pulse신호 발생
 * FCPU			: 84Mhz
 * Prescaler	: 84 - 1
 * Counter		: 400 - 1
 * 약 25Khz.?
 *
 * CW / CCW 핀으로 모터 방향 신호 인가
 * 경우에 따라 BRAKE 핀에 신호 인가
 *
 *********************************************
 * 모터 스펙
 * 최대 571 RPM, 정격 450 RPM
 * 정격 토크 3kg*cm
 * 6 ppr
 * datasheet랑은 다르게 0V일 때 최대속도, 1.3V 이상일 때 멈춤
 *******************************************/


#ifndef __MY36GP_3650_H__
#define __MY36GP_3650_H__


#include "stm32f4xx_hal.h"


#define AVG_FILTERING_VAL	0.9999
#define WINDOW_SIZE			10

///////////////////////////////////////////////////
/* configuration define */

#define FL_SPEED_PORT       GPIOA
#define FL_SPEED_PIN        GPIO_PIN_8

#define FL_DIR_PORT         GPIOA
#define FL_DIR_PIN          GPIO_PIN_6

#define FL_BRAKE_PORT       GPIOB
#define FL_BRAKE_PIN        GPIO_PIN_10

///////////////////////////////////////////////////

#define GEAR_RATIO			14
#define MOTOR_PPR			6

#define CW          		0
#define CCW         		1

#define ADC_MIN_VALUE		50		// 전원 인가 환경마다 달라질 수 있음
#define ADC_MAX_VALUE       4095
#define PWM_MAX_VALUE       999

#define MAX_RPM     		571
#define RATED_RPM			450


#define MIN_SPEED_CCR		400
#define RATED_SPEED_CCR		230

//#define FL_P_GAIN			4		// 개루프없음
#define FL_P_GAIN			0.685	// 개루프있음

float float_map(float x, float in_min, float in_max, float out_min, float out_max);
uint32_t uint32_map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);
float f_AvgFilter(float f_new_data);
uint32_t u32_AvgFilter(uint32_t u32_new_data);
float f_MovingAverage(float f_input);


float Period2RPM(uint32_t u32_period_us);


void FL_BrakeEnable(void);
void FL_BrakeDisable(void);
void FL_SetDir(uint8_t dir);

void FL_RunMotor(float rpm, uint8_t dir);
void FL_FeebackMotorControl(float f_input_rpm, float f_meas_rpm, uint8_t u8_dir);
void motorVelocityCheck(uint16_t adc_value, uint8_t dir);




#endif  // __MY36GP_3650_H__
