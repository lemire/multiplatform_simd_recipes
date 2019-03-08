#pragma once
#include <cstdint>
#include <cstddef>


size_t scalar_deletezeros(uint8_t *data, size_t length) {
  size_t pos = 0;
  for (size_t i = 0; i < length; i++) {
    uint8_t val = data[i];
    data[pos] = val;
    pos += (val != 0);
  }
  return pos;
}
