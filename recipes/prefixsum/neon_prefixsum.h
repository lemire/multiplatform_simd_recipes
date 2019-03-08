#pragma once
#ifdef __aarch64__

#define USING_NEON

#include <arm_neon.h>
#include <cstdint>
#include <cstddef>

#include "scalar_prefixsum.h"

// this is slow?
static uint32x4_t PrefixSum(uint32x4_t curr, uint32x4_t prev) {
  uint32x4_t zero = {0, 0, 0, 0};
  uint32x4_t add = vextq_u32(zero, curr, 3);
  const uint8x16_t BroadcastLast = {12, 13, 14, 15, 12, 13, 14, 15,
                                    12, 13, 14, 15, 12, 13, 14, 15};
  prev = vreinterpretq_u32_u8(
      vqtbl1q_u8(vreinterpretq_u8_u32(prev), BroadcastLast));
  curr = vaddq_u32(curr, add);
  add = vextq_u32(zero, curr, 2);
  curr = vaddq_u32(curr, prev);
  curr = vaddq_u32(curr, add);
  return curr;
}

void neon_prefixsum(uint32_t *data, size_t length) {
  uint32x4_t prev = {0, 0, 0, 0};
  uint32x4_t curr;
  for (size_t i = 0; i < length / 4; i++) {
    curr = vld1q_u32(data + 4 * i);
    uint32x4_t csum = PrefixSum(curr, prev);
    vst1q_u32(data + 4 * i, csum);
    prev = csum;
  }
  scalar_prefixsum_leftover(data, length, 4);
}

#endif
