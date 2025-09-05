/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include "menu.h"
#include "HX711.h"
#include "kalman.h"

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
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */

bool ready_to_read = 0;
extern weight_t weight [NUM_OF_WEIGHT_SENSOR];

uint16_t UART_TX_counter = 0;
button_t buttons = {0,0,0,0,0,0,0,0};

uint16_t one_sec_counter = 0;

char SwNewName[32];
char SwCurrName[32];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

void ButtonHandler(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(100);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_UART_Transmit_DMA(&huart3, DispUart.txBuff, DISP_TX_BUFF);
  DispInit();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++) {
		  if (weight[i].offsett_status == false) {
			  ready_to_read = 0;
			  break;
		  } else {
			  ready_to_read = 1;
		  }
	  }
	  if (ready_to_read == 0) {
		  HX711OffsettTask();
	  }

	  HX711GetDataTask();
	  ButtonHandler();
	  MenuChangeLine();
	  DispTask();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, STATUS_LED_Pin|OUT_1_Pin|OUT_2_Pin|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : STATUS_LED_Pin OUT_1_Pin OUT_2_Pin PA15 */
  GPIO_InitStruct.Pin = STATUS_LED_Pin|OUT_1_Pin|OUT_2_Pin|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_R_Pin */
  GPIO_InitStruct.Pin = BTN_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_L_Pin BTN_UP_Pin BTN_DOWN_Pin PB3
                           PB5 PB8 PB9 */
  GPIO_InitStruct.Pin = BTN_L_Pin|BTN_UP_Pin|BTN_DOWN_Pin|GPIO_PIN_3
                          |GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


void ButtonHandler(void)
{
	//button 1
	if(BTN_R_READ() == 0 && buttons.RIGHT_flag == 0) {
		buttons.RIGHT_flag = 1;
		buttons.RIGHT_state = 1;
		buttons.R_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
	}
	if(BTN_R_READ() == 1) {
		buttons.RIGHT_flag = 0;
		buttons.R_long_press_cnt = 0;
	}
	//button 4
	if(BTN_L_READ() == 0 && buttons.LEFT_flag == 0) {
		buttons.LEFT_flag = 1;
		buttons.LEFT_state = 1;
		buttons.L_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
	}
	if(BTN_L_READ() == 1) {
		buttons.LEFT_flag = 0;
		buttons.L_long_press_cnt = 0;
	}
	//button 3
	if(BTN_UP_READ() == 0 && buttons.UP_flag == 0) {
		buttons.UP_flag = 1;
		buttons.UP_state = 1;
		buttons.U_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
	}
	if(BTN_UP_READ() == 1) {
		buttons.UP_flag = 0;
		buttons.U_long_press_cnt = 0;
	}
	//button 2
	if(BTN_DOWN_READ() == 0 && buttons.DOWN_flag == 0) {
		buttons.DOWN_flag = 1;
		buttons.DOWN_state = 1;
		buttons.D_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
	}
	if(BTN_DOWN_READ() == 1) {
		buttons.DOWN_flag = 0;
		buttons.D_long_press_cnt = 0;
	}

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		UART_TX_counter = 5;
		DispUart.packTxCnt++;		// go to next packet
		DispUart.pauseTmr = 1;		// start timeout
		//HAL_UART_Transmit_DMA(&huart3, DispUart.txBuff, DISP_TX_BUFF);
	}
}


//   1 ms timer
//---------------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3) {
		if (UART_TX_counter){
			UART_TX_counter--;
		} else {
			DispUart.pauseTmr = 0;
			HAL_UART_Transmit_DMA(&huart3, DispUart.txBuff, DISP_TX_BUFF);
		}

		if (buttons.R_long_press_cnt) {
			if (buttons.R_long_press_cnt == 1) { buttons.RIGHT_state = BTN_LONG_PRESS; }
			buttons.R_long_press_cnt --;
		}
		if (buttons.L_long_press_cnt) {
			if (buttons.L_long_press_cnt == 1) { buttons.LEFT_state = BTN_LONG_PRESS; }
			buttons.L_long_press_cnt --;
		}
		if (buttons.U_long_press_cnt) {
			if (buttons.U_long_press_cnt == 1) { buttons.UP_state = BTN_LONG_PRESS; }
			buttons.U_long_press_cnt --;
		}
		if (buttons.D_long_press_cnt) {
			if (buttons.D_long_press_cnt == 1) { buttons.DOWN_state = BTN_LONG_PRESS; }
			buttons.D_long_press_cnt --;
		}


		if(one_sec_counter < 1000) {one_sec_counter++;}
		else
		{
			one_sec_counter = 0;
			if(max_weight_rst_counter){
				if(max_weight_rst_counter == 1) {
					for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++) {
						weight[i].max_kg = 0;
					}
				}
				max_weight_rst_counter--;
			}
		}

		for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++)
		{
			if(weight[i].read_cnt){ weight[i].read_cnt --; }

			if(weight[i].kg > BUZZER_ACTIV_WEIGHT_KG)
			{
				if(weight[i].active_state_cnt) {
					weight[i].active_state_cnt --;
				} else {
					weight[i].signal_state = 1; ////Threshold reached!
				}
			} else {
				weight[i].active_state_cnt = 0;
				weight[i].signal_state = 0; ////Threshold not reached!
			}
		}

		if(calibr_cnt){ calibr_cnt --; }


		if(buzzer_counter) {
			buzzer_counter--;
			BUZZER_OUT(1);
			LED_BLUE(0);
		} else {
			BUZZER_OUT(0);
			LED_BLUE(1);
		}
	}
}

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
#ifdef USE_FULL_ASSERT
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
