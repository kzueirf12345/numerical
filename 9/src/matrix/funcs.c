#include <math.h>

#include "matrix/funcs.h"
#include "logger/src/logger.h"
#include "utils/utils.h"

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

    const size_t mat_dst_size = i_size * k_size;

    for (size_t ind = 0; ind < mat_dst_size; ++ind) {
        mat_dst[ind] = 0;
    }

    for (size_t i = 0; i < i_size; ++i) {
        for (size_t j = 0; j < j_size; ++j) {
            for (size_t k = 0; k < k_size; ++k) {
                mat_dst[i * k_size + k] += mat1[i * j_size + j] * mat2[j * k_size + k];
            }
        }
    }
}

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