#include <itpp/itcomm.h>

#include "check_nid.h"



using namespace itpp;

// Ideas taken from http://op25.osmocom.org/trac/wiki.png/browser/op25/gr-op25/lib/decoder_ff_impl.cc
// See also p25_training_guide.pdf page 48.
// See also tia-102-baaa-a-project_25-fdma-common_air_interface.pdf page 40.
// BCH encoder/decoder implementation from IT++. GNU GPL 3 license.

itpp::BCH bch(63, 16, 11, "6 3 3 1 1 4 1 3 6 7 2 3 5 4 5 3", true);

/** Convenience class to calculate the parity of the DUID values. Keeps a table with the expected outcomes
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

/** Checks a NID value, returns the fixed NAC, DUID and also an indication if it failed to decode the NID.
 *
 *  \param bch_code Input. An array to the 63 bytes, each containing one bit of the NID.
 *  \param new_nac Output. An address where to store the calculated NAC value after error correction. Should
 *                 be large enough to accommodate for an integer.
 *  \param new_duid Output. An address where to store the calculated DUID value after error correction. Should
 *                  be able to accommodate 3 chars.
 *  \param parity Input. The parity value read.
 *  \return 0 if there were errors processing the NID.
 */
int check_NID(char* bch_code, int* new_nac, char* new_duid, unsigned char parity)
{
    int result;

    // Fill up with the input given
    bvec input(63);
    for(unsigned int i=0; i<63; i++) {
        input[i] = bch_code[i];
    }

    // Decode it
    bvec decoded = bch.decode(input);

    // Check if the decoded output is zero
    bool zero = true;
    for (int i=0; i<decoded.length(); i++) {
        if (decoded[i] != 0) {
            zero = false;
            break;
        }
    }

    if (zero) {
        // Decode failed
        result = 0;
    } else {
        // Encode it again
        bvec encoded = bch.encode(decoded);

        // Take the fixed NAC from the encoded output. It's a 12 bit number starting from position 47.
        // Just convert the output from a binary sequence to an integer.
        int nac = 0;
        for (int i=47; i<47+12; i++) {
            nac <<= 1;
            nac |= (int)encoded[i];
        }
        *new_nac = nac;

        // Take the fixed DUID from the encoded output. 4 bit value starting at position 59.
        unsigned char x = (((int)encoded[59])<<1) + ((int)encoded[60]);
        unsigned char y = (((int)encoded[61])<<1) + ((int)encoded[62]);
        new_duid[0] = x + '0';
        new_duid[1] = y + '0';
        new_duid[2] = 0;

        // Check the parity
        unsigned char expected_parity = parity_table.get_value(x,y);

        // Returns false (0) if they don't match
        result = (expected_parity == parity);
    }

    return result;
}
