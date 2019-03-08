#pragma once

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

void tellme() {
#ifdef USING_SSE
  printf("- Using SSE\n");
#endif
#ifdef USING_SIMDPP
  printf("- Using simdpp %s\n", STRINGIZE(SIMDPP_ARCH_NAMESPACE));
#endif
#ifdef USING_DIMSUM
  printf("- Using dimsum\n");
#endif
#ifdef USING_NEON
  printf("- Using neon\n");
#endif
}
