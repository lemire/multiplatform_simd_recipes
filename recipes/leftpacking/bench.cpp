#include "benchmark.h"
#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>

#include "scalar_leftpacking.h"
#include "sse_leftpacking.h"
#include "simdpp_leftpacking.h"
#include "neon_leftpacking.h"
#include "simdutil.h"



void demo(size_t N) {
  uint8_t *data = new uint8_t[N];
  for (size_t i = 0; i < N; i++) {
    data[i] = rand() % 10;
  }
  uint8_t *copydata = new uint8_t[N];

  int size = N;
  bool verbose = true;
  int repeat = 50;

  BEST_TIME_NOCHECK(scalar_deletezeros(copydata, N),
                    memcpy(copydata, data, sizeof(uint8_t) * N), repeat,
                    size, verbose);
#ifdef USING_SSE
  BEST_TIME_NOCHECK(sse_deletezeros(copydata, N),
                    memcpy(copydata, data, sizeof(uint8_t) * N), repeat,
                    size, verbose);
#endif
#ifdef USING_SIMDPP
  BEST_TIME_NOCHECK(simdpp_deletezeros(copydata, N),
                    memcpy(copydata, data, sizeof(uint8_t) * N), repeat,
                    size, verbose);
#endif
#ifdef USING_NEON
  BEST_TIME_NOCHECK(neon_deletezeros(copydata, N),
                    memcpy(copydata, data, sizeof(uint8_t) * N), repeat,
                    size, verbose);
#endif
  delete[] data;
  delete[] copydata;
}

int main() {
  tellme();
  demo(50000);
  printf("Code is good\n");
}
