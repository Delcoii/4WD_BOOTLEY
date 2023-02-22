/*
 * MY36GP_3650.h
 *
 * Author       : Delcoii
 * Date         : 23.01.20
 * parameter    : TIM2, GPIO outputs
 *
 ******************************************
 * 건국대학교 팀카이 미니자율주행자동차 소모임
 * 행복찾아 멀리 가지 말자.
 * 다들 성공합시다
 *
 ******************************************
 *
 * TIM2 이용, 목표 RPM에 따라 pulse신호 발생
 * FCPU			: 84Mhz
 * Prescaler	: 4 - 1
 * Counter		: 820 - 1
 * 약 26KHz
 *
 * CW / CCW 핀으로 모터 방향 신호 인가
 * 경우에 따라 BRAKE 핀에 신호 인가
 *
 *********************************************
 * 모터 스펙
 * 최대 571 RPM, 정격 450 RPM
 * 정격 토크 3kg*cm
 * 6 ppr
 * 기어비 14
 * datasheet랑은 다르게 0V일 때 최대속도, 1.3V 이상일 때 멈춤
 *******************************************/


#ifndef __MY36GP_3650_HPP__
#define __MY36GP_3650_HPP__


#include "stm32f4xx_hal.h"

///////////////////////////////////////////////////
/* configuration define */
#define FL_SPEED_PORT       GPIOA
#define FL_SPEED_PIN        GPIO_PIN_0
#define FL_DIR_PORT         GPIOB
#define FL_DIR_PIN          GPIO_PIN_1
#define FL_BRAKE_PORT       GPIOB
#define FL_BRAKE_PIN        GPIO_PIN_7

#define FR_SPEED_PORT       GPIOA
#define FR_SPEED_PIN        GPIO_PIN_1
#define FR_DIR_PORT         GPIOB
#define FR_DIR_PIN          GPIO_PIN_15
#define FR_BRAKE_PORT       GPIOC
#define FR_BRAKE_PIN        GPIO_PIN_13

#define RL_SPEED_PORT       GPIOB
#define RL_SPEED_PIN        GPIO_PIN_10
#define RL_DIR_PORT         GPIOB
#define RL_DIR_PIN          GPIO_PIN_14
#define RL_BRAKE_PORT       GPIOC
#define RL_BRAKE_PIN        GPIO_PIN_14

#define RR_SPEED_PORT       GPIOB
#define RR_SPEED_PIN        GPIO_PIN_2
#define RR_DIR_PORT         GPIOB
#define RR_DIR_PIN          GPIO_PIN_13
#define RR_BRAKE_PORT       GPIOC
#define RR_BRAKE_PIN        GPIO_PIN_15

///////////////////////////////////////////////////

#define CW                  0
#define CCW                 1



#define OUTPUT_HIGH         9999
// #define FL_MAX_RPM     	571
#define FL_RATED_RPM		450
#define FL_MIN_SPEED_CCR	750
#define FL_MOTOR_STOP_CCR	730
#define FL_RATED_SPEED_CCR	140

#define FR_RATED_RPM		450
#define FR_MIN_SPEED_CCR	750
#define FR_MOTOR_STOP_CCR	730
#define FR_RATED_SPEED_CCR	140

#define RL_RATED_RPM		450
#define RL_MIN_SPEED_CCR	750
#define RL_MOTOR_STOP_CCR	730
#define RL_RATED_SPEED_CCR	140

#define RR_RATED_RPM		450
#define RR_MIN_SPEED_CCR	750
#define RR_MOTOR_STOP_CCR	730
#define RR_RATED_SPEED_CCR	140


#define WINDOW_SIZE		    10
#define GEAR_RATIO			14
#define MOTOR_PPR			6





void FL_RunMotor(float rpm, uint8_t dir);
void FR_RunMotor(float rpm, uint8_t dir);
void RL_RunMotor(float rpm, uint8_t dir);
void RR_RunMotor(float rpm, uint8_t dir);

float Period2RPM(uint32_t u32_period_us);

float FL_MovingAverage(float f_input);
float FR_MovingAverage(float f_input);
float RL_MovingAverage(float f_input);
float RR_MovingAverage(float f_input);


float float_map(float x, float in_min, float in_max, float out_min, float out_max);
uint32_t uint32_map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);

#endif  // __MY36GP_3650_HPP__
