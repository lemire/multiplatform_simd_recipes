#pragma once
#ifdef __AVX2__

#define USING_AVX

#include <cstdint>
#include <cstddef>
#include <x86intrin.h>

int avx_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  __m256i targetvec = _mm256_set1_epi8(target);
  for(; i + sizeof(__m256i) <=  length; i += sizeof(__m256i)) {
      __m256i streamvec = _mm256_loadu_si256((const __m256i*)(stream + i));
      __m256i mask = _mm256_cmpeq_epi8(targetvec, streamvec);
      if(!_mm256_testz_si256(mask,mask)) {
        int m = _mm256_movemask_epi8(mask);
        return _tzcnt_u32(m) + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

int avx_movemask_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  int m;
  __m256i targetvec = _mm256_set1_epi8(target);
  for(; i + sizeof(__m256i) <=  length; i += sizeof(__m256i)) {
      __m256i streamvec = _mm256_loadu_si256((const __m256i*)(stream + i));
      __m256i mask = _mm256_cmpeq_epi8(targetvec, streamvec);
      if((m = _mm256_movemask_epi8(mask))!=0) {
        return _tzcnt_u32(m) + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}


size_t avx_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = avx_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = avx_findfirst(stream, t + 1, length, target);
  }
  return sum;
}


size_t avx_movemask_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = avx_movemask_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = avx_movemask_findfirst(stream, t + 1, length, target);
  }
  return sum;
}
#endif
