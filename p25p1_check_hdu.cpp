
#include "p25p1_check_hdu.h"
#include "ReedSolomon.hpp"
#include "Golay24.hpp"

//#define __CHECK_HDU_DEBUG__

int check_and_fix_golay24(char* hex, char* parity, int* fixed_errors)
{
    static DSDGolay24 golay24;

#ifdef __CHECK_HDU_DEBUG__
    printf("[");
    for(unsigned int i=0; i<6; i++) {
        printf("%c", (hex[i] != 0)? 'X': ' ');
    }
    printf("]  [");
    for(unsigned int i=12; i<24; i++) {
        printf("%c", (parity[i-12] != 0)? 'X': ' ');
    }
    printf("]");
#endif

    int uncorrectable_errors = golay24.decode(hex, parity, fixed_errors);

#ifdef __CHECK_HDU_DEBUG__
    printf(" -> [");
    for(unsigned int i=0; i<6; i++) {
        printf("%c", (hex[i] != 0)? 'X': ' ');
    }
    printf("]");
    if (uncorrectable_errors) {
        printf("  Errors: +4");
    } else {
        printf("  Errors: %i", *fixed_errors);
    }
    printf("\n");
#endif

    return uncorrectable_errors;
}

void check_and_fix_redsolomon63(char* data, char* parity)
{
    static DSDReedSolomon_63_20_17 reed_solomon63;

#ifdef __CHECK_HDU_DEBUG__
    char original[20][6];
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 6; j++) {
            original[i][j] = data[i*6+j];
        }
    }
#endif

    reed_solomon63.decode(data, parity);

#ifdef __CHECK_HDU_DEBUG__
    printf("Results for Reed-Solomon code (n=%3d, k=%3d, t= %3d)\n\n", nn, kk, tt);
    printf("  i  original fixed\n");
    for (int i = 0; i < 20; i++) {
        printf("%3d  [", i);
        for (int j = 0; j < 6; j++) {
            printf("%c", (original[i][j] == 1)? 'X' : ' ');
        }
        printf("] [");
        for (int j = 0; j < 6; j++) {
            printf("%c", (data[i*6+j] == 1)? 'X' : ' ');
        }
        printf("]\n");
    }
    printf("\n");
#endif
}
