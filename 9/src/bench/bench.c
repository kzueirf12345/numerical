#include <limits.h>
#include <math.h>
#include <x86intrin.h>
#include <time.h>

#include "bench/bench.h"
#include "logger/src/logger.h"
#include "matrix/funcs.h"
#include "utils/utils.h"

#define CASE_ENUM_TO_STR_(error) case error: return #error;
const char* mat_bench_strerror(enum MatBenchError error) {
    switch (error) {
        CASE_ENUM_TO_STR_(MAT_BENCH_ERROR_SUCCESS)
        CASE_ENUM_TO_STR_(MAT_BENCH_ERROR_STANDARD_ERRNO)
        default:
            return "UNKNOWN_ERROR";
    }

    return "UNKNOWN_ERROR";
}
#undef CASE_ENUM_TO_STR_

enum MatBenchError matbench(
    const size_t bucket_iterations_cnt,
    const size_t buckets_cnt,
    const matmul_t matmul,
    const size_t i_size, const size_t j_size, const size_t k_size,
    double* mean_ptr, double* disp_ptr
) {
    lassert(buckets_cnt > 0, "");
    lassert(bucket_iterations_cnt > 0, "");
    lassert(!is_invalid_ptr(matmul), "");
    lassert(!is_invalid_ptr(mean_ptr), "");
    lassert(!is_invalid_ptr(disp_ptr), "");
    lassert(i_size > 0, "");
    lassert(j_size > 0, "");
    lassert(k_size > 0, "");

    float* const mat1       = (float*)calloc(i_size * j_size, sizeof(*mat1));

    if (!mat1) {
        perror("Can't calloc mat1");
        return MAT_BENCH_ERROR_STANDARD_ERRNO;
    }

    float* const mat2       = (float*)calloc(j_size * k_size, sizeof(*mat2));

    if (!mat2) {
        perror("Can't calloc mat2");
        free(mat1);
        return MAT_BENCH_ERROR_STANDARD_ERRNO;
    }

    float* const mat_dst    = (float*)calloc(i_size * k_size, sizeof(*mat_dst));

    if (!mat_dst) {
        perror("Can't calloc mat_dst");
        free(mat1);
        free(mat2);
        return MAT_BENCH_ERROR_STANDARD_ERRNO;
    }

    double* const buckets_mins = (double*)calloc(buckets_cnt, sizeof(*buckets_mins));

    if (!buckets_mins) {
        perror("Can't calloc buckets_time");
        free(mat1);
        free(mat2);
        free(mat_dst);
        return MAT_BENCH_ERROR_STANDARD_ERRNO;
    }

    double mean = 0;

    for (size_t bucket = 0; bucket < buckets_cnt; ++bucket) {
        double min = INFINITY;

        for (size_t iteration = 0; iteration < bucket_iterations_cnt; ++iteration) {
            matfill(mat1, i_size, j_size);
            matfill(mat2, j_size, k_size);
            
            matmul(mat1, mat2, mat_dst, i_size, j_size, k_size); // Прогрев

            unsigned int $dummy$ = 0;
            const unsigned long long start = __rdtscp(&$dummy$);

            matmul(mat1, mat2, mat_dst, i_size, j_size, k_size);

            const unsigned long long end = __rdtscp(&$dummy$);

            const double time = (double)(end - start);
            if (time < min) {
                min = time;
            }
        }

        buckets_mins[bucket] = min;
        mean += min;
    }

    free(mat1);
    free(mat2);
    free(mat_dst);

    mean = mean / (double)buckets_cnt;

    double disp = 0;

    for (size_t time_ind = 0; time_ind < buckets_cnt; ++time_ind) {
        const double x = buckets_mins[time_ind];
        const double dev = x - mean;
        disp += dev * dev;
    }

    disp = disp / (double)(buckets_cnt - 1);

    *mean_ptr = mean;
    *disp_ptr = disp;

    free(buckets_mins);

    return MAT_BENCH_ERROR_SUCCESS;
}