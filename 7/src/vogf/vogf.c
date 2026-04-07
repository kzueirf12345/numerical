#include "vogf.h"

#include <assert.h>
#include <fenv.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

#include "tables.h"
#include "utils/utils.h"

#pragma GCC diagnostic ignored "-Wfloat-equal"

//==============================CONSTANTS===========================================================

#define LN2_ (0.693147180559945309417232121458)

#define FLOAT_C1 (1)
#define FLOAT_C2 (-0.5)
#define FLOAT_C3 (0.33333333333333331482961625624739099293947219848633)
#define FLOAT_C4 (-0.24999999999999997224442438437108648940920829772949)
#define FLOAT_C5 (0.200000003335021797612469640625931788235902786254883)
#define FLOAT_C6 (-0.1666666704508244778448755596400587819516658782959)
#define FLOAT_C7 (-0.5238095287429466173634295955707784742116928100586)
#define FLOAT_C8 (0.82033280923573437259932461529388092458248138427734)
#define FLOAT_C9 (2.16917543723563961684703826904296875e7)
#define FLOAT_C10 (-3.0279761666952438652515411376953125e7)


//==============================HELPERS=============================================================

enum FloatClass {          
    FLOAT_CLASS_NORMAL        =   0b00000,
    FLOAT_CLASS_DENORMAL      =   0b00010,
    FLOAT_CLASS_ZERO          =   0b00011,
    FLOAT_CLASS_INFINITY      =   0b00100,
    FLOAT_CLASS_NAN           =   0b00101,
};

static inline enum FloatClass get_float_class_(uint32_t exp, uint32_t mant) {
    if (exp == 0xFFu) {
        return (mant == 0) ? FLOAT_CLASS_INFINITY : FLOAT_CLASS_NAN;
    }
    
    if (exp == 0u) {
        return (mant == 0) ? FLOAT_CLASS_ZERO : FLOAT_CLASS_DENORMAL;
    }
    
    return FLOAT_CLASS_NORMAL;
}

//==============================VOGF=============================================================

float vogf(float x) {

    if (x == 1.f) {
        return FLOAT_POS_ZERO;
    }

    uint32_t xu = as_uint32(x);
    uint32_t exp = (xu & FLOAT_EXP_MASK) >> FLOAT_MANT_SIZE;
    uint32_t mant = xu & FLOAT_MANT_MASK;
    uint32_t sign = xu & FLOAT_SIGN_MASK;

    if (sign && (x != 0.f)) { //negative
        errno = EDOM;
        feraiseexcept(FE_INVALID);
        return FLOAT_QNAN;
    }

    enum FloatClass class = get_float_class_(exp, mant);

    int32_t extra_pow = 0;

    switch (class) {
        case FLOAT_CLASS_NAN:
            errno = EDOM;
            feraiseexcept(FE_INVALID);
            return x;

        case FLOAT_CLASS_ZERO:
            errno = ERANGE;
            feraiseexcept(FE_DIVBYZERO);
            return FLOAT_NEG_INF;

        case FLOAT_CLASS_INFINITY: // neg infinity check in neg
            return FLOAT_POS_INF;

        case FLOAT_CLASS_DENORMAL:
            x *= (float)(1u << FLOAT_MANT_SIZE); // make normal
            xu = as_uint32(x);
            exp = (xu & FLOAT_EXP_MASK) >> FLOAT_MANT_SIZE;
            mant = xu & FLOAT_MANT_MASK;
            extra_pow = -(int32_t)FLOAT_MANT_SIZE;
            break;

        case FLOAT_CLASS_NORMAL:
            if (x == 1.0f) {
                return 0.0f;
            }
            break;

        default:
            break;
    }

//-----------------------------------------REDUCTION------------------------------------------------

    const uint32_t magic_offset = 0x3f2aaaab;
    uint32_t adjusted_xu = xu - magic_offset;

    int32_t exponent_offset = (int32_t)adjusted_xu >> FLOAT_MANT_SIZE;

    uint32_t reduced_f_u = xu - ((uint32_t)exponent_offset << FLOAT_MANT_SIZE);
    float reduced_f = as_float(reduced_f_u);

    int32_t total_pow = exponent_offset + extra_pow;

    const size_t ind = (reduced_f_u & FLOAT_MANT_MASK) >> (FLOAT_MANT_SIZE - TABLE_BIT_CNT);

    const double r = (double)R_TABLE[ind] * (double)reduced_f - 1.0;

//---------------------------------------POLYNOM----------------------------------------------------


    const double p = r * 
            (FLOAT_C1 + r * 
            (FLOAT_C2 + r * 
            (FLOAT_C3 + r * 
            (FLOAT_C4 + r * 
            (FLOAT_C5 + r * 
            (FLOAT_C6))))));

//---------------------------------------RECONSTRUCTION---------------------------------------------

    const double res = (double)total_pow * LN2_ + (T_TABLE[ind]) + p;

    if ((double)(float)res != res) {
        feraiseexcept(FE_INEXACT);
    }

    return (float)res;
}