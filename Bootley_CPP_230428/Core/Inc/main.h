/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ASSI_BRAKE_Pin GPIO_PIN_13
#define ASSI_BRAKE_GPIO_Port GPIOC
#define RR_BRAKE_Pin GPIO_PIN_0
#define RR_BRAKE_GPIO_Port GPIOC
#define RL_BRAKE_Pin GPIO_PIN_1
#define RL_BRAKE_GPIO_Port GPIOC
#define FL_FG_Pin GPIO_PIN_2
#define FL_FG_GPIO_Port GPIOC
#define FL_FG_EXTI_IRQn EXTI2_IRQn
#define FR_FG_Pin GPIO_PIN_3
#define FR_FG_GPIO_Port GPIOC
#define FR_FG_EXTI_IRQn EXTI3_IRQn
#define FL_SPEED_Pin GPIO_PIN_0
#define FL_SPEED_GPIO_Port GPIOA
#define FR_SPEED_Pin GPIO_PIN_1
#define FR_SPEED_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define FL_BRAKE_Pin GPIO_PIN_4
#define FL_BRAKE_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define RR_FG_Pin GPIO_PIN_4
#define RR_FG_GPIO_Port GPIOC
#define RR_FG_EXTI_IRQn EXTI4_IRQn
#define RL_FG_Pin GPIO_PIN_5
#define RL_FG_GPIO_Port GPIOC
#define RL_FG_EXTI_IRQn EXTI9_5_IRQn
#define FR_BRAKE_Pin GPIO_PIN_0
#define FR_BRAKE_GPIO_Port GPIOB
#define FL_DIR_Pin GPIO_PIN_1
#define FL_DIR_GPIO_Port GPIOB
#define RR_SPEED_Pin GPIO_PIN_2
#define RR_SPEED_GPIO_Port GPIOB
#define RL_SPEED_Pin GPIO_PIN_10
#define RL_SPEED_GPIO_Port GPIOB
#define RR_DIR_Pin GPIO_PIN_13
#define RR_DIR_GPIO_Port GPIOB
#define RL_DIR_Pin GPIO_PIN_14
#define RL_DIR_GPIO_Port GPIOB
#define FR_DIR_Pin GPIO_PIN_15
#define FR_DIR_GPIO_Port GPIOB
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define ASSI_MANUAL_Pin GPIO_PIN_15
#define ASSI_MANUAL_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ASSI_AUTO_Pin GPIO_PIN_7
#define ASSI_AUTO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
