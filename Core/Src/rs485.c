#include "rs485.h"

#include <string.h>

#include "main.h"

typedef enum {
    RS485_TEST_IDLE = 0,
    RS485_TEST_SEND_UP,
    RS485_TEST_WAIT_AFTER_UP,
    RS485_TEST_SEND_DOWN,
    RS485_TEST_WAIT_AFTER_DOWN
} rs485_test_state_t;

static UART_HandleTypeDef *rs485_uart = NULL;

static volatile bool rs485_tx_busy = false;
static volatile bool rs485_rx_complete = false;
static volatile bool rs485_response_pending = false;
static volatile uint16_t rs485_received_size = 0U;
static volatile uint32_t interval_counter = 5000U;
static volatile uint16_t min_send_interval = RS485_TX_MIN_INTERVAL_MS;
volatile uint8_t tx_command_cnt = 0;
volatile uint8_t tx_command_btn_cnt = 0;

static uint8_t rs485_command = RS485_COMMAND_NONE;
volatile uint8_t rs485_command_btn = RS485_COMMAND_NONE;
static uint8_t rs485_status = 0U;
static uint32_t rs485_status_sequence = 0U;
static bool rs485_status_valid = false;

static uint8_t tx_frame[RS485_TX_FRAME_SIZE];
static uint16_t tx_frame_9bit[RS485_TX_FRAME_SIZE];

static uint16_t rx_idle_buffer_9bit[RS485_RX_IDLE_BUFFER_SIZE];
static uint8_t rx_idle_buffer[RS485_RX_IDLE_BUFFER_SIZE];
static uint8_t rx_processing_buffer[RS485_RX_IDLE_BUFFER_SIZE];
static bool rx_processing_address_bit = false;

static void RS485_EnableReceiver(void)
{
    HAL_GPIO_WritePin(RE_DE_2_GPIO_Port, RE_DE_2_Pin, GPIO_PIN_RESET);
}

static void RS485_EnableTransmitter(void)
{
    HAL_GPIO_WritePin(RE_DE_2_GPIO_Port, RE_DE_2_Pin, GPIO_PIN_SET);
}

static uint8_t RS485_CalculateCrc(const uint8_t *frame)
{
    uint8_t crc = 0U;

    for (uint8_t byte_index = 0U; byte_index < 3U; byte_index++) {
        uint8_t data = frame[byte_index];

        for (uint8_t bit_index = 0U; bit_index < 8U; bit_index++) {
            uint8_t mix = (uint8_t)((data ^ crc) & 0x01U);
            crc >>= 1U;
            if (mix != 0U) {
                crc ^= 0x8CU;
            }
            data >>= 1U;
        }
    }

    return crc;
}

static void RS485_StartReceive(void)
{
    if ((rs485_uart != NULL) && (rs485_uart->RxState == HAL_UART_STATE_READY)) {
        (void)HAL_UARTEx_ReceiveToIdle_IT(rs485_uart, (uint8_t *)rx_idle_buffer_9bit, RS485_RX_IDLE_BUFFER_SIZE);
    }
}

static bool RS485_ProcessReceivedFrame(void)
{
    uint8_t frame[RS485_RX_FRAME_SIZE];
    uint16_t size;
    bool address_bit;

    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (!rs485_rx_complete) {
        if (primask == 0U) {
            __enable_irq();
        }
        return false;
    }

    size = rs485_received_size;
    address_bit = rx_processing_address_bit;
    memcpy(frame, rx_processing_buffer, sizeof(frame));
    rs485_rx_complete = false;

    if (primask == 0U) {
        __enable_irq();
    }

    if ((size < RS485_RX_FRAME_SIZE) || !address_bit || (frame[0] != RS485_FRAME_ADDRESS) /*|| (RS485_CalculateCrc(frame) != frame[3])*/) {
        return false;
    }

    rs485_status = frame[2];
    rs485_status_sequence++;
    rs485_status_valid = true;
    rs485_response_pending = true;
    return true;
}

HAL_StatusTypeDef RS485_SendCommand(uint8_t command)
{
    if ((rs485_uart == NULL) || rs485_tx_busy) {
        return HAL_BUSY;
    }

    tx_frame[0] = RS485_FRAME_ADDRESS;
    tx_frame[1] = RS485_FRAME_TAG;
    tx_frame[2] = command;
    tx_frame[3] = RS485_CalculateCrc(tx_frame);

    if (intfx_9bit_compose(tx_frame_9bit, tx_frame, RS485_TX_FRAME_SIZE, 0U) != INTFX_OK) {
        return HAL_ERROR;
    }

    rs485_tx_busy = true;
    RS485_EnableTransmitter();

    HAL_StatusTypeDef status = HAL_UART_Transmit_IT(rs485_uart, (const uint8_t *)tx_frame_9bit, RS485_TX_FRAME_SIZE);

    if (status != HAL_OK) {
        rs485_tx_busy = false;
        RS485_EnableReceiver();
    }
    return status;
}

static void RS485_SendCommandBroker(void)
{
    static uint8_t active_button_command = RS485_COMMAND_NONE;
    uint8_t command;
    bool button_command_pending;

    if (rs485_tx_busy || (!rs485_response_pending && (min_send_interval != 0U))) {
        return;
    }

    button_command_pending = rs485_command_btn != RS485_COMMAND_NONE;

    if (button_command_pending) {
        if (active_button_command != rs485_command_btn) {
            active_button_command = rs485_command_btn;
            tx_command_btn_cnt = 0U;
        }
        command = rs485_command_btn;
    } else {
        active_button_command = RS485_COMMAND_NONE;

        if (rs485_command == RS485_COMMAND_NONE) {
            return;
        }
        command = rs485_command;
    }

    if (RS485_SendCommand(command) == HAL_OK) {
        if (button_command_pending) {
            tx_command_btn_cnt++;
            if ((tx_command_btn_cnt >= RS485_TX_FRAME_MAX_NUM) && (rs485_command_btn == command)) {
                rs485_command_btn = RS485_COMMAND_NONE;
                active_button_command = RS485_COMMAND_NONE;
                tx_command_btn_cnt = 0U;
            }
        } else {
            tx_command_cnt++;
        }
        rs485_response_pending = false;
    }
}

void RS485_Init(UART_HandleTypeDef *huart)
{
    rs485_uart = huart;
    rs485_tx_busy = false;
    rs485_rx_complete = false;
    rs485_response_pending = false;
    rs485_received_size = 0U;
    interval_counter = 5000U;
    min_send_interval = RS485_TX_MIN_INTERVAL_MS;
    tx_command_cnt = 0U;
    tx_command_btn_cnt = 0U;
    rs485_command = RS485_COMMAND_NONE;
    rs485_command_btn = RS485_COMMAND_NONE;
    rs485_status = 0U;
    rs485_status_sequence = 0U;
    rs485_status_valid = false;

    RS485_EnableReceiver();
    RS485_StartReceive();
}

bool RS485_GetStatus(uint8_t *status, uint32_t *sequence)
{
    if ((status == NULL) || (sequence == NULL)) {
        return false;
    }

    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    bool valid = rs485_status_valid;
    *status = rs485_status;
    *sequence = rs485_status_sequence;

    if (primask == 0U) {
        __enable_irq();
    }

    return valid;
}

void RS485_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    RS485_EnableReceiver();
    rs485_tx_busy = false;
    min_send_interval = RS485_TX_MIN_INTERVAL_MS;
}

void RS485_UART_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    if (size > RS485_RX_IDLE_BUFFER_SIZE) {
        size = RS485_RX_IDLE_BUFFER_SIZE;
    }

    if (size >= RS485_RX_FRAME_SIZE) {
        if (intfx_9bit_decompose(rx_idle_buffer, rx_idle_buffer_9bit, (uint8_t)size) == INTFX_OK) {
            memcpy(rx_processing_buffer, rx_idle_buffer, size);
            rx_processing_address_bit = (rx_idle_buffer_9bit[0] & RS485_ADDRESS_BIT) != 0U;
            rs485_received_size = size;
            rs485_rx_complete = true;
        }
    }

    __HAL_UART_CLEAR_OREFLAG(huart);
    RS485_StartReceive();
}

void RS485_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    RS485_EnableReceiver();
    rs485_tx_busy = false;
    rs485_rx_complete = false;
    min_send_interval = RS485_TX_MIN_INTERVAL_MS;

    (void)HAL_UART_AbortReceive(huart);
    __HAL_UART_CLEAR_OREFLAG(huart);
    RS485_StartReceive();
}

void RS485_Timer1msCallback(void)
{
    if ((interval_counter > 0U) && (rs485_command_btn == RS485_COMMAND_NONE)) {
        interval_counter--;
    }

    if (min_send_interval > 0U) {
        min_send_interval--;
    }
}

void RS485_RemoteControlProcesing(void)
{
    static rs485_test_state_t test_state = RS485_TEST_IDLE;

    (void)RS485_ProcessReceivedFrame();

    bool time_is_allowed = RTC_IsCurrentTimeInRange(&settings);

    if ((settings.test_mode != TEST_MODE_ON) || time_is_allowed == false || rb_btn.TEST_ON_flag == false) {
        rs485_command = RS485_COMMAND_NONE;
        interval_counter = (RS485_INTERVAL_MIN_S * 1000);
        tx_command_cnt = 0U;
        test_state = RS485_TEST_IDLE;
        RS485_SendCommandBroker();
        return;
    }

    if (rs485_command_btn != RS485_COMMAND_NONE) {
        RS485_SendCommandBroker();
        return;
    }

    switch (test_state) {
    case RS485_TEST_IDLE:
        tx_command_cnt = 0U;
        rs485_command = RS485_COMMAND_UP;
        test_state = RS485_TEST_SEND_UP;
        break;

    case RS485_TEST_SEND_UP:
        rs485_command = RS485_COMMAND_UP;
        if (tx_command_cnt >= RS485_TX_FRAME_MAX_NUM) {
            tx_command_cnt = 0U;
            rs485_command = RS485_COMMAND_NONE;
            interval_counter = (uint32_t)settings.rs485_command_interval_s * 1000U;
            test_state = RS485_TEST_WAIT_AFTER_UP;
        }
        break;

    case RS485_TEST_WAIT_AFTER_UP:
        rs485_command = RS485_COMMAND_NONE;
        if (interval_counter == 0U) {
            tx_command_cnt = 0U;
            rs485_command = RS485_COMMAND_DOWN;
            test_state = RS485_TEST_SEND_DOWN;
        }
        break;

    case RS485_TEST_SEND_DOWN:
        rs485_command = RS485_COMMAND_DOWN;
        if (tx_command_cnt >= RS485_TX_FRAME_MAX_NUM) {
            tx_command_cnt = 0U;
            rs485_command = RS485_COMMAND_NONE;
            interval_counter = (uint32_t)settings.rs485_command_interval_s * 1000U;
            test_state = RS485_TEST_WAIT_AFTER_DOWN;
        }
        break;

    case RS485_TEST_WAIT_AFTER_DOWN:
        rs485_command = RS485_COMMAND_NONE;
        if (interval_counter == 0U) {
            tx_command_cnt = 0U;
            rs485_command = RS485_COMMAND_UP;
            test_state = RS485_TEST_SEND_UP;
        }
        break;

    default:
        rs485_command = RS485_COMMAND_NONE;
        interval_counter = 0U;
        tx_command_cnt = 0U;
        test_state = RS485_TEST_IDLE;
        break;
    }

    RS485_SendCommandBroker();
}
