#pragma once
#ifdef __aarch64__

#define USING_NEON

#include <arm_neon.h>
#include <cstdint>
#include <cstddef>
#include "scalar_xorshift.h"

/* Keys for scalar xorshift128. Must be non-zero
These are modified by xorshift128plus.
 */
struct neon_xorshift128plus_key_s {
    uint64x2_t part1;
    uint64x2_t part2;
};

typedef struct neon_xorshift128plus_key_s neon_xorshift128plus_key_t;

/**
* You can create a new key like so...
*  neon_xorshift128plus_key_t mykey;
*  neon_xorshift128plus_init(324,4444,&mykey);
*
* This feeds the two integers (324 and 4444) as seeds to the random
* number generator.
*
*  Then you can generate random numbers like so...
*      neon_xorshift128plus(&mykey);
* If your application is threaded, each thread should have its own
* key.
*
*
* The seeds (key1 and key2) should be non-zero. You are responsible for
* checking that they are non-zero.
*/
static void neon_xorshift128plus_init(uint64_t key1, uint64_t key2, neon_xorshift128plus_key_t *key) {
	uint64_t S0[2];
	uint64_t S1[2];
	S0[0] = key1;
	S1[0] = key2;
	xorshift128plus_jump_onkeys(*S0, *S1, S0 + 1, S1 + 1);
	key->part1 = vld1q_u64(S0);
	key->part2 = vld1q_u64(S1);
}

/*
 Return a 128-bit random "number"
 */
static uint64x2_t neon_xorshift128plus(neon_xorshift128plus_key_t *key) {
	uint64x2_t s1 = key->part1;
	const uint64x2_t s0 = key->part2;
	key->part1 = key->part2;
	s1 = veorq_u64(key->part2, vqshlq_n_u64(key->part2, 23));
	key->part2 = veorq_u64(
			veorq_u64(veorq_u64(s1, s0),
					vshrq_n_u64(s1, 18)), vshrq_n_u64(s0, 5));
	return vaddq_u64(key->part2, s0);
}


void neon_fillrandomarray(uint64_t * array, size_t len) {
  neon_xorshift128plus_key_t mykey;
  neon_xorshift128plus_init(324,4444,&mykey);
  for(size_t k = 0; k < len / 2; k ++) {
     uint64x2_t v = neon_xorshift128plus(&mykey);
     vst1q_u64(array + 2 * k,v);
  }
  if(len / 2 * len != len) {
    uint64_t buffer[2];
    uint64x2_t v = neon_xorshift128plus(&mykey);
    vst1q_u64(buffer,v);
    array[len - 1] = buffer[0];
  }
}

#endif
