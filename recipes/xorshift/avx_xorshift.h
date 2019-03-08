#pragma once
#ifdef __AVX2__

#define USING_AVX

#include <cstdint>
#include <cstddef>
#include <x86intrin.h>

#include "sse_xorshift.h"

struct avx_xorshift128plus_key_s {
    __m256i part1;
    __m256i part2;
};

typedef struct avx_xorshift128plus_key_s avx_xorshift128plus_key_t;

/**
* You can create a new key like so...
*  avx_xorshift128plus_key_t mykey;
*  avx_xorshift128plus_init(324,4444,&mykey);
*
* This feeds the two integers (324 and 4444) as seeds to the random
* number generator.
*
*  Then you can generate random numbers like so...
*      avx_xorshift128plus(&mykey);
* If your application is threaded, each thread should have its own
* key.
*
*
* The seeds (key1 and key2) should be non-zero. You are responsible for
* checking that they are non-zero.
*/
static void avx_xorshift128plus_init(uint64_t key1, uint64_t key2, avx_xorshift128plus_key_t *key) {
	uint64_t S0[4];
	uint64_t S1[4];
	S0[0] = key1;
	S1[0] = key2;
	xorshift128plus_jump_onkeys(*S0, *S1, S0 + 1, S1 + 1);
	xorshift128plus_jump_onkeys(*(S0 + 1), *(S1 + 1), S0 + 2, S1 + 2);
	xorshift128plus_jump_onkeys(*(S0 + 2), *(S1 + 2), S0 + 3, S1 + 3);
	key->part1 = _mm256_loadu_si256((const __m256i *) S0);
	key->part2 = _mm256_loadu_si256((const __m256i *) S1);
}

/*
 Return a 256-bit random "number"
 */
static __m256i avx_xorshift128plus(avx_xorshift128plus_key_t *key) {
	__m256i s1 = key->part1;
	const __m256i s0 = key->part2;
	key->part1 = key->part2;
	s1 = _mm256_xor_si256(key->part2, _mm256_slli_epi64(key->part2, 23));
	key->part2 = _mm256_xor_si256(
			_mm256_xor_si256(_mm256_xor_si256(s1, s0),
					_mm256_srli_epi64(s1, 18)), _mm256_srli_epi64(s0, 5));
	return _mm256_add_epi64(key->part2, s0);
}


void avx_fillrandomarray(uint64_t * array, size_t len) {
  avx_xorshift128plus_key_t mykey;
  avx_xorshift128plus_init(324,4444,&mykey);
  for(size_t k = 0; k < len / 4; k ++) {
     __m256i v = avx_xorshift128plus(&mykey);
     _mm256_storeu_si256((__m256i *) array + k,v);
  }
    uint64_t buffer[4];
    __m256i v = avx_xorshift128plus(&mykey);
    _mm256_storeu_si256((__m256i *) buffer,v);
    size_t lo = len - len / 4 * 4;
    for(size_t k = 0; k < lo; k++) {
      array[len / 4 * 4 + k] = buffer[k];
    }
}
#endif
