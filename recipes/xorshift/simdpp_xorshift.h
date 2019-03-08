#pragma once
#include "autosimdpp.h"
#include <simdpp/simd.h>
#include <cstdint>
#include <cstddef>

#define USING_SIMDPP

#include "sse_xorshift.h"


struct simdpp_xorshift128plus_key_s {
    simdpp::uint64<2> part1;
    simdpp::uint64<2> part2;
};

typedef struct simdpp_xorshift128plus_key_s simdpp_xorshift128plus_key_t;



static void simdpp_xorshift128plus_init(uint64_t key1, uint64_t key2, simdpp_xorshift128plus_key_t *key) {
	uint64_t S0[2];
	uint64_t S1[2];
	S0[0] = key1;
	S1[0] = key2;
	xorshift128plus_jump_onkeys(*S0, *S1, S0 + 1, S1 + 1);
	key->part1 = simdpp::load_u(S0);
	key->part2 = simdpp::load_u(S1);
}

/*
 Return a 128-bit random "number"
 */
static simdpp::uint64<2> simdpp_xorshift128plus(simdpp_xorshift128plus_key_t *key) {
	simdpp::uint64<2> s1 = key->part1;
	simdpp::uint64<2> s0 = key->part2;
	key->part1 = key->part2;
	s1 = key->part2 ^ (key->part2 << 23);
	key->part2 = (((s1 ^ s0) ^ (s1 >> 18)) ^ (s0 << 5));
	return key->part2 + s0;
}


void simdpp_fillrandomarray(uint64_t * array, size_t len) {
  simdpp_xorshift128plus_key_t mykey;
  simdpp_xorshift128plus_init(324,4444,&mykey);
  for(size_t k = 0; k < len / 2; k ++) {
     simdpp::uint64<2> v = simdpp_xorshift128plus(&mykey);
     simdpp::store_u(array + 2 * k,v);
  }
  if(len / 2 * len != len) {
    uint64_t buffer[2];
     simdpp::uint64<2> v = simdpp_xorshift128plus(&mykey);
     simdpp::store_u(buffer,v);
    array[len - 1] = buffer[0];
  }
}
