#include <limits.h>
#include <x86intrin.h>
#include <time.h>

#include "bench/bench.h"
#include "logger/src/logger.h"
#include "utils/utils.h"

unsigned long long matbench(
    const size_t iterations_cnt,
    const matmul_t matmul, 
    const float* const mat1, 
    const float* const mat2, 
          float* const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
) {
    lassert(iterations_cnt > 0, "");
    lassert(!is_invalid_ptr(matmul), "");
    lassert(!is_invalid_ptr(mat1), "");
    lassert(!is_invalid_ptr(mat2), "");
    lassert(!is_invalid_ptr(mat_dst), "");
    lassert(i_size > 0, "");
    lassert(j_size > 0, "");
    lassert(k_size > 0, "");

    matmul(mat1, mat2, mat_dst, i_size, j_size, k_size);

    __asm__ volatile("" : : "g" (mat_dst) : "memory");

    unsigned int $dummy$ = 0;
    unsigned long long start = __rdtscp(&$dummy$);

    for (size_t iteration = 0; iteration < iterations_cnt; ++iteration) {
        matmul(mat1, mat2, mat_dst, i_size, j_size, k_size);

        __asm__ volatile("" : : "g" (mat_dst) : "memory");
    }

    unsigned long long end = __rdtscp(&$dummy$);

    return (end - start) / iterations_cnt;
}