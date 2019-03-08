
#ifdef __aarch64__

#warning "dimsum disabled on ARM"
#else
#include <dimsum.h>

#include <cstdint>
#include <cstddef>
#define USING_DIMSUM
#include "sse_xorshift.h"


struct dimsum_xorshift128plus_key_s {
    dimsum::Simd128<uint64> part1;
    dimsum::Simd128<uint64> part2;
};

typedef struct dimsum_xorshift128plus_key_s dimsum_xorshift128plus_key_t;



static void dimsum_xorshift128plus_init(uint64_t key1, uint64_t key2, dimsum_xorshift128plus_key_t *key) {
	uint64_t S0[2];
	uint64_t S1[2];
	S0[0] = key1;
	S1[0] = key2;
	xorshift128plus_jump_onkeys(*S0, *S1, S0 + 1, S1 + 1);
	key->part1.copy_from(S0, dimsum::flags::element_aligned);
	key->part2.copy_from(S1, dimsum::flags::element_aligned);
}

/*
 Return a 128-bit random "number"
 */
static dimsum::Simd128<uint64> dimsum_xorshift128plus(dimsum_xorshift128plus_key_t *key) {
	dimsum::Simd128<uint64> s1 = key->part1;
	const dimsum::Simd128<uint64> s0 = key->part2;
	key->part1 = key->part2;
	s1 = key->part2 ^ (key->part2 << 23);
	key->part2 = (((s1 ^ s0) ^ (s1 >> 18)) ^ (s0 << 5));
	return key->part2 + s0;
}


void dimsum_fillrandomarray(uint64_t * array, size_t len) {
  dimsum_xorshift128plus_key_t mykey;
  dimsum_xorshift128plus_init(324,4444,&mykey);
  for(size_t k = 0; k < len / 2; k ++) {
     dimsum::Simd128<uint64> v = dimsum_xorshift128plus(&mykey);
     v.copy_to(array + 2 * k,dimsum::flags::element_aligned);
  }
  if(len / 2 * len != len) {
    uint64_t buffer[2];
     dimsum::Simd128<uint64> v = dimsum_xorshift128plus(&mykey);
     v.copy_to(buffer,dimsum::flags::element_aligned);
    array[len - 1] = buffer[0];
  }
}

#endif
