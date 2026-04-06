#include "testing.h"

#include <omp.h>
#include <math.h>
#include <float.h>
#include <mpfr.h>
#include <errno.h>
#include <fenv.h>

#include "utils/utils.h"
#include "utils/concole.h"
#include "vogf/vogf.h"
#include "logger/liblogger.h" // IWYU pragma: keep

#pragma GCC diagnostic ignored "-Wfloat-equal"

#define CRITICAL_ULP_ERROR_ (3.5f)


static float calculate_ulp_diff_(float actual, double expected) {

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

    const uint64_t expected_u = as_uint64(expected);
    const uint64_t expected_exp = expected_u & DOUBLE_EXP_MASK;
    const uint64_t ulp_u = expected_exp - ((uint64_t)FLOAT_MANT_SIZE << DOUBLE_MANT_SIZE);
    const double ulp = as_double(ulp_u);

    double ulp_error = fabs((double)actual - expected) / ulp;

    return (float)ulp_error;
}

static void print_final_res_(int failed_cnt, FILE* const output) {
    const bool colored_text_supported = (output == stdout);

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
}

int vogf_test_res(
    const float testcase[], 
    const size_t testcase_size, 
    const char* testcase_name, 
    FILE* const output,
    bool only_incorrect
) {
    lassert(!is_invalid_ptr(testcase), "testcase array is not valid");
    lassert(testcase_size > 0, "testcase array size is zero");
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

    for (size_t test_ind = 0; test_ind < testcase_size; ++test_ind) {
        const float test_num = testcase[test_ind];

        const float res_vogf = vogf(test_num);
        
        mpfr_set_d(x_mpfr, test_num, MPFR_RNDN); 
        mpfr_log(res_mpfr, x_mpfr, MPFR_RNDN);

        const double expected = mpfr_get_d(res_mpfr, MPFR_RNDN);

        const float ulp_error = calculate_ulp_diff_(res_vogf, expected);

        max_ulp_error = MAX(ulp_error, max_ulp_error);

        if (!only_incorrect || ulp_error > CRITICAL_ULP_ERROR_) {
            fprintf(
                output,
                "x: %.20e\t|\tVogf: %.20e\t|\t Expected: %.20e\t|\tULP Error: %.3f\n", 
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
                    "ULP more then critical value %.3f. ULP: %.3f\n", 
                    CRITICAL_ULP_ERROR_, ulp_error
                );
            }
            ++failed_cnt;
        }
    }

    mpfr_clear(x_mpfr);
    mpfr_clear(res_mpfr);

    print_final_res_(failed_cnt, output);

    if (colored_text_supported) {
        fprintf(output, YELLOW_TEXT("Max ULP error: %.3f\n"), max_ulp_error);
    }
    else {
        fprintf(output, "Max ULP error: %.3f\n", max_ulp_error);
    }

    return failed_cnt;
}


int vogf_test_all_positive(FILE* const output) {
    lassert(!is_invalid_ptr(output), "output stream is not valid");

    const bool colored_text_supported = (output == stdout);

    int total_failed = 0;
    float global_max_ulp = 0.0f;

    const uint32_t max_bits = as_uint32(INFINITY); 
    
    const uint32_t progress_step = max_bits / 10000;

    double start_time = omp_get_wtime();

    fprintf(output, "\nTest all positive correct values\n");

    #pragma omp parallel
    {
        mpfr_t x_mpfr, res_mpfr;
        mpfr_init2(x_mpfr, 128);
        mpfr_init2(res_mpfr, 128);

        int local_failed = 0;
        float local_max_ulp = 0.0f;

        #pragma omp for schedule(static, 1048576) // 2^20
        for (uint32_t i = as_uint32(0.9f); i <= as_uint32(1.1f); ++i) {
            static_assert(sizeof(float) == sizeof(i), "for bit cast");
            float test_num = as_float(i);

            float res_vogf = vogf(test_num);

            mpfr_set_flt(x_mpfr, test_num, MPFR_RNDN);
            mpfr_log(res_mpfr, x_mpfr, MPFR_RNDN);
            double expected = mpfr_get_d(res_mpfr, MPFR_RNDN);

            float ulp_error = calculate_ulp_diff_(res_vogf, expected);

            local_max_ulp = MAX(ulp_error, local_max_ulp);
            
            if (ulp_error > CRITICAL_ULP_ERROR_) {
                #pragma omp critical
                {
                    fprintf(
                        output,
                        "\nx: %.20e\t|\tVogf: %.20e\t|\t Expected: %.20e\t|\tULP Error: %.3f\n", 
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

    double end_time = omp_get_wtime();
    printf("\rTest finished in %.2f seconds.\n", end_time - start_time);

    print_final_res_(total_failed, output);

    if (colored_text_supported) {
        fprintf(output, YELLOW_TEXT("Max ULP error: %.3f\n"), global_max_ulp);
    }
    else {
        fprintf(output, "Max ULP error: %.3f\n", global_max_ulp);
    }

    return total_failed;
}

int vogf_test_flags(
    const vogf_flag_test_t testcase[], 
    const size_t testcase_size, 
    const char* testcase_name, 
    FILE* const output,
    bool only_incorrect
) {
    lassert(!is_invalid_ptr(testcase), "testcase array is not valid");
    lassert(testcase_size > 0, "testcase array size is zero");
    lassert(!is_invalid_ptr(output), "output stream is not valid");

    const bool colored_text_supported = (output == stdout);

    if (testcase_name) {
        fprintf(output, "\n%s\n", testcase_name);
    }

    int failed_cnt = 0;

    for (size_t test_ind = 0; test_ind < testcase_size; ++test_ind) {
        errno = 0;
        feclearexcept(FE_ALL_EXCEPT);

        const vogf_flag_test_t test = testcase[test_ind];
        const float test_num = test.x;
        
        [[maybe_unused]] const float res_vogf = vogf(test_num);

        const int vogf_errno = errno;
        const int vogf_fenv = fetestexcept(FE_ALL_EXCEPT);

        [[maybe_unused]] const float res_logf = logf(test_num);

        const int logf_errno = errno;
        const int logf_fenv = fetestexcept(FE_ALL_EXCEPT);

        const bool incorrect = vogf_errno != logf_errno || vogf_fenv != logf_fenv;

        if (!only_incorrect || incorrect) {
            fprintf(
                output,
                "%s"
                "x: %.20e\t|\tvogf_errno: %x\t|\tvogf_fenv: %x\t|\tlogf_errno: %x\t|\tlogf_fenv: %x\t\n"
                "Desc: '%s'\n",
                (incorrect ? (colored_text_supported ? RED_TEXT("INCORRECT\n")  : "INCORRECT\n") : ""),
                test_num, (unsigned)vogf_errno, (unsigned)vogf_fenv, (unsigned)logf_errno, (unsigned)logf_fenv,
                test.desc
            );
        }

        failed_cnt += incorrect;
    }

    print_final_res_(failed_cnt, output);

    return failed_cnt;
}