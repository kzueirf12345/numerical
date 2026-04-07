#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>
#include <math.h>
#include <omp.h>

#include "logger/src/logger.h"
#include "utils/utils.h"
#include "logger/liblogger.h" // IWYU pragma: keep
#include "flags/flags.h"
#include "utils/concole.h"
#include "testing/testing.h"

static int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
static int dtor_all(flags_objs_t* const flags_objs);

//==================================================================================================

int main(const int argc, char* const argv[])
{
    flags_objs_t flags_objs  = {};

    INT_ERROR_HANDLE(init_all(&flags_objs, argc, argv));

    if (flags_objs.need_help) {
        printf(HELP_MESSAGE);
        INT_ERROR_HANDLE(                                                    dtor_all(&flags_objs));
        return EXIT_SUCCESS;
    }

    const bool colored_text_supported = (flags_objs.out == stdout);

    fprintf(flags_objs.out, "seed: %u\n", flags_objs.seed);

//Testcase

    const char* const testcase_spec_name = "Testing some specific values";

    const float testcase_spec[] = {
        0.0f, -0.0f, INFINITY, -INFINITY, NAN, -1.0f, 1.0f, //special
        as_float(0x00800000u),  // min normal
        as_float(0x7f7fffffu),  // max normal
        as_float(0x007FFFFFu),  // max denormal
        as_float(0x00000001u),  // min denormal
        2.f, 8.f, 0.5f, 0.125f, // 2^n
        1073741824.0f, // 2^30
        1.4142135f, 1.4142136f, // sqrt(2)
        nextafterf(1.f, INFINITY), 1, nextafterf(1.f, -INFINITY), 0.9999f, // 1
        0.70710677f, // 1/sqrt(2)
        3.14159265f, // pi
        2.71828182f, // e
        0.33333333f, // 1/3
    };

    const size_t testcase_spec_size = sizeof(testcase_spec) / sizeof(*testcase_spec);

//Testcase

    const char* const testcase_rand_name = "Testing random values";
    float testcase_rand[100000];
    const size_t testcase_rand_size = sizeof(testcase_rand) / sizeof(*testcase_rand);

    for (size_t test_ind = 0; test_ind < testcase_rand_size; ++test_ind) {
        static_assert(sizeof(float) == sizeof(int), "For bitcast");
        while (fpclassify(testcase_rand[test_ind] = as_float((uint32_t)rand())) != FP_NORMAL);
    }

//Testcase

    const char* const testcase_flags_name = "Testing flags";

    const vogf_flag_test_t testcase_flags[] = {
        { NAN,       "NAN" },
        { -1.0f,     "Negative value" },
        { -INFINITY, "Negative infinity" },
        { 0.0f,      "Positive zero" },
        { -0.0f,     "Negative zero" },
        { 1.0f,      "Exact one (no flags)" },
        { INFINITY,  "Positive infinity (no flags)" },
        { 2.0f,      "Normal value (inexact result)" }
    };

    const size_t testcase_flags_size = sizeof(testcase_flags) / sizeof(*testcase_flags);

//Testing

    fprintf(stderr, "0x%xu\n", as_uint32((float)sqrt(2)));

    int npassed_cnt = 0;
    int res = 0;

    res = vogf_test_res(testcase_spec, testcase_spec_size, testcase_spec_name, flags_objs.out, flags_objs.only_incorrect);
    npassed_cnt += (res != 0);
    
    res = vogf_test_flags(testcase_flags, testcase_flags_size, testcase_flags_name, flags_objs.out, flags_objs.only_incorrect);
    npassed_cnt += (res != 0);
    
    // res = vogf_test_all_positive(flags_objs.out);
    // npassed_cnt += (res != 0);
    
    res = vogf_test_res(testcase_rand, testcase_rand_size, testcase_rand_name, flags_objs.out, flags_objs.only_incorrect);
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
