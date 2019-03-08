Case with a tiny table:
https://github.com/lemire/simdprune
See thinprune_epi8
It is quite slow.

===============

Note: this is a case where the best strategy differs between sse and neon, so 
portability might be an issue


"left packing". E.g., I give you a vector X, a vector Y.
You want ...

1. How many elements K are equal?
2. A new vector having the K elements in sequence... we don't care about leftover elements.


This is generally useful... It is in SVE's documentation. AVX-512 introduces instructions to do this
(and so does SVE).



Follows some code that depends critically on pdep. But pdep is pretty much specific to Intel (very slow on Ryzen and there may not be an equivalent on ARM).
I have something like it:
https://github.com/lemire/simdprune/blob/master/simdprune.h#L53-L64

==================CUT HERE===================


// Scatters the bits into nibbles and replicates each bit four times.
static PIK_INLINE uint64_t ReplicateBitsIntoNibbles(const uint32_t keep) {
  const uint64_t scattered = _pdep_u64(keep, 0x1111111111111111ULL);
  return scattered * 15;
}

// Returns up to 16 nibbles "i" such that keep[i] == 1, packed into the lowest
// bits of a 64-bit word. This approach requires BMI2 and is about 1 cycle
// slower than two 8-bit table lookups, but we prefer to reduce memory traffic.
static PIK_INLINE uint64_t IndicesForCompress(const uint32_t keep) {
  const uint64_t replicated4 = ReplicateBitsIntoNibbles(keep);
  // Compress only the keep=1 nibbles into 64-bits.
  return _pext_u64(0xFEDCBA9876543210, replicated4);
}

// Returns 16 nibbles indicating from which byte offset to load so that each
// code [byte] with keep=1 has the same value as before compression.
static PIK_INLINE uint64_t IndicesForExpand(const uint32_t keep) {
  const uint64_t replicated4 = ReplicateBitsIntoNibbles(keep);
  // Sequentially inject indices into nibbles where keep=1.
  return _pdep_u64(0xFEDCBA9876543210, replicated4);
}

// Returns one byte per nibble for use in shuffling.
static PIK_INLINE u8x16 BytesFromNibbles(const uint64_t nibbles) {
  const uint64_t mask4 = 0x0F0F0F0F0F0F0F0FULL;
  const uint64_t lo = nibbles & mask4;
  const uint64_t hi = (nibbles >> 4) & mask4;
  return u8x16(_mm_unpacklo_epi8(_mm_cvtsi64_si128(lo), _mm_cvtsi64_si128(hi)));
}

// Returns keep & (1U << i) ? 0xFF : 0x00 for i = [0, 32).
static PIK_INLINE u8x32 MaskFromBits(const uint32_t keep) {
  SIMD_ALIGN constexpr uint64_t replicate8[4] = {
  	0x0000000000000000ULL, 0x0101010101010101ULL, 0x0202020202020202ULL,
  	0x0303030303030303ULL};
  const u8x32 keep64(
  	shuffle_bytes(set1(u32x8(), keep), load(u64x4(), replicate8)));

  // Test one bit in each of the 8x-replicated bytes.
  const u8x32 slices(set1(u64x4(), 0x8040201008040201ULL));
  return (keep64 & slices) == slices;
}

// Stores only the "codes" whose corresponding "keep" bit is 1.
static PIK_INLINE void CompressAndStore(const u8x32 codes, const uint32_t keep,
                                    	uint8_t* const PIK_RESTRICT to) {
  const uint32_t keep0 = keep & 0xFFFF;
  const uint32_t keep1 = keep >> 16;
  const uint32_t num_keep0 = _mm_popcnt_u32(keep0);
  const uint64_t nibbles0 = IndicesForCompress(keep0);
  const uint64_t nibbles1 = IndicesForCompress(keep1);
  const u8x16 control0 = BytesFromNibbles(nibbles0);
  const u8x16 control1 = BytesFromNibbles(nibbles1);
  const u8x16 codes0(_mm256_castsi256_si128(codes));
  const u8x16 codes1(_mm256_extracti128_si256(codes, 1));
  const u8x16 compressed0(_mm_shuffle_epi8(codes0, control0));
  const u8x16 compressed1(_mm_shuffle_epi8(codes1, control1));
  store_unaligned(compressed0, to);
  store_unaligned(compressed1, to + num_keep0);
}

// Concatenates lower/upper 128-bit halves into 256 bits.
// TODO: we're missing this in the API - call it concat or combine?
static PIK_INLINE u8x32 Concatenate2x128(const u8x16& v0, const u8x16& v1) {
  const u8x32 v0_256(_mm256_castsi128_si256(v0));
  return u8x32(_mm256_inserti128_si256(v0_256, v1, 1));
}

// Loads 128-bit halves from the two addresses and concatenates them.
static PIK_INLINE u8x32 Load2x128(const uint8_t* const PIK_RESTRICT from0,
                              	const uint8_t* const PIK_RESTRICT from1) {
  return Concatenate2x128(load_unaligned(u8x16(), from0),
                      	load_unaligned(u8x16(), from1));
}

// Loads up to 32 unaligned bytes; lanes where keep=1 match the original input,
// and all others are 0. This is useful for decoders that merge multiple
// partial results via addition.
static PIK_INLINE u8x32 LoadAndExpand00(const uint8_t* const PIK_RESTRICT from,
                                    	const uint32_t keep) {
  const uint32_t keep0 = keep & 0xFFFF;
  const uint32_t keep1 = keep >> 16;
  const uint32_t num_keep0 = _mm_popcnt_u32(keep0);
  const u8x32 compressed = Load2x128(from, from + num_kceep0);
  const uint64_t nibbles0 = IndicesForExpand(keep0);
  const uint64_t nibbles1 = IndicesForExpand(keep1);
  const u8x32 control =
  	Concatenate2x128(BytesFromNibbles(nibbles0), BytesFromNibbles(nibbles1));
  const u8x32 keep_mask = MaskFromBits(keep);
  const u8x32 expanded(_mm256_shuffle_epi8(compressed, control));
  return expanded & keep_mask;
}


==================CUT HERE===================
