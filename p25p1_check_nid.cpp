#include <itpp/itcomm.h>

#include "p25p1_check_nid.h"



using namespace itpp;

// Ideas taken from http://op25.osmocom.org/trac/wiki.png/browser/op25/gr-op25/lib/decoder_ff_impl.cc
// See also p25_training_guide.pdf page 48.
// See also tia-102-baaa-a-project_25-fdma-common_air_interface.pdf page 40.
// BCH encoder/decoder implementation from IT++. GNU GPL 3 license.

itpp::BCH bch(63, 16, 11, "6 3 3 1 1 4 1 3 6 7 2 3 5 4 5 3", true);

/**
 * Convenience class to calculate the parity of the DUID values. Keeps a table with the expected outcomes
 * for fast lookup.
 */
class ParityTable
{
private:
    unsigned char table[16];

    unsigned char get_index(unsigned char x, unsigned char y)
    {
        return (x<<2) + y;
    }

public:
    ParityTable()
    {
        for (unsigned int i=0; i<sizeof(table); i++) {
            table[i] = 0;
        }
        table[get_index(1,1)] = 1;
        table[get_index(2,2)] = 1;
    }

    unsigned char get_value(unsigned char x, unsigned char y)
    {
        return table[get_index(x,y)];
    }

} parity_table;

int check_NID(char* bch_code, int* new_nac, char* new_duid, unsigned char parity)
{
    int result;

    // Fill up with the given input
    bvec input(63);
    for(unsigned int i=0; i<63; i++) {
        input[i] = bch_code[i];
    }

    // Decode it
    bvec decoded, cw_isvalid;
    bool ok = bch.decode(input, decoded, cw_isvalid);

    if (!ok) {
        // Decode failed
        result = 0;
    } else {
        // Take the NAC from the decoded output. It's a 12 bit number starting from position 0.
        // Just convert the output from a binary sequence to an integer.
        int nac = 0;
        for (int i=0; i<12; i++) {
            nac <<= 1;
            nac |= (int)decoded[i];
        }
        *new_nac = nac;

        // Take the fixed DUID from the encoded output. 4 bit value starting at position 12.
        unsigned char new_duid_0 = (((int)decoded[12])<<1) + ((int)decoded[13]);
        unsigned char new_duid_1 = (((int)decoded[14])<<1) + ((int)decoded[15]);
        new_duid[0] = new_duid_0 + '0';
        new_duid[1] = new_duid_1 + '0';
        new_duid[2] = 0;    // Null terminate

        // Check the parity
        unsigned char expected_parity = parity_table.get_value(new_duid_0, new_duid_1);

        if (expected_parity != parity) {
            // Ignore, not sure what to do
            //printf("Error in parity detected?");
        }

        result = 1;
    }

    return result;
}
