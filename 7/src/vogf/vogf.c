#include "vogf.h"

#include <assert.h>
#include <fenv.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

static_assert(sizeof(float) == sizeof(uint32_t), "Operands must have the same size");

//==============================CONSTANTS===========================================================
#define FLOAT_OFFSET_ (127u)
#define MANT_SIZE (23u)
#define MANT_MASK 0x7FFFFFu
#define EXP_SIZE (8u)
#define EXP_MASK (0xFFu << MANT_SIZE)
#define SIGN_SIZE (1u)
#define SIGN_MASK (1u << (MANT_SIZE + EXP_SIZE))

#define LN2_ (0.693147180559945309417232121458f)

#define FLOAT_C1 (0.999991595745086669921875f)
#define FLOAT_C2 (-0.499361336231231689453125f)
#define FLOAT_C3 (0.325206577777862548828125f)
#define FLOAT_C4 (-0.21004854142665863037109375f)
#define FLOAT_C5 (0.10122220218181610107421875f)
#define FLOAT_C6 (-0.02386914193630218505859375f)

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

    const float f = normal_mant - 1.f;

//---------------------------------------POLYNOM----------------------------------------------------

    float p = f * (FLOAT_C1 + f * (FLOAT_C2 + f * (FLOAT_C3 + f * (FLOAT_C4 + f * FLOAT_C5))));

//---------------------------------------RECONSTRUCTION---------------------------------------------

    return (float)x_pow * LN2_ + p; // TODO Lookup table
}

/*!SECTION

fpminimax(log1p(x), [|1,2,3,4,5,6|], [|single, single, single, single, single, single|], [0, 1]);

*/