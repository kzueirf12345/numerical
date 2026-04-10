#include <math.h>
#include <string.h>

#include "matrix/funcs.h"
#include "logger/src/logger.h"
#include "utils/utils.h"
#include "utils/concole.h"

void matmul_correct(
    const float* const mat1, 
    const float* const mat2, 
          float* const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
) {
    lassert(!is_invalid_ptr(mat1), "");
    lassert(!is_invalid_ptr(mat2), "");
    lassert(!is_invalid_ptr(mat_dst), "");
    lassert(i_size > 0, "");
    lassert(j_size > 0, "");
    lassert(k_size > 0, "");

    for (size_t i = 0; i < i_size; ++i) {
        for (size_t k = 0; k < k_size; ++k) {
            mat_dst[i * k_size + k] = 0;

            for (size_t j = 0; j < j_size; ++j) {
                mat_dst[i * k_size + k] += mat1[i * j_size + j] * mat2[j * k_size + k];
            }
        }
    }
}

void matmul_optimized(
    const float* restrict const mat1, 
    const float* restrict const mat2, 
          float* restrict const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
) {
    lassert(!is_invalid_ptr(mat1), "");
    lassert(!is_invalid_ptr(mat2), "");
    lassert(!is_invalid_ptr(mat_dst), "");
    lassert(i_size > 0, "");
    lassert(j_size > 0, "");
    lassert(k_size > 0, "");

    memset(mat_dst, 0, i_size * k_size * sizeof(*mat_dst));

    for (size_t i = 0; i < i_size; ++i) {
        for (size_t j = 0; j < j_size; ++j) {
            for (size_t k = 0; k < k_size; ++k) {
                mat_dst[i * k_size + k] += mat1[i * j_size + j] * mat2[j * k_size + k];
            }
        }
    }
}

#define BLOCK_HEIGHT_ (8)
#define BLOCK_WIDTH_  (16)
void matmul_optimized2(
    const float* restrict const mat1, 
    const float* restrict const mat2, 
          float* restrict const mat_dst,
    const size_t i_size, const size_t j_size, const size_t k_size
) {
    lassert(!is_invalid_ptr(mat1), "");
    lassert(!is_invalid_ptr(mat2), "");
    lassert(!is_invalid_ptr(mat_dst), "");
    lassert(i_size > 0, "");
    lassert(j_size > 0, "");
    lassert(k_size > 0, "");
    lassert(((uintptr_t)mat1    % ALIGNMENT) == 0, "Memory is not aligned");
    lassert(((uintptr_t)mat2    % ALIGNMENT) == 0, "Memory is not aligned");
    lassert(((uintptr_t)mat_dst % ALIGNMENT) == 0, "Memory is not aligned");

    
    const float* __restrict a = __builtin_assume_aligned(mat1, ALIGNMENT);
    const float* __restrict b = __builtin_assume_aligned(mat2, ALIGNMENT);
    float* __restrict       c = __builtin_assume_aligned(mat_dst, ALIGNMENT);

    memset(c, 0, i_size * k_size * sizeof(*c));

    for (size_t i = 0; i < i_size; i += BLOCK_HEIGHT_) {
        for (size_t k = 0; k < k_size; k += BLOCK_WIDTH_) {
            
            float acc0[BLOCK_WIDTH_] = {0}; 
            float acc1[BLOCK_WIDTH_] = {0}; 
            float acc2[BLOCK_WIDTH_] = {0}; 
            float acc3[BLOCK_WIDTH_] = {0};
            float acc4[BLOCK_WIDTH_] = {0}; 
            float acc5[BLOCK_WIDTH_] = {0}; 
            float acc6[BLOCK_WIDTH_] = {0}; 
            float acc7[BLOCK_WIDTH_] = {0};

            for (size_t j = 0; j < j_size; ++j) {
                
                const float* b_row = &b[j * k_size + k];

                const float a0 = a[(i + 0) * j_size + j];
                const float a1 = a[(i + 1) * j_size + j];
                const float a2 = a[(i + 2) * j_size + j];
                const float a3 = a[(i + 3) * j_size + j];
                const float a4 = a[(i + 4) * j_size + j];
                const float a5 = a[(i + 5) * j_size + j];
                const float a6 = a[(i + 6) * j_size + j];
                const float a7 = a[(i + 7) * j_size + j];

                for (size_t v = 0; v < BLOCK_WIDTH_; ++v) {
                    acc0[v] += a0 * b_row[v];
                    acc1[v] += a1 * b_row[v];
                    acc2[v] += a2 * b_row[v];
                    acc3[v] += a3 * b_row[v];
                    acc4[v] += a4 * b_row[v];
                    acc5[v] += a5 * b_row[v];
                    acc6[v] += a6 * b_row[v];
                    acc7[v] += a7 * b_row[v];
                }
            }

            for (size_t v = 0; v < BLOCK_WIDTH_; ++v) {
                c[(i + 0) * k_size + k + v] = acc0[v];
                c[(i + 1) * k_size + k + v] = acc1[v];
                c[(i + 2) * k_size + k + v] = acc2[v];
                c[(i + 3) * k_size + k + v] = acc3[v];
                c[(i + 4) * k_size + k + v] = acc4[v];
                c[(i + 5) * k_size + k + v] = acc5[v];
                c[(i + 6) * k_size + k + v] = acc6[v];
                c[(i + 7) * k_size + k + v] = acc7[v];
            }
        }
    }
}
#undef BLOCK_HEIGHT_
#undef BLOCK_WIDTH_

void matfill(
    float* const mat,
    const size_t rows, const size_t cols
) {
    lassert(!is_invalid_ptr(mat), "");
    lassert(rows > 0, "");
    lassert(cols > 0, "");

    const size_t size = rows * cols;
    for (size_t ind = 0; ind < size; ++ind) {
        mat[ind] = (float)rand() / (float)(RAND_MAX);
    }
}

void matprint(
    FILE* const out,
    float* const mat,
    const size_t rows, const size_t cols
) {
    lassert(!is_invalid_ptr(mat), "");
    lassert(!is_invalid_ptr(out), "");
    lassert(rows > 0, "");
    lassert(cols > 0, "");

    fprintf(out, "Matrix [%zu X %zu]\n", rows, cols);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            fprintf(out, "%10g\t", mat[row * cols + col]);
        }
        fprintf(out, "\n");
    }

    fprintf(out, "\n");
}

bool mat_is_equal(
    const float* const mat1, 
    const float* const mat2,
    const size_t rows, const size_t cols
) {
    lassert(!is_invalid_ptr(mat1), "");
    lassert(!is_invalid_ptr(mat2), "");
    lassert(rows > 0, "");
    lassert(cols > 0, "");

    const size_t size = rows * cols;
    for (size_t ind = 0; ind < size; ++ind) {
        if (fabsf(mat1[ind] - mat2[ind]) > FLOAT_EPSILON) {
            return false;
        }
    }

    return true;
}