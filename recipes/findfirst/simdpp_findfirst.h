#pragma once
#include "autosimdpp.h"
#include <simdpp/simd.h>
#include <cstdint>
#include <cstddef>

#define USING_SIMDPP


static int simdpp_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  simdpp::uint8<16> targetvec = simdpp::make_uint(target);
  for(; i + 16 <=  length; i += 16) {
      simdpp::uint8<16> streamvec = simdpp::load_u((uint8_t*)stream + i);
      simdpp::mask_int8<16> mask = (targetvec == streamvec);
      simdpp::uint8<16> maskint = simdpp::bit_cast<simdpp::uint8<16>,simdpp::mask_int8<16>>(mask);
      if(simdpp::test_bits_any(maskint) ) {
        int M = simdpp::extract_bits<7>(maskint);
        return __builtin_ctz(M) + i;
      } 
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

static int simdpp_movemask_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  int m;
  simdpp::uint8<16> targetvec = simdpp::make_uint(target);
  for(; i + 16 <=  length; i += 16) {
      simdpp::uint8<16> streamvec = simdpp::load_u((uint8_t*)stream + i);
      simdpp::mask_int8<16> mask = (targetvec == streamvec);
      simdpp::uint8<16> maskint = simdpp::bit_cast<simdpp::uint8<16>,simdpp::mask_int8<16>>(mask);
      if((m = simdpp::extract_bits<7>(maskint)) != 0 ) {
        return __builtin_ctz(m) + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

size_t simdpp_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = simdpp_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = simdpp_findfirst(stream, t + 1, length, target);
  }
  return sum;
}


size_t simdpp_movemask_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = simdpp_movemask_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = simdpp_movemask_findfirst(stream, t + 1, length, target);
  }
  return sum;
}
