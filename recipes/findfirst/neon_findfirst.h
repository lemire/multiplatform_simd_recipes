#pragma once
#ifdef __aarch64__

#define USING_NEON

#include <arm_neon.h>
#include <cstdint>
#include <cstddef>


static inline uint64_t is_not_zero(uint8x16_t v) {
  uint64x2_t v64 = vreinterpretq_u64_u8(v);
  uint32x2_t v32 = vqmovn_u64(v64);
  uint64x1_t result = vreinterpret_u64_u32(v32);
  return vget_lane_u64(result, 0);
}


static uint16_t neonmovemask_addv(uint8x16_t input8) {
  const uint8x16_t shufmask = { 0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15};
  input8 = vqtbl1q_u8(input8, shufmask); // A B C D E F G H I J K L M N O P -> A I B J C K D L E  M F  N G O H P
  uint16x8_t input = vreinterpretq_u16_u8(input8);
  const uint16x8_t bitmask ={ 0x0101 , 0x0202, 0x0404, 0x0808, 0x1010, 0x2020, 0x4040, 0x8080 };
  uint16x8_t minput = vandq_u16(input, bitmask);
  return vaddvq_u16(minput);
}

static int neon_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  uint8x16_t targetvec = {target, target, target, target, target, target, target, target, target, target, target, target, target, target, target, target};;
  for(; i + 16 <=  length; i += 16) {
      uint8x16_t streamvec = vld1q_u8((uint8_t *)(stream + i));
      uint8x16_t mask = vceqq_u8(targetvec,streamvec);
      if(is_not_zero(mask)) {// hopefully, this test is very cheap
        int m = neonmovemask_addv(mask); // probably not fast
        return __builtin_ctz(m) + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

static int neon_movemask_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  int m;
  uint8x16_t targetvec = {target, target, target, target, target, target, target, target, target, target, target, target, target, target, target, target};;
  for(; i + 16 <=  length; i += 16) {
      uint8x16_t streamvec = vld1q_u8((uint8_t *)(stream + i));
      uint8x16_t mask = vceqq_u8(targetvec,streamvec);
      if((m = neonmovemask_addv(mask)) !=0 ) {
        return __builtin_ctz(m) + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}


static int neon_twostep_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  int m;
  uint8x16_t targetvec = {target, target, target, target, target, target, target, target, target, target, target, target, target, target, target, target};;
  for(; i + 16 <=  length; i += 16) {
      uint8x16_t streamvec = vld1q_u8((uint8_t *)(stream + i));
      uint8x16_t mask = vceqq_u8(targetvec,streamvec);
      uint64_t w1 = vgetq_lane_u64(vreinterpretq_u64_u8(mask),0);
      uint64_t w2 = vgetq_lane_u64(vreinterpretq_u64_u8(mask),1);
      if(w1 != 0) {
         return __builtin_ctzll(w1) / 8 + i;
      }
      if(w2 != 0) {
         return __builtin_ctzll(w2) / 8 + 8 + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

size_t neon_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = neon_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = neon_findfirst(stream, t + 1, length, target);
  }
  return sum;
}


size_t neon_twostep_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = neon_twostep_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = neon_twostep_findfirst(stream, t + 1, length, target);
  }
  return sum;
}

size_t neon_movemask_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = neon_movemask_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = neon_movemask_findfirst(stream, t + 1, length, target);
  }
  return sum;
}
#endif
