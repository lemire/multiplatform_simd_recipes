#pragma once
#ifdef __aarch64__

#define USING_NEON

#include <arm_neon.h>
#include <cstdint>
#include <cstddef>

#include "scalar_delta.h"

static uint32x4_t Delta(uint32x4_t curr, uint32x4_t prev) {
  return vsubq_u32(curr, vextq_u32(prev, curr, 3));
}

void neon_delta(uint32_t *data, size_t length) {
  scalar_delta_leftover(data, length, 4);
  uint32x4_t prev = {0, 0, 0, 0};
  uint32x4_t curr;
  for (size_t i = 0; i < length / 4; i++) {
    curr = vld1q_u32(data + 4 * i);
    uint32x4_t diff = Delta(curr, prev);
    vst1q_u32(data + 4 * i, diff);
    prev = curr;
  }
}

#endif
