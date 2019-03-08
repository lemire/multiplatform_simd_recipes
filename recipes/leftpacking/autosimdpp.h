#pragma once
#ifdef __SSE2__
#define SIMDPP_ARCH_X86_SSE2
#endif

#ifdef __SSE3__
#define SIMDPP_ARCH_X86_SSE3
#endif

#ifdef __SSSE3__
#define SIMDPP_ARCH_X86_SSSE3
#endif

#ifdef __SSE4_1__
#define SIMDPP_ARCH_X86_SSE4_1
#endif

#ifdef __AVX__
#define SIMDPP_ARCH_X86_AVX
#endif

#ifdef __AVX2__
#define SIMDPP_ARCH_X86_AVX2
#endif

#ifdef __aarch64__
#define SIMDPP_ARCH_ARM_NEON
#endif
