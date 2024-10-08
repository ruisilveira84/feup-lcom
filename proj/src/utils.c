#include <lcom/lcf.h>

#include <stdint.h>

#include "utils.h"

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL) return 1;
  *lsb = (uint8_t)val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL) return 1;
  val = val >> 8;
  *msb = (uint8_t)val;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if(value == NULL) return 1;
  uint32_t out;
  if(sys_inb(port, &out)) return 1;
  *value = (uint8_t) out;
  return 0;
}
