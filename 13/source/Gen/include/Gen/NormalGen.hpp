#pragma once

#include <immintrin.h>
#include <sleef.h>
#include <numbers>

#include "Rand.hpp"

namespace gen {

class NormalGenVec {

public:

    NormalGenVec(MinstdRandVec base_rng) 
        : base_rng_(base_rng) {}

    __m512 operator()(__m512 mean, __m512 stddev) {
        if (is_cached_) {
            is_cached_ = false;
            return _mm512_fmadd_ps(stddev, cache_val_, mean);
        }

        __m512 u1 = base_rng_.Getf01();
        __m512 u2 = base_rng_.Getf01();

        // sqrt(-2 * ln(u1))
        const __m512 minus_two = _mm512_set1_ps(-2.0f);
        __m512 ln_u1 = Sleef_logf16_u10(u1);
        __m512 first_mul = _mm512_sqrt_ps(_mm512_mul_ps(minus_two, ln_u1));

        // 2 * pi * u2
        const __m512 two_pi = _mm512_set1_ps(2.0f * std::numbers::pi_v<float>);
        __m512 trig_arg = _mm512_mul_ps(two_pi, u2);

        __m512 cos_vals = Sleef_cosf16_u10(trig_arg);
        __m512 sin_vals = Sleef_sinf16_u10(trig_arg);

        __m512 z1 = _mm512_mul_ps(first_mul, cos_vals);
        __m512 z2 = _mm512_mul_ps(first_mul, sin_vals);

        cache_val_ = z2;
        is_cached_ = true;

        return _mm512_fmadd_ps(stddev, z1, mean);
    }

private:

    MinstdRandVec base_rng_;
    bool is_cached_ = false;
    __m512 cache_val_;
    
};

} // namespace rialto