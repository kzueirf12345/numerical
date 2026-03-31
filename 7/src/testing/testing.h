#ifndef VOGF_SRC_TESTING_TESTING_H
#define VOGF_SRC_TESTING_TESTING_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int vogf_test_res(
    const float testcase[], 
    const size_t testcase_size, 
    const char* testcase_name, 
    FILE* const output,
    bool only_incorrect
);

int vogf_test_all_positive(FILE* const output);

typedef struct FlagTest {
    float x;
    const char* desc;
} vogf_flag_test_t;

int vogf_test_flags(
    const vogf_flag_test_t testcase[], 
    const size_t testcase_size, 
    const char* testcase_name, 
    FILE* const output,
    bool only_incorrect
);

#endif /*VOGF_SRC_TESTING_TESTING_H*/