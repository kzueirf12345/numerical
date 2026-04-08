#include <stdio.h>
#include <stdlib.h>

#include "matrix/funcs.h"
#include "utils/utils.h"
#include "logger/liblogger.h" // IWYU pragma: keep
#include "flags/flags.h"
#include "utils/concole.h"

int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
int dtor_all(flags_objs_t* const flags_objs);

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

    srand(flags_objs.seed);

    const size_t a_rows = 8;
    const size_t a_cols = 8;
    const size_t b_rows = a_cols;
    const size_t b_cols = 8;

    float* const a = (float*)calloc(a_rows * a_cols, sizeof(*a));
    float* const b = (float*)calloc(b_rows * b_cols, sizeof(*b));

    matfill(a, a_rows, a_cols);
    matfill(b, b_rows, b_cols);

    matprint(stdout, a, a_rows, a_cols);
    matprint(stdout, b, b_rows, b_cols);

    const size_t c_rows = a_rows;
    const size_t c_cols = b_cols;

    float* const c1 = (float*)calloc(c_rows * c_cols, sizeof(*c1));
    float* const c2 = (float*)calloc(c_rows * c_cols, sizeof(*c2));

    matmul_correct  (a, b, c1, c_rows, a_cols, c_cols);
    matmul_optimized(a, b, c2, c_rows, a_cols, c_cols);

    matprint(stdout, c1, c_rows, c_cols);
    matprint(stdout, c2, c_rows, c_cols);

    printf(
        "\n%s\n", 
        (mat_is_equal(c1, c2, c_rows, c_cols) 
       ? GREEN_TEXT("IS EQUAL!") 
       : RED_TEXT("NOT EQUAL!"))
    );

    free(a);
    free(b);
    free(c1);
    free(c2);

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