
#ifndef __P25P1_LDU_H__
#define __P25P1_LDU_H__

#include "dsd.h"
#include "p25p1_const.h"

// separate imbe frames and deinterleave
void
process_IMBE (dsd_opts* opts, dsd_state* state, int* status_count);

/**
 * Read an hex word, its parity bits and attempts to error correct it using the Hamming algorithm.
 */
void
read_and_correct_hex_word (dsd_opts* opts, dsd_state* state, char* hex, int* status_count);

#endif // __P25P1_LDU_H__
