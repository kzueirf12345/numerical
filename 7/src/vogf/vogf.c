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
#define FLOAT_C2 (-0.49999)
#define FLOAT_C3 (0.3333333432674407958984375)

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

    uint32_t xu = *(uint32_t*)&x;
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
            return x;

        case FLOAT_CLASS_ZERO:
            errno = ERANGE;
            feraiseexcept(FE_DIVBYZERO);
            return FLOAT_NEG_INF;

        case FLOAT_CLASS_INFINITY: // neg infinity check in neg
            return FLOAT_POS_INF;

        case FLOAT_CLASS_DENORMAL:
            x *= (float)(1u << FLOAT_MANT_SIZE); // make normal
            xu = *(uint32_t*)&x;
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

/*!SECTION
ln(x)=ln(mant*2^pow)=ln(mant)+ln(2^pow)=ln(mant)+pow⋅ln(2)
*/
    const int32_t x_pow = (int32_t)exp - (int32_t)FLOAT_OFFSET_ + extra_pow;

    const uint32_t normal_mantu = (FLOAT_OFFSET_ << FLOAT_MANT_SIZE) | mant; // [1, 2)
    const float normal_mant = *(const float*)(&normal_mantu);

    const size_t ind = mant >> (FLOAT_MANT_SIZE - TABLE_BIT_CNT);

    const double r = (double)R_TABLE[ind] * (double)normal_mant - 1.; // use double because near 1 float accuracy is not enough

//---------------------------------------POLYNOM----------------------------------------------------

    const double p = r * (FLOAT_C1 + r * (FLOAT_C2));

//---------------------------------------RECONSTRUCTION---------------------------------------------

    const double res = (double)x_pow * LN2_ + T_TABLE[ind] + p;

    if ((double)(float)res != res) {
        feraiseexcept(FE_INEXACT);
    }

    return (float)res;
}