
#include "HX711.h"
#include "main.h"
#include "softuart.h"
#include "kalman.h"
#include <string.h>

kalman_filter_t filter[NUM_OF_WEIGHT_SENSOR] = {0};

uint8_t  sens_channel = 0;
uint8_t  calibr_cnt = 0;

uint16_t max_weight_rst_counter = 0;

bool	 alarm_status = 0;
bool 	 buzzer_flag = 0;
uint16_t buzzer_counter = 0;
uint16_t alarm_out_cnt = 0;
void (*ptr_hx711_change_transfer_mode)(void) = NULL;

void HX711ChangeTransferMode(void)
{
	for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++){
		weight[i].raw_zero_offset = 0;
		weight[i].offsett_status = 0;
		weight[i].max_kg = 0;
		weight[i].prev_kg = 0;
		weight[i].kg = 0;
		weight[i].raw_data = 0;
		weight[i].raw_sum = 0;
		weight[i].measure_cnt = 0;
		MovingAvg_Init(&weight[i].avg_filter, settings.avrg_measure_num);
	}
}

uint8_t HX711TransmitCommand_UART(uint8_t channel, uint8_t command, uint8_t num)
{
	for(uint8_t i = 0; i < num; i++) {
		SoftUartPuts(channel, &command, 1);
		SoftUartWaitUntilTxComplate(channel);
	}
	return 0;
}

void HX711Init_UART(void){
	// If you wont to use more then 2 channels of soft UART, then you need to define and set RXn, TXn pins.
	if (settings.data_transfer_mode == 1) {
		HAL_GPIO_WritePin(TX1_Port, TX1_Pin, 1);
		HAL_GPIO_WritePin(TX2_Port, TX2_Pin, 1);
		SoftUartInit(0, TX1_Port, TX1_Pin, RX1_Port, RX1_Pin);
		SoftUartInit(1, TX2_Port, TX2_Pin, RX2_Port, RX2_Pin);
		SoftUartEnableRx(0);
		SoftUartEnableRx(1);
		HAL_Delay(1);
		weight[0].uart_data.command = CH_A_ACTIVE_TX_MODE1;
		weight[1].uart_data.command = CH_A_ACTIVE_TX_MODE1;

		HX711TransmitCommand_UART(0, weight[0].uart_data.command, 2);
		HX711TransmitCommand_UART(1, weight[1].uart_data.command, 2);
	} else {
		PD_SCK_1(0);
		PD_SCK_2(0);
		SoftUartDisableRx(0);
		SoftUartDisableRx(1);
	}
}

bool HX711DataValidate_UART(uart_data_t *data, uint8_t channel)
{
	uint8_t len = 0;
	uint8_t local_index = 0;
	static uint8_t local_flag = 0;

	if(SUart[channel].RxIndex >= HX711_UART_BUF_SIZE){
		len = SUart[channel].RxIndex;
		uint8_t start = 0;
		if(local_flag) {
			local_flag = 0;
			start = 1;
		}
		for(uint8_t i = start; i < len; i++){
			if(SUart[channel].Buffer->Rx[i] == 0xAA){ //if first byte in array is equal 0xAA
				local_index = i;
				local_flag = 1;
				break;
			}
		}
		if(local_flag)
		{
			if(SUart[channel].Buffer->Rx[local_index + HX711_UART_BUF_SIZE-1] == 0xFF){ //if last byte in array is equal 0xFF
				local_flag = 0;
				SUart[channel].RxIndex = 0;
				memcpy(data->buf, &SUart[channel].Buffer->Rx[local_index], HX711_UART_BUF_SIZE);
				memset(SUart[channel].Buffer->Rx, 0, SoftUartRxBufferSize);
				uint16_t check_sum = 0;
				for (uint8_t i = 1; i < 7; i++) {
					check_sum += data->buf[i];
				}
				if ((data->buf[7] * 256 + data->buf[8]) == check_sum) {  // Verify if the checksum is correct
					//return (data->buf[4] * 65536 + data->buf[5] * 256 + data->buf[6]);
					data->rx_pkt_cnt ++;
					return true;
				} else {
					data->error_pkt_cnt ++;
					return false;
				}
			}
		}
	}
	return false;
}

int32_t HX711ReadRaw_UART(uart_data_t *data)
{
	if (data->buf[1] == data->command) {
		// Calculate the detection result (here we get the AD value)
		return (data->buf[4] * 65536 + data->buf[5] * 256 + data->buf[6]);
	} else {
		return 0;
	}
}


// ініціалізація
void MovingAvg_Init(moving_avg_t *avg, uint16_t window_size) {
    avg->avg_window = window_size;
	avg->head 		= 0;
    avg->tail 		= 0;
    avg->sum 		= 0;
    avg->count 		= 0;
    memset(avg->buffer, 0, sizeof(avg->buffer));
}

void SetAvgNum(moving_avg_t *avg, uint16_t window_size) {
	avg->avg_window = window_size;
	avg->head 		= 0;
	avg->tail 		= 0;
	avg->sum 		= 0;
	avg->count 		= 0;
	for (uint8_t i = window_size; i < MAX_WINDOW; i++){
		avg->buffer[i] = 0;
	}
}

void MovingAvg_InitAll(void)
{
	for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++) {
		MovingAvg_Init(&weight[i].avg_filter, settings.avrg_measure_num);
	}
}

// додавання нового значення і обчислення середнього
int32_t MovingAvg_Update(moving_avg_t *avg, int32_t new_val) {
	if (avg->count == avg->avg_window) {
		avg->sum -= avg->buffer[avg->head];
		avg->head = (avg->head + 1) % avg->avg_window;
	} else {
		avg->count++;
	}
	avg->buffer[avg->tail] = new_val;
	avg->sum += new_val;
	avg->tail = (avg->tail + 1) % avg->avg_window;

	return (int32_t)(avg->sum / avg->count);
}

static inline void delay_us(uint32_t us)
{
    // Простий варіант для коротких затримок; краще реалізувати через TIM/DWT
    volatile uint32_t count = us *  (SystemCoreClock / 1000000UL) / 60; // груба оцінка
    while (count--) { __NOP(); };
}

GPIO_PinState HX711_DOUT_READ(uint8_t channel) {
    if (channel == 0) {
        return DOUT_READ_1();
    } else {
        return DOUT_READ_2();
    }
}

static inline void HX711_PD_SCK(uint8_t channel, uint8_t state) {
    if (channel == 0) {
    	PD_SCK_1(state);
    } else {
    	PD_SCK_2(state);
    }
}


/* Зчитати одне сире значення (signed 24-bit) */
bool HX711_read_raw(int32_t *out, uint8_t gain_pulses, uint8_t channel)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);//debug pin
    uint32_t raw = 0;

    // Чекаємо готовності
    uint32_t timeout = 200000;//~ 300ms
    while (HX711_DOUT_READ(channel) != GPIO_PIN_RESET) {
        if (--timeout == 0) return false;
    }

    // Зчитуємо 24 біта
    for (uint8_t i = 0; i < 24; ++i) {
        HX711_PD_SCK(channel, 1);
        delay_us(5);
        raw = (raw << 1);
        if (HX711_DOUT_READ(channel) != GPIO_PIN_RESET) {
            raw |= 1;
        }
        HX711_PD_SCK(channel, 0);
        delay_us(5);
    }

    // додаткові такти для вибору gain (1/2/3)
    for (uint8_t i = 0; i < gain_pulses; ++i) {
        HX711_PD_SCK(channel, 1);
        delay_us(5);
        HX711_PD_SCK(channel, 0);
        delay_us(5);
    }

    // sign-extend 24->32
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }
    *out = (int32_t)raw;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);//debug pin
    return true;
}

int32_t HX711ReadData (weight_t *weight, uint8_t channel)
{
	int32_t value = 0;

	if(HX711_DOUT_READ(channel) == GPIO_PIN_RESET) {
		if(HX711_read_raw(&value, HX711_GAIN_PULSES, channel)) {
			weight->COM_ERR_flag = 0;
			weight->measure_cnt ++;
		} else {
			weight->COM_ERR_flag = 1; //read err
			return 0;
		}
	} else {
		weight->COM_ERR_flag = 1; //pin high state
		return 0;
	}
	return value;
}

bool HX711GetData(weight_t *weight, uint8_t channel)
{
	bool status = 0;

	if(weight->read_cnt == 0 || HX711_DOUT_READ(channel) == GPIO_PIN_RESET) {

		if(HX711_DOUT_READ(channel) == GPIO_PIN_RESET && weight->before_read_cnt == 0) {
			weight->read_cnt = HX711_DATA_RATE_TIME_MS;
			weight->before_read_cnt = (HX711_TIME_BEFORE_READ + 1);
		}

		if (weight->before_read_cnt == 1) {
			weight->before_read_cnt = 0;
			if (weight->offsett_status == false)
			{
				if(weight->measure_cnt < AVRG_OFFSETT_MEASURE_NUM) {
					weight->raw_sum += HX711ReadData(weight, channel);
				} else {
					//write zero offsett
					weight->measure_cnt = 0;
					weight->raw_zero_offset = (int32_t)(weight->raw_sum / AVRG_OFFSETT_MEASURE_NUM);
					weight->raw_sum = 0;
					weight->offsett_status = true;
				}
			}
			else //offsett_status == true
			{
				int32_t new_raw = HX711ReadData(weight, channel);
				weight->raw_data = MovingAvg_Update(&weight->avg_filter, new_raw);
				weight->raw_data -= weight->raw_zero_offset;
				weight->unfilt_kg = (float)weight->raw_data / KG_DIV; //convert to kg
				weight->kg = (float)kalman_filtering(&filter[sens_channel], weight->unfilt_kg, 1.0f, 10.0f);

				if(weight->prev_kg <= settings.alarm_threshold_kg && weight->kg > settings.alarm_threshold_kg && weight->COM_ERR_flag == 0) {
					if(weight->active_state_cnt == 0) { weight->active_state_cnt = settings.data_normalize_time; } //MAX_DATA_NORMALIZ_TIME_MS
				}
				weight->prev_kg = weight->kg;
			}

			if (weight->COM_ERR_flag) //if there was an ERR while reading
			{
				weight->measure_cnt = 0;
				weight->raw_sum = 0;
				weight->kg = 0;
			}
		}

		status = weight->COM_ERR_flag;
	}
	if(weight->kg < 0) { weight->kg = 0.001f; }
	if(weight->kg > weight->max_kg) {
		weight->max_kg = weight->kg;
		max_weight_rst_counter = MAX_WEIGHT_RESET_TIME_S;
	}
	return status;
}

bool HX711GetData_UART(weight_t *weight, uint8_t channel)
{
	bool status = 0;

	int32_t new_raw = 0;
	if (weight->offsett_status == false)
	{
		if(weight->measure_cnt < AVRG_OFFSETT_MEASURE_NUM) {
			new_raw = HX711ReadRaw_UART(&weight->uart_data);
			if (new_raw <= 0) {
				weight->raw_sum = 0;
				weight->measure_cnt = 0;
			} else {
				weight->raw_sum += new_raw;
				weight->measure_cnt ++;
			}
		} else {
			//write zero offsett
			weight->measure_cnt = 0;
			weight->raw_zero_offset = (int32_t)(weight->raw_sum / AVRG_OFFSETT_MEASURE_NUM);
			weight->raw_sum = 0;
			weight->offsett_status = true;
		}
	}
	else //offsett_status == true
	{
		new_raw = HX711ReadRaw_UART(&weight->uart_data);
		weight->raw_data = MovingAvg_Update(&weight->avg_filter, new_raw);
		weight->raw_data -= weight->raw_zero_offset;
		weight->unfilt_kg = (float)weight->raw_data / KG_DIV; //convert to kg
		weight->kg = (float)kalman_filtering(&filter[sens_channel], weight->unfilt_kg, 1.0f, 10.0f);

		if(weight->prev_kg <= settings.alarm_threshold_kg && weight->kg > settings.alarm_threshold_kg && weight->COM_ERR_flag == 0) {
			if(weight->active_state_cnt == 0) { weight->active_state_cnt = settings.data_normalize_time; } //MAX_DATA_NORMALIZ_TIME_MS
		}
		weight->prev_kg = weight->kg;
	}

	if (weight->COM_ERR_flag) //if there was an ERR while reading
	{
		weight->measure_cnt = 0;
		weight->raw_sum = 0;
		weight->kg = 0;
	}

	status = weight->COM_ERR_flag;
	if(weight->kg < 0) { weight->kg = 0.001f; }
	if(weight->kg > weight->max_kg) {
		weight->max_kg = weight->kg;
		max_weight_rst_counter = MAX_WEIGHT_RESET_TIME_S;
	}
	return status;
}


bool HX711GetDataTask(void)
{
	bool status = 0;
	if (start_reading_data_cnt == 0) {
		if(settings.data_transfer_mode == 1){
			if (HX711DataValidate_UART(&weight[sens_channel].uart_data, sens_channel) == true) {
				status = HX711GetData_UART(&weight[sens_channel], sens_channel);
			}
		} else {
			status = HX711GetData(&weight[sens_channel], sens_channel);
		}
		if(sens_channel < (NUM_OF_WEIGHT_SENSOR - 1)) {
			sens_channel ++;
		} else {
			sens_channel = 0;
		}
	}

	return status;
}

void OffsettStatusCheck(void)
{
	for(uint8_t i = 0; i < NUM_OF_WEIGHT_SENSOR; i++) {
		if (weight[i].offsett_status == false) {
			ready_to_read = 0;
			break;
		} else {
			ready_to_read = 1;
		}
	}
}

