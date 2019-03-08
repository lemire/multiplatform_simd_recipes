
#ifdef __aarch64__

#warning "dimsum disabled on ARM"
#else
#include <dimsum.h>

#include <cstdint>
#include <cstddef>
#define USING_DIMSUM
#include "mandel.h"


// taken from dimsum_test.cc
template <typename SimdType, typename... Args>
SimdType SimdList(Args... args) {
  static_assert(sizeof...(Args) == SimdType::size(),
                "Mismatched number of elements.");
  using T = typename SimdType::value_type;
  T buffer[SimdType::size()] = {static_cast<T>(args)...};
  return SimdType(buffer, dimsum::flags::element_aligned);
}

void
mandel_dimsum(uint8_t *image, const struct spec *s)
{
    dimsum::Simd128<float> xmin(s->xlim[0]);
    dimsum::Simd128<float> ymin(s->ylim[0]);
    dimsum::Simd128<float> xscale((s->xlim[1] - s->xlim[0]) / s->width);
    dimsum::Simd128<float> yscale((s->ylim[1] - s->ylim[0]) / s->height);
    dimsum::Simd128<float> threshold(4);
    dimsum::Simd128<float> one(1);
    dimsum::Simd128<float> iter_scale(1.0f / s->iterations);
    dimsum::Simd128<float> depth_scale(s->depth - 1);

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x += 4) {
            dimsum::Simd128<float> mx = SimdList<dimsum::Simd128<float>>(x + 3, x + 2, x + 1, x + 0);
            dimsum::Simd128<float> my(y);
            dimsum::Simd128<float> cr = ((mx * xscale) + xmin);
            dimsum::Simd128<float> ci = ((my * yscale) + ymin);
            dimsum::Simd128<float> zr = cr;
            dimsum::Simd128<float> zi = ci;
            int k = 1;
            dimsum::Simd128<float> mk(k);
            while (++k < s->iterations) {
                /* Compute z1 from z0 */
                dimsum::Simd128<float> zr2 = (zr * zr);
                dimsum::Simd128<float> zi2 = (zi * zi);
                dimsum::Simd128<float> zrzi = (zr * zi);
                /* zr1 = zr0 * zr0 - zi0 * zi0 + cr */
                /* zi1 = zr0 * zi0 + zr0 * zi0 + ci */
                zr = ((zr2 - zi2) + cr);
                zi = ((zrzi +  zrzi) + ci);

                /* Increment k */
                zr2 = (zr * zr);
                zi2 = (zi * zi);
                dimsum::Simd128<float> mag2 = (zr2 +  zi2);
                dimsum::Simd128<uint32> mask = dimsum::bit_cast<uint32>(dimsum::cmp_lt(mag2  , threshold));
                mk = (dimsum::bit_cast<float>(mask & dimsum::bit_cast<uint32>(one)) + mk);

                /* Early bailout */
              //if  (movemask(dimsum::cmp_eq(mask, dimsum::Simd128<uint32>(0))) == 0)
               if ((mask[0] == 0) && (mask[1] == 0) && (mask[2] == 0) && (mask[3] == 0)) // I don't know how to do this fast
                    break;
            }
            mk = (mk * iter_scale);
            mk = dimsum::sqrt(mk);
            mk = (mk *  depth_scale);

            dimsum::Simd128<uint32_t> pixels = dimsum::static_simd_cast<uint32,float>(mk);
            uint8_t *dst = image + y * s->width * 3 + x * 3;
            uint8_t src[16];
            pixels.copy_to((uint32_t*)src,dimsum::flags::element_aligned);
            for (int i = 0; i < 4; i++) {
                dst[i * 3 + 0] = src[i * 4];
                dst[i * 3 + 1] = src[i * 4];
                dst[i * 3 + 2] = src[i * 4];
            }
        }
    }
}

#endif
