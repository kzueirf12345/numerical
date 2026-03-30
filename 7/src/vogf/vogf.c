#include "vogf.h"

#include <assert.h>
#include <fenv.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

#include "tables.h"

static_assert(sizeof(float) == sizeof(uint32_t), "Operands must have the same size");

//==============================CONSTANTS===========================================================
#define FLOAT_OFFSET_ (127u)
#define MANT_SIZE (23u)
#define MANT_MASK (0x7FFFFFu)
#define EXP_SIZE (8u)
#define EXP_MASK (0xFFu << MANT_SIZE)
#define SIGN_SIZE (1u)
#define SIGN_MASK (1u << (MANT_SIZE + EXP_SIZE))

#define LN2_ (0.693147180559945309417232121458)

#define FLOAT_C1 (1)
#define FLOAT_C2 (-0.5)
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

    const uint32_t xu = *(uint32_t*)&x;

    if ((xu & SIGN_MASK) && (x != 0.f)) { //negative
        errno = FE_INVALID;
        return FLOAT_QNAN;
    }

    const uint32_t exp = (xu & EXP_MASK) >> MANT_SIZE;
    const uint32_t mant = xu & MANT_MASK;

    enum FloatClass class = get_float_class_(exp, mant);

    switch (class) {
        case FLOAT_CLASS_NAN:
            return FLOAT_QNAN;
        case FLOAT_CLASS_ZERO:
            errno = FE_DIVBYZERO;
            return FLOAT_NEG_INF;
        case FLOAT_CLASS_INFINITY: // neg infinity check in neg
            return FLOAT_POS_INF;
        case FLOAT_CLASS_DENORMAL:
            return FLOAT_SNAN; // TODO handle
        case FLOAT_CLASS_NORMAL:
        default:
            break;
    }

//-----------------------------------------REDUCTION------------------------------------------------

/*!SECTION
ln(x)=ln(mant*2^pow)=ln(mant)+ln(2^pow)=ln(mant)+pow⋅ln(2)
*/
    const int32_t x_pow = (int32_t)exp - (int32_t)FLOAT_OFFSET_;

    const uint32_t normal_mantu = (FLOAT_OFFSET_ << MANT_SIZE) | mant; // [1, 2)
    const float normal_mant = *(const float*)(&normal_mantu);

    const size_t ind = mant >> (MANT_SIZE - TABLE_BIT_CNT);

    const double r = (double)R_TABLE[ind] * (double)normal_mant - 1.; // use double because near 1 float accuracy is not enough

//---------------------------------------POLYNOM----------------------------------------------------

    double p = r * (FLOAT_C1 + r * (FLOAT_C2 + r * FLOAT_C3));

//---------------------------------------RECONSTRUCTION---------------------------------------------

    return (float)((double)x_pow * LN2_ + T_TABLE[ind] + p);
}