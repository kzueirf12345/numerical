#include "Testing/Benchmarking.hpp"

#include <cmath>
#include <concepts>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdbool.h>
#include <unistd.h>
#include <sched.h> 

#include "Rand/Rand.hpp"
#include "Measurer/Measurer.hpp"

namespace minstd_rand {

void BenchThroughput(
    std::ostream& out, uint32_t seed, size_t buckets, size_t batches, size_t iterations
) {
    std::minstd_rand std_gen(seed);
    auto std_rand_func = [&std_gen](size_t /*i*/) {
        return std_gen();
    };

    const measurer::Val res_std = measurer::Runner::benchThroughput(
        buckets, batches, iterations, measurer::Runner::VoidSetup, std_rand_func
    );

    minstd_rand::MinstdRand my_scalar_gen(seed);
    auto my_scalar_func = [&my_scalar_gen](size_t /*i*/) {
        return my_scalar_gen();
    };

    const measurer::Val res_scalar = measurer::Runner::benchThroughput(
        buckets, batches, iterations, measurer::Runner::VoidSetup, my_scalar_func
    );

    minstd_rand::MinstdRandVec my_vec_gen(seed);
    auto my_vec_func = [&my_vec_gen](size_t /*i*/) {
        return my_vec_gen();
    };

    const measurer::Val res_vec = measurer::Runner::benchThroughput(
        buckets, batches, iterations, measurer::Runner::VoidSetup, my_vec_func
    );

    auto print_row = [&out](const std::string& name, measurer::Val v, double factor = 1.0) {
        out << std::left << std::setw(25) << name 
            << std::fixed << std::setprecision(5)
            << "Mean: " << std::setw(10) << v.mean * factor << " clks | "
            << "StdDev: " << v.stddev * factor << " clks" << std::endl;
    };
    
    out << "\n===Throughput Benchmark===\n";
    print_row("std::minstd_rand", res_std);
    print_row("My MinstdRand", res_scalar);
    print_row("My MinstdRandVec", res_vec, 1.0 / 16.0);
}

void BenchLatency(std::ostream& out, uint32_t seed, size_t buckets, size_t iterations) {

    std::minstd_rand std_gen(seed);
    auto std_rand_func = [&std_gen]() {
        return std_gen();
    };

    const measurer::Val res_std = measurer::Runner::benchLatency(
        buckets, iterations, measurer::Runner::VoidSetup, std_rand_func
    );

    minstd_rand::MinstdRand my_scalar_gen(seed);
    auto my_scalar_func = [&my_scalar_gen]() {
        my_scalar_gen();
    };

    const measurer::Val res_scalar = measurer::Runner::benchLatency(
        buckets, iterations, measurer::Runner::VoidSetup, my_scalar_func
    );

    minstd_rand::MinstdRandVec my_vec_gen(seed);
    auto my_vec_func = [&my_vec_gen]() {
        my_vec_gen();
    };

    const measurer::Val res_vec = measurer::Runner::benchLatency(
        buckets, iterations, measurer::Runner::VoidSetup, my_vec_func
    );

    auto print_row = [&out](const std::string& name, measurer::Val v, double factor = 1.0) {
        out << std::left << std::setw(25) << name 
            << std::fixed << std::setprecision(5)
            << "Mean: " << std::setw(10) << v.mean * factor << " clks | "
            << "StdDev: " << v.stddev * factor << " clks" << std::endl;
    };
    
    out << "\n===Latency Benchmark===\n";
    print_row("std::minstd_rand", res_std);
    print_row("My MinstdRand", res_scalar);
    print_row("My MinstdRandVec", res_vec, 1.0 / 16.0);
}

struct PiThreadData {
    size_t iterations = 0;
    uint32_t seed = 0;
    size_t hits = 0;
};

//=====================================PI BENCH=====================================================

static inline constexpr const uint64_t BASE_MULTIPLIER = 48271ull;
static inline constexpr const uint64_t MODULUS_POW = 31;
static inline constexpr const uint64_t MODULUS = ((UINT64_C(1) << MODULUS_POW) - 1);
static inline constexpr const uint64_t MODULUS2 = MODULUS * MODULUS;

static uint64_t fast_pow_mod(uint64_t pow) {
    uint64_t res = 1;

    while (pow > 0) {
        if (pow % 2 == 0) {
            res *= res;
            pow /= 2;
        }
        else {
            res *= BASE_MULTIPLIER;
            --pow;
        }
        res %= MODULUS;
    }

    return res;
}

static std::vector<PiThreadData> GeneratePiThreadsData(
    const size_t cores_cnt, const size_t iterations, const uint32_t seed
) {
    assert(cores_cnt > 0);
    assert(iterations > 0);

    const size_t iterations_by_thread = iterations / cores_cnt;
    const uint64_t multiplier = fast_pow_mod(iterations_by_thread);
    
    std::vector<PiThreadData> threads_data(
        cores_cnt, 
        {.iterations = iterations_by_thread, .seed = seed, .hits = 0}
    );

    threads_data[0].iterations += iterations % cores_cnt;

    for (size_t thread_num = 1; thread_num < cores_cnt; ++thread_num) {
        threads_data[thread_num].seed = static_cast<uint32_t>(
            (static_cast<uint64_t>(threads_data[thread_num - 1].seed) * multiplier) % MODULUS
        );
    }

    return threads_data;
}

template<typename T>
concept VectorGenerator = requires(T gen) {
    { gen() };
    requires std::is_same_v<decltype(gen()), __m512i>;
};

template<typename T>
concept ScalarGenerator = requires(T gen) {
    { gen() } -> std::convertible_to<uint32_t>;
};

template<typename T>
concept Generator = minstd_rand::ScalarGenerator<T> || minstd_rand::VectorGenerator<T>;


template <Generator GenT>
static void* PiThreadFoo(void* arg) {
    PiThreadData* data = static_cast<PiThreadData*>(arg);
    GenT gen(data->seed);
    size_t local_hits = 0;

    if constexpr (VectorGenerator<GenT>) {
        assert(data->iterations % 8 == 0);

        const static __m512i modulus2_vec = _mm512_set1_epi64(MODULUS2);

        const size_t iterations = data->iterations / 8;

        for (size_t iteration = 0; iteration < iterations; ++iteration) {
            const __m512i vec = gen();

            const __m512i x2_vec = _mm512_mul_epu32(vec, vec);
            const __m512i vec_shifted = _mm512_srli_epi64(vec, 32);
            const __m512i y2_vec  = _mm512_mul_epu32(vec_shifted, vec_shifted);

            const __m512i dist2_vec = _mm512_add_epi64(x2_vec, y2_vec);

            const __mmask8 mask = _mm512_cmple_epu64_mask(dist2_vec, modulus2_vec);

            local_hits += _mm_popcnt_u32(mask);
        }
    }
    else if constexpr (ScalarGenerator<GenT>) {
        for (size_t iteration = 0; iteration < data->iterations; ++iteration) {
            const uint64_t x = static_cast<uint64_t>(gen());
            const uint64_t y = static_cast<uint64_t>(gen());

            local_hits += (x * x + y * y <= MODULUS2);
        }
    }
    
    data->hits = local_hits;

    return data;
}

struct PiMeasurement {
    double pi = NAN;
    uint64_t time = 0;
};

template <Generator GenT>
PiMeasurement BenchSomeGen(
    const size_t cores_cnt, const size_t iterations, const uint32_t seed
) {
    std::vector<PiThreadData> threads_data{
        GeneratePiThreadsData(cores_cnt, iterations, seed)
    };
    std::vector<pthread_t> threads(cores_cnt);

    size_t hits = 0;

    _mm_lfence();
    const uint64_t start = __rdtsc();
    _mm_lfence();

    for (size_t thread_num = 0; thread_num < cores_cnt; ++thread_num) {
        pthread_create(
            &threads[thread_num], nullptr, PiThreadFoo<GenT>, &threads_data[thread_num]
        );
    }

    for (size_t thread_num = 0; thread_num < cores_cnt; ++thread_num) {
        pthread_join(threads[thread_num], nullptr);
        hits += threads_data[thread_num].hits;
    }

    _mm_lfence();
    const uint64_t end = __rdtsc();
    _mm_lfence();

    const uint64_t time = end - start;

    const double pi = 4. * static_cast<double>(hits) / iterations;

    return {.pi = pi, .time = time};
}


void BenchPi(
    std::ostream& out, uint32_t seed, size_t iterations
) {
    const size_t cores_cnt = sysconf(_SC_NPROCESSORS_ONLN);

    PiMeasurement res_scalar = BenchSomeGen<MinstdRand>      (cores_cnt, iterations, seed);
    PiMeasurement    res_std = BenchSomeGen<std::minstd_rand>(cores_cnt, iterations, seed);
    PiMeasurement    res_vec = BenchSomeGen<MinstdRandVec>   (cores_cnt, iterations, seed);

    auto print_row = [&out](const std::string& name, PiMeasurement res) {
        out << std::left << std::setw(25) << name 
            << std::fixed
            << "Pi:   " << std::right  << std::setprecision(10) << std::setw(20) << res.pi << " | "        
            << "Time: " << std::right  << std::setw(15) << res.time << " clks\n";
    };

    out << "\n===Pi Benchmark===\n"
        << std::right << std::setw(28) 
        << "Correct Pi:" 
        << std::fixed << std::setw(23) 
        << "3.1415265359" << "\n";
    print_row("std::minstd_rand", res_std);
    print_row("My MinstdRand", res_scalar);
    print_row("My MinstdRandVec", res_vec);
}

} // namespace minstd_rand