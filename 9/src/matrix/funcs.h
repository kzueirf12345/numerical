#ifndef MATMUL_SRC_MATMUL_MATMUL_H
#define MATMUL_SRC_MATMUL_MATMUL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void matmul_correct(
    const float* const mat1, 
    const float* const mat2, 
          float* const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
);

void matmul_optimized(
    const float* restrict const mat1, 
    const float* restrict const mat2, 
          float* restrict const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
);

void matmul_optimized2(
    const float* restrict const mat1, 
    const float* restrict const mat2, 
          float* restrict const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
);

void matfill(
    float* const mat,
    const size_t rows, const size_t cols
);

void matprint(
    FILE* const out,
    float* const mat,
    const size_t rows, const size_t cols
);

bool mat_is_equal(
    const float* const mat1, 
    const float* const mat2,
    const size_t rows, const size_t cols
);

#endif /*MATMUL_SRC_MATMUL_MATMUL_H*/