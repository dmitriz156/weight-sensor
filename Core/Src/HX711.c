
#include "HX711.h"
#include "main.h"
#include "kalman.h"

kalman_filter_t filter[NUM_OF_WEIGHT_SENSOR] = {0};

uint8_t  sens_channel = 0;
uint8_t  calibr_cnt = 0;

uint16_t max_weight_rst_counter = 0;

bool	 alarm_status = 0;
bool 	 buzzer_flag = 0;
uint16_t buzzer_counter = 0;

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

/* Усереднення N вимірів */
bool HX711_read_average(int32_t *out_avg, uint8_t samples, uint8_t gain_pulses, uint8_t channel)
{
    int64_t sum = 0;
    int32_t v;
    uint8_t got = 0;
    for (uint8_t i = 0; i < samples; ++i) {
    	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);//debug pin
        if (!HX711_read_raw(&v, gain_pulses, channel)) return false;
        //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);//debug pin
        sum += v;
        got++;
    }
    if (got == 0) return false;
    *out_avg = (int32_t)(sum / got);
    return true;
}


bool HX711_zero_offsett(int32_t *offset, uint8_t channel)
{
	int64_t sum = 0;
	int32_t value = 0;
	uint8_t i = 0;

	HX711_read_raw(offset, HX711_GAIN_PULSES, channel);
	*offset = 0;
	while(i < 20) {
		if(calibr_cnt == 0 || HX711_DOUT_READ(channel) == GPIO_PIN_RESET) {
			calibr_cnt = HX711_DATA_RATE_TIME_MS;
			if (!HX711_read_raw(&value, HX711_GAIN_PULSES, channel)) {
				return false; // якщо зчитування невдале – вихід
			}
			sum += value;
			i++;
		}
	}

	*offset = (int32_t)(sum / 20);
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
	bool status;

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
				if(weight->measure_cnt < AVRG_MEASURE_NUMBER) {
					weight->raw_sum += HX711ReadData(weight, channel);
				} else {
					weight->measure_cnt = 0;
					weight->raw_data = (int32_t)(weight->raw_sum / AVRG_MEASURE_NUMBER);
					weight->raw_sum = 0;
					//weight measurement preprocesing
					weight->raw_data -= weight->raw_zero_offset;
					weight->unfilt_kg = (float)weight->raw_data / KG_DIV; //convert to kg
					weight->kg = (float)kalman_filtering(&filter[sens_channel], weight->unfilt_kg, 1.0f, 10.0f);
				}

				if(weight->prev_kg <= settings.alarm_threshold_kg && weight->kg > settings.alarm_threshold_kg && weight->COM_ERR_flag == 0) {
					if(weight->active_state_cnt == 0) { weight->active_state_cnt = MAX_DATA_NORMALIZ_TIME_MS; }
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


bool HX711GetDataTask(void)
{
	bool status = 0;
	if (start_reading_data_cnt == 0) {
		status = HX711GetData(&weight[sens_channel], sens_channel);

		if(sens_channel < (NUM_OF_WEIGHT_SENSOR - 1)) {
			sens_channel ++;
		} else {
			sens_channel = 0;
		}
	}

	return status;
}



