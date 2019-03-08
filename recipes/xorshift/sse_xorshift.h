#pragma once
#ifdef __SSSE3__

#define USING_SSE

#include <cstdint>
#include <cstddef>

#include <x86intrin.h>
#include "scalar_xorshift.h"

/* Keys for scalar xorshift128. Must be non-zero
These are modified by xorshift128plus.
 */
struct sse_xorshift128plus_key_s {
    __m128i part1;
    __m128i part2;
};

typedef struct sse_xorshift128plus_key_s sse_xorshift128plus_key_t;


/**
* You can create a new key like so...
*  sse_xorshift128plus_key_t mykey;
*  sse_xorshift128plus_init(324,4444,&mykey);
*
* This feeds the two integers (324 and 4444) as seeds to the random
* number generator.
*
*  Then you can generate random numbers like so...
*      sse_xorshift128plus(&mykey);
* If your application is threaded, each thread should have its own
* key.
*
*
* The seeds (key1 and key2) should be non-zero. You are responsible for
* checking that they are non-zero.
*/
static void sse_xorshift128plus_init(uint64_t key1, uint64_t key2, sse_xorshift128plus_key_t *key) {
	uint64_t S0[2];
	uint64_t S1[2];
	S0[0] = key1;
	S1[0] = key2;
	xorshift128plus_jump_onkeys(*S0, *S1, S0 + 1, S1 + 1);
	key->part1 = _mm_loadu_si128((const __m128i *) S0);
	key->part2 = _mm_loadu_si128((const __m128i *) S1);
}

/*
 Return a 128-bit random "number"
 */
static __m128i sse_xorshift128plus(sse_xorshift128plus_key_t *key) {
	__m128i s1 = key->part1;
	const __m128i s0 = key->part2;
	key->part1 = key->part2;
	s1 = _mm_xor_si128(key->part2, _mm_slli_epi64(key->part2, 23));
	key->part2 = _mm_xor_si128(
			_mm_xor_si128(_mm_xor_si128(s1, s0),
					_mm_srli_epi64(s1, 18)), _mm_srli_epi64(s0, 5));
	return _mm_add_epi64(key->part2, s0);
}


void sse_fillrandomarray(uint64_t * array, size_t len) {
  sse_xorshift128plus_key_t mykey;
  sse_xorshift128plus_init(324,4444,&mykey);
  for(size_t k = 0; k < len / 2; k ++) {
     __m128i v = sse_xorshift128plus(&mykey);
     _mm_storeu_si128((__m128i *) array + k,v);
  }
  if(len / 2 * len != len) {
    uint64_t buffer[2];
    __m128i v = sse_xorshift128plus(&mykey);
    _mm_storeu_si128((__m128i *) buffer,v);
    array[len - 1] = buffer[0];
  }
}

#endif
