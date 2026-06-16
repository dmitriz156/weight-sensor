#include "main.h"
#include "rtc.h"
#include <stddef.h>
#include <stdio.h>

#include "stm32f1xx_hal_rtc_ex.h"

#define RTC_BACKUP_MARKER_REGISTER       RTC_BKP_DR1
#define RTC_BACKUP_MARKER_INV_REGISTER   RTC_BKP_DR2
#define RTC_OFFSET_LOW_REGISTER          RTC_BKP_DR3
#define RTC_OFFSET_HIGH_REGISTER         RTC_BKP_DR4
#define RTC_OFFSET_LOW_INV_REGISTER      RTC_BKP_DR5
#define RTC_OFFSET_HIGH_INV_REGISTER     RTC_BKP_DR6
#define RTC_BACKUP_MARKER                0x703AU
#define RTC_BACKUP_MARKER_INV            ((uint16_t)(~RTC_BACKUP_MARKER))
#define RTC_REGISTER_TIMEOUT_MS          1000U
#define RTC_CALENDAR_EPOCH_YEAR          2000U
#define RTC_CALENDAR_LAST_OFFSET_YEAR    2099U

static RTC_HandleTypeDef *rtc_handle = NULL;
static uint32_t rtc_boot_counter = 0U;
static bool rtc_ready = false;

static rtc_time_set_t rtc_current_time = {0U, 0U, 0U, 1U, 1U, RTC_CALENDAR_EPOCH_YEAR};
static rtc_time_set_t rtc_time_offset = {0U, 0U, 0U, 1U, 1U, RTC_CALENDAR_EPOCH_YEAR};
static rtc_time_set_t rtc_edit_time = {0U, 0U, 0U, 1U, 1U, RTC_CALENDAR_EPOCH_YEAR};

static void RTC_UpdateCurrentTime(void);

static bool RTC_IsLeapYear(uint16_t year)
{
    return ((year % 4U) == 0U) && (((year % 100U) != 0U) || ((year % 400U) == 0U));
}

static uint8_t RTC_DaysInMonth(uint16_t year, uint8_t month)
{
    static const uint8_t days_in_month[] = {
        31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U
    };

    if ((month == 0U) || (month > 12U)) {
        return 0U;
    }

    if ((month == 2U) && RTC_IsLeapYear(year)) {
        return 29U;
    }

    return days_in_month[month - 1U];
}

static bool RTC_TimeIsValid(const rtc_time_set_t *time)
{
    if ((time == NULL) ||
        (time->year < RTC_CALENDAR_EPOCH_YEAR) ||
        (time->year > RTC_CALENDAR_LAST_OFFSET_YEAR) ||
        (time->month == 0U) ||
        (time->month > 12U) ||
        (time->day == 0U) ||
        (time->day > RTC_DaysInMonth(time->year, time->month)) ||
        (time->hours > 23U) ||
        (time->minutes > 59U) ||
        (time->seconds > 59U)) {
        return false;
    }

    return true;
}

static void RTC_ClampDay(rtc_time_set_t *time)
{
    uint8_t max_day = RTC_DaysInMonth(time->year, time->month);

    if (time->day > max_day) {
        time->day = max_day;
    }
}

static uint16_t RTC_GetTimeField(const rtc_time_set_t *time, rtc_time_field_t field)
{
    switch (field) {
    case RTC_TIME_FIELD_HOURS:
        return time->hours;
    case RTC_TIME_FIELD_MINUTES:
        return time->minutes;
    case RTC_TIME_FIELD_SECONDS:
        return time->seconds;
    case RTC_TIME_FIELD_DAY:
        return time->day;
    case RTC_TIME_FIELD_MONTH:
        return time->month;
    case RTC_TIME_FIELD_YEAR:
        return time->year;
    default:
        return 0U;
    }
}

static uint32_t RTC_TimeToEpochSeconds(const rtc_time_set_t *time)
{
    uint32_t days = 0U;

    for (uint16_t year = RTC_CALENDAR_EPOCH_YEAR; year < time->year; year++) {
        days += RTC_IsLeapYear(year) ? 366U : 365U;
    }

    for (uint8_t month = 1U; month < time->month; month++) {
        days += RTC_DaysInMonth(time->year, month);
    }

    days += (uint32_t)time->day - 1U;

    return (days * RTC_SECONDS_PER_DAY) +
           ((uint32_t)time->hours * RTC_SECONDS_PER_HOUR) +
           ((uint32_t)time->minutes * RTC_SECONDS_PER_MINUTE) +
           time->seconds;
}

static void RTC_EpochSecondsToTime(uint64_t epoch_seconds, rtc_time_set_t *time)
{
    uint64_t days = epoch_seconds / RTC_SECONDS_PER_DAY;
    uint32_t day_seconds = (uint32_t)(epoch_seconds % RTC_SECONDS_PER_DAY);
    uint16_t year = RTC_CALENDAR_EPOCH_YEAR;
    uint8_t month = 1U;

    while (days >= (RTC_IsLeapYear(year) ? 366U : 365U)) {
        days -= RTC_IsLeapYear(year) ? 366U : 365U;
        year++;
    }

    while (days >= RTC_DaysInMonth(year, month)) {
        days -= RTC_DaysInMonth(year, month);
        month++;
    }

    time->year = year;
    time->month = month;
    time->day = (uint8_t)days + 1U;
    time->hours = (uint8_t)(day_seconds / RTC_SECONDS_PER_HOUR);
    day_seconds %= RTC_SECONDS_PER_HOUR;
    time->minutes = (uint8_t)(day_seconds / RTC_SECONDS_PER_MINUTE);
    time->seconds = (uint8_t)(day_seconds % RTC_SECONDS_PER_MINUTE);
}

static void RTC_WriteOffsetSeconds(uint32_t offset_seconds)
{
    uint16_t low = (uint16_t)(offset_seconds & 0xFFFFU);
    uint16_t high = (uint16_t)(offset_seconds >> 16U);

    HAL_RTCEx_BKUPWrite(rtc_handle, RTC_OFFSET_LOW_REGISTER, low);
    HAL_RTCEx_BKUPWrite(rtc_handle, RTC_OFFSET_HIGH_REGISTER, high);
    HAL_RTCEx_BKUPWrite(rtc_handle, RTC_OFFSET_LOW_INV_REGISTER, (uint16_t)(~low));
    HAL_RTCEx_BKUPWrite(rtc_handle, RTC_OFFSET_HIGH_INV_REGISTER, (uint16_t)(~high));
}

static bool RTC_ReadOffsetSeconds(uint32_t *offset_seconds)
{
    uint16_t low = (uint16_t)HAL_RTCEx_BKUPRead(rtc_handle, RTC_OFFSET_LOW_REGISTER);
    uint16_t high = (uint16_t)HAL_RTCEx_BKUPRead(rtc_handle, RTC_OFFSET_HIGH_REGISTER);
    uint16_t low_inv = (uint16_t)HAL_RTCEx_BKUPRead(rtc_handle, RTC_OFFSET_LOW_INV_REGISTER);
    uint16_t high_inv = (uint16_t)HAL_RTCEx_BKUPRead(rtc_handle, RTC_OFFSET_HIGH_INV_REGISTER);

    if ((low_inv != (uint16_t)(~low)) || (high_inv != (uint16_t)(~high))) {
        return false;
    }

    *offset_seconds = ((uint32_t)high << 16U) | low;
    return true;
}

static uint32_t RTC_ReadCounter(void)
{
    uint16_t high_before;
    uint16_t high_after;
    uint16_t low;

    do {
        high_before = (uint16_t)(rtc_handle->Instance->CNTH & RTC_CNTH_RTC_CNT);
        low = (uint16_t)(rtc_handle->Instance->CNTL & RTC_CNTL_RTC_CNT);
        high_after = (uint16_t)(rtc_handle->Instance->CNTH & RTC_CNTH_RTC_CNT);
    } while (high_before != high_after);

    return ((uint32_t)high_after << 16U) | low;
}

static HAL_StatusTypeDef RTC_WaitForRegisterWrite(void)
{
    uint32_t start_tick = HAL_GetTick();

    while ((rtc_handle->Instance->CRL & RTC_CRL_RTOFF) == 0U) {
        if ((uint32_t)(HAL_GetTick() - start_tick) > RTC_REGISTER_TIMEOUT_MS) {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

static HAL_StatusTypeDef RTC_WriteCounter(uint32_t counter)
{
    HAL_StatusTypeDef status = RTC_WaitForRegisterWrite();
    if (status != HAL_OK) {
        return status;
    }

    __HAL_RTC_WRITEPROTECTION_DISABLE(rtc_handle);
    WRITE_REG(rtc_handle->Instance->CNTH, counter >> 16U);
    WRITE_REG(rtc_handle->Instance->CNTL, counter & RTC_CNTL_RTC_CNT);
    __HAL_RTC_WRITEPROTECTION_ENABLE(rtc_handle);

    return RTC_WaitForRegisterWrite();
}

static bool RTC_BackupMarkerIsValid(void)
{
    return (HAL_RTCEx_BKUPRead(rtc_handle, RTC_BACKUP_MARKER_REGISTER) == RTC_BACKUP_MARKER) &&
           (HAL_RTCEx_BKUPRead(rtc_handle, RTC_BACKUP_MARKER_INV_REGISTER) == RTC_BACKUP_MARKER_INV);
}

static void RTC_WriteBackupMarker(void)
{
    HAL_RTCEx_BKUPWrite(rtc_handle, RTC_BACKUP_MARKER_REGISTER, RTC_BACKUP_MARKER);
    HAL_RTCEx_BKUPWrite(rtc_handle, RTC_BACKUP_MARKER_INV_REGISTER, RTC_BACKUP_MARKER_INV);
}

static void RTC_SecondsToElapsed(uint32_t total_seconds, rtc_elapsed_time_t *elapsed)
{
    if (elapsed == NULL) {
        return;
    }

    elapsed->total_seconds = total_seconds;
    elapsed->days = total_seconds / RTC_SECONDS_PER_DAY;
    total_seconds %= RTC_SECONDS_PER_DAY;
    elapsed->hours = (uint8_t)(total_seconds / RTC_SECONDS_PER_HOUR);
    total_seconds %= RTC_SECONDS_PER_HOUR;
    elapsed->minutes = (uint8_t)(total_seconds / RTC_SECONDS_PER_MINUTE);
    elapsed->seconds = (uint8_t)(total_seconds % RTC_SECONDS_PER_MINUTE);
}

HAL_StatusTypeDef RTC_Module_Init(RTC_HandleTypeDef *hrtc)
{
    if (hrtc == NULL) {
        return HAL_ERROR;
    }

    rtc_handle = hrtc;
    rtc_ready = false;

    HAL_StatusTypeDef status = HAL_RTC_Init(rtc_handle);
    if (status != HAL_OK) {
        rtc_handle = NULL;
        return status;
    }

    bool first_initialization = !RTC_BackupMarkerIsValid();
    if (first_initialization) {
        status = RTC_WriteCounter(0U);
        if (status != HAL_OK) {
            rtc_handle = NULL;
            return status;
        }
        RTC_WriteBackupMarker();
    }

    uint32_t offset_seconds = 0U;
    if (first_initialization || !RTC_ReadOffsetSeconds(&offset_seconds)) {
        offset_seconds = RTC_TimeToEpochSeconds(&rtc_time_offset);
        RTC_WriteOffsetSeconds(offset_seconds);
    } else {
        RTC_EpochSecondsToTime(offset_seconds, &rtc_time_offset);
    }

    rtc_boot_counter = RTC_ReadCounter();
    rtc_ready = true;
    RTC_UpdateCurrentTime();
    return HAL_OK;
}

bool RTC_Module_IsReady(void)
{
    return rtc_ready;
}

uint32_t RTC_GetPersistentSeconds(void)
{
    if (!rtc_ready) {
        return 0U;
    }

    return RTC_ReadCounter();
}

uint32_t RTC_GetUptimeSeconds(void)
{
    if (!rtc_ready) {
        return 0U;
    }

    return RTC_ReadCounter() - rtc_boot_counter;
}

void RTC_GetPersistentElapsed(rtc_elapsed_time_t *elapsed)
{
    RTC_SecondsToElapsed(RTC_GetPersistentSeconds(), elapsed);
}

void RTC_GetUptime(rtc_elapsed_time_t *elapsed)
{
    RTC_SecondsToElapsed(RTC_GetUptimeSeconds(), elapsed);
}

static void RTC_UpdateCurrentTime(void)
{
    if (!rtc_ready) {
        return;
    }

    uint64_t offset_seconds = RTC_TimeToEpochSeconds(&rtc_time_offset);
    RTC_EpochSecondsToTime(offset_seconds + RTC_ReadCounter(), &rtc_current_time);
}

void RTC_Process(void)
{
    static uint32_t previous_rtc_second = UINT32_MAX;
    uint32_t current_rtc_second = RTC_GetPersistentSeconds();

    if (current_rtc_second != previous_rtc_second) {
        previous_rtc_second = current_rtc_second;
        RTC_UpdateCurrentTime();
    }
}

uint16_t RTC_GetCurrentTimeField(rtc_time_field_t field)
{
    return RTC_GetTimeField(&rtc_current_time, field);
}

HAL_StatusTypeDef RTC_SetCurrentTime(const rtc_time_set_t *current_time)
{
    if (!rtc_ready || !RTC_TimeIsValid(current_time)) {
        return HAL_ERROR;
    }

    uint32_t counter_seconds = RTC_ReadCounter();
    uint32_t current_seconds = RTC_TimeToEpochSeconds(current_time);

    if (current_seconds < counter_seconds) {
        HAL_StatusTypeDef status = RTC_WriteCounter(0U);
        if (status != HAL_OK) {
            return status;
        }

        RTC_WriteBackupMarker();
        rtc_boot_counter = 0U;
        counter_seconds = 0U;
    }

    uint32_t offset_seconds = current_seconds - counter_seconds;
    RTC_EpochSecondsToTime(offset_seconds, &rtc_time_offset);
    RTC_WriteOffsetSeconds(offset_seconds);
    RTC_UpdateCurrentTime();
    return HAL_OK;
}

void RTC_BeginTimeEdit(void)
{
    RTC_UpdateCurrentTime();
    rtc_edit_time = rtc_current_time;
    RTC_ClampDay(&rtc_edit_time);
}

uint16_t RTC_GetEditedTimeField(rtc_time_field_t field)
{
    return RTC_GetTimeField(&rtc_edit_time, field);
}

void RTC_ChangeEditedTimeField(rtc_time_field_t field, bool increase)
{
    switch (field) {
    case RTC_TIME_FIELD_MINUTES:
        if (increase && rtc_edit_time.minutes < 59U) {
            rtc_edit_time.minutes++;
        } else if (!increase && rtc_edit_time.minutes > 0U) {
            rtc_edit_time.minutes--;
        }
        break;
    case RTC_TIME_FIELD_HOURS:
        if (increase && rtc_edit_time.hours < 23U) {
            rtc_edit_time.hours++;
        } else if (!increase && rtc_edit_time.hours > 0U) {
            rtc_edit_time.hours--;
        }
        break;
    case RTC_TIME_FIELD_DAY:
        if (increase && rtc_edit_time.day < RTC_DaysInMonth(rtc_edit_time.year, rtc_edit_time.month)) {
            rtc_edit_time.day++;
        } else if (!increase && rtc_edit_time.day > 1U) {
            rtc_edit_time.day--;
        }
        break;
    case RTC_TIME_FIELD_MONTH:
        if (increase && rtc_edit_time.month < 12U) {
            rtc_edit_time.month++;
        } else if (!increase && rtc_edit_time.month > 1U) {
            rtc_edit_time.month--;
        }
        RTC_ClampDay(&rtc_edit_time);
        break;
    case RTC_TIME_FIELD_YEAR:
        if (increase && rtc_edit_time.year < RTC_CALENDAR_LAST_OFFSET_YEAR) {
            rtc_edit_time.year++;
        } else if (!increase && rtc_edit_time.year > RTC_CALENDAR_EPOCH_YEAR) {
            rtc_edit_time.year--;
        }
        RTC_ClampDay(&rtc_edit_time);
        break;
    default:
        break;
    }
}

HAL_StatusTypeDef RTC_ApplyEditedTime(void)
{
    return RTC_SetCurrentTime(&rtc_edit_time);
}

uint16_t RTC_TimeOfDayToMinutes(uint16_t hours, uint16_t minutes)
{
    return (uint16_t)((hours * RTC_SECONDS_PER_MINUTE) + minutes);
}

bool RTC_IsTimeRangeValid(save_flash_t *sett)
{
    if ((sett->test_start_hours > 23U) || (sett->test_stop_hours > 23U) || (sett->test_start_minutes > 59U) || (sett->test_stop_minutes > 59U)) {
        return false;
    }
    if ( RTC_TimeOfDayToMinutes(sett->test_stop_hours, sett->test_stop_minutes) <= RTC_TimeOfDayToMinutes(sett->test_start_hours, sett->test_start_minutes)) {
        return false;
    }
    return true;
}

bool RTC_IsCurrentTimeInRange(save_flash_t *sett)
{
    if (!RTC_IsTimeRangeValid(sett)) {
        return false;
    }

    uint16_t current_minutes = RTC_TimeOfDayToMinutes(rtc_current_time.hours, rtc_current_time.minutes);
    uint16_t start = RTC_TimeOfDayToMinutes(sett->test_start_hours, sett->test_start_minutes);
    uint16_t stop = RTC_TimeOfDayToMinutes(sett->test_stop_hours, sett->test_stop_minutes);

    return current_minutes >= start && current_minutes <= stop;
}

void RTC_FormatTimeOfDay(char *buffer, size_t buffer_size, uint16_t hours, uint16_t minutes)
{
    if ((buffer == NULL) || (buffer_size == 0U)) {
        return;
    }

    snprintf(buffer, buffer_size, "%02u:%02u", (unsigned int)hours, (unsigned int)minutes);
}

HAL_StatusTypeDef RTC_ResetPersistentTime(void)
{
    if (!rtc_ready) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = RTC_WriteCounter(0U);
    if (status == HAL_OK) {
        RTC_WriteBackupMarker();
        rtc_boot_counter = 0U;
        RTC_UpdateCurrentTime();
    }

    return status;
}
