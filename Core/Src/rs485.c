#include "rs485.h"

#include <stdbool.h>

#include "main.h"

#define RS485_FRAME_ADDRESS              2U
#define RS485_FRAME_TAG                  2U
#define RS485_COMMAND_CONFIRM_TIMEOUT_MS 25U
#define RS485_TEST_COMMAND_UP             RS485_COMMAND_UP_1
#define RS485_TEST_COMMAND_DOWN           RS485_COMMAND_DOWN_1

typedef enum {
    RS485_TEST_IDLE = 0,
    RS485_TEST_SEND_UP,
    RS485_TEST_WAIT_AFTER_UP,
    RS485_TEST_SEND_DOWN,
    RS485_TEST_WAIT_AFTER_DOWN
} rs485_test_state_t;

static UART_HandleTypeDef *rs485_uart = NULL;
static volatile bool rs485_tx_busy = false;
static volatile bool rs485_status_valid = false;
static volatile uint8_t rs485_status = 0U;
static volatile uint32_t rs485_status_sequence = 0U;
static volatile uint8_t rs485_command = RS485_COMMAND_NONE;
static volatile bool rs485_response_pending = false;
static volatile bool rs485_tx_was_completed = false;
static volatile uint32_t rs485_last_tx_counter = 0U;
static volatile uint8_t rs485_last_sent_command = RS485_COMMAND_NONE;
static volatile uint32_t rs485_command_tx_sequence = 0U;

static uint16_t tx_buffer[RS485_TX_FRAME_SIZE];
static uint16_t rx_buffer[RS485_RX_FRAME_SIZE];
static uint16_t rx_idle_buffer[RS485_RX_IDLE_BUFFER_SIZE];
static uint8_t rx_index = 0U;
uint8_t tx_complate_flag = 0;
uint8_t rx_complate_flag = 0;
volatile uint16_t interval_counter = 0;
volatile uint16_t min_send_interval = 0;

static void RS485_EnableReceiver(void)
{
    HAL_GPIO_WritePin(RE_DE_2_GPIO_Port, RE_DE_2_Pin, GPIO_PIN_RESET);
}

static void RS485_EnableTransmitter(void)
{
    HAL_GPIO_WritePin(RE_DE_2_GPIO_Port, RE_DE_2_Pin, GPIO_PIN_SET);
}

static uint8_t RS485_CalculateCrc(const uint16_t *frame)
{
    uint8_t crc = 0U;

    for (uint8_t byte_index = 0U; byte_index < 3U; byte_index++) {
        uint8_t data = (uint8_t)frame[byte_index];
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
        (void)HAL_UARTEx_ReceiveToIdle_IT(
                rs485_uart,
                (uint8_t *)rx_idle_buffer,
                RS485_RX_IDLE_BUFFER_SIZE);
    }
}

static bool RS485_ProcessReceivedWord(uint16_t received_word)
{
    if ((received_word & RS485_ADDRESS_BIT) != 0U) {
        rx_index = 0U;
        if ((uint8_t)received_word == RS485_FRAME_ADDRESS) {
            rx_buffer[rx_index++] = received_word;
        }
        return false;
    }

    if (rx_index == 0U) {
        return false;
    }

    rx_buffer[rx_index++] = received_word;
    if (rx_index < RS485_RX_FRAME_SIZE) {
        return false;
    }

    bool frame_valid =
            ((uint8_t)rx_buffer[1] == RS485_FRAME_TAG) &&
            (RS485_CalculateCrc(rx_buffer) == (uint8_t)rx_buffer[3]);

    if (frame_valid) {
        rs485_status = (uint8_t)rx_buffer[2];
        rs485_status_sequence++;
        rs485_status_valid = true;
    }

    rx_index = 0U;
    return frame_valid;
}

static HAL_StatusTypeDef RS485_Transmit(const uint16_t *data, uint16_t size)
{
    if ((rs485_uart == NULL) || (data == NULL) || (size == 0U) || rs485_tx_busy) {
        return HAL_BUSY;
    }

    uint32_t current_counter = one_sec_counter;
    if (rs485_tx_was_completed &&
        ((uint32_t)(current_counter - rs485_last_tx_counter) <
         RS485_TX_MIN_INTERVAL_MS)) {
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

static void RS485_TrySendPendingResponse(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (rs485_response_pending) {
        if (RS485_SendCommand(rs485_command) == HAL_OK) {
            rs485_response_pending = false;
        }
    }

    if (primask == 0U) {
        __enable_irq();
    }
}

void RS485_Init(UART_HandleTypeDef *huart)
{
    rs485_uart = huart;
    rs485_tx_busy = false;
    rs485_status_valid = false;
    rs485_status = 0U;
    rs485_status_sequence = 0U;
    rs485_command = RS485_COMMAND_NONE;
    rs485_response_pending = false;
    rs485_tx_was_completed = false;
    rs485_last_tx_counter = 0U;
    rs485_last_sent_command = RS485_COMMAND_NONE;
    rs485_command_tx_sequence = 0U;
    rx_index = 0U;
    RS485_EnableReceiver();
    RS485_StartReceive();
}

HAL_StatusTypeDef RS485_SendCommand(uint8_t command)
{
    if ((rs485_uart == NULL) || rs485_tx_busy) {
        return HAL_BUSY;
    }

    tx_buffer[0] = RS485_ADDRESS_BIT | RS485_FRAME_ADDRESS;
    tx_buffer[1] = RS485_FRAME_TAG;
    tx_buffer[2] = command;
    tx_buffer[3] = RS485_CalculateCrc(tx_buffer);

    HAL_StatusTypeDef status = RS485_Transmit(tx_buffer, RS485_TX_FRAME_SIZE);
    if (status == HAL_OK) {
        rs485_last_sent_command = command;
        rs485_command_tx_sequence++;
    }

    return status;
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

static void RS485_GetLastTransmission(uint8_t *command, uint32_t *sequence)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    *command = rs485_last_sent_command;
    *sequence = rs485_command_tx_sequence;

    if (primask == 0U) {
        __enable_irq();
    }
}

void RS485_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    RS485_EnableReceiver();
    rs485_tx_busy = false;
    rs485_last_tx_counter = one_sec_counter;
    rs485_tx_was_completed = true;
}

void RS485_UART_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    bool frame_received = false;

    if (size > RS485_RX_IDLE_BUFFER_SIZE) {
        size = RS485_RX_IDLE_BUFFER_SIZE;
    }

    for (uint16_t index = 0U; index < size; index++) {
        if (RS485_ProcessReceivedWord(rx_idle_buffer[index])) {
            frame_received = true;
        }
    }

    RS485_StartReceive();

    if (frame_received) {
        rs485_response_pending = true;
    }
}

void RS485_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if ((rs485_uart == NULL) || (huart != rs485_uart)) {
        return;
    }

    RS485_EnableReceiver();
    rs485_tx_busy = false;
    rs485_last_tx_counter = one_sec_counter;
    rs485_tx_was_completed = true;
    rx_index = 0U;
    RS485_StartReceive();
}

void RS485_RemoteControlProcesing (void)
{
    static rs485_test_state_t test_state = RS485_TEST_IDLE;
    // static uint8_t status_before_command = 0U;
    // static uint32_t last_status_sequence = 0U;
    // static uint32_t state_started_counter = 0U;
    // static uint32_t command_tx_sequence_reference = 0U;
    // static bool command_was_sent = false;

    bool time_is_allowed = RTC_IsCurrentTimeInRange(
            settings.test_start_hours,
            settings.test_start_minutes,
            settings.test_stop_hours,
            settings.test_stop_minutes);

    if ((settings.test_mode != TEST_MODE_ON) || !time_is_allowed) {
        RS485_SetCommand(RS485_COMMAND_NONE);
        test_state = RS485_TEST_IDLE;
        RS485_TrySendPendingResponse();
        return;
    }

    uint8_t current_status = 0U;
    uint32_t current_sequence = 0U;
    if (!RS485_GetStatus(&current_status, &current_sequence)) {
        RS485_SetCommand(RS485_COMMAND_NONE);
        RS485_TrySendPendingResponse();
        return;
    }

    uint32_t current_counter = one_sec_counter;
    uint8_t current_position = current_status & RS485_STATUS_POSITION_MASK;
    uint8_t last_sent_command = RS485_COMMAND_NONE;
    uint32_t command_tx_sequence = 0U;
    RS485_GetLastTransmission(&last_sent_command, &command_tx_sequence);

    switch (test_state) {
    case RS485_TEST_IDLE:
        rs485_command = RS485_TEST_COMMAND_UP;
        test_state = RS485_TEST_SEND_UP;
        min_send_interval = RS485_TX_MIN_INTERVAL_MS;
        break;

    case RS485_TEST_SEND_UP:
        if (!min_send_interval && !interval_counter) {
            rs485_command = RS485_COMMAND_UP_1;
        }

        if (command_was_sent && (current_sequence != last_status_sequence)) {
            last_status_sequence = current_sequence;
            if (current_position != status_before_command) {
                rs485_command = RS485_COMMAND_NONE;
                state_started_counter = current_counter;
                test_state = RS485_TEST_WAIT_AFTER_UP;
                break;
            }
        }

        if (command_was_sent &&
            ((uint32_t)(current_counter - state_started_counter) >=
             RS485_COMMAND_CONFIRM_TIMEOUT_MS)) {
            status_before_command = current_position;
            last_status_sequence = current_sequence;
            state_started_counter = current_counter;
            command_tx_sequence_reference = command_tx_sequence;
            command_was_sent = false;
            rs485_command = RS485_TEST_COMMAND_DOWN;
            test_state = RS485_TEST_SEND_DOWN;
        }
        break;

    case RS485_TEST_WAIT_AFTER_UP:
        if ((uint32_t)(current_counter - state_started_counter) >=
            ((uint32_t)settings.rs485_command_interval_s * 1000U)) {
            status_before_command = current_position;
            last_status_sequence = current_sequence;
            state_started_counter = current_counter;
            command_tx_sequence_reference = command_tx_sequence;
            command_was_sent = false;
            RS485_SetCommand(RS485_TEST_COMMAND_DOWN);
            test_state = RS485_TEST_SEND_DOWN;
        }
        break;

    case RS485_TEST_SEND_DOWN:
        if (!command_was_sent &&
            (command_tx_sequence != command_tx_sequence_reference) &&
            (last_sent_command == RS485_TEST_COMMAND_DOWN)) {
            command_was_sent = true;
            status_before_command = current_position;
            last_status_sequence = current_sequence;
            state_started_counter = current_counter;
        }

        if (command_was_sent && (current_sequence != last_status_sequence)) {
            last_status_sequence = current_sequence;
            if (current_position != status_before_command) {
                RS485_SetCommand(RS485_COMMAND_NONE);
                state_started_counter = current_counter;
                test_state = RS485_TEST_WAIT_AFTER_DOWN;
                break;
            }
        }

        if (command_was_sent &&
            ((uint32_t)(current_counter - state_started_counter) >=
             RS485_COMMAND_CONFIRM_TIMEOUT_MS)) {
            status_before_command = current_position;
            last_status_sequence = current_sequence;
            state_started_counter = current_counter;
            command_tx_sequence_reference = command_tx_sequence;
            command_was_sent = false;
            RS485_SetCommand(RS485_TEST_COMMAND_UP);
            test_state = RS485_TEST_SEND_UP;
        }
        break;

    case RS485_TEST_WAIT_AFTER_DOWN:
        if ((uint32_t)(current_counter - state_started_counter) >=
            ((uint32_t)settings.rs485_command_interval_s * 1000U)) {
            status_before_command = current_position;
            last_status_sequence = current_sequence;
            state_started_counter = current_counter;
            command_tx_sequence_reference = command_tx_sequence;
            command_was_sent = false;
            RS485_SetCommand(RS485_TEST_COMMAND_UP);
            test_state = RS485_TEST_SEND_UP;
        }
        break;

    default:
        RS485_SetCommand(RS485_COMMAND_NONE);
        test_state = RS485_TEST_IDLE;
        command_was_sent = false;
        break;
    }

    RS485_TrySendPendingResponse();
}
