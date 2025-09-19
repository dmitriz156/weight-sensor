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
//#include "kalman.h"
#include "HX711.h"
#include "menu.h"
#include "setting.h"
#include "display.h"
#include "menu_text.h"


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//typedef signed int 			s32;
//typedef signed short int 	s16;
//typedef signed char 		s8;
//
//typedef unsigned int 		u32;
//typedef unsigned short int 	u16;
//typedef unsigned char 		u8;

typedef enum {
	ALARM_ST_ALONE = 0,
	ALARM_SYNCHRO  = 1,
} sensors_mod_t;

typedef struct {
//#define ADDR_FLASH_MOD
//#define ADDR_FLASH_THRESHOLD
	sensors_mod_t mod_config;
	sensors_mod_t mod_config_prev;

	uint16_t alarm_threshold_kg;
	uint16_t alarm_threshold_kg_prev;

	bool flash_read_flag;
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

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define NUM_OF_WEIGHT_SENSOR    			2
#define ADDR_FLASH   						(0x08010000)//page 64

//#define BUZZER_ACTIV_WEIGHT_KG				5 // weight limit in KG
#define HX711_GAIN_PULSES 					1 // for example: 1 = 128x, 2 = 64x, 3 = 32x
#define KG_DIV								45000.0f
#define BUZZER_ACTIVE_TIME_S				3.0f
#define ONE_SEC								1000.0f
#define AVRG_MEASURE_NUMBER					2 //number of measurements for averaging
#define AVRG_OFFSETT_MEASURE_NUM			20
#define MAX_WEIGHT_RESET_TIME_S				30
#define MAX_DATA_NORMALIZ_TIME_MS			400 //time to detect and confirm threshold reaching


#define HX711_DATA_RATE_TIME_MS				110 // 10 SPS (from HX711 datasheet)
#define DISPLAY_OUT_INTERVAL			    100


#define BTN_LONG_PRESS_TIME_MS	1000

#define BTN_R_READ()			HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) //pin 16
#define BTN_L_READ()			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)//pin 13
#define BTN_UP_READ()			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)//pin 14
#define BTN_DOWN_READ()			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)//pin 15

#define STATUS_IN()     		HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) //pin 23
#define CONFIG_JUMPER()			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) //pin 24

#define DOUT_READ_1()       	HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) //pin 27
#define PD_SCK_READ_1()     	HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) //pin 28
#define DOUT_READ_2()       	HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) //pin 29
#define PD_SCK_READ_2()     	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)//pin 30

#define PD_SCK_1(state)     	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,  (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)//pin 28
#define PD_SCK_2(state)     	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)//pin 30

#define BUZZER_OUT(state) 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,  (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)//pin 6 //OUT1
#define STATUS_OUT(state) 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,  (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)//pin 7 //OUT2
#define LED_BLUE(state)   		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,  (state) ? GPIO_PIN_SET : GPIO_PIN_RESET)//LED OPERATE

extern bool ready_to_read;
extern save_flash_t settings;

extern button_t btn;
extern char SwNewName[];
extern char SwCurrName[];


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern void ButtonsResetLong(void);
extern void ButtonsReset(void);
//extern void Flash_WriteData(uint32_t addr, uint16_t data);
//extern uint16_t FlashGetData(uint32_t addr);
//extern void ConfigReadWrite(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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
