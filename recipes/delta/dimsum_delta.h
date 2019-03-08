#pragma once
#ifdef __aarch64__

#warning "dimsum disabled on ARM"
#else
#include <dimsum.h>

#include <cstdint>
#include <cstddef>

#include "scalar_delta.h"

#define USING_DIMSUM

// specializing for 128-bit

static dimsum::Simd128<uint32> Delta128(dimsum::Simd128<uint32> curr,
                                       dimsum::Simd128<uint32> prev) {
  return curr - dimsum::shuffle<3, 4, 5, 6>(prev, curr);
}

void dimsum_delta(uint32_t *data, size_t length) {
  static_assert(sizeof(unsigned int) == sizeof(uint32_t));
  scalar_delta_leftover(data, length, 4);
  dimsum::Simd128<uint32> prev = dimsum::Simd128<uint32>::list(0, 0, 0, 0);
  dimsum::Simd128<uint32> curr;
  for (size_t i = 0; i < length / 4; i++) {
    curr.copy_from((unsigned int *)data + 4 * i, dimsum::flags::element_aligned);
    dimsum::Simd128<uint32> diff = Delta128(curr, prev);
    diff.copy_to((unsigned int *)data + 4 * i, dimsum::flags::element_aligned);
    prev = curr;
  }
}

/**
* lane independence is doable, but crazy.
* We are keeping this implementation, but not using it.
*/

/*
template <size_t offset, size_t... indices>
static dimsum::NativeSimd<uint32_t>
vector_extract_impl(dimsum::NativeSimd<uint32_t> curr,
                                dimsum::NativeSimd<uint32_t> prev,
                                std::index_sequence<indices...>) {
  return dimsum::shuffle<(indices + dimsum::NativeSimd<uint32_t>::size() -
offset)...>(curr, prev);
}

template <size_t offset>
static dimsum::NativeSimd<uint32_t> vector_extract(dimsum::NativeSimd<uint32_t>
curr,
                                dimsum::NativeSimd<uint32_t> prev) {
  return vector_extract_impl<offset>(curr,
prev,std::make_index_sequence<dimsum::NativeSimd<uint32_t>::size()>{});
}


static dimsum::NativeSimd<uint32_t> Delta(dimsum::NativeSimd<uint32_t> curr,
                           dimsum::NativeSimd<uint32_t> prev) {
  return curr - vector_extract<1>(curr, prev);
}
*/

#endif // ARM NEON
