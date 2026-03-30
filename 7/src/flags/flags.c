#include "flags/flags.h"

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "logger/liblogger.h" // IWYU pragma: keep
#include "utils/utils.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* flags_strerror(const enum FlagsError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(FLAGS_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(FLAGS_ERROR_FAILURE);
        default:
            return "UNKNOWN_FLAGS_ERROR";
    }
    return "UNKNOWN_FLAGS_ERROR";
}
#undef CASE_ENUM_TO_STRING_


enum FlagsError flags_objs_ctor(flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    if (!strncpy(flags_objs->log_folder, "./log/", FILENAME_MAX))
    {
        perror("Can't strncpy flags_objs->log_folder");
        return FLAGS_ERROR_SUCCESS;
    }

    if (!strncpy(flags_objs->out_filename, "STDOUT", FILENAME_MAX))
    {
        perror("Can't strncpy flags_objs->in_filename");
        return FLAGS_ERROR_SUCCESS;
    }

    flags_objs->out = NULL;

    flags_objs->seed = (unsigned int)time(NULL);

    flags_objs->only_incorrect = true;

    return FLAGS_ERROR_SUCCESS;
}

enum FlagsError flags_objs_dtor (flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    if (flags_objs->out && flags_objs->out != stdout && fclose(flags_objs->out))
    {
        perror("Can't fclose out file");
        return FLAGS_ERROR_FAILURE;
    }

    return FLAGS_ERROR_SUCCESS;
}

enum FlagsError flags_processing(flags_objs_t* const flags_objs, 
                                 const int argc, char* const argv[])
{
    lassert(!is_invalid_ptr(flags_objs), "");
    lassert(!is_invalid_ptr(argv), "");
    lassert(argc, "");

    int getopt_rez = 0;
    while ((getopt_rez = getopt(argc, argv, "al:o:s:")) != -1)
    {
        switch (getopt_rez)
        {
            case 'l':
            {
                if (!strncpy(flags_objs->log_folder, optarg, FILENAME_MAX))
                {
                    perror("Can't strncpy flags_objs->log_folder");
                    return FLAGS_ERROR_FAILURE;
                }

                break;
            }
            case 'o':
            {
                if (!strncpy(flags_objs->out_filename, optarg, FILENAME_MAX))
                {
                    perror("Can't strncpy flags_objs->out_filename");
                    return FLAGS_ERROR_FAILURE;
                }

                break;
            }
            case 's':
            {
                if (sscanf(optarg, "%u", &flags_objs->seed) != 1)
                {
                    perror("Can't sscanf flags_objs->seed");
                    return FLAGS_ERROR_FAILURE;
                }

                break;
            }
            case 'a':
            {
                flags_objs->only_incorrect = false;
                break;
            }

            default:
            {
                fprintf(stderr, "Getopt error - d: %d, c: %c\n", getopt_rez, (char)getopt_rez);
                return FLAGS_ERROR_FAILURE;
            }
        }
    }

    if (strcmp(flags_objs->out_filename, "STDOUT") == 0) 
    {
        flags_objs->out = stdout;
    }
    else if (!(flags_objs->out = fopen(flags_objs->out_filename, "wb"))) 
    {
        perror("Can't open out file");
        return FLAGS_ERROR_FAILURE;
    }

    return FLAGS_ERROR_SUCCESS;
}