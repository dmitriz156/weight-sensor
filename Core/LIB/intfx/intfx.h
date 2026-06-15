#ifndef _INTFX_H
#define _INTFX_H

#define INTFX_VERSION_MAJOR 0
#define INTFX_VERSION_MINOR 9
#define INTFX_VERSION_BUILD 0
#define INTFX_VERSION       ((INTFX_VERSION_MAJOR << 16) | (INTFX_VERSION_MINOR << 8) | INTFX_VERSION_BUILD)

#include <stdint.h>
#include <string.h>  // for memcpy

#include "intfx_defs.h"
#include "intfx_algo.h"

#define INTFX_MERGE_UINT8_TO_UINT32(msb, b3, b2, b1)      (uint32_t)((msb << 24) | (b3 << 16) | (b2 << 8) | b1)
#define INTFX_MERGE_UINT8_TO_UINT24(msb, b2, b1)          INTFX_MERGE_UINT8_TO_UINT32(0, msb, b2, b1)
#define INTFX_MERGE_UINT8_TO_UINT16(msb, lsb)             (uint16_t)((msb << 8) | lsb)

#define INTFX_MERGE_UINT8_WRAP(_1, _2, _3, _4, NAME, ...) NAME
#define INTFX_MERGE_UINT8(...)     \
  INTFX_MERGE_UINT8_WRAP(          \
      __VA_ARGS__,                 \
      INTFX_MERGE_UINT8_TO_UINT32, \
      INTFX_MERGE_UINT8_TO_UINT24, \
      INTFX_MERGE_UINT8_TO_UINT16) \
  (__VA_ARGS__)

#ifndef INTFX_CHECKSUM_DEFAULT_ALGO
  #define INTFX_CHECKSUM_DEFAULT_ALGO INTFX_CHECKSUM_ALGO_CRC8
#endif

#define CRC8_MAX_PACKAGE_SIZE  15
#define CRC16_MAX_PACKAGE_SIZE 4095

typedef enum intfx_status {
  INTFX_OK,
  INTFX_E_NULL_PTR,
  INTFX_E_CHECKSUM_MISMATCH,
  INTFX_E_INVALID_PARAM
} intfx_status_t;

typedef enum intfx_checksum_algo {
  INTFX_CHECKSUM_ALGO_CRC8,
  INTFX_CHECKSUM_ALGO_CRC16
} intfx_checksum_algo_t;

PACKED_STRUCT_START

typedef struct packed {
  uint8_t address;  // Destination device address
  uint8_t size;     // Size of data payload
  uint8_t tag;      // Data tag
} intfx_pkg_hdr_t;

PACKED_STRUCT_END

#define intfx_package_init \
  (intfx_pkg_hdr_t)        \
  {                        \
    .address = 0,          \
    .size    = 0,          \
    .tag     = 0,          \
  }

typedef struct intfx_checksum_info {
  union {
    uint8_t b8_val;
    uint16_t b16_val;
    uint32_t b32_val;
  } value;                     // CRC value spread between the types

  uint8_t len;                 // Length of CRC value in bytes

  intfx_checksum_algo_t algo;  // Algorithm used for CRC calculation
} intfx_checksum_info_t;

#define intfx_checksum_info_init          \
  (intfx_checksum_info_t)                 \
  {                                       \
    .value = {0},                         \
    .len   = 0,                           \
    .algo  = INTFX_CHECKSUM_DEFAULT_ALGO, \
  }

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Serializes data into a destination buffer
 *
 * @param address The address of message receiver
 * @param tag The tag of the data being serialized
 * @param dst The destination buffer where the serialized data will be stored
 * @param src The source buffer containing the data to be serialized
 * @param size The size of the data to be serialized
 *
 * @return The length of the serialized data in bytes. If the destination buffer
 * or source buffer is NULL, or if the input size is zero, the function returns
 * zero.
 */
uint8_t intfx_serialize(const uint8_t address, const uint8_t tag, uint8_t *restrict dst, uint8_t *restrict src, const uint8_t size);

/**
 * @brief Deserialize package from the source buffer into the
 * destination structure
 *
 * @param package_info Pointer to the intfx_pkg_hdr_t structure
 * @param data_wrapper Pointer to the destination data wrapper buffer
 * @param src Pointer to the source buffer containing the serialized data
 *
 * @return Result of the deserialization process
 * @retval INTFX_OK: Deserialization was successful
 * @retval INTFX_E_CHECKSUM_MISMATCH: CRC check failed, indicating a data corruption
 * @retval INTFX_E_NULL_PTR: Either the package_info or data_wrapper or src pointer is NULL
 */
intfx_status_t intfx_deserialize(intfx_pkg_hdr_t *package_info, uint8_t *data_wrapper, uint8_t *restrict src);

/**
 * @brief Computes checksum for the given block of data
 * @param pc_block Pointer to the start of the data block
 * @param len Length of the data block in bytes
 * @return intfx_checksum_info_t structure instance
 */
intfx_checksum_info_t intfx_get_checksum(uint8_t *pc_block, const uint8_t len);

/**
 * @brief Composes a 9-bit value from an array of 8-bit values
 *
 * @param dst Pointer to the destination array of 16-bit values
 * @param src Pointer to the source array of 8-bit values
 * @param size Size of the source array
 * @param b9set_indx Index of the element in the source array to set the 9th bit
 *
 * @return INTFX_OK if successful, otherwise an error code
 */
intfx_status_t intfx_9bit_compose(uint16_t *dst, uint8_t *src, const uint8_t size, const uint8_t b9set_indx);

/**
 * @brief Decomposes a 9-bit value array into an 8-bit value array
 *
 * @param dst Pointer to the destination array of 8-bit values
 * @param src Pointer to the source array of 16-bit values
 * @param size Size of the array
 *
 * @return INTFX_OK if successful, otherwise an error code
 */
intfx_status_t intfx_9bit_decompose(uint8_t *dst, uint16_t *src, const uint8_t size);

/**
 * @brief Aligns a packet from a circular buffer to a linear buffer.
 *
 * @param circular_buffer Pointer to the circular buffer
 * @param linear_buffer Pointer to the linear buffer
 * @param start The starting index of the packet in the circular buffer
 * @param length The length of the packet
 * @param buffer_size The size of the circular buffer
 *
 * @return INTFX_OK if the alignment is successful, otherwise an error code.
 */
intfx_status_t intfx_align_package(const uint8_t *restrict circular_buffer, uint8_t *restrict linear_buffer, size_t start, size_t length, size_t buffer_size);

#ifdef __cplusplus
}
#endif
#endif /* _INTFX_H */
