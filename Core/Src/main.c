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
extern MenuTypeDef Menu;
extern weight_t weight [NUM_OF_WEIGHT_SENSOR];
save_flash_t settings = {0};
uint16_t UART_TX_counter = 0;
button_t btn = {0};

uint16_t one_sec_counter = 0;

SettParamDef SettParam[MEASURE_ITEM_NUM];  // min,max,def,step of parameters
uint16_t *pSettReg[MEASURE_ITEM_NUM];  // pointer to settings value

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

void Flash_ErasePage(uint32_t addr);
void Flash_WriteByte(uint32_t addr, uint8_t data);
uint8_t Flash_ReadByte(uint32_t addr);
void ConfigReadWrite(void);

void OutHandler(void);
void ButtonsReset(void);
void ButtonsResetLong(void);
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

	 ConfigReadWrite();

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
	  DispTask();
	  OutHandler();
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

  /*Configure GPIO pins : STATUS_LED_Pin PA15 */
  GPIO_InitStruct.Pin = STATUS_LED_Pin|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_R_Pin */
  GPIO_InitStruct.Pin = BTN_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_1_Pin OUT_2_Pin */
  GPIO_InitStruct.Pin = OUT_1_Pin|OUT_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_L_Pin BTN_UP_Pin BTN_DOWN_Pin PB8
                           PB9 */
  GPIO_InitStruct.Pin = BTN_L_Pin|BTN_UP_Pin|BTN_DOWN_Pin|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
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

void Flash_ErasePage(uint32_t addr)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = addr;
    EraseInitStruct.NbPages     = 1;

    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
}

void Flash_WriteByte(uint32_t addr, uint8_t data)
{
    HAL_FLASH_Unlock();

    Flash_ErasePage(addr);

    // Flash в F1 пишеться тільки halfword (16 біт)
    uint16_t halfword = (uint16_t)data;

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, halfword);

    HAL_FLASH_Lock();
}

uint8_t Flash_ReadByte(uint32_t addr)
{
    uint16_t halfword = *(__IO uint16_t*)addr;
    return (uint8_t)(halfword & 0xFF);
}

void ConfigReadWrite(void)
{
	if (settings.flash_write_flag && Menu.pageIndx != MENU_PAGE_MODE && Menu.sysMsg == MENU_SM_NO) {
		Flash_WriteByte(FLASH_ADDR, (uint8_t)settings.mod_config);
		settings.mod_config_prev = settings.mod_config;
		settings.flash_write_flag = 0;
	}
	if (settings.flash_read_flag == 0) {
		uint8_t mod_temp = Flash_ReadByte(FLASH_ADDR);
		if (mod_temp <= ALARM_SYNCHRO) {
			settings.mod_config = (sensors_mod_t)mod_temp;
		} else {
			settings.mod_config = ALARM_ST_ALONE;
		}
		settings.mod_config_prev = settings.mod_config;
		settings.flash_read_flag = 1;
	}
}

void OutHandler(void)
{
	// BUZZER activate condition start
	if(STATUS_IN() == 0){
		alarm_status = 1;
	} else {
		alarm_status = 0;
	}
	//----
	if (settings.mod_config == ALARM_ST_ALONE)
	{
		//split alarm mode
		for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++) {
			if (weight[i].signal_state) {
				buzzer_flag = 1;
				break;
			} else {
				buzzer_flag = 0;
			}
		}
		if(buzzer_flag) {
			if (alarm_status == 0) { STATUS_OUT(1); }
		} else {
			STATUS_OUT(0);
		}
	}
	else if (settings.mod_config == ALARM_SYNCHRO)
	{
		//synchronized alarm mode
		for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++) {
			if(weight[i].signal_state) {
				STATUS_OUT(1);
				break;
			} else {
				STATUS_OUT(0);
			}
		}
		if(alarm_status) {
			buzzer_flag = 1;
		} else {
			buzzer_flag = 0;
		}
	}
	if(buzzer_flag) {
		buzzer_counter = (uint16_t)(ONE_SEC * BUZZER_ACTIVE_TIME_S);
	}
	//----
	// BUZZER activate condition end
}

void ButtonsReset(void)
{
	if (btn.DOWN_state  == BTN_PRESS) btn.DOWN_state  = BTN_IDLE;
	if (btn.UP_state    == BTN_PRESS) btn.UP_state    = BTN_IDLE;
	if (btn.RIGHT_state == BTN_PRESS) btn.RIGHT_state = BTN_IDLE;
	if (btn.LEFT_state  == BTN_PRESS) btn.LEFT_state  = BTN_IDLE;
}
void ButtonsResetLong(void)
{
	if (btn.DOWN_state  == BTN_LONG_PRESS) btn.DOWN_state  = BTN_IDLE;
	if (btn.UP_state    == BTN_LONG_PRESS) btn.UP_state    = BTN_IDLE;
	if (btn.RIGHT_state == BTN_LONG_PRESS) btn.RIGHT_state = BTN_IDLE;
	if (btn.LEFT_state  == BTN_LONG_PRESS) btn.LEFT_state  = BTN_IDLE;
}

void ButtonHandler(void)
{
	//button 3
	if(BTN_UP_READ() == 0 && btn.UP_flag == 0) {
		btn.UP_flag = 1;
		btn.U_debounce_cnt = DEBOUNCE_TIME_MS;
	}
	if(btn.U_debounce_cnt == 1) {
		if(BTN_UP_READ() == 0) {
			btn.U_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
		}
		btn.U_debounce_cnt = 0;
	}
	if(BTN_UP_READ() == 1 && btn.U_debounce_cnt == 0) {
		if(btn.U_long_press_cnt > 1) {
			btn.UP_state = BTN_PRESS;
			btn.U_long_press_cnt = 0;
		}
		btn.UP_flag = 0;
	}

	//button 2
	if(BTN_DOWN_READ() == 0 && btn.DOWN_flag == 0) {
		btn.DOWN_flag = 1;
		btn.D_debounce_cnt = DEBOUNCE_TIME_MS;
	}
	if(btn.D_debounce_cnt == 1) {
		if(BTN_DOWN_READ() == 0) {
			btn.D_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
		}
		btn.D_debounce_cnt = 0;
	}
	if(BTN_DOWN_READ() == 1 && btn.D_debounce_cnt == 0) {
		if(btn.D_long_press_cnt > 1) {
			btn.DOWN_state = BTN_PRESS;
			btn.D_long_press_cnt = 0;
		}
		btn.DOWN_flag = 0;
	}

	//button 1
	if(BTN_R_READ() == 0 && btn.RIGHT_flag == 0) {
		btn.RIGHT_flag = 1;
		btn.R_debounce_cnt = DEBOUNCE_TIME_MS;
	}
	if(btn.R_debounce_cnt == 1) {
		if(BTN_R_READ() == 0) {
			btn.R_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
		}
		btn.R_debounce_cnt = 0;
	}
	if(BTN_R_READ() == 1 && btn.R_debounce_cnt == 0) {
		if(btn.R_long_press_cnt > 1) {
			btn.RIGHT_state = BTN_PRESS;
			btn.R_long_press_cnt = 0;
		}
		btn.RIGHT_flag = 0;
	}

	//button 4
	if(BTN_L_READ() == 0 && btn.LEFT_flag == 0) {
		btn.LEFT_flag = 1;
		btn.L_debounce_cnt = DEBOUNCE_TIME_MS;
	}
	if(btn.L_debounce_cnt == 1) {
		if(BTN_L_READ() == 0) {
			btn.L_long_press_cnt = BTN_LONG_PRESS_TIME_MS;
		}
		btn.L_debounce_cnt = 0;
	}
	if(BTN_L_READ() == 1 && btn.L_debounce_cnt == 0) {
		if(btn.L_long_press_cnt > 1) {
			btn.LEFT_state = BTN_PRESS;
			btn.L_long_press_cnt = 0;
		}
		btn.LEFT_flag = 0;
	}

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		UART_TX_counter = 5;
		DispUart.packTxCnt++;		// go to next packet
		DispUart.pauseTmr = 1;		// start timeout
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

		if(btn.R_debounce_cnt > 1) {
			btn.R_debounce_cnt --;
		}
		if(btn.L_debounce_cnt > 1) {
			btn.L_debounce_cnt --;
		}
		if(btn.U_debounce_cnt > 1) {
			btn.U_debounce_cnt --;
		}
		if(btn.D_debounce_cnt > 1) {
			btn.D_debounce_cnt --;
		}

		if (btn.U_long_press_cnt) {
			if (btn.U_long_press_cnt == 1) {
				btn.UP_state = BTN_LONG_PRESS; }
			btn.U_long_press_cnt --;
		}
		if (btn.D_long_press_cnt) {
			if (btn.D_long_press_cnt == 1) {
				btn.DOWN_state = BTN_LONG_PRESS; }
			btn.D_long_press_cnt --;
		}
		if (btn.R_long_press_cnt) {
			if (btn.R_long_press_cnt == 1) {
				btn.RIGHT_state = BTN_LONG_PRESS; }
			btn.R_long_press_cnt --;
		}
		if (btn.L_long_press_cnt) {
			if (btn.L_long_press_cnt == 1) {
				btn.LEFT_state = BTN_LONG_PRESS; }
			btn.L_long_press_cnt --;
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

			if(weight[i].kg > settings.alarm_treshold_kg)
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
			LED_BLUE(1);
		} else {
			BUZZER_OUT(0);
			LED_BLUE(0);
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
