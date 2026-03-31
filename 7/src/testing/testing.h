#ifndef VOGF_SRC_TESTING_TESTING_H
#define VOGF_SRC_TESTING_TESTING_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int test_vogf(
    const float testcases[], 
    const size_t testcases_size, 
    const char* suite_name, 
    FILE* const output,
    bool only_incorrect
);

int test_all_vogf(FILE* const output);

#endif /*VOGF_SRC_TESTING_TESTING_H*/