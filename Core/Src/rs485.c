#include "rs485.h"

#include <stdbool.h>

#include "main.h"

static UART_HandleTypeDef *rs485_uart = NULL;
static volatile bool rs485_tx_busy = false;
static uint32_t rs485_last_tx_tick = 0U;

static uint16_t rs485_tx_frame[RS485_TX_FRAME_SIZE] = {
    RS485_ADDRESS_BIT | (uint16_t)'1',
    (uint16_t)'2',
    (uint16_t)'3',
    (uint16_t)'4',
    (uint16_t)'5'
};

static void RS485_EnableReceiver(void)
{
    HAL_GPIO_WritePin(RE_DE_2_GPIO_Port, RE_DE_2_Pin, GPIO_PIN_RESET);
}

static void RS485_EnableTransmitter(void)
{
    HAL_GPIO_WritePin(RE_DE_2_GPIO_Port, RE_DE_2_Pin, GPIO_PIN_SET);
}

static HAL_StatusTypeDef RS485_Transmit(const uint16_t *data, uint16_t size)
{
    if ((rs485_uart == NULL) || (data == NULL) || (size == 0U) || rs485_tx_busy) {
        return HAL_BUSY;
    }

    rs485_tx_busy = true;
    RS485_EnableTransmitter();

    HAL_StatusTypeDef status = HAL_UART_Transmit_IT(rs485_uart, (const uint8_t *)data, size);
    if (status != HAL_OK) {
        rs485_tx_busy = false;
        RS485_EnableReceiver();
    }

    return status;
}

void RS485_Init(UART_HandleTypeDef *huart)
{
    rs485_uart = huart;
    rs485_tx_busy = false;
    rs485_last_tx_tick = HAL_GetTick();
    RS485_EnableReceiver();
}

void RS485_Process(void)
{
    if ((rs485_uart == NULL) || rs485_tx_busy) {
        return;
    }

    uint32_t current_tick = HAL_GetTick();
    if ((uint32_t)(current_tick - rs485_last_tx_tick) < RS485_TX_INTERVAL_MS) {
        return;
    }

    if (RS485_Transmit(rs485_tx_frame, RS485_TX_FRAME_SIZE) == HAL_OK) {
        rs485_last_tx_tick = current_tick;
    }
}

void RS485_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    RS485_EnableReceiver();
    rs485_tx_busy = false;
}

void RS485_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    RS485_EnableReceiver();
    rs485_tx_busy = false;
}
