#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ReedSolomon.hpp"

TEST(ReedSolomonTest, Test1)
{
    ReedSolomon_63_47_17 rs;
    int recd[63] = {};
    int expected[63] = {};
    int output[63];

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
    expected[16] = 1;
    expected[22] = 8;


    rs.decode(recd, output); /* recd[] is returned in polynomial form */

    EXPECT_THAT(expected, testing::ElementsAreArray(output, 63));
}
