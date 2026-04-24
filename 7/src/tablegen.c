#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>
#include <unistd.h>
// L1d = 2^18 byte = 2^15 double (reserve 2^15/2/2= 2^13)
#define K (8u)

int main() {
    const size_t size = (1u << K);

    float arr[(1u << K)];
    
    mpfr_t x_mid, r_val, t_val, one;
    mpfr_inits2(1024, x_mid, r_val, t_val, one, NULL);
    mpfr_set_d(one, 1.0, MPFR_RNDN);

    printf(
        "// This file is generated!\n"
        "#define TABLE_BIT_CNT (%uu)\n"
        "#define TABLE_SIZE (1u << %uu)\n\n",
        K, K
    );
    
    printf("static const float R_TABLE[TABLE_SIZE] = {\n");
    for (size_t i = 0; i < size; ++i) {
        // x_mid = 1.0 + (i + 0.5) / 2^K
        mpfr_set_ui(x_mid, i, MPFR_RNDN);
        mpfr_add_d(x_mid, x_mid, 0.5, MPFR_RNDN);
        mpfr_div_ui(x_mid, x_mid, size, MPFR_RNDN);
        mpfr_add_d(x_mid, x_mid, 1.0, MPFR_RNDN);

        // R_i = 1.0 / x_mid
        mpfr_div(r_val, one, x_mid, MPFR_RNDN);
        float r_float = mpfr_get_flt(r_val, MPFR_RNDN);

        if (i == 0) {
            r_float = 1.0f; 
        }

        arr[i] = r_float;
        
        printf(
            "\t%.50ff,\n", 
            r_float
        );
    }
    printf("};\n\n");

    printf("static const double T_TABLE[TABLE_SIZE] = {\n");
    for (size_t i = 0; i < size; ++i) {
        float r_float = arr[i];

        // T_i = ln(1.0 / Ri_float) = -ln(Ri_float)
        mpfr_set_flt(r_val, r_float, MPFR_RNDN);
        mpfr_log(t_val, r_val, MPFR_RNDN);
        mpfr_neg(t_val, t_val, MPFR_RNDN);
        
        double t_double = mpfr_get_d(t_val, MPFR_RNDN);
        printf(
            "\t%.100f,\n", 
            t_double
        );
    }
    printf("};\n");

    mpfr_clears(x_mid, r_val, t_val, one, NULL);
    return EXIT_SUCCESS;
}