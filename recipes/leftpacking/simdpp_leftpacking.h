#pragma once
#include "autosimdpp.h"
#include <simdpp/simd.h>
#include <cstdint>
#include <cstddef>

#define USING_SIMDPP

#include "simdprune_tables.h"

static simdpp::uint8<16> prune_epi8( simdpp::uint8<16> x, int mask) {
      return simdpp::permute_bytes16(
          x, simdpp::load_u<simdpp::uint8<16> >(mask128_epi8 + mask * 16));
}


size_t simdpp_deletezeros(uint8_t *data, size_t length) {
  simdpp::uint8<16> target = simdpp::make_zero();
  size_t pos = 0;
  for (size_t i = 0; i < length / 16; i++) {
    simdpp::uint8<16> curr = simdpp::load_u(data + i * 16);
    simdpp::mask_int8<16> c = (curr == target);
    int m = simdpp::extract_bits<7>(simdpp::bit_cast<simdpp::uint8<16>,simdpp::mask_int8<16>>(c));
    curr = prune_epi8(curr,m);
    simdpp::store_u(data + pos, curr);
    pos += 16 - __builtin_popcount(m);
  }
  // we finish it off
  for (size_t j = length - (length % 16); j < length; j++) {
    uint8_t val = data[j];
    data[pos] = val;
    pos += (val != 0);
  }
  return pos;
}
