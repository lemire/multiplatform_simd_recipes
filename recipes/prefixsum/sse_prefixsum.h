#pragma once
#ifdef __SSSE3__

#define USING_SSE

#include <cstdint>
#include <cstddef>

#include <x86intrin.h>

#include "scalar_prefixsum.h"

static __m128i PrefixSum(__m128i curr, __m128i prev) {
  __m128i Add = _mm_slli_si128(curr, 4); // Cycle 1: [- A B C] (already done)
  prev = _mm_shuffle_epi32(prev, 0xFF);  // Cycle 2: [P P P P]
  curr = _mm_add_epi32(curr, Add);       // Cycle 2: [A AB BC CD]
  Add = _mm_slli_si128(curr, 8);         // Cycle 3: [- - A AB]
  curr = _mm_add_epi32(curr, prev);      // Cycle 3: [PA PAB PBC PCD]
  return _mm_add_epi32(curr, Add);       // Cycle 4: [PA PAB PABC PABCD]
}

void sse_prefixsum(uint32_t *data, size_t length) {
  __m128i prev = _mm_setzero_si128();
  __m128i curr;
  __m128i *data128 = (__m128i *)data;
  for (size_t i = 0; i < length / 4; i++) {
    curr = _mm_loadu_si128(data128 + i);
    __m128i csum = PrefixSum(curr, prev);
    _mm_storeu_si128(data128 + i, csum);
    prev = csum;
  }
  scalar_prefixsum_leftover(data, length, 4);
}

#endif
