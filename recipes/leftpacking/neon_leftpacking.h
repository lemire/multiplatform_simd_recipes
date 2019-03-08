#pragma once
#ifdef __aarch64__

#define USING_NEON

#include <arm_neon.h>
#include <cstdint>
#include <cstddef>


#include "bigtable.h"

static uint8x16_t prune_epi8( uint8x16_t x, int mask) {
    uint8x16_t shuf = vld1q_u8(shufmask + 16 * mask);
    return vqtbl1q_u8(x,shuf);
}


static inline uint16_t neonmovemask_addv(uint8x16_t input8) {
  uint16x8_t input = vreinterpretq_u16_u8(input8);
  const uint16x8_t bitmask = { 0x0101 , 0x0202, 0x0404, 0x0808, 0x1010, 0x2020, 0x4040, 0x8080 };
  uint16x8_t minput = vandq_u16(input, bitmask);
  return vaddvq_u16(minput);
}


size_t neon_deletezeros(uint8_t *data, size_t length) {
  uint8x16_t target = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  size_t pos = 0;
  for (size_t i = 0; i < length / 16; i++) {
    uint8x16_t curr = vld1q_u8((uint8_t *)data + i * 16);
    uint8x16_t c = vcgeq_u8(curr,target); 
    int numberofnonzeros = vaddvq_u8(vshrq_n_u8(c,7));
    int m = neonmovemask_addv(c);
    curr = prune_epi8(curr,m);
    vst1q_u8(data + pos, curr);
    pos +=  numberofnonzeros;
  }
  // we finish it off
  for (size_t j = length - (length % 16); j < length; j++) {
    uint8_t val = data[j];
    data[pos] = val;
    pos += (val != 0);
  }
  return pos;
}

#endif
