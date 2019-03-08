#pragma once
#include <cstdint>
#include <cstddef>

void scalar_delta(uint32_t *data, size_t length) {
  if (length == 0)
    return;
  for (size_t i = length - 1; i >= 1; i--) {
    data[i] = data[i] - data[i - 1];
  }
}

// computes the deltas over the last length%blocksize elements
void scalar_delta_leftover(uint32_t *data, size_t length, uint32_t blocksize) {
  size_t lengthlo = length % blocksize;
  if (lengthlo == 0)
    return; // important
  scalar_delta(data + (length - lengthlo), lengthlo);
  if (length > lengthlo) {
    data[length - lengthlo] -= data[length - lengthlo - 1];
  }
}
