#include <stdio.h>
#include <stdlib.h>

#include "utils/utils.h"
#include "logger/liblogger.h" // IWYU pragma: keep
#include "flags/flags.h"

int init_all(flags_objs_t* const flags_objs, const int argc, char* const * argv);
int dtor_all(flags_objs_t* const flags_objs);

//==================================================================================================

int main(const int argc, char* const argv[])
{
    flags_objs_t flags_objs  = {};

    INT_ERROR_HANDLE(init_all(&flags_objs, argc, argv));

    fprintf(stderr, "kek\n");

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