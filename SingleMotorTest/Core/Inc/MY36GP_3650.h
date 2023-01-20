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