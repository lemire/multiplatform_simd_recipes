#include "benchmark.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>

#include "scalar_mandelbrot.h"
#include "sse_mandelbrot.h"
#include "avx_mandelbrot.h"
#include "simdpp_mandelbrot.h"
#include "dimsum_mandelbrot.h"
#include "neon_mandelbrot.h"
#include "simdutil.h"



void demo(size_t N) {
    /* Config */
    const struct spec spec = {
        .width = 1440,
        .height = 1080,
        .depth = 256,
        .xlim = {-2.5, 1.5},
        .ylim = {-1.5, 1.5},
        .iterations = 256
    };
        uint8_t *image = new uint8_t[spec.width * spec.height * 3];

  int size = spec.width * spec.height;
  bool verbose = true;
  int repeat = 1;


  BEST_TIME_NOCHECK(mandel_scalar(image, & spec),memset(image,0,size * 3), repeat,
                    size, verbose);
#ifdef USING_SSE
  BEST_TIME_NOCHECK(mandel_sse(image, & spec),memset(image,0,size * 3), repeat,
                    size, verbose);
#endif
#ifdef USING_AVX
  BEST_TIME_NOCHECK(mandel_avx(image, & spec),memset(image,0,size * 3), repeat,
                    size, verbose);
#endif

#ifdef USING_DIMSUM
  BEST_TIME_NOCHECK(mandel_dimsum(image, & spec),memset(image,0,size * 3), repeat,
                    size, verbose);
#endif
#ifdef USING_SIMDPP
  BEST_TIME_NOCHECK(mandel_simdpp(image, & spec),memset(image,0,size * 3), repeat,
                    size, verbose);
#endif
#ifdef USING_NEON
  BEST_TIME_NOCHECK(mandel_neon_bang(image, & spec),memset(image,0,size * 3), repeat,
                    size, verbose);
  BEST_TIME_NOCHECK(mandel_neon(image, & spec),memset(image,0,size * 3), repeat,
                    size, verbose);
#endif
  printf("bogus: %d \n",(int) image[0]);
  delete[] image;
}

int main() {
  tellme();
  demo(50000);
  printf("Code is good\n");
}
