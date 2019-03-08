#pragma once
#include "autosimdpp.h"
#include <simdpp/simd.h>
#include <cstdint>
#include <cstddef>

#define USING_SIMDPP

#include "mandel.h"

simdpp::float32<4> make_float1(float x) {
  return simdpp::make_float(x);
}

void mandel_simdpp(uint8_t *image, const struct spec *s) {
    simdpp::float32<4> xmin = make_float1(s->xlim[0]);
    simdpp::float32<4> ymin = make_float1(s->ylim[0]);
    simdpp::float32<4> xscale = make_float1((s->xlim[1] - s->xlim[0]) / s->width);
    simdpp::float32<4> yscale = make_float1((s->ylim[1] - s->ylim[0]) / s->height);
    simdpp::float32<4> threshold = make_float1(4);
    simdpp::float32<4> one = make_float1(1);
    simdpp::float32<4> iter_scale = make_float1(1.0f / s->iterations);
    simdpp::float32<4> depth_scale = make_float1(s->depth - 1);

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x += 4) {
            simdpp::float32<4> mx = simdpp::make_float(x + 3, x + 2, x + 1, x + 0);
            simdpp::float32<4> my = make_float1(y);
            simdpp::float32<4> cr = (mx * xscale) + xmin;
            simdpp::float32<4> ci = (my * yscale) + ymin;
            simdpp::float32<4> zr = cr;
            simdpp::float32<4> zi = ci;
            int k = 1;
            simdpp::float32<4> mk = make_float1(k);
            while (++k < s->iterations) {
                /* Compute z1 from z0 */
                simdpp::float32<4> zr2 = (zr * zr);
                simdpp::float32<4> zi2 = (zi * zi);
                simdpp::float32<4> zrzi = (zr * zi);
                /* zr1 = zr0 * zr0 - zi0 * zi0 + cr */
                /* zi1 = zr0 * zi0 + zr0 * zi0 + ci */
                zr = ((zr2 - zi2) + cr);
                zi = ((zrzi + zrzi) + ci);

                /* Increment k */
                zr2 = (zr * zr);
                zi2 = (zi * zi);
                simdpp::float32<4> mag2 = (zr2 + zi2);
                simdpp::uint32<4> mask = simdpp::bit_cast<simdpp::uint32<4>>(mag2 < threshold);
                mk = ((mask & one) + mk);

                /* Early bailout */
                if (!simdpp::test_bits_any(mask) )
                    break;
            }
            mk = (mk * iter_scale);
            mk = simdpp::sqrt(mk);
            mk = (mk * depth_scale);
            simdpp::uint32<4> pixels = to_int32(mk);
            uint8_t *dst = image + y * s->width * 3 + x * 3;
            uint8_t src[16];
            simdpp::store_u(src,pixels);
            for (int i = 0; i < 4; i++) {
                dst[i * 3 + 0] = src[i * 4];
                dst[i * 3 + 1] = src[i * 4];
                dst[i * 3 + 2] = src[i * 4];
            }
        }
    }
}
