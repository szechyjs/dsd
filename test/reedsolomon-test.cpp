/**
 * Unit tests for the Reed-Solomon implementation.
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ReedSolomon.hpp"

static void byte_to_binary(char* b, int x)
{
    int z, i;

    for (z = 32, i=0; z > 0; z >>= 1, i++) {
        b[i] = ((x & z) != 0) ? 'X' : ' ';
    }
    b[6] = 0;
}

TEST(ReedSolomonTest, Test1)
{
    ReedSolomon_63 rs(8);
    int recd[63] = {};
    int expected[63] = {};
    int output[63];

    // send this message
    recd[ 0] = 8+4;
    recd[ 1] = 3;
    recd[ 2] = 8+2;
    recd[ 3] = 16+1;
    recd[ 4] = 16+2;
    recd[ 5] = 32+8+3;      // error
    recd[ 6] = 4;
    recd[ 7] = 32+2;
    recd[ 8] = 32+2;        // error
    recd[ 9] = 32+16+8+1;
    recd[10] = 32+16+1;
    recd[11] = 32+16+3;
    recd[12] = 16+8;        // error
    recd[13] = 32+2;        // error
    recd[14] = 16+4;
    recd[15] = 16+4+3;
    recd[16+0] = 1;
    recd[16+1] = 0;
    recd[16+2] = 0;
    recd[16+3] = 0;
    recd[16+4] = 0;
    recd[16+5] = 0;
    recd[16+6] = 8;

    // expect this message
    expected[ 0] = 8+4;
    expected[ 1] = 3;
    expected[ 2] = 8+2;
    expected[ 3] = 16+1;
    expected[ 4] = 16+2;
    expected[ 5] = 32+3;
    expected[ 6] = 4;
    expected[ 7] = 32+2;
    expected[ 8] = 32+8+2;
    expected[ 9] = 32+16+8+1;
    expected[10] = 32+16+1;
    expected[11] = 32+16+3;
    expected[12] = 16;
    expected[13] = 32+8+2;
    expected[14] = 16+4;
    expected[15] = 16+4+3;
    expected[16+0] = 1;
    expected[16+1] = 0;
    expected[16+2] = 0;
    expected[16+3] = 0;
    expected[16+4] = 0;
    expected[16+5] = 0;
    expected[16+6] = 8;


    rs.decode(recd, output); /* recd[] is returned in polynomial form */

    /*
    printf("Results for Reed-Solomon code (n=%3d, k=%3d, t= %3d)\n\n", nn, kk, tt);
    printf("  i  recd[i](decoded)\n");
    for (int i = 0; i < nn; i++) {
        char b[9];
        byte_to_binary(b, output[i]);
        printf("%3d    [%s]\n", i, b);
    }
    printf("\n");
    */

    EXPECT_THAT(expected, testing::ElementsAreArray(output, 63));
}
