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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
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



uint32_t u32_counter_10us = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*
	 * TIM10 이용 1us ticker 구현
	 *
	 * NUCELO - F446RE 보드 이용
	 * APB1 = 84Mhz
	 * APB2 = 84Mhz
	 *
	 * TIM10
	 * prescaler 	: 42 - 1
	 * counter		: 2 - 1
	 */
	if(htim->Instance == TIM2)
	{
		u32_counter_10us += 1;
	}
//	u32_counter_us++;
}



/*
 * GPIO interrupt 함수
 * EXTI4 이용 (PC4 핀)
 * TIM10으로 구현한 1us ticker를 이용하여 인터럽트 발생 시마다
 * 현재의 측정 tick값에서 이전의 tick값의 차를 주기로 설정
 *
 * 전역변수 u32_FL_period_us에 계산한 주기값 저장
 */
uint32_t u32_FL_period_10us;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t u32_FL_tick_10us[2];

	if(GPIO_Pin == GPIO_PIN_4)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);		// for test

		u32_FL_tick_10us[0] = u32_FL_tick_10us[1];
		u32_FL_tick_10us[1] = u32_counter_10us;		// from global variable

		u32_FL_period_10us = u32_FL_tick_10us[1] - u32_FL_tick_10us[0];
	}
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint32_t u32_adc_accel_value;
	uint32_t u32_adc_steering_value;
    uint32_t u32_ccr_value;
    float f_voltage;

    uint32_t u32_FL_RPM;
    uint32_t u32_period_us;


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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

 	  HAL_TIM_Base_Start_IT(&htim2);

  	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 100);   // adc값 읽힐 때까지 최대 100ms 대기
      u32_adc_accel_value = HAL_ADC_GetValue(&hadc1);
//      HAL_ADC_Stop(&hadc1);

      HAL_ADC_PollForConversion(&hadc1, 100);   // adc값 읽힐 때까지 최대 100ms 대기
      u32_adc_steering_value = HAL_ADC_GetValue(&hadc1);

      HAL_ADC_Stop(&hadc1);


      f_voltage = float_map((float)u32_adc_accel_value, 0.0, (float)ADC_MAX_VALUE, 0.0, 3.3);		// 3.3은 stm32 구동전압
      u32_ccr_value = uint32_map(u32_adc_accel_value, 0, ADC_MAX_VALUE, 0, PWM_MAX_VALUE);

      if (u32_adc_steering_value < (ADC_MAX_VALUE / 2))
    	  motorVelocityCheck(u32_adc_accel_value, CCW);
      else
    	  motorVelocityCheck(u32_adc_accel_value, CW);


      u32_period_us = u32_FL_period_10us * 10;
      u32_FL_RPM = (60 * 1000000) / ((u32_period_us * 12) * 14);

//      printf("adc1 : %d\t adc2 : %d\t v : %f \t", u32_adc_accel_value, u32_adc_steering_value, f_voltage);
//      printf("adc1 %d\t adc2 %d\t", u32_adc_accel_value, u32_adc_steering_value);
      printf("ccr %d\t (half)period %d us\t", u32_ccr_value, u32_period_us);
      printf("RPM %d \r\n", u32_FL_RPM);


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
	  printf("you can't sleep now");
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
