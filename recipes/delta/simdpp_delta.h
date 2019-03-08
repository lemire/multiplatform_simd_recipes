#pragma once
#include "autosimdpp.h"
#include <simdpp/simd.h>
#include <cstdint>
#include <cstddef>

#include "scalar_delta.h"

#define USING_SIMDPP
static simdpp::uint32<4> simdppDelta(simdpp::uint32<4> curr, simdpp::uint32<4> prev) {
  return curr - simdpp::align4<3>(prev, curr);
}

void simdpp_delta(uint32_t *data, size_t length) {
  scalar_delta_leftover(data, length, 4);
  simdpp::uint32<4> prev = simdpp::make_zero();
  for (size_t i = 0; i < length / 4; i++) {
    simdpp::uint32<4> curr = simdpp::load_u(data + 4 * i);
    simdpp::uint32<4> diff = simdppDelta(curr, prev);
    simdpp::store_u(data + 4 * i, diff);
    prev = curr;
  }
}
