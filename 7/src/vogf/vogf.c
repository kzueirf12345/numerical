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

#define LN2_HI (0.693147122859954833984375f)
#define LN2_LO (5.7699988786907852045260369777679443359375e-08f)

#define FLOAT_C2 (-0.5f)
#define FLOAT_C3 (0.f) 
// #define FLOAT_C4 (-0.25f)
// #define FLOAT_C5 (9.523817338049411773681640625e-3f)
// #define FLOAT_C6 (-0.16170634329319000244140625f)

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

/*!SECTION
ln(x)=ln(mant*2^pow)=ln(mant)+ln(2^pow)=ln(mant)+pow⋅ln(2)
*/
    int32_t x_pow = (int32_t)exp - (int32_t)FLOAT_OFFSET_ + extra_pow;

    const size_t ind = mant >> (FLOAT_MANT_SIZE - TABLE_BIT_CNT);

    uint32_t adj_exp = FLOAT_OFFSET_;
    if (ind >= CAT_IND) {
        ++x_pow;
        --adj_exp;
    }

    const uint32_t normal_mantu = (adj_exp << FLOAT_MANT_SIZE) | mant;
    const float normal_mant = as_float(normal_mantu);

    const float r = R_TABLE[ind] * normal_mant - 1.f;

//---------------------------------------POLYNOM----------------------------------------------------

    const float p = r * r * 
                (FLOAT_C2 + r *
                    (FLOAT_C3 + r));

    // const float p = r * (1.f + r * (-0.4999999701976776123046875f + r * 0.333331406116485595703125f));

//---------------------------------------RECONSTRUCTION---------------------------------------------

    float low_part = T_TABLE_LO[ind] + p + ((float)x_pow * LN2_LO);

    float high_part = T_TABLE_HI[ind] + ((float)x_pow * LN2_HI);

    const float res = (low_part + r) + high_part;

    // if ((double)(float)res != res) { //TODO check
    //     feraiseexcept(FE_INEXACT);
    // }

    return res;
}