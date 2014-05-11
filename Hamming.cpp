
#include "Hamming.hpp"

// definition outside class declaration
Hamming_10_6_3_data Hamming_10_6_3::data;

// definition outside class declaration
// Should go *after* Hamming_10_6_3_data since Hamming_10_6_3_TableImpl_data constructor uses
// Hamming_10_6_3 which in its turn depends on Hamming_10_6_3_data
Hamming_10_6_3_TableImpl_data Hamming_10_6_3_TableImpl::data;


int Hamming_10_6_3::decode(std::bitset<10>& input)
{
    int error_count;

    // Compute syndromes
    int s0 = ((data.h0 & input).count() & 1) << 3;
    int s1 = ((data.h1 & input).count() & 1) << 2;
    int s2 = ((data.h2 & input).count() & 1) << 1;
    int s3 = ((data.h3 & input).count() & 1);
    int parity = s0 | s1 | s2 | s3;

    if (parity == 0) {
        //std::cout << input << " is OK" << std::endl;
        error_count = 0;
    } else {
        // Error detected, attempt to fix it
        int bad_bit_index = data.bad_bit_table[parity];

        if (bad_bit_index < 0) {
            //std::cout << "Irrecoverable error: " << std::bitset<6>(input.to_string()) << std::endl;
            error_count = 2;
        } else {
            // Error in a data bit, or more than one error
            // If there is one erroneous bit, the bad one is the one indicated by parity
            if (bad_bit_index < 4) {
                // Error detected in a parity bit
                //std::cout << "Error on parity bit " << bad_bit_index << std::endl;
                error_count = 1;
            } else {
                input.flip(bad_bit_index);
                //std::cout << "Error on data bit " << bad_bit_index << ", hopefully fixed: " << std::bitset<6>(input.to_string()) << std::endl;
                error_count = 1;
            }
        }
    }
    //std::cout << std::endl;

    return error_count;
}

int Hamming_10_6_3::encode(std::bitset<6>& input)
{
    // Compute syndromes
    int s0 = ((data.gt0 & input).count() & 1) << 3;
    int s1 = ((data.gt1 & input).count() & 1) << 2;
    int s2 = ((data.gt2 & input).count() & 1) << 1;
    int s3 = ((data.gt3 & input).count() & 1);
    int parity = s0 | s1 | s2 | s3;

    return parity;
}

int Hamming_10_6_3_TableImpl::decode(int input, int* output)
{
    assert (input < 1024 && input >= 0);

    // Making use of a table...

    *output = data.fixed_values[input];

    return data.error_counts[input];
}

int Hamming_10_6_3_TableImpl::encode(int input)
{
    assert (input < 64 && input >= 0);

    // Making use of a table...

    return data.encode_parities[input];
}
