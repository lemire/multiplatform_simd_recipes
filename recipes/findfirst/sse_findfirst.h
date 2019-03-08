#pragma once
#ifdef __SSSE3__

#define USING_SSE

#include <cstdint>
#include <cstddef>

#include <x86intrin.h>

static int sse_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  __m128i targetvec = _mm_set1_epi8(target);
  for(; i + sizeof(__m128i) <=  length; i += sizeof(__m128i)) {
      __m128i streamvec = _mm_loadu_si128((const __m128i*)(stream + i));
      __m128i mask = _mm_cmpeq_epi8(targetvec, streamvec);
      if(!_mm_testz_si128(mask,mask)) {
        int m = _mm_movemask_epi8(mask);
        return _tzcnt_u32(m) + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

static int sse_movemask_findfirst(const char * stream, size_t startindex, size_t length, const char target) {
  size_t i = startindex;
  int m;
  __m128i targetvec = _mm_set1_epi8(target);
  for(; i + sizeof(__m128i) <=  length; i += sizeof(__m128i)) {
      __m128i streamvec = _mm_loadu_si128((const __m128i*)(stream + i));
      __m128i mask = _mm_cmpeq_epi8(targetvec, streamvec);
      if((m = _mm_movemask_epi8(mask))!=0) {
        return _tzcnt_u32(m) + i;
      }
  }
  for(; i < length; i++) {
    if(stream[i] == target) return i;
  }
  return -1;
}

size_t sse_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = sse_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = sse_findfirst(stream, t + 1, length, target);
  }
  return sum;
}


size_t sse_movemask_findall(const char * stream, size_t length, const char target) {
  size_t sum = 0;
  int t = sse_movemask_findfirst(stream, 0, length, target);
  while(t != -1) {
    sum ++;
    t = sse_movemask_findfirst(stream, t + 1, length, target);
  }
  return sum;
}

#endif
