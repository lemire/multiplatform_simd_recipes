#include "benchmark.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <cassert>

#include "scalar_findfirst.h"
#include "sse_findfirst.h"
#include "avx_findfirst.h"
#include "simdpp_findfirst.h"
//#include "dimsum_findfirst.h"
#include "neon_findfirst.h"
#include "simdutil.h"

void demo() {
  printf("[demo]\n");
  const char target = 'x';
  size_t N = 1024;
  const int repeat = 5;
  char *array = (char *) malloc((N + 1) * sizeof(char));
  array[N] = 0;
  for(size_t matches = 1; matches <= N; matches *= 2) {
    memset(array,' ',N * sizeof(char));
    for(size_t k = 0; k < matches; k++) {
      array[rand() % N] = target;
    }
    size_t count = scalar_findall(array, N, target);

    printf("N = %zu, matches = %zu, density %f \n", N, count,(float) count / (float) N);


    BEST_TIME(scalar_findall(array, N, target), count, , repeat,  N, true);
    BEST_TIME(strchr_findall(array, N, target), count, , repeat,  N, true);
#ifdef USING_SSE
    BEST_TIME(sse_findall(array, N, target), count, , repeat,  N, true);
    BEST_TIME(sse_movemask_findall(array, N, target), count, , repeat,  N, true);
#endif
#ifdef USING_AVX
    BEST_TIME(avx_findall(array, N, target), count, , repeat,  N, true);
    BEST_TIME(avx_movemask_findall(array, N, target), count, , repeat,  N, true);
#endif
#ifdef USING_NEON
    BEST_TIME(neon_findall(array, N, target), count, , repeat,  N, true);
    BEST_TIME(neon_twostep_findall(array, N, target), count, , repeat,  N, true);
    BEST_TIME(neon_movemask_findall(array, N, target), count, , repeat,  N, true);
#endif
#ifdef USING_SIMDPP
    BEST_TIME(simdpp_findall(array, N, target), count, , repeat,  N, true);
    BEST_TIME(simdpp_movemask_findall(array, N, target), count, , repeat,  N, true);
#endif
    printf("===\n");

  }
  free(array);
}
int main() {
  demo();
  return EXIT_SUCCESS;
}
