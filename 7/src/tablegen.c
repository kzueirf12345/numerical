#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>
#include <unistd.h>
#include <stdint.h>

// L1d = 2^18 byte = 2^15 double (reserve 2^15/2/2= 2^13)
#define K (13u)
#define SQRT2_MANT_THRESHOLD (0x3504F3u)

#define FLOAT_MANT_SIZE 23

int main() {
    const size_t size = (1u << K);

    float arr[(1u << K)];
    float arr_lo[(1u << K)];
    float arr_hi[(1u << K)];
    
    mpfr_t x_val, r_val, t_val, one;
    mpfr_inits2(1024, x_val, r_val, t_val, one, NULL);
    mpfr_set_d(one, 1.0, MPFR_RNDN);

    size_t cat_ind = 0;

    for (size_t i = 0; i < size; ++i) {

        const uint32_t mant_bits = (uint32_t)i << (FLOAT_MANT_SIZE - K);
        const uint32_t mant_mid = mant_bits | (1u << (FLOAT_MANT_SIZE - K - 1));

        mpfr_set_ui(x_val, mant_mid, MPFR_RNDN);
        mpfr_div_2ui(x_val, x_val, 23, MPFR_RNDN);
        mpfr_add_d(x_val, x_val, 1.0, MPFR_RNDN);

        if (mant_bits > SQRT2_MANT_THRESHOLD) {
            mpfr_div_ui(x_val, x_val, 2, MPFR_RNDN);
            if (cat_ind == 0) {
                cat_ind = i;
            }
        }

        mpfr_div(r_val, one, x_val, MPFR_RNDN);
        float r_flt = mpfr_get_flt(r_val, MPFR_RNDN);

        if (i == 0) {
            r_flt = 1.0f; 
        }

        arr[i] = r_flt;
    }

    for (size_t i = 0; i < size; ++i) {
        float r_float = arr[i];

        mpfr_set_flt(r_val, r_float, MPFR_RNDN);
        mpfr_log(t_val, r_val, MPFR_RNDN);
        mpfr_neg(t_val, t_val, MPFR_RNDN);

        float t_hi = mpfr_get_flt(t_val, MPFR_RNDN);
        mpfr_sub_d(t_val, t_val, (double)t_hi, MPFR_RNDN);
        float t_lo = mpfr_get_flt(t_val, MPFR_RNDN);

        arr_lo[i] = t_lo;
        arr_hi[i] = t_hi;
    }

    mpfr_clears(x_val, r_val, t_val, one, NULL);

    printf(
        "// This file is generated!\n"
        "#define TABLE_BIT_CNT (%uu)\n"
        "#define TABLE_SIZE (1u << %uu)\n"
        "#define CAT_IND (%zuull)\n\n",
        K, K, cat_ind
    );
    
    printf("static const float R_TABLE[TABLE_SIZE] = {\n");
    for (size_t i = 0; i < size; ++i) {
        printf(
            "\t%.80ef,\n", 
            arr[i]
        );
    }
    printf("};\n");

    printf("static const float T_TABLE_HI[TABLE_SIZE] = {\n");
    for (size_t i = 0; i < size; ++i) {
        printf(
            "\t%.80ef,\n", 
            arr_hi[i]
        );
    }
    printf("};\n");

    printf("static const float T_TABLE_LO[TABLE_SIZE] = {\n");
    for (size_t i = 0; i < size; ++i) {
        printf(
            "\t%.80ef,\n", 
            arr_lo[i]
        );
    }
    printf("};\n");

    return EXIT_SUCCESS;
}