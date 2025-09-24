
#ifndef APPLICATION_CORE_HX711_H_
#define APPLICATION_CORE_HX711_H_

#include "stdint.h"
#include "stdbool.h"
#include "main.h"

#define CHANNEL_A 0
#define CHANNEL_B 1
#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

typedef struct
{
  GPIO_TypeDef  *clk_gpio;
  GPIO_TypeDef  *dat_gpio;
  uint16_t      clk_pin;
  uint16_t      dat_pin;
  long       	Aoffset;
  float         Ascale;
  uint8_t		Again;
  long       	Boffset;
  float         Bscale;
  uint8_t		Bgain;
  
}hx711_t;

typedef struct
{
	bool     offsett_status;
	int32_t  raw_data;
	int32_t  raw_zero_offset;
	float    unfilt_kg;
	float    kg;
	float    prev_kg;
	float	 max_kg;
	bool	 COM_ERR_flag; //communication error flag
	uint16_t read_cnt;

	bool	 signal_state;
	bool	 signal_switch_flag;
	uint16_t active_state_cnt;
	uint16_t unactive_state_cnt;
	uint8_t  measure_cnt;
	int64_t  raw_sum;
} weight_t;

//extern weight_t weight[NUM_OF_WEIGHT_SENSOR];

extern uint8_t  sens_channel;
extern uint8_t  calibr_cnt;

extern uint16_t max_weight_rst_counter;

extern bool alarm_status;
extern bool buzzer_flag;
extern uint16_t buzzer_counter;

extern UART_HandleTypeDef huart3;
extern weight_t weight[];

GPIO_PinState HX711_DOUT_READ(uint8_t channel);

bool HX711_read_raw(int32_t *out, uint8_t gain_pulses, uint8_t channel);
bool HX711_read_average(int32_t *out_avg, uint8_t samples, uint8_t gain_pulses, uint8_t channel);
bool HX711_zero_offsett(int32_t *offset, uint8_t channel);
bool HX711GetDataTask(void);


#endif /* APPLICATION_CORE_HX711_H_ */
