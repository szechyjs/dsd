/**
 * Unit tests for the Golay24 implementation.
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Golay24.hpp"

class Golay24Test : public testing::Test
{
protected:

  /**
   * Converts an 24-bit word packed in an integer to a binary string.
   */
  static const char *hex_to_bin24(unsigned long x)
  {
      static char b[27];
      int z, i;

      for (z = 1, i = 0; i < 26; z <<= 1, i++)
      {
          b[i] = ((x & z) != 0)? 'X' : ' ';
          if (i == 5 || i == 12) {
              i++;
              b[i] = '-';
          }
      }
      b[26] = '\0';

      return b;
  }

  /**
   * Attempts to FEC an hex word in the DSD format writing some useful debugging information on the way.
   * \param expected_errors Set it to 4 to indicate that we expect uncorrectable errors
   */
  static void fix_golay24(char* hex, char* sgp, char* expected, int expected_errors)
  {
      DSDGolay24 golay24;

      unsigned int trashed_codeword = golay24.adapt_to_codeword(hex, 6, sgp);

      //printf("[%s]", hex_to_bin24(trashed_codeword));

      int fixed_errors = 0;
      int many_errors = golay24.decode_6(hex, sgp, &fixed_errors); // try to correct bit errors

      /*
      printf(" -> ");
      printf("[");
      for (unsigned int i=0; i<6; i++) {
          printf("%c", hex[i]? 'X' : ' ');
      }
      printf("] ");
      if (many_errors) {
          // Signal that there were errors that couldn't get fixed, probably more than 4.
          printf(" +4 errors");
      } else {
          printf("  %i errors", fixed_errors);
      }
      */

      // check
      if (expected_errors == 4) {
          EXPECT_EQ(1, many_errors);
      } else {
          EXPECT_EQ(0, many_errors);
          EXPECT_EQ(expected_errors, fixed_errors);
      }

      //printf("\n");
  }

  static void check_encode(char* hex, char* expected_parity)
  {
      DSDGolay24 golay24;
      char parity[12];

      golay24.encode_6(hex, parity);

      for(unsigned int i=0; i<12; i++) {
          EXPECT_EQ(expected_parity[i], parity[i]);
      }
  }
};

TEST_F(Golay24Test, Test1)
{
    char hex[6] = { 1, 0, 0, 0, 1, 1};
    char parity[12] = { 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0};
    char expected[6] = { 1, 0, 0, 0, 1, 1};

    fix_golay24(hex, parity, expected, 1);
}

TEST_F(Golay24Test, Test2)
{
    char hex[6] = { 1, 0, 1, 0, 1, 0};
    char parity[12] = { 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1};
    char expected[6] = { 1, 0, 1, 0, 1, 0};

    fix_golay24(hex, parity, expected, 0);
}

TEST_F(Golay24Test, Test3)
{
    char hex[6] = { 1, 0, 1, 1, 1, 0};
    char parity[12] = { 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0};
    char expected[6] = { 1, 0, 1, 1, 1, 0};

    fix_golay24(hex, parity, expected, 4);    // should fail, many errors
}

TEST_F(Golay24Test, Test4)
{
    char hex[6] = { 0, 0, 1, 0, 0, 0};
    char parity[12] = { 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0};
    char expected[6] = { 0, 0, 1, 0, 0, 0};

    fix_golay24(hex, parity, expected, 0);
}

TEST_F(Golay24Test, Test5)
{
    char hex[6] = { 1, 0, 0, 0, 0, 0};
    char parity[12] = { 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1};
    char expected[6] = { 1, 0, 0, 1, 0, 0};

    fix_golay24(hex, parity, expected, 4);    // should fail, many errors
}

TEST_F(Golay24Test, Test6)
{
    char hex[6] = { 0, 0, 0, 0, 0, 0};
    char parity[12] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0};
    char expected[6] = { 0, 0, 0, 0, 0, 0};

    fix_golay24(hex, parity, expected, 4);    // should fail, many errors
}

TEST_F(Golay24Test, Test7)
{
    char hex[6] = { 1, 0, 0, 0, 0, 0};
    char parity[12] = { 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
    char expected[6] = { 0, 0, 1, 0, 0, 0};

    fix_golay24(hex, parity, expected, 2);
}

TEST_F(Golay24Test, Test_encode_1)
{
    char hex[6] = { 1, 0, 1, 0, 1, 0};
    char expected_parity[12] = { 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1};

    check_encode(hex, expected_parity);
}

TEST_F(Golay24Test, Test_encode_2)
{
    char hex[6] = { 0, 0, 1, 0, 0, 0};
    char expected_parity[12] = { 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0};

    check_encode(hex, expected_parity);
}

TEST_F(Golay24Test, Test_encode_3)
{
    char hex[6] = { 0, 0, 0, 0, 0, 0};
    char expected_parity[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    check_encode(hex, expected_parity);
}
