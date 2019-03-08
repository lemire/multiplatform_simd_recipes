#pragma once
#include <cstdint>
#include <cstddef>

void scalar_prefixsum(uint32_t *data, size_t length) {
  if (length == 0)
    return;
  for (size_t i = 1; i < length; i++) {
    data[i] += data[i - 1];
  }
}

void scalar_prefixsum_leftover(uint32_t *data, size_t length,
                               uint32_t blocksize) {
  size_t lengthlo = length % blocksize;
  size_t startpoint = length - lengthlo;
  if (startpoint == 0)
    startpoint++;
  for (size_t i = startpoint; i < length; i++) {
    data[i] += data[i - 1];
  }
}
