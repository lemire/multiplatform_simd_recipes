// Rewritten from https://github.com/lemire/SIMDxorshift (copyright Daniel
// Lemire, Apache 2.0 license): wrapped in class, ported to SIMD API.
class Xorshift128Plus {
 public:
  // 2 or 4 separate instances of Xorshift128+ running in parallel.
  static constexpr size_t N = SIMD_MIN(Full<uint64_t>::N, 4);

  using D = Part<uint64_t, N>;

  Xorshift128Plus(const uint64_t key1, const uint64_t key2) {
    state_[0] = key1;
    state_[N] = key2;
    Jump(state_[0], state_[N + 0], state_ + 1, state_ + N + 1);
    if (N == 4) {
      Jump(state_[1], state_[N + 1], state_ + 2, state_ + N + 2);
      Jump(state_[2], state_[N + 2], state_ + 3, state_ + N + 3);
    }
  }

  // Returns 128 or 256 bit vector of random bits (u64 lane type).
  D::V operator()() {
    const D d;
    auto s1 = load(d, state_ + 0);
    const auto s0 = load(d, state_ + N);
    const auto ret = s0 + s1;
    store(s0, d, state_);
    s1 ^= shift_left<23>(s1);
    const auto new_s1 = s1 ^ s0 ^ shift_right<18>(s1) ^ shift_right<5>(s0);
    store(new_s1, d, state_ + N);
    return ret;
  }

 private:
  static void NextForJump(uint64_t* SIMD_RESTRICT ps0,
                          uint64_t* SIMD_RESTRICT ps1) {
    uint64_t s1 = *ps0;
    const uint64_t s0 = *ps1;
    *ps0 = s0;
    s1 ^= s1 << 23;
    *ps1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
  }

  static void Jump(uint64_t in1, uint64_t in2, uint64_t* SIMD_RESTRICT output1,
                   uint64_t* SIMD_RESTRICT output2) {
    static const uint64_t kBits[] = {0x8A5CD789635D2DFFull,
                                     0x121FD2155C472F96ull};
    uint64_t s0 = 0;
    uint64_t s1 = 0;
    for (size_t i = 0; i < 2; ++i) {
      for (size_t b = 0; b < 64; b++) {
        if (kBits[i] & (1ULL << b)) {
          s0 ^= in1;
          s1 ^= in2;
        }
        NextForJump(&in1, &in2);
      }
    }
    output1[0] = s0;
    output2[0] = s1;
  }

  SIMD_ALIGN uint64_t state_[N * 2];
};
