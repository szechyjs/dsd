
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <bitset>

#include "Hamming.hpp"

class HammingTest : public testing::Test
{
protected:

    static void print_debug(int value, int fixed, int error_count)
    {
        std::cout << std::bitset<10>(value) << ": ";
        switch (error_count) {
            case 0:
                std::cout << "OK";
                break;
            case 1:
                std::cout << "one error detected, hopefully fixed: " << std::bitset<6>(fixed);
                break;
            case 2:
                std::cout << "irrecoverable :(";
                break;
            default:
                break;
        }
        std::cout << std::endl;
    }

    static void check_hamming(int v, int expected_v, int expected_errors)
    {
        Hamming_10_6_3 hamming;
        Hamming_10_6_3_TableImpl hamming_table;

        int output_1;
        int output_2;
        int error_count_1 = hamming.decode(v, &output_1);
        int error_count_2 = hamming_table.decode(v, &output_2);

        //print_debug(v, output_1, error_count_1);

        if (expected_errors != -1) {
            EXPECT_THAT(expected_errors, error_count_1);
        }
        EXPECT_THAT(error_count_1, error_count_2);

        if (expected_v != -1) {
            EXPECT_THAT(expected_v, output_1);
        }
        EXPECT_THAT(output_1, output_2);
    }

    static void check_hamming(Hamming_Inteface* hamming, char* hex, char* parity, int expected_errors, char* expected)
    {
        int value = Hamming_10_6_3_TableImpl::convert_hex_parity_to_int(hex, parity);
        int error_count = hamming->decode(hex, parity);
        int fixed = Hamming_10_6_3_TableImpl::convert_hex_to_int(hex);

        //print_debug(value, fixed, error_count);

        EXPECT_THAT(expected_errors, error_count);
        for (unsigned int i=0; i<6; i++) {
            EXPECT_THAT(expected[i], hex[i]);
        }
    }

};

TEST_F(HammingTest, Test1)
{
    // OK
    std::bitset<10>    value(std::string("1010100110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 0);
}

TEST_F(HammingTest, Test2)
{
    // Error on parity bit 0 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1010100111"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test3)
{
    // Error on parity bit 1 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1010100100"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test4)
{
    // Error on parity bit 2 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1010100010"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test5)
{
    // Error on parity bit 3 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1010101110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test6)
{
    // Error on bit 4 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1010110110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test7)
{
    // Error on bit 5 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1010000110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test8)
{
    // Error on bit 6 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1011100110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test9)
{
    // Error on bit 7 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1000100110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test10)
{
    // Error on bit 8 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("1110100110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test11)
{
    // Error on bit 9 (9 is the left-most and 0 is the right-most)
    std::bitset<10>    value(std::string("0010100110"));
    std::bitset< 6> expected(std::string("101010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test12)
{
    // Two errors
    std::bitset<10>    value(std::string("0010100111"));
    std::bitset< 6> expected(std::string("001010"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 2);
}

TEST_F(HammingTest, Test13)
{
    // Two errors, in this case they go undetected, thinks it's a parity error
    std::bitset<10>    value(std::string("1011000110"));
    std::bitset< 6> expected(std::string("101100"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 1);
}

TEST_F(HammingTest, Test14)
{
    // OK
    std::bitset<10>    value(std::string("0001000111"));
    std::bitset< 6> expected(std::string("000100"));
    check_hamming((int)value.to_ulong(), (int)expected.to_ulong(), 0);
}

TEST_F(HammingTest, TestEveryValue)
{
    // Test every posible value
    for (int v=0; v<1023; v++) {
        check_hamming(v, -1, -1);
    }
}

TEST_F(HammingTest, Test_hex_parity_interface_no_errors)
{
    char hex   [6] = {1,1,1,0,1,0};
    char fixed [6] = {1,1,1,0,1,0};
    char parity[4] = {1,0,1,1};

    Hamming_10_6_3 hamming;
    check_hamming(&hamming, hex, parity, 0, fixed);

    Hamming_10_6_3_TableImpl hamming_table;
    check_hamming(&hamming_table, hex, parity, 0, fixed);
}

TEST_F(HammingTest, Test_hex_parity_interface_one_error_1)
{
    char hex_1 [6] = {1,1,1,1,1,0};
    char hex_2 [6] = {1,1,1,1,1,0};
    char fixed [6] = {1,1,1,0,1,0};
    char parity[4] = {1,0,1,1};

    Hamming_10_6_3 hamming;
    check_hamming(&hamming, hex_1, parity, 1, fixed);
    // hex is modified in place

    Hamming_10_6_3_TableImpl hamming_table;
    check_hamming(&hamming_table, hex_2, parity, 1, fixed);
    // hex is modified in place
}

TEST_F(HammingTest, Test_hex_parity_interface_one_error_2)
{
    char hex_1 [6] = {1,1,1,1,1,0};
    char hex_2 [6] = {1,1,1,1,1,0};
    char fixed [6] = {1,1,1,1,1,1};
    char parity[4] = {0,0,0,0};

    Hamming_10_6_3 hamming;
    check_hamming(&hamming, hex_1, parity, 1, fixed);
    // hex is modified in place

    Hamming_10_6_3_TableImpl hamming_table;
    check_hamming(&hamming_table, hex_2, parity, 1, fixed);
    // hex is modified in place
}

TEST_F(HammingTest, Test_hex_parity_interface_one_error_3)
{
    char hex_1 [6] = {1,0,0,0,0,0};
    char hex_2 [6] = {1,0,0,0,0,0};
    char fixed [6] = {0,0,0,0,0,0};
    char parity[4] = {0,0,0,0};

    Hamming_10_6_3 hamming;
    check_hamming(&hamming, hex_1, parity, 1, fixed);
    // hex is modified in place

    Hamming_10_6_3_TableImpl hamming_table;
    check_hamming(&hamming_table, hex_2, parity, 1, fixed);
    // hex is modified in place
}
