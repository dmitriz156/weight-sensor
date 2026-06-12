#ifndef INC_RTC_H_
#define INC_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#define RTC_SECONDS_PER_MINUTE    60U
#define RTC_SECONDS_PER_HOUR      3600U
#define RTC_SECONDS_PER_DAY       86400U

typedef struct {
    uint32_t total_seconds;
    uint32_t days;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} rtc_elapsed_time_t;

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_set_t;

typedef enum {
    RTC_TIME_FIELD_HOURS = 0,
    RTC_TIME_FIELD_MINUTES,
    RTC_TIME_FIELD_SECONDS,
    RTC_TIME_FIELD_DAY,
    RTC_TIME_FIELD_MONTH,
    RTC_TIME_FIELD_YEAR,
    RTC_TIME_FIELD_INVALID
} rtc_time_field_t;

HAL_StatusTypeDef RTC_Module_Init(RTC_HandleTypeDef *hrtc);
bool RTC_Module_IsReady(void);

uint32_t RTC_GetPersistentSeconds(void);
uint32_t RTC_GetUptimeSeconds(void);

void RTC_GetPersistentElapsed(rtc_elapsed_time_t *elapsed);
void RTC_GetUptime(rtc_elapsed_time_t *elapsed);
void RTC_Process(void);
uint16_t RTC_GetCurrentTimeField(rtc_time_field_t field);

HAL_StatusTypeDef RTC_SetCurrentTime(const rtc_time_set_t *current_time);
void RTC_BeginTimeEdit(void);
uint16_t RTC_GetEditedTimeField(rtc_time_field_t field);
void RTC_ChangeEditedTimeField(rtc_time_field_t field, bool increase);
HAL_StatusTypeDef RTC_ApplyEditedTime(void);

uint16_t RTC_TimeOfDayToMinutes(uint16_t hours, uint16_t minutes);
bool RTC_IsTimeRangeValid(uint16_t start_hours, uint16_t start_minutes,
                          uint16_t stop_hours, uint16_t stop_minutes);
bool RTC_IsCurrentTimeInRange(uint16_t start_hours, uint16_t start_minutes,
                              uint16_t stop_hours, uint16_t stop_minutes);
void RTC_FormatTimeOfDay(char *buffer, size_t buffer_size, uint16_t hours, uint16_t minutes);

HAL_StatusTypeDef RTC_ResetPersistentTime(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_RTC_H_ */
