#ifndef INC_RS485_H_
#define INC_RS485_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define RS485_TX_INTERVAL_MS    500U
#define RS485_TX_FRAME_SIZE     5U
#define RS485_ADDRESS_BIT       0x0100U

void RS485_Init(UART_HandleTypeDef *huart);
void RS485_Process(void);
void RS485_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void RS485_UART_ErrorCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* INC_RS485_H_ */
