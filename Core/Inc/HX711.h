
#ifndef APPLICATION_CORE_HX711_H_
#define APPLICATION_CORE_HX711_H_

#include "stdint.h"
#include "stdbool.h"
#include "main.h"

#define MAX_WINDOW				10
#define HX711_UART_BUF_SIZE		10
#define HX711_UART

#define CHANNEL_A 0
#define CHANNEL_B 1
#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

typedef enum {
	//command to config mode 1
	CH_A_PASSIVE_TX_MODE1 	= 0xA1,	// Represents Channel A, Working Mode 1, Passive Transmission (one request → one response)
	CH_A_ACTIVE_TX_MODE1 	= 0xA2,	// Represents Channel A, Working Mode 1, Active Transmission
	CH_B_PASSIVE_TX_MODE1 	= 0xB1,	// Represents Channel B, Working Mode 1, Passive Transmission (one request → one response)
	CH_B_ACTIVE_TX_MODE1 	= 0xB2,	// Represents Channel B, Working Mode 1, Active Transmission
	//command to config mode 2
	CH_A_PASSIVE_TX_MODE2 	= 0xA3,	// Represents Channel A, Working Mode 2, Passive Transmission (one request → one response)
	CH_A_ACTIVE_TX_MODE2 	= 0xA4,	// Represents Channel A, Working Mode 2, Active Transmission
	CH_B_PASSIVE_TX_MODE2 	= 0xB3,	// Represents Channel B, Working Mode 2, Passive Transmission (one request → one response)
	CH_B_ACTIVE_TX_MODE2 	= 0xB4,	// Represents Channel B, Working Mode 2, Active Transmission
	//calibration mode command
	CH_A_CALIBR_MODE 		= 0xA5,	// Enter the Channel A calibration function
	CH_A_TARE 				= 0xAA,	// Perform tare (zero calibration) Ch A
	CH_B_TARE				= 0xBA,	// Perform tare (zero calibration) Ch B
	CH_A_INCREAS_K 			= 0xAB,	// Increasing the calibration coefficient K (Actual Weight = Weight_jingzhong / K)
	CH_A_DECREAS_K 			= 0xAC	// Decreasing the calibration coefficient K (Actual Weight = Weight_jingzhong / K)
}HX711_command_t;

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

typedef struct {
	uint8_t command;
	uint8_t tx_flag;
	uint8_t rx_flag;
	uint8_t rx_index;
	uint8_t rx_len;
	uint8_t buf[HX711_UART_BUF_SIZE];
} uart_data_t;

typedef struct {
    int32_t buffer[MAX_WINDOW];
    uint8_t head; // Points to the oldest element
    uint8_t tail; // Points to the newest element
    int64_t sum;
    uint8_t count; // Number of elements currently in the buffer
    uint16_t avg_window;
} moving_avg_t;

typedef struct
{
	bool     offsett_status;
	int32_t  raw_data;
	int32_t  raw_zero_offset;
	int64_t  raw_sum;
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
	uint8_t  before_read_cnt;

	uart_data_t uart_data;
	moving_avg_t avg_filter;
} weight_t;

//extern weight_t weight[NUM_OF_WEIGHT_SENSOR];

extern uint8_t  sens_channel;
extern uint8_t  calibr_cnt;

extern uint16_t max_weight_rst_counter;

extern bool alarm_status;
extern bool buzzer_flag;
extern uint16_t buzzer_counter;
extern uint16_t alarm_out_cnt;

extern UART_HandleTypeDef huart3;
extern weight_t weight[];

extern void (*ptr_hx711_change_transfer_mode)(void);

GPIO_PinState HX711_DOUT_READ(uint8_t channel);

void HX711ChangeTransferMode(void);
void HX711Init_UART(void);
bool HX711_read_raw(int32_t *out, uint8_t gain_pulses, uint8_t channel);
bool HX711GetDataTask(void);
void OffsettStatusCheck(void);
void MovingAvg_Init(moving_avg_t *avg, uint16_t window_size);
void SetAvgNum(moving_avg_t *avg, uint16_t window_size);
void MovingAvg_InitAll(void);

#endif /* APPLICATION_CORE_HX711_H_ */
