#pragma once
#ifdef __SSSE3__

#define USING_SSE

#include <cstdint>
#include <cstddef>

#include <x86intrin.h>

#include "scalar_delta.h"

static __m128i Delta(__m128i curr, __m128i prev) {
  return _mm_sub_epi32(curr, _mm_alignr_epi8(curr, prev, 12));
}

void sse_delta(uint32_t *data, size_t length) {
  scalar_delta_leftover(data, length, 4);
  __m128i prev = _mm_setzero_si128();
  __m128i curr;
  __m128i *data128 = (__m128i *)data;
  for (size_t i = 0; i < length / 4; i++) {
    curr = _mm_loadu_si128(data128 + i);
    __m128i diff = Delta(curr, prev);
    _mm_storeu_si128(data128 + i, diff);
    prev = curr;
  }
}

#endif
