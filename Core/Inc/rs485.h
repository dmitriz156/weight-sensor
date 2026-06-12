#ifndef INC_RS485_H_
#define INC_RS485_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "stm32f1xx_hal.h"

#define RS485_TX_FRAME_SIZE     4U
#define RS485_RX_FRAME_SIZE     4U
#define RS485_RX_IDLE_BUFFER_SIZE 16U
#define RS485_ADDRESS_BIT       0x0100U
#define RS485_TX_MIN_INTERVAL_MS 10U

#define RS485_COMMAND_NONE      0x00U
#define RS485_COMMAND_UP_1      0x01U
#define RS485_COMMAND_UP_2      0x02U
#define RS485_COMMAND_DOWN_1    0x04U
#define RS485_COMMAND_DOWN_2    0x08U
#define RS485_COMMAND_STOP_1    0x10U
#define RS485_COMMAND_STOP_2    0x20U

#define RS485_STATUS_TOP_1      0x01U
#define RS485_STATUS_TOP_2      0x02U
#define RS485_STATUS_BOTTOM_1   0x04U
#define RS485_STATUS_BOTTOM_2   0x08U
#define RS485_STATUS_STOP_1     0x10U
#define RS485_STATUS_STOP_2     0x20U
#define RS485_STATUS_POSITION_MASK  0x3FU


extern volatile uint16_t interval_counter;
extern volatile uint16_t min_send_interval;

void RS485_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef RS485_SendCommand(uint8_t command);
bool RS485_GetStatus(uint8_t *status, uint32_t *sequence);
void RS485_RemoteControlProcesing(void);
void RS485_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void RS485_UART_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size);
void RS485_UART_ErrorCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* INC_RS485_H_ */
