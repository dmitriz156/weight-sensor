#include "intfx_algo.h"

uint8_t crc8(uint8_t *pc_block, uint8_t len)
{
  uint8_t crc = 0xFF;
  uint8_t i;

  while (len--) {
    crc ^= *pc_block++;

    for (i = 0; i < 8; i++) {
      crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
  }

  return crc;
}

uint16_t crc16(uint8_t *pc_block, uint16_t len)
{
  uint16_t crc = 0xFFFF;
  uint8_t i;

  while (len--) {
    crc ^= *pc_block++ << 8;

    for (i = 0; i < 8; i++) {
      crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
  }
  return crc;
}
