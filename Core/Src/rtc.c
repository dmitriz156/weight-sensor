#include "rtc.h"

#include <stddef.h>

#include "stm32f1xx_hal_rtc_ex.h"

#define RTC_BACKUP_MARKER_REGISTER       RTC_BKP_DR1
#define RTC_BACKUP_MARKER_INV_REGISTER   RTC_BKP_DR2
#define RTC_BACKUP_MARKER                0x703AU
#define RTC_BACKUP_MARKER_INV            ((uint16_t)(~RTC_BACKUP_MARKER))
#define RTC_REGISTER_TIMEOUT_MS          1000U

static RTC_HandleTypeDef *rtc_handle = NULL;
static uint32_t rtc_boot_counter = 0U;
static bool rtc_ready = false;

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

    if (!RTC_BackupMarkerIsValid()) {
        status = RTC_WriteCounter(0U);
        if (status != HAL_OK) {
            rtc_handle = NULL;
            return status;
        }
        RTC_WriteBackupMarker();
    }

    rtc_boot_counter = RTC_ReadCounter();
    rtc_ready = true;
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

HAL_StatusTypeDef RTC_ResetPersistentTime(void)
{
    if (!rtc_ready) {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = RTC_WriteCounter(0U);
    if (status == HAL_OK) {
        RTC_WriteBackupMarker();
        rtc_boot_counter = 0U;
    }

    return status;
}
