/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdint.h>

#include "typedef.h"
#include "menu.h"
#include "setting.h"
#include "display.h"
#include "menu_text.h"
#include "rtc.h"


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef struct {
	uint16_t rs485_command_interval_s;
	uint16_t test_mode;
	uint16_t test_start_hours;
	uint16_t test_start_minutes;
	uint16_t test_stop_hours;
	uint16_t test_stop_minutes;

	bool flash_write_flag;
} save_flash_t;


typedef enum {
	BTN_IDLE = 0,
	BTN_PRESS,
	BTN_LONG_PRESS
} btn_state_t;

typedef enum {
	BTN_UP = 0,
	BTN_DOWN,
	BTN_L,
	BTN_R
}cur_btn_state_t;

typedef struct
{
  bool DOWN_flag;
  bool UP_flag;
  bool RIGHT_flag;
  bool LEFT_flag;

  btn_state_t DOWN_state;
  btn_state_t UP_state;
  btn_state_t RIGHT_state;
  btn_state_t LEFT_state;

  uint8_t D_debounce_cnt;
  uint8_t U_debounce_cnt;
  uint8_t R_debounce_cnt;
  uint8_t L_debounce_cnt;

  uint16_t D_long_press_cnt;
  uint16_t U_long_press_cnt;
  uint16_t R_long_press_cnt;
  uint16_t L_long_press_cnt;

  cur_btn_state_t cur_state;

} button_t;
#define DEBOUNCE_TIME_MS		60

typedef enum {
	STOPED,
	MOVE_UP,
	MOVE_DOWN,
	SET_UP,
	SET_DOWN,
	EFO_MOVE_UP
} status_t;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define FW_NAME_VERSION						"RB TEST RC"
#define ADDR_FLASH   						(0x08010000)//page 64

#define ONE_SEC								1000.0f

#define BTN_LONG_PRESS_TIME_MS				800

#define BTN_R_READ()			HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) //pin 16
#define BTN_L_READ()			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)//pin 13
#define BTN_UP_READ()			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)//pin 14
#define BTN_DOWN_READ()			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)//pin 15

#define LED_BLUE(state)   		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,  (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)//LED OPERATE

extern save_flash_t settings;
extern uint8_t rx_flag;

extern button_t btn;
extern dummy_t dummy;
extern status_t status_RB;
extern volatile uint32_t one_sec_counter;

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern void ButtonsResetLong(void);
extern void ButtonsReset(void);
void RoadBlockerTestProcesing(void);
//extern void Flash_WriteData(uint32_t addr, uint16_t data);
//extern uint16_t FlashGetData(uint32_t addr);
//extern void ConfigReadWrite(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RE_DE_2_Pin GPIO_PIN_1
#define RE_DE_2_GPIO_Port GPIOA
#define STATUS_LED_Pin GPIO_PIN_4
#define STATUS_LED_GPIO_Port GPIOA
#define BTN_R_Pin GPIO_PIN_5
#define BTN_R_GPIO_Port GPIOA
#define OUT_1_Pin GPIO_PIN_6
#define OUT_1_GPIO_Port GPIOA
#define OUT_2_Pin GPIO_PIN_7
#define OUT_2_GPIO_Port GPIOA
#define BTN_L_Pin GPIO_PIN_12
#define BTN_L_GPIO_Port GPIOB
#define BTN_UP_Pin GPIO_PIN_13
#define BTN_UP_GPIO_Port GPIOB
#define BTN_DOWN_Pin GPIO_PIN_14
#define BTN_DOWN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
