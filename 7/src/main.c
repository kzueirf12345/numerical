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

    const bool colored_text_supported = (flags_objs.out == stdout);

    fprintf(flags_objs.out, "seed: %u\n", flags_objs.seed);

//Testcase1

    const char* const testcase1_name = "Testing some specific values";

    const float testcase1[] = {
        0.0f, -0.0f, INFINITY, NAN, -1.0f, 1.0f, //special
        (*(float*)&(uint32_t){0x00800000u}),  // min normal
        (*(float*)&(uint32_t){0x7f7fffffu}),  // max normal
        (*(float*)&(uint32_t){0x007FFFFFu}),  // max denormal
        (*(float*)&(uint32_t){0x00000001u}),  // min denormal
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
    float testcase2[50000];
    const size_t testcase2_size = sizeof(testcase2) / sizeof(*testcase2);

    for (size_t test_ind = 0; test_ind < testcase2_size; ++test_ind) {
        static_assert(sizeof(float) == sizeof(int), "For bitcast");
        testcase2[test_ind] = (*(float*)&(int32_t){rand()});
    }

//Testcase3

    const char* const testcase3_name = "Testing near 1";
    float testcase3[] = {
        *(float*)&(uint32_t){0x3f7ffff1u},
        *(float*)&(uint32_t){0x3f7ffff2u},
        *(float*)&(uint32_t){0x3f7ffff3u},
        *(float*)&(uint32_t){0x3f7ffff4u},
        *(float*)&(uint32_t){0x3f7ffff5u},
        *(float*)&(uint32_t){0x3f7ffff6u},
        *(float*)&(uint32_t){0x3f7ffff7u},
        *(float*)&(uint32_t){0x3f7ffff8u},
        *(float*)&(uint32_t){0x3f7ffff9u},
        *(float*)&(uint32_t){0x3f7ffffau},
        *(float*)&(uint32_t){0x3f7ffffbu},
        *(float*)&(uint32_t){0x3f7ffffcu},
        *(float*)&(uint32_t){0x3f7ffffdu},
        *(float*)&(uint32_t){0x3f7ffffeu},
        *(float*)&(uint32_t){0x3f7fffffu},
    };
        
    const size_t testcase3_size = sizeof(testcase3) / sizeof(*testcase3);

//Testing

    int npassed_cnt = 0;
    int res = 0;

    res = test_vogf(testcase1, testcase1_size, testcase1_name, flags_objs.out, flags_objs.only_incorrect);
    npassed_cnt += (res != 0);

    res = test_vogf(testcase2, testcase2_size, testcase2_name, flags_objs.out, flags_objs.only_incorrect);
    npassed_cnt += (res != 0);

    res = test_vogf(testcase3, testcase3_size, testcase3_name, flags_objs.out, flags_objs.only_incorrect);
    npassed_cnt += (res != 0);

    res = test_all_vogf(flags_objs.out);
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
