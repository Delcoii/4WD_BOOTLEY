/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "MY36GP_3650.h"
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FIRST_INDEX		0
#define SECOND_INDEX	1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//uint32_t u32_FL_captured_val_us;
//
///*
// * 모터 pulse의 주파수를 측정
// * rising, falling edge에서 주파수값 갱신
// * TIM3에서 prescaler (84-1)로 설정함
// */
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//
//	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if FL pulse edge detected
//	{
//		u32_FL_captured_val_us = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
//
//	}
//
//}

uint32_t u32_FL_period_us;

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	static uint32_t u32_FL_tick_us[2];
//
//	if(GPIO_Pin == GPIO_PIN_4)
//	{
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);		// for test
//
//		u32_FL_tick_us[0] = u32_FL_tick_us[1];
//		u32_FL_tick_us[1] = TIM10->CNT;
//
//		u32_FL_period_us = u32_FL_tick_us[1] - u32_FL_tick_us[0];
//	}
//}

uint32_t capture[2];
bool saved_index = FIRST_INDEX;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if(GPIO_Pin == GPIO_PIN_4)
	{

		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);		// for test

		if (saved_index)
		{
			capture[0] = TIM10->CNT;
			saved_index = FIRST_INDEX;
		}
		else
		{
			capture[1] = TIM10->CNT;
			saved_index = SECOND_INDEX;
		}
	}
}

float GetRPM()
{
	uint32_t period_us;
	if(saved_index)
	{
		period_us = (capture[1] - capture[0]) * 2;
	}
	else
	{
		period_us = (capture[0] - capture[1]) * 2;

	}

	return (60*1000000) / (float)(period_us * MOTOR_PPR * GEAR_RATIO);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint32_t u32_adc_value;
    uint32_t u32_ccr_value;
    float f_voltage;
    float f_FL_period_us;

    float f_rpm, f_filt_rpm;
    float f_input_rpm;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */

  	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  	  HAL_TIM_Base_Start_IT(&htim10);
//  	  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 100);   // adc값 읽힐 때까지 최대 100ms 대기
      u32_adc_value = HAL_ADC_GetValue(&hadc1);
      HAL_ADC_Stop(&hadc1);

      f_voltage = float_map((float)u32_adc_value, 0.0, 4095.0, 0.0, 3.3);

      f_input_rpm = float_map(((float)u32_adc_value < 200)?200:u32_adc_value, 200, 4095.0, 0.0, 450);

//      motorVelocityCheck(u32_adc_value, CCW);
      FL_RunMotor(f_input_rpm, CW);

//      f_FL_period_us = (float)u32_FL_period_us;
//      f_rpm =  Period2RPM(f_FL_period_us * 2);
      f_rpm = GetRPM();
      if (f_rpm < 600);
      	  f_filt_rpm = f_MovingAverage(f_rpm);

      printf("adc value : %d\tvoltage : %f\tin_rpm : %f\t", u32_adc_value, f_voltage, f_input_rpm);
//      printf("motor pulse : %f us\r\n", f_FL_period_us);
      printf("rpm : %f\r\n", f_filt_rpm);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
