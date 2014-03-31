
#ifndef __P25P1_HDU_H__
#define __P25P1_HDU_H__

/**
 * Reads a dibit keeping track of the status dibits that shows up every 70 bits
 */
void read_dibit (dsd_opts* opts, dsd_state* state, char* output, int* status_count);

/**
 * Read an "hex" word. An hex word is made up of 3 dibits. Also keeps track of the status dibits.
 */
void read_hex (dsd_opts* opts, dsd_state* state, char* hex, int* status_count);

#endif // __P25P1_HDU_H__
