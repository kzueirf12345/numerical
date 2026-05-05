#include "rialto/rialto.hpp"

#include <cassert>
#include <cmath>
#include <immintrin.h>
#include <sleef.h> 
#include <thread>
#include <omp.h>

#ifdef _MSC_VER
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

#include "Gen/NormalGen.hpp"
#include "Gen/Rand.hpp"

namespace rialto {

std::vector<float> Pricer::BlackScholes(const BatchContext& ctx) {
    const size_t n = ctx.count;
    std::vector<float> prices(n);

    const size_t vec_size = 16;

    assert(n % vec_size == 0);

    const size_t num_chunks = n / vec_size;

    const __m512 v_half = _mm512_set1_ps(0.5f);
    const __m512 v_one  = _mm512_set1_ps(1.0f);
    const __m512 v_inv_sqrt2 = _mm512_set1_ps(0.70710678118654752440f); 

    #pragma omp parallel for schedule(static)
    for (size_t chunk = 0; chunk < num_chunks; ++chunk) {
        size_t i = chunk * vec_size;

        __m512 v_s0  = _mm512_loadu_ps(&ctx.start_cost[i]);
        __m512 v_k   = _mm512_loadu_ps(&ctx.strike[i]);
        __m512 v_t   = _mm512_loadu_ps(&ctx.period[i]);
        __m512 v_r   = _mm512_loadu_ps(&ctx.rate[i]);
        __m512 v_vol = _mm512_loadu_ps(&ctx.volatility[i]);

        // vol * sqrt(t)
        __m512 v_sqrt_t = _mm512_sqrt_ps(v_t);
        __m512 v_den    = _mm512_mul_ps(v_vol, v_sqrt_t);

        // ln(S0 / K)
        __m512 v_s0_div_k = _mm512_div_ps(v_s0, v_k);
        __m512 v_ln       = Sleef_logf16_u10(v_s0_div_k);

        // (r + 0.5 * vol^2) * t
        __m512 v_vol_sqr = _mm512_mul_ps(v_vol, v_vol);
        __m512 v_drift   = _mm512_add_ps(v_r, _mm512_mul_ps(v_half, v_vol_sqr));
        __m512 v_drift_t = _mm512_mul_ps(v_drift, v_t);

        // d1 = (ln(S0/K) + (r + 0.5 * vol^2) * t) / (vol * sqrt(t))
        __m512 v_d1 = _mm512_div_ps(_mm512_add_ps(v_ln, v_drift_t), v_den);
        
        // d2 = d1 - vol * sqrt(t)
        __m512 v_d2 = _mm512_sub_ps(v_d1, v_den);

        // CDF(x) = 0.5 * (1 + erf(x / sqrt(2)))
        __m512 v_erf1 = Sleef_erff16_u10(_mm512_mul_ps(v_d1, v_inv_sqrt2));
        __m512 v_cdf1 = _mm512_mul_ps(v_half, _mm512_add_ps(v_one, v_erf1));

        __m512 v_erf2 = Sleef_erff16_u10(_mm512_mul_ps(v_d2, v_inv_sqrt2));
        __m512 v_cdf2 = _mm512_mul_ps(v_half, _mm512_add_ps(v_one, v_erf2));

        // exp(-r * t)
        __m512 v_minus_r = _mm512_sub_ps(_mm512_setzero_ps(), v_r);
        __m512 v_exp_rt  = Sleef_expf16_u10(_mm512_mul_ps(v_minus_r, v_t));

        // S0 * CDF(d1) - K * exp(-r*T) * CDF(d2)
        __m512 v_k_exp = _mm512_mul_ps(v_k, v_exp_rt);
        __m512 v_price = _mm512_sub_ps(_mm512_mul_ps(v_s0, v_cdf1), _mm512_mul_ps(v_k_exp, v_cdf2));

        _mm512_storeu_ps(&prices[i], v_price);
    }

    return prices;
}

static inline constexpr const uint64_t BASE_MULTIPLIER = 48271ull;
static inline constexpr const uint64_t MODULUS_POW = 31;
static inline constexpr const uint64_t MODULUS = ((UINT64_C(1) << MODULUS_POW) - 1);

static uint64_t FastPowMod(uint64_t pow) {
    uint64_t base = BASE_MULTIPLIER;
    uint64_t res = 1;

    while (pow > 0) {
        if (pow % 2 == 1) {
            res = (res * base) % MODULUS;
        }
        base = (base * base) % MODULUS;
        pow /= 2;
    }

    return res;
}

std::vector<float> Pricer::MonteCarlo(
    const BatchContext& ctx, const size_t iterations_cnt, const uint64_t base_seed
) {
    const size_t n = ctx.count;
    std::vector<float> prices(n, 0.0);

    const size_t vec_size = 16;

    assert(iterations_cnt % vec_size == 0);

    const size_t steps = iterations_cnt / vec_size;
    const size_t actual_iterations = steps * vec_size;

    assert(steps % 2 == 0); // for correct skip-ahead

    const size_t num_threads_hw = std::thread::hardware_concurrency();
    const size_t num_threads = std::min(num_threads_hw, n);

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    auto worker = [&](size_t start_idx, size_t end_idx) {
        for (size_t i = start_idx; i < end_idx; ++i) {
            
            const uint64_t current_multiplier = FastPowMod(steps * i);
            const uint32_t option_seed = static_cast<uint32_t>((base_seed * current_multiplier) % MODULUS);

            gen::MinstdRandVec base_rng(option_seed);
            gen::NormalGenVec norm_gen(base_rng);

            const float s0    = static_cast<float>(ctx.start_cost[i]);
            const float k     = static_cast<float>(ctx.strike[i]);
            const float t     = static_cast<float>(ctx.period[i]);
            const float r     = static_cast<float>(ctx.rate[i]);
            const float vol   = static_cast<float>(ctx.volatility[i]);

            const float drift  = (r - 0.5f * vol * vol) * t;
            const float stddev = vol * std::sqrt(t);

            const __m512 v_drift  = _mm512_set1_ps(drift);
            const __m512 v_stddev = _mm512_set1_ps(stddev);
            const __m512 v_s0     = _mm512_set1_ps(s0);
            const __m512 v_k      = _mm512_set1_ps(k);
            const __m512 v_zero   = _mm512_setzero_ps();

            __m512 v_sum_payoffs = _mm512_setzero_ps();

            for (size_t step = 0; step < steps; ++step) {
                __m512 v_power = norm_gen(v_drift, v_stddev);
                __m512 v_exp_val = Sleef_expf16_u10(v_power);
                __m512 v_st = _mm512_mul_ps(v_s0, v_exp_val);
                
                __m512 v_diff = _mm512_sub_ps(v_st, v_k);
                __m512 v_payoff = _mm512_max_ps(v_diff, v_zero);
                
                v_sum_payoffs = _mm512_add_ps(v_sum_payoffs, v_payoff);
            }

            float sum_payoffs_scalar = _mm512_reduce_add_ps(v_sum_payoffs);

            prices[i] = std::exp(-ctx.rate[i] * ctx.period[i]) * sum_payoffs_scalar / actual_iterations;
        }
    };

    const size_t chunk_size = n / num_threads;
    const size_t remainder = n % num_threads;
    size_t current_start = 0;

    for (size_t t = 0; t < num_threads; ++t) {
        const size_t current_chunk = chunk_size + (t < remainder);
        const size_t current_end = current_start + current_chunk;

        threads.emplace_back(worker, current_start, current_end);

        current_start = current_end;
    }

    for (auto& t : threads) {
        t.join();
    }

    return prices;
}

} //namespace rialto