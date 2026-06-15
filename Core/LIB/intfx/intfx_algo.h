#ifndef _INTFX_ALGO_H
#define _INTFX_ALGO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Computes CRC-8 for the given block of data
 * @param pc_block Pointer to the start of the data block
 * @param len Length of the data block in bytes
 * @return Computed CRC-8 value.
 *
 * @details:
 * - Poly  : 0x31 x^8 + x^5 + x^4 + 1
 * - Init  : 0xFF
 * - Revert: false
 * - XorOut: 0x00
 * - Check : 0xF7 ("123456789")
 * - MaxLen: 15 bytes (127 bits)
 */
uint8_t crc8(uint8_t *pc_block, uint8_t len);

/**
 * @brief Computes CRC-16 CCITT for the given block of data
 * @param pc_block Pointer to the start of the data block
 * @param len Length of the data block in bytes
 * @return Computed CRC-16 CCITT value.
 *
 * @details:
 * - Poly  : 0x1021 x^16 + x^12 + x^5 + 1
 * - Init  : 0xFFFF
 * - Revert: false
 * - XorOut: 0x0000
 * - Check : 0x29B1 ("123456789")
 * - MaxLen: 4095 bytes (32767 bits)
 */
uint16_t crc16(uint8_t *pc_block, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif /* _INTFX_ALGO_H */