
#ifndef P25P1_HDU_H_9f95c3a5072842e8aaf94444e1452d20
#define P25P1_HDU_H_9f95c3a5072842e8aaf94444e1452d20


#include "p25p1_heuristics.h"

/**
 * Reads a dibit (two bits) from the stream of data. Takes into account the periodic occurrence of status
 * bits that show up every 36 dibits. Status bits are discarded.
 * \param opts A pointer the the DSD options.
 * \param state A pointer the the DSD state structure.
 * \param output A pointer to be filled with the dibit read. Two bytes used for each dibit.
 * \param status_count Variable used to keep track of the status symbols in the dibit stream. There is one
 * status every 36 dibits. With this counter we can skip the status.
 * \param analog_signal A pointer with the actual analog value read from the input signal and that has been
 * interpreted as the dibit we are returning in output.
 * \param did_read_status Address were a boolean is returned. This is set to true when one status symbol was
 * read (and skipped). This indicates that the sequence on dibits has been broken and is used by the P25
 * heuristics.
 */
int read_dibit (dsd_opts* opts, dsd_state* state, char* output, int* status_count, int* analog_signal,
        int* did_read_status);

/**
 * Reads a number of dibits and stores its value together with the actual analog value that has been
 * intrepreted as the dibit into an AnalogSignal structure. This is used later in the P25 heuristics module
 * to improve the accuracy of the digitizer.
 * \param opts A pointer the the DSD options.
 * \param state A pointer the the DSD state structure.
 * \param output A pointer to be filled with the dibits read.
 * \param count The number of bits to read (remember: 1 dibit = 2 bits).
 * \param status_count Variable used to keep track of the status symbols in the dibit stream. There is one
 * status every 36 dibits. With this counter we can skip the status.
 * \param analog_signal_array The pointer to the start of a sequence of AnalogSignal structures. The
 * information is updated for each dibit read.
 * \param analog_signal_index The actual index into the AnalogSignal array. This is increased here wich each
 * dibit read.
 */
void read_dibit_update_analog_data (dsd_opts* opts, dsd_state* state, char* buffer, unsigned int count,
        int* status_count, AnalogSignal* analog_signal_array, int* analog_signal_index);

/**
 * Reads a word, made up of length bits or length/2 dibits. Also keeps track of the status dibits.
 */
void read_word (dsd_opts* opts, dsd_state* state, char* word, unsigned int length, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index);

/**
 * Reads 4 bits from the stream of data. Those are the parity bits that follow each hex word.
 */
void read_hamm_parity (dsd_opts* opts, dsd_state* state, char* parity, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index);

/**
 * Reads 12 bits from the stream of data. Those are the extended Golay parity bits that follow a word of data.
 */
void read_golay24_parity (dsd_opts* opts, dsd_state* state, char* parity, int* status_count, AnalogSignal* analog_signal_array, int* analog_signal_index);

/**
 * Reads bits from the data stream that should all be zeros. This is valuable information for the
 * heuristics module and we update it.
 * \param opts A pointer the the DSD options.
 * \param state A pointer the the DSD state structure.
 * \param analog_signal_array The pointer to the start of a sequence of AnalogSignal structures.
 * \param length Number of bits to read.
 * \param status_count Variable used to keep track of the status symbols in the dibit stream.
 * \param new_sequence Bool used to update the sequence_broken value of the first AnalogSignal element.
 */
void read_zeros(dsd_opts* opts, dsd_state* state, AnalogSignal* analog_signal_array, unsigned int length,
        int* status_count, int new_sequence);

#endif // P25P1_HDU_H_9f95c3a5072842e8aaf94444e1452d20
