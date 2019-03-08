#include "benchmark.h"
#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>

#include "scalar_delta.h"
#include "sse_delta.h"
#include "simdpp_delta.h"
#include "dimsum_delta.h"
#include "neon_delta.h"
#include "simdutil.h"

static void summe(const uint32_t *deltas, uint32_t *sums, size_t N) {
  sums[0] = deltas[0];
  for (size_t i = 1; i < N; i++) {
    sums[i] = deltas[i] + sums[i - 1];
  }
}

static bool equal(const uint32_t *a, const uint32_t *b, size_t N) {
  for (size_t i = 0; i < N; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

void demo(size_t N) {
  uint32_t *deltas = new uint32_t[N];
  uint32_t *sums = new uint32_t[N];
  for (size_t i = 0; i < N; i++) {
    deltas[i] = rand();
  }
  int size = N;
  bool verbose = true;
  int repeat = 50;

  BEST_TIME_NOCHECK(scalar_delta(sums, N), summe(deltas, sums, N), repeat, size, verbose);
  assert(equal(sums, deltas, N));
#ifdef USING_SSE
  BEST_TIME_NOCHECK(sse_delta(sums, N), summe(deltas, sums, N), repeat, size, verbose);
  assert(equal(sums, deltas, N));
#endif
#ifdef USING_SIMDPP
  BEST_TIME_NOCHECK(simdpp_delta(sums, N), summe(deltas, sums, N), repeat, size, verbose);
  assert(equal(sums, deltas, N));
#endif
#ifdef USING_DIMSUM
  BEST_TIME_NOCHECK(dimsum_delta(sums, N), summe(deltas, sums, N), repeat, size, verbose);
  assert(equal(sums, deltas, N));
#endif
#ifdef USING_NEON
  BEST_TIME_NOCHECK(neon_delta(sums, N), summe(deltas, sums, N), repeat, size, verbose);
  assert(equal(sums, deltas, N));
#endif
  delete[] deltas;
  delete[] sums;
}


int main() {
  tellme();
  demo(10000);
  printf("Code is good\n");
}
