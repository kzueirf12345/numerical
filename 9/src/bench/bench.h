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

enum MatBenchError {
    MAT_BENCH_ERROR_SUCCESS            =   0,
    MAT_BENCH_ERROR_STANDARD_ERRNO      =   1,
};

const char* mat_bench_strerror(enum MatBenchError error);

#define MAT_BENCH_ERROR_HANDLE(call_func, ...)                                                          \
    do {                                                                                            \
        enum MatBenchError error_handler = call_func;                                               \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            mat_bench_strerror(error_handler));                                     \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

enum MatBenchError matbench(
    const size_t bucket_iterations_cnt,
    const size_t buckets_cnt,
    const matmul_t matmul,
    const size_t i_size, const size_t j_size, const size_t k_size,
    double* mean_ptr, double* disp_ptr
);

#endif /*MATMUL_SRC_BENCH_BENCH_H*/