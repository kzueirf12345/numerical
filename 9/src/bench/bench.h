#ifndef MATMUL_SRC_BENCH_BENCH_H
#define MATMUL_SRC_BENCH_BENCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef void (*matmul_t) (
    const float* const mat1, 
    const float* const mat2, 
          float* const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
);

unsigned long long matbench(
    const size_t iterations_cnt,
    const matmul_t matmul, 
    const float* const mat1, 
    const float* const mat2, 
          float* const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
);

#endif /*MATMUL_SRC_BENCH_BENCH_H*/