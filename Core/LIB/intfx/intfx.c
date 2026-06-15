#include "intfx.h"

uint8_t intfx_serialize(const uint8_t address, const uint8_t tag, uint8_t *restrict dst, uint8_t *restrict src, const uint8_t size)
{
  uint8_t len = 0;

  if (dst != NULL && (src != NULL || size == 0)) {
    intfx_checksum_info_t checksum_info = intfx_checksum_info_init;

    *(intfx_pkg_hdr_t *)dst = (intfx_pkg_hdr_t){
        .address = address,
        .size    = size,
        .tag     = tag,
    };

    len = sizeof(intfx_pkg_hdr_t);

    if (size > 0) {
      memcpy((dst + len), src, size);
      len += size;
    }

    checksum_info = intfx_get_checksum(dst, len);

    memcpy(dst + len, &checksum_info.value, checksum_info.len);

    len += checksum_info.len;
  }

  return len;
}

intfx_status_t intfx_deserialize(intfx_pkg_hdr_t *package_info, uint8_t *data_wrapper, uint8_t *restrict src)
{
  intfx_status_t rslt = INTFX_OK;

  if (data_wrapper != NULL && src != NULL && package_info != NULL) {
    const intfx_pkg_hdr_t *package      = (intfx_pkg_hdr_t *)src;
    intfx_checksum_info_t checksum_info = intfx_checksum_info_init;

    const uint8_t len = sizeof(intfx_pkg_hdr_t) + package->size;
    uint32_t checksum = 0;

    checksum_info = intfx_get_checksum(src, len);

    memcpy(&checksum, (src + len), checksum_info.len);

    if (checksum == checksum_info.value.b32_val) {
      memcpy(package_info, package, sizeof(intfx_pkg_hdr_t));

      if (package->size > 0) {
        memcpy(data_wrapper, (src + sizeof(intfx_pkg_hdr_t)), package->size);
      }
    } else {
      rslt = INTFX_E_CHECKSUM_MISMATCH;
    }
  } else {
    rslt = INTFX_E_NULL_PTR;
  }

  return rslt;
}

intfx_checksum_info_t intfx_get_checksum(uint8_t *pc_block, const uint8_t len)
{
  intfx_checksum_info_t checksum_info = intfx_checksum_info_init;

  if (pc_block != NULL && len != 0) {
    if (len > CRC8_MAX_PACKAGE_SIZE) {
      checksum_info.algo = INTFX_CHECKSUM_ALGO_CRC16;
	  
    }

    switch (checksum_info.algo) {
      case INTFX_CHECKSUM_ALGO_CRC8:
        checksum_info.value.b8_val = crc8(pc_block, len);
        checksum_info.len          = sizeof(uint8_t);
        break;

      case INTFX_CHECKSUM_ALGO_CRC16:
        checksum_info.value.b16_val = crc16(pc_block, len);
        checksum_info.len           = sizeof(uint16_t);
        break;
    }
  }

  return checksum_info;
}

intfx_status_t intfx_9bit_compose(uint16_t *dst, uint8_t *src, const uint8_t size, const uint8_t b9set_indx)
{
  if (dst == NULL || src == NULL) {
    return INTFX_E_NULL_PTR;
  }

  if (b9set_indx >= size) {
    return INTFX_E_INVALID_PARAM;
  }

  for (size_t i = 0; i < size; i++) {
    if (i == b9set_indx) {
      dst[i] = (uint16_t)src[i] | 0x0100;
    } else {
      dst[i] = (uint16_t)src[i];
    }
  }

  return INTFX_OK;
}

intfx_status_t intfx_9bit_decompose(uint8_t *dst, uint16_t *src, const uint8_t size)
{
  if (dst == NULL || src == NULL) {
    return INTFX_E_NULL_PTR;
  }

  for (size_t i = 0; i < size; i++) {
      dst[i] = (uint8_t)(src[i] & 0xFF);
  }

  return INTFX_OK;
}

intfx_status_t intfx_align_package(const uint8_t *restrict circular_buffer, uint8_t *restrict linear_buffer, size_t start, size_t length, size_t buffer_size)
{
  if (circular_buffer == NULL || linear_buffer == NULL) {
    return INTFX_E_NULL_PTR;
  }

  if (length > buffer_size || start >= buffer_size) {
    return INTFX_E_INVALID_PARAM;
  }

  if (start + length <= buffer_size) {
    memcpy(linear_buffer, circular_buffer + start, length);
  } else {
    size_t first_part_size  = buffer_size - start;
    size_t second_part_size = length - first_part_size;

    memcpy(linear_buffer, circular_buffer + start, first_part_size);
    memcpy(linear_buffer + first_part_size, circular_buffer, second_part_size);
  }
  return INTFX_OK;
}
