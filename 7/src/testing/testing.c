#include "testing.h"

#include <omp.h>
#include <math.h>
#include <float.h>
#include <mpfr.h>

#include "utils/utils.h"
#include "utils/concole.h"
#include "vogf/vogf.h"
#include "logger/liblogger.h" // IWYU pragma: keep

#pragma GCC diagnostic ignored "-Wfloat-equal"

#define CRITICAL_ULP_ERROR_ (3.5f)

static float calculate_ulp_diff(float actual, double expected) {

    if (isnan(actual) || isnan(expected)) {
        return (isnan(actual) && isnan(expected)) ? 0.0f : FLT_MAX;
    }

    if (isinf(actual) || isinf(expected)) {
        return (actual == (float)expected) ? 0.0f : FLT_MAX;
    }

    if (actual == 0.f && expected == 0.f) {
        return (signbit(actual) == signbit(expected)) ? 0.0f : FLT_MAX;
    }

    if (actual == (float)expected) {
        return 0.0f;
    }

    float ulp_error = 0.5;
    for (; (actual = nextafterf(actual, (float)expected)) != (float)expected; ++ulp_error);

    return ulp_error;
}

int test_vogf(
    const float tests[], 
    const size_t tests_size, 
    const char* testcase_name, 
    FILE* const output,
    bool only_incorrect
) {
    lassert(!is_invalid_ptr(tests), "testcases array is not valid");
    lassert(tests_size > 0, "testcases size is zero");
    lassert(!is_invalid_ptr(output), "output stream is not valid");

    const bool colored_text_supported = (output == stdout);

    if (testcase_name) {
        fprintf(output, "\n%s\n", testcase_name);
    }

    mpfr_t x_mpfr;
    mpfr_t res_mpfr;
    mpfr_init2(x_mpfr, 128);
    mpfr_init2(res_mpfr, 128);

    int failed_cnt = 0;

    float max_ulp_error = 0;

    for (size_t test_ind = 0; test_ind < tests_size; ++test_ind) {
        const float test_num = tests[test_ind];

        const float res_vogf = vogf(test_num);
        
        mpfr_set_d(x_mpfr, test_num, MPFR_RNDN); 
        mpfr_log(res_mpfr, x_mpfr, MPFR_RNDN);

        const double expected = mpfr_get_d(res_mpfr, MPFR_RNDN);

        const float ulp_error = calculate_ulp_diff(res_vogf, expected);

        max_ulp_error = MAX(ulp_error, max_ulp_error);

        if (!only_incorrect || ulp_error > CRITICAL_ULP_ERROR_) {
            fprintf(
                output,
                "x: %.20f\t|\tVogf: %.20f\t|\t Expected: %.20f\t|\tULP Error: %.1f\n", 
                test_num, res_vogf, (float)expected, ulp_error
            );
        }

        if (ulp_error > CRITICAL_ULP_ERROR_) {
            if (colored_text_supported) {
                fprintf(
                    output,
                    RED_TEXT("ULP more then critical value %g. ULP: %g\n"), 
                    CRITICAL_ULP_ERROR_, ulp_error
                );
            }
            else {
                fprintf(
                    output,
                    "ULP more then critical value %.1f. ULP: %.1f\n", 
                    CRITICAL_ULP_ERROR_, ulp_error
                );
            }
            ++failed_cnt;
        }
    }

    mpfr_clear(x_mpfr);
    mpfr_clear(res_mpfr);

    if (failed_cnt == 0) {
        if (colored_text_supported) {
            fprintf(output, GREEN_TEXT("ALL TEST PASSED!!!\n"));
        }
        else {
            fprintf(output, "ALL TEST PASSED!!!\n");
        }
    }
    else {
        if (colored_text_supported) {
            fprintf(output, RED_TEXT("%d TESTS NOT PASSED!!!\n"), failed_cnt);
        }
        else {
            fprintf(output, "%d TESTS NOT PASSED!!!\n", failed_cnt);
        }
    }

    if (colored_text_supported) {
        fprintf(output, YELLOW_TEXT("Max ULP error: %.1f\n"), max_ulp_error);
    }
    else {
        fprintf(output, "Max ULP error: %.1f\n", max_ulp_error);
    }

    return failed_cnt;
}


int test_all_vogf(FILE* const output) {
    lassert(!is_invalid_ptr(output), "output stream is not valid");

    const bool colored_text_supported = (output == stdout);

    int total_failed = 0;
    float global_max_ulp = 0.0f;

    const uint32_t max_bits = 0x7F800000; 
    
    const uint32_t progress_step = max_bits / 10000;

    double start_time = omp_get_wtime();

    printf("\nTest all positive correct values\n");

    #pragma omp parallel
    {
        mpfr_t x_mpfr, res_mpfr;
        mpfr_init2(x_mpfr, 128);
        mpfr_init2(res_mpfr, 128);

        int local_failed = 0;
        float local_max_ulp = 0.0f;

        #pragma omp for schedule(static, 65536)
        for (uint32_t i = 0; i <= max_bits; ++i) {
            static_assert(sizeof(float) == sizeof(i), "for bit cast");
            float test_num = *(float*)&i;

            float res_vogf = vogf(test_num);

            mpfr_set_flt(x_mpfr, test_num, MPFR_RNDN);
            mpfr_log(res_mpfr, x_mpfr, MPFR_RNDN);
            double expected = mpfr_get_d(res_mpfr, MPFR_RNDN);

            float ulp_error = calculate_ulp_diff(res_vogf, expected);

            local_max_ulp = MAX(ulp_error, local_max_ulp);
            
            if (ulp_error > CRITICAL_ULP_ERROR_) {
                #pragma omp critical
                {
                    fprintf(
                        output,
                        "x: %.20e\t|\tVogf: %.20e\t|\t Expected: %.20e\t|\tULP Error: %.1f\n", 
                        test_num, res_vogf, (float)expected, ulp_error
                    );
                    ++local_failed;
                }
            }

            if (omp_get_thread_num() == 0 && i % progress_step == 0) {
                printf("\rProgress: %.2f%%...", ((double)i * 100. / max_bits));
                fflush(stdout);
            }
        }

        #pragma omp critical
        {
            total_failed += local_failed;
            global_max_ulp = MAX(local_max_ulp, global_max_ulp);
        }

        mpfr_clear(x_mpfr);
        mpfr_clear(res_mpfr);
    }

    printf("\rProgress: 100%%...");
    fflush(stdout);

    double end_time = omp_get_wtime();
    printf("\nTest finished in %.2f seconds.\n", end_time - start_time);

    if (total_failed == 0) {
        if (colored_text_supported) {
            fprintf(output, GREEN_TEXT("ALL TEST PASSED!!!\n"));
        }
        else {
            fprintf(output, "ALL TEST PASSED!!!\n");
        }
    }
    else {
        if (colored_text_supported) {
            fprintf(output, RED_TEXT("%d TESTS NOT PASSED!!!\n"), total_failed);
        }
        else {
            fprintf(output, "%d TESTS NOT PASSED!!!\n", total_failed);
        }
    }

    if (colored_text_supported) {
        fprintf(output, YELLOW_TEXT("Max ULP error: %.1f\n"), global_max_ulp);
    }
    else {
        fprintf(output, "Max ULP error: %.1f\n", global_max_ulp);
    }

    return total_failed;
}