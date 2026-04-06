#include "utils.h"

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

enum PtrState is_invalid_ptr(const void* ptr)
{
    errno = 0;
    if (ptr == NULL)
    {
        return PTR_STATES_NULL;
    }

    char filename[] = "/tmp/chupapi_munyanya.XXXXXX";
    const int fd = mkstemp(filename);

    if (fd == -1) 
    {
        perror("Can't mkstemp file");
        return PTR_STATES_ERROR;
    }
    
    const ssize_t write_result = write(fd, ptr, 1);

    if (remove(filename))
    {
        perror("Can't remove temp file");
        return PTR_STATES_ERROR;
    }

    if (close(fd))
    {
        perror("Can't close temp file");
        return PTR_STATES_ERROR;
    }

    if (write_result == 1)
        return PTR_STATES_VALID;

    if (errno == EFAULT) 
    {
        errno = 0;
        return PTR_STATES_INVALID;
    }
    
    perror("Unpredictable errno state, after write into temp file");
    return PTR_STATES_ERROR;
}

int is_empty_file (FILE* file)
{
    if (is_invalid_ptr(file))
    {
        fprintf(stderr, "Is empty file nvalid\n");
        return -1;
    }

    int seek_temp = SEEK_CUR;

    if (fseek(file, 0, SEEK_END))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    const int res = ftell(file) > 2;

    if (fseek(file, 0, seek_temp))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    return res;
}

uint32_t as_uint32(float f) {
    uint32_t u;
    memcpy(&u, &f, 4);
    return u;
}

float as_float(uint32_t u) {
    float f;
    memcpy(&f, &u, 4);
    return f;
}

uint64_t as_uint64(double lf) {
    uint64_t ul;
    memcpy(&ul, &lf, 8);
    return ul;
}

double as_double(uint64_t ul) {
    double lf;
    memcpy(&lf, &ul, 8);
    return lf;
}