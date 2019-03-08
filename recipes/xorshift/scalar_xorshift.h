#pragma once
#include <cstdint>
#include <cstddef>



struct xorshift128plus_key_s {
    uint64_t part1;
    uint64_t part2;
};

typedef struct xorshift128plus_key_s xorshift128plus_key_t;

/* used by xorshift128plus_jump_onkeys */
static void xorshift128plus_onkeys(uint64_t * ps0, uint64_t * ps1) {
        uint64_t s1 = *ps0;
        const uint64_t s0 = *ps1;
        *ps0 = s0;
        s1 ^= s1 << 23; // a
        *ps1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
}

/* used by sse_xorshift128plus_init */
static void xorshift128plus_jump_onkeys(uint64_t in1, uint64_t in2,
                uint64_t * output1, uint64_t * output2) {
        /* todo: streamline */
        static const uint64_t JUMP[] = { 0x8a5cd789635d2dff, 0x121fd2155c472f96 };
        uint64_t s0 = 0;
        uint64_t s1 = 0;
        for (unsigned int i = 0; i < sizeof(JUMP) / sizeof(*JUMP); i++)
                for (int b = 0; b < 64; b++) {
                        if (JUMP[i] & 1ULL << b) {
                                s0 ^= in1;
                                s1 ^= in2;
                        }
                        xorshift128plus_onkeys(&in1, &in2);
                }
        output1[0] = s0;
        output2[0] = s1;
}


/**
*
* You can create a new key like so...
*   xorshift128plus_key_t mykey;
*   xorshift128plus_init(324, 4444,&mykey);
*
* or directly if you prefer:
*  xorshift128plus_key_t mykey = {.part1=324,.part2=4444}
*
*  Then you can generate random numbers like so...
*      xorshift128plus(&mykey);
* If your application is threaded, each thread should have its own
* key.
*
*
* The seeds (key1 and key2) should be non-zero. You are responsible for
* checking that they are non-zero.
*
*/
static inline void xorshift128plus_init(uint64_t key1, uint64_t key2, xorshift128plus_key_t *key) {
  key->part1 = key1;
  key->part2 = key2;
}


/*
Return a new 64-bit random number
*/

static uint64_t xorshift128plus(xorshift128plus_key_t * key) {
    uint64_t s1 = key->part1;
    const uint64_t s0 = key->part2;
    key->part1 = s0;
    s1 ^= s1 << 23; // a
    key->part2 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
    return key->part2 + s0;
}

void scalar_fillrandomarray(uint64_t * array, size_t len) {
  xorshift128plus_key_t mykey;
  xorshift128plus_init(324, 4444,&mykey);
  for(size_t i = 0 ; i < len; i++) {
    array[i] = xorshift128plus(&mykey);
  }
}
