#ifndef VOGF_SRC_UTILS_UTILS_H
#define VOGF_SRC_UTILS_UTILS_H

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifndef NDEBUG

#define IF_DEBUG(...) __VA_ARGS__
#define IF_ELSE_DEBUG(smth, other_smth) smth

#define IF_NDEBUG(...)
#define IF_ELSE_NDEBUG(smth, other_smth) other_smth

#else /*NDEBUG*/

#define IF_DEBUG(...)
#define IF_ELSE_DEBUG(smth, other_smth) other_smth

#define IF_NDEBUG(...) __VA_ARGS__
#define IF_ELSE_NDEBUG(smth, other_smth) smth

#endif /*NDEBUG*/

static_assert(sizeof(float)  == sizeof(uint32_t) && sizeof(uint32_t) == sizeof(unsigned int),  "For bitcast");
static_assert(sizeof(double) == sizeof(uint64_t) && sizeof(uint64_t) == sizeof(unsigned long long), "For bitcast");

#define FLOAT_OFFSET_       (127u)
#define FLOAT_MANT_SIZE     (23u)
#define FLOAT_MANT_MASK     ((1u << FLOAT_MANT_SIZE) - 1u)
#define FLOAT_EXP_SIZE      (8u)
#define FLOAT_EXP_MASK      (((1u << FLOAT_EXP_SIZE) - 1u) << FLOAT_MANT_SIZE)
#define FLOAT_SIGN_SIZE     (1u)
#define FLOAT_SIGN_MASK     (1u << (FLOAT_MANT_SIZE + FLOAT_EXP_SIZE))

#define DOUBLE_OFFSET_      (1023ull)
#define DOUBLE_MANT_SIZE    (52ull)
#define DOUBLE_MANT_MASK    ((1ull << DOUBLE_MANT_SIZE) - 1ull)
#define DOUBLE_EXP_SIZE     (11ull)
#define DOUBLE_EXP_MASK     (((1ull << DOUBLE_EXP_SIZE) - 1ull) << DOUBLE_MANT_SIZE)
#define DOUBLE_SIGN_SIZE    (1ull)
#define DOUBLE_SIGN_MASK    (1ull << (FLOAT_MANT_SIZE + FLOAT_EXP_SIZE))

#define POISON_UINT32 (uint32_t)(0xDEADBEAFu)
#define POISON_FLOAT  (*(float*)&(uint32_t){0xDEADBEAFu})

#define INT_ERROR_HANDLE(call_func, ...)                                                            \
    do {                                                                                            \
        int error_handler = call_func;                                                              \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Errno: %d\n",                                     \
                            errno);                                                                 \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

enum PtrState
{
    PTR_STATES_VALID   = 0,
    PTR_STATES_NULL    = 1,
    PTR_STATES_INVALID = 2,
    PTR_STATES_ERROR   = 3
};
static_assert(PTR_STATES_VALID == 0, "");

enum PtrState is_invalid_ptr(const void* ptr);

int is_empty_file (FILE* file);

#endif /*VOGF_SRC_UTILS_UTILS_H*/