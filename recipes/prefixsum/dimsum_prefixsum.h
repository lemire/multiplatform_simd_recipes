#pragma once
#ifdef __aarch64__
#warning "dimsum disabled on ARM"
#else
#include <dimsum.h>

#include <cstdint>
#include <cstddef>

#include "scalar_prefixsum.h"

#define USING_DIMSUM

// specializing for 128-bit

static dimsum::Simd128<uint32> PrefixSum128(dimsum::Simd128<uint32> curr,
                                            dimsum::Simd128<uint32> prev) {
  dimsum::Simd128<uint32> zero = dimsum::Simd128<uint32>::list(0, 0, 0, 0);
  dimsum::Simd128<uint32> Add = dimsum::shuffle<3, 4, 5, 6>(
      zero, curr); // Cycle 1: [- A B C] (already done)
  prev = dimsum::shuffle<3, 3, 3, 3>(prev, prev); // Cycle 2: [P P P P]
  curr = curr + Add;                              // Cycle 2: [A AB BC CD]
  Add = dimsum::shuffle<2, 3, 4, 5>(zero, curr);  // Cycle 3: [- - A AB]
  curr = curr + prev;                             // Cycle 3: [PA PAB PBC PCD]
  return curr + Add; // Cycle 4: [PA PAB PABC PABCD]
}

void dimsum_prefixsum(uint32_t *data, size_t length) {
  //static_assert(sizeof(int) == sizeof(uint32_t));
  dimsum::Simd128<uint32> prev = dimsum::Simd128<uint32>::list(0, 0, 0, 0);
  dimsum::Simd128<uint32> curr;
  for (size_t i = 0; i < length / 4; i++) {
    curr.copy_from((unsigned int *)data + 4 * i,
                   dimsum::flags::element_aligned);
    dimsum::Simd128<uint32> csum = PrefixSum128(curr, prev);
    csum.copy_to((unsigned int *)data + 4 * i, dimsum::flags::element_aligned);
    prev = csum;
  }
  scalar_prefixsum_leftover(data, length, 4);
}

#endif // ARM NEON
