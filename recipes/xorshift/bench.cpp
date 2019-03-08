#include "benchmark.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>

#include "scalar_xorshift.h"
#include "sse_xorshift.h"
#include "avx_xorshift.h"
#include "simdpp_xorshift.h"
#include "dimsum_xorshift.h"
#include "neon_xorshift.h"
#include "simdutil.h"



void demo(size_t N) {
  uint64_t *data = new uint64_t[N];
  int size = N;
  bool verbose = true;
  int repeat = 50;
  uint64_t bogus = 0;

  BEST_TIME_NOCHECK(scalar_fillrandomarray(data, N),memset(data,0,N*sizeof(uint64_t)), repeat,
                    size, verbose);
  bogus += data[0];
#ifdef USING_SSE
  BEST_TIME_NOCHECK(sse_fillrandomarray(data, N),, repeat,
                    size, verbose);
  bogus += data[0];
#endif
#ifdef USING_AVX
  BEST_TIME_NOCHECK(avx_fillrandomarray(data, N),, repeat,
                    size, verbose);
  bogus += data[0];
#endif
#ifdef USING_DIMSUM
  BEST_TIME_NOCHECK(dimsum_fillrandomarray(data, N),, repeat,
                    size, verbose);
  bogus += data[0];
#endif
#ifdef USING_SIMDPP
  BEST_TIME_NOCHECK(simdpp_fillrandomarray(data, N),, repeat,
                    size, verbose);
  bogus += data[0];
#endif
#ifdef USING_NEON
  BEST_TIME_NOCHECK(neon_fillrandomarray(data, N),, repeat,
                    size, verbose);
  bogus += data[0];
#endif
  printf("bogus: %d \n",(int) bogus);
  delete[] data;
}

int main() {
  tellme();
  demo(50000);
  printf("Code is good\n");
}
