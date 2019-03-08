#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include "scalar_prefixsum.h"
#include "sse_prefixsum.h"
#include "simdpp_prefixsum.h"
#include "dimsum_prefixsum.h"
#include "neon_prefixsum.h"
#include "simdutil.h"

static void summe(const uint32_t *deltas, uint32_t *sums, size_t N) {
  if (N == 0)
    return;
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
  summe(deltas, sums, N);
  uint32_t *copydeltas = new uint32_t[N];
  memcpy(copydeltas, deltas, sizeof(uint32_t) * N);
  scalar_prefixsum(copydeltas, N);
  assert(equal(sums, copydeltas, N));
#ifdef USING_SSE
  memcpy(copydeltas, deltas, sizeof(uint32_t) * N);
  sse_prefixsum(copydeltas, N);
  assert(equal(sums, copydeltas, N));
#endif
#ifdef USING_SIMDPP
  memcpy(copydeltas, deltas, sizeof(uint32_t) * N);
  simdpp_prefixsum(copydeltas, N);
  assert(equal(sums, copydeltas, N));
#endif
#ifdef USING_DIMSUM
  memcpy(copydeltas, deltas, sizeof(uint32_t) * N);
  dimsum_prefixsum(copydeltas, N);
  assert(equal(sums, copydeltas, N));
#endif
#ifdef USING_NEON
  memcpy(copydeltas, deltas, sizeof(uint32_t) * N);
  neon_prefixsum(copydeltas, N);
  assert(equal(sums, copydeltas, N));
#endif
  delete[] deltas;
  delete[] copydeltas;
  delete[] sums;
}

int main() {
  tellme();
  for (size_t i = 1; i < 1024; i++)
    demo(i);
  printf("Code is good\n");
}
