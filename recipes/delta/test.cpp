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
  if(N == 0) return;
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
  scalar_delta(sums, N);
  assert(equal(sums, deltas, N));
#ifdef USING_SSE
  summe(deltas, sums, N);
  sse_delta(sums, N);
  assert(equal(sums, deltas, N));
#endif
#ifdef USING_SIMDPP
  summe(deltas, sums, N);
  simdpp_delta(sums, N);
  assert(equal(sums, deltas, N));
#endif
#ifdef USING_DIMSUM
  summe(deltas, sums, N);
  dimsum_delta(sums, N);
  assert(equal(sums, deltas, N));
#endif
#ifdef USING_NEON
  summe(deltas, sums, N);
  neon_delta(sums, N);
  assert(equal(sums, deltas, N));
#endif
  delete[] deltas;
  delete[] sums;
}

int main() {
  tellme();
  for (size_t i = 0; i < 1024; i++)
    demo(i);
  printf("Code is good\n");
}
