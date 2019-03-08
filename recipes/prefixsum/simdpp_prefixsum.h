#pragma once
#include "autosimdpp.h"
#include <simdpp/simd.h>
#include <cstdint>
#include <cstddef>

#include "scalar_prefixsum.h"
#define USING_SIMDPP

static simdpp::uint32<4> simdppPrefixSum(simdpp::uint32<4> curr,
                                  simdpp::uint32<4> prev) {
  simdpp::uint32<4> Add =
      simdpp::move4_r<1>(curr); // Cycle 1: [- A B C] (already done)
  prev = simdpp::permute4<3, 3, 3, 3>(
      prev);         // _mm_shuffle_epi32(prev, 0xFF); // Cycle 2: [P P P P]
  curr = curr + Add; // Cycle 2: [A AB BC CD]
  Add = simdpp::move4_r<2>(curr); // Cycle 3: [- - A AB]
  curr = curr + prev;             // Cycle 3: [PA PAB PBC PCD]*/
  return curr + Add;              // Cycle 4: [PA PAB PABC PABCD]
}

void simdpp_prefixsum(uint32_t *data, size_t length) {
  simdpp::uint32<4> prev = simdpp::make_zero();
  for (size_t i = 0; i < length / 4; i++) {
    simdpp::uint32<4> curr = simdpp::load_u(data + 4 * i);
    simdpp::uint32<4> csum = simdppPrefixSum(curr, prev);
    simdpp::store_u(data + 4 * i, csum);
    prev = csum;
  }
  scalar_prefixsum_leftover(data, length, 4);
}
