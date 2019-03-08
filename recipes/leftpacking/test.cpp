#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include "scalar_leftpacking.h"
#include "sse_leftpacking.h"
#include "simdpp_leftpacking.h"
#include "neon_leftpacking.h"
#include "simdutil.h"


static bool equal(const uint8_t *a, const uint8_t *b, size_t N) {
  for (size_t i = 0; i < N; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

void demo(size_t N) {
  uint8_t *data = new uint8_t[N];
  for (size_t i = 0; i < N; i++) {
    data[i] = rand() % 10;
  }
  uint8_t *copydata = new uint8_t[N];
  uint8_t *expecteddata = new uint8_t[N];
  memcpy(expecteddata, data, sizeof(uint8_t) * N);
  size_t expected = scalar_deletezeros(expecteddata, N);
#ifdef USING_SSE
  memcpy(copydata, data, sizeof(uint8_t) * N);
  size_t sseval = sse_deletezeros(copydata, N);
  assert(sseval == expected);
  assert(equal(expecteddata,copydata,expected));
#endif
#ifdef USING_SIMDPP
  memcpy(copydata, data, sizeof(uint8_t) * N);
  size_t simdppval = simdpp_deletezeros(copydata, N);
  assert(simdppval == expected);
  assert(equal(expecteddata,copydata,expected));
#endif
#ifdef USING_NEON
  memcpy(copydata, data, sizeof(uint8_t) * N);
  size_t neonval = neon_deletezeros(copydata, N);
  assert(neonval == expected);
  assert(equal(expecteddata,copydata,expected));
#endif
  delete[] data;
  delete[] copydata;
  delete[] expecteddata;
}

int main() {
  tellme();
  for (size_t i = 1; i < 1024; i++)
    demo(i);
  printf("Code is good\n");
}
