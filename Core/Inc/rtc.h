#ifndef INC_RTC_H_
#define INC_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
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

HAL_StatusTypeDef RTC_Module_Init(RTC_HandleTypeDef *hrtc);
bool RTC_Module_IsReady(void);

uint32_t RTC_GetPersistentSeconds(void);
uint32_t RTC_GetUptimeSeconds(void);

void RTC_GetPersistentElapsed(rtc_elapsed_time_t *elapsed);
void RTC_GetUptime(rtc_elapsed_time_t *elapsed);

HAL_StatusTypeDef RTC_ResetPersistentTime(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_RTC_H_ */
