#pragma once
#ifdef __SSSE3__

#define USING_SSE

#include <cstdint>
#include <cstddef>

#include <x86intrin.h>

#include "simdprune_tables.h"

static __m128i prune_epi8( __m128i x, int mask) {
      return _mm_shuffle_epi8(
          x, _mm_loadu_si128((const __m128i *)mask128_epi8 + mask));
}


size_t sse_deletezeros(uint8_t *data, size_t length) {
  __m128i target = _mm_setzero_si128();
  __m128i *data128 = (__m128i *)data;
  size_t pos = 0;
  for (size_t i = 0; i < length / sizeof(__m128i); i++) {
    __m128i curr = _mm_loadu_si128(data128 + i);
    __m128i c = _mm_cmpeq_epi8(curr, target);
    int m = _mm_movemask_epi8(c);
    curr = prune_epi8(curr,m);
    _mm_storeu_si128((__m128i *)(data + pos), curr);
    pos += 16 - __builtin_popcount(m);
  }
  // we finish it off
  for (size_t j = length - (length % sizeof(__m128i)); j < length; j++) {
    uint8_t val = data[j];
    data[pos] = val;
    pos += (val != 0);
  }
  return pos;
}

#endif
