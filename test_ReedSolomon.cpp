/**
 * Unit tests for the Reed-Solomon implementation.
 */

#include <assert.h>

#include "ReedSolomon.hpp"

static void byte_to_binary(char* b, int x)
{
    int z, i;

    for (z = 32, i=0; z > 0; z >>= 1, i++) {
        b[i] = ((x & z) != 0) ? 'X' : ' ';
    }
    b[6] = 0;
}

static void test_reed_solomon63()
{
    ReedSolomon_63_47_17 rs;

    int data[kk];
    int bb[nn - kk];
    int recd[nn];
    int output[nn];

    for (int i = 0; i < nn; i++) {
        recd[i] = 0;
    }

    // send this message
    recd[16+0] = 1;
    recd[16+6] = 8;

    recd[ 0] = 8+4;
    recd[ 1] = 3;
    recd[ 2] = 8+2;
    recd[ 3] = 16+1;
    recd[ 4] = 16+2;
    recd[ 5] = 32+8+3;
    recd[ 6] = 4;
    recd[ 7] = 32+2;
    recd[ 8] = 32+2;
    recd[ 9] = 32+16+8+1;
    recd[10] = 32+16+1;
    recd[11] = 32+16+3;
    recd[12] = 16+8;
    recd[13] = 32+2;
    recd[14] = 16+4;
    recd[15] = 16+4+3;

    rs.decode(recd, output); /* recd[] is returned in polynomial form */

    /* print out the relevant stuff */
    printf("Results for Reed-Solomon code (n=%3d, k=%3d, t= %3d)\n\n", nn, kk, tt);
    printf("  i  recd[i](decoded)\n");
    for (int i = 0; i < nn; i++) {
        char b[9];
        byte_to_binary(b, output[i]);
        printf("%3d    [%s]\n", i, b);
    }
    printf("\n");

    assert(output[ 0] == 8+4);
    assert(output[ 1] == 3);
    assert(output[ 2] == 8+2);
    assert(output[ 3] == 16+1);
    assert(output[ 4] == 16+2);
    assert(output[ 5] == 32+3);
    assert(output[ 6] == 4);
    assert(output[ 7] == 32+2);
    assert(output[ 8] == 32+8+2);
    assert(output[ 9] == 32+16+8+1);
    assert(output[10] == 32+16+1);
    assert(output[11] == 32+16+3);
    assert(output[12] == 16);
    assert(output[13] == 32+8+2);
    assert(output[14] == 16+4);
    assert(output[15] == 16+4+3);
    assert(output[16] == 1);
    assert(output[17] == 0);
    assert(output[18] == 0);
    assert(output[19] == 0);
    assert(output[20] == 0);
    assert(output[21] == 0);
    assert(output[22] == 8);
    assert(output[23] == 0);
    assert(output[24] == 0);
    assert(output[25] == 0);
    assert(output[26] == 0);
    assert(output[27] == 0);
    assert(output[28] == 0);
    assert(output[29] == 0);
    assert(output[30] == 0);
    assert(output[31] == 0);
    assert(output[32] == 0);
    assert(output[33] == 0);
    assert(output[34] == 0);
    assert(output[35] == 0);
    assert(output[36] == 0);
    assert(output[37] == 0);
    assert(output[38] == 0);
    assert(output[39] == 0);
    assert(output[40] == 0);
    assert(output[41] == 0);
    assert(output[42] == 0);
    assert(output[43] == 0);
    assert(output[44] == 0);
    assert(output[45] == 0);
    assert(output[46] == 0);
    assert(output[47] == 0);
    assert(output[48] == 0);
    assert(output[49] == 0);
    assert(output[50] == 0);
    assert(output[51] == 0);
    assert(output[52] == 0);
    assert(output[53] == 0);
    assert(output[54] == 0);
    assert(output[55] == 0);
    assert(output[56] == 0);
    assert(output[57] == 0);
    assert(output[58] == 0);
    assert(output[59] == 0);
    assert(output[60] == 0);
    assert(output[61] == 0);
    assert(output[62] == 0);
    assert(output[63] == 0);
}

/*
int main(int argc, char** argv)
{
    test_reed_solomon63();

    return 0;
}
*/
