#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>
#include <math.h>

#include "logger/src/logger.h"
#include "utils/utils.h"
#include "logger/liblogger.h" // IWYU pragma: keep
#include "flags/flags.h"
#include "vogf/vogf.h"
#include "utils/concole.h"

float calculate_ulp_diff(float actual, double expected);

int test_vogf(
    const float testcases[], 
    const size_t testcases_size, 
    const char* suite_name, 
    FILE* const output
);

static int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
static int dtor_all(flags_objs_t* const flags_objs);

//==================================================================================================

int main(const int argc, char* const argv[])
{
    flags_objs_t flags_objs  = {}; //TODO seed

    INT_ERROR_HANDLE(init_all(&flags_objs, argc, argv));

    const bool colored_text_supported = (flags_objs.out == stdout);

    fprintf(flags_objs.out, "seed: %u\n\n", flags_objs.seed);

//Testcase1

    const char* const testcase1_name = "Testing some specific values";

    const float testcase1[] = {
        0.0f, -0.0f, INFINITY, NAN, -1.0f, 1.0f, //special
        (*(float*)&(uint32_t){0x00800000u}),  // min normal
        (*(float*)&(uint32_t){0x7f7fffffu}),  // max normal
        // (*(float*)&(uint32_t){0x007FFFFFu}),  // max denormal
        // (*(float*)&(uint32_t){0x00000001u}),  // min denormal
        2.f, 4.f, 0.5f, // 2^n
        1073741824.0f, // 2^30
        1.4142135f, 1.4142136f, // sqrt(2)
        1.000001f, 0.999999f, 1.0001f, 0.9999f, // 1
        0.70710677f, // 1/sqrt(2)
        3.14159265f, // pi
        2.71828182f, // e
        0.33333333f, // 1/3
    };

    const size_t testcase1_size = sizeof(testcase1) / sizeof(*testcase1);

//Testcase2

    const char* const testcase2_name = "Testing random values";
    float testcase2[100];
    const size_t testcase2_size = sizeof(testcase2) / sizeof(*testcase2);

    for (size_t test_ind = 0; test_ind < testcase2_size; ++test_ind) {
        static_assert(sizeof(float) == sizeof(int), "For bitcast");
        testcase2[test_ind] = (*(float*)&(int32_t){rand()});
    }

//Testing

    int npassed_cnt = 0;
    int res = 0;

    res = test_vogf(testcase1, testcase1_size, testcase1_name, flags_objs.out);
    npassed_cnt += (res != 0);

    res = test_vogf(testcase2, testcase2_size, testcase2_name, flags_objs.out);
    npassed_cnt += (res != 0);

    if (npassed_cnt == 0) {
        if (colored_text_supported) {
            fprintf(flags_objs.out, GREEN_TEXT("\nALL TESTCASES PASSED!!!\n"));
        }
        else {
            fprintf(flags_objs.out, "\nALL TESTCASES PASSED!!!\n");
        }
    }
    else {
        if (colored_text_supported) {
            fprintf(flags_objs.out, RED_TEXT("\n%d TESTCASES NOT PASSED!!!\n"), npassed_cnt);
        }
        else {
            fprintf(flags_objs.out, "\n%d TESTCASES NOT PASSED!!!\n", npassed_cnt);
        }
    }

    INT_ERROR_HANDLE(                                                        dtor_all(&flags_objs));

    return EXIT_SUCCESS;
}

//==================================================================================================

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
float calculate_ulp_diff(float actual, double expected) {

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
#pragma GCC diagnostic pop

#define CRITICAL_ULP_ERROR_ (3.5f)
int test_vogf(
    const float tests[], 
    const size_t tests_size, 
    const char* testcase_name, 
    FILE* const output
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
    mpfr_init2(x_mpfr, 256);
    mpfr_init2(res_mpfr, 256);

    int npassed_cnt = 0;

    for (size_t test_ind = 0; test_ind < tests_size; ++test_ind) {
        const float test_num = tests[test_ind];

        const float res_vogf = vogf(test_num);
        
        mpfr_set_d(x_mpfr, test_num, MPFR_RNDN); 
        mpfr_log(res_mpfr, x_mpfr, MPFR_RNDN);

        const double expected = mpfr_get_d(res_mpfr, MPFR_RNDN);

        const float ulp_error = calculate_ulp_diff(res_vogf, expected);

        fprintf(
            output,
            "x: %f | Vogf: %f | Expected: %f | ULP Error: %f\n", 
            test_num, res_vogf, expected, ulp_error
        );

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
                    "ULP more then critical value %g. ULP: %g\n", 
                    CRITICAL_ULP_ERROR_, ulp_error
                );
            }
            ++npassed_cnt;
        }
    }

    mpfr_clear(x_mpfr);
    mpfr_clear(res_mpfr);

    if (npassed_cnt == 0) {
        if (colored_text_supported) {
            fprintf(output, GREEN_TEXT("ALL TEST PASSED!!!\n"));
        }
        else {
            fprintf(output, "ALL TEST PASSED!!!\n");
        }
    }
    else {
        if (colored_text_supported) {
            fprintf(output, RED_TEXT("%d TESTS NOT PASSED!!!\n"), npassed_cnt);
        }
        else {
            fprintf(output, "%d TESTS NOT PASSED!!!\n", npassed_cnt);
        }
    }

    return npassed_cnt;
}
#undef CRITICAL_ULP_ERROR_

enum LoggError logger_init(char* const log_folder);


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv)
{
    lassert(argc, "");
    lassert(!is_invalid_ptr(argv), "");

    FLAGS_ERROR_HANDLE(flags_objs_ctor (flags_objs));
    FLAGS_ERROR_HANDLE(flags_processing(flags_objs, argc, argv));

    srand(flags_objs->seed);
    
    if (logger_init(flags_objs->log_folder))
    {
        fprintf(stderr, "Can't logger init\n");
        flags_objs_dtor(flags_objs);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
int dtor_all(flags_objs_t* const flags_objs)
{
    LOGG_ERROR_HANDLE(                                                              logger_dtor(););
    FLAGS_ERROR_HANDLE(                                               flags_objs_dtor(flags_objs););
    return EXIT_SUCCESS;
}
#pragma GCC diagnostic pop

#define LOGOUT_FILENAME "logout.log"
enum LoggError logger_init(char* const log_folder)
{
    lassert(!is_invalid_ptr(log_folder), "");

    char logout_filename[FILENAME_MAX] = {};
    if (snprintf(logout_filename, FILENAME_MAX, "%s%s", log_folder, LOGOUT_FILENAME) <= 0)
    {
        perror("Can't snprintf logout_filename");
        return EXIT_FAILURE;
    }

    LOGG_ERROR_HANDLE(logger_ctor());
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL));
    LOGG_ERROR_HANDLE(logger_set_logout_file(logout_filename));
    
    return EXIT_SUCCESS;
}
#undef LOGOUT_FILENAME
