
#ifndef P25P1_LDU_H_a3c417fcb7804991b0e6d828066bd133
#define P25P1_LDU_H_a3c417fcb7804991b0e6d828066bd133

#include "dsd.h"

#include "p25p1_hdu.h"
#include "p25p1_const.h"
#include "p25p1_heuristics.h"

/**
 * Separate imbe frames and deinterleave.
 * This important methods read the IMBE data from the stream and passes it to the vocoder to produce audio.
 * \param opts The DSD options.
 * \param state The DSD state.
 * \status_count An index that allows us to skip the status words interleaved every 36 dibit in the data
 * stream.
 */
void process_IMBE (dsd_opts* opts, dsd_state* state, int* status_count);

/**
 * Reads an hex word, its parity bits and attempts to error correct it using the Hamming FEC.
 * \param opts The DSD options.
 * \param state The DSD state.
 * \param hex Pointer where to store the read hex word. Six bytes, one per bit.
 * \param status_count An index that allows us to skip the status words interleaved every 36 dibit in the data
 * stream.
 * \param analog_singal_array Pointer to a sequence of AnalogSignal elements.
 * \param analog_signal_index The current index in the analog_singal_array. This value is increased on each
 * dibit read.
 */
void read_and_correct_hex_word (dsd_opts* opts, dsd_state* state, char* hex, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index);

/**
 * Correct the information in analog_signal_array according with the content of data, which has been
 * error corrected and should be valid.
 * \param Dibits that have already been error corrected and we trust are correct.
 * \count Number of dibits.
 * \param analog_signal_array Pointer to a sequence of AnalogSignal elements, as many as the value of count.
 */
void correct_hamming_dibits(char* hex_data, int hex_count, AnalogSignal* analog_signal_array);

/**
 * Logs some debug info.
 */
void debug_ldu_header(dsd_state* state);

#endif // P25P1_LDU_H_a3c417fcb7804991b0e6d828066bd133
