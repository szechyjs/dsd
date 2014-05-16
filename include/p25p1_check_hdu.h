#ifndef __P25P1_CHECK_HDU_H__
#define __P25P1_CHECK_HDU_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Attempts to correct an hex (6 bit) word using the Golay(24,12,8) FEC.
 * \param hex The 6 bit word to error correct, given as a char array of 6 elements, each one bit.
 * \param parity A 12 bit word with the Golay parity data, given as a char array of 12 elements.
 * \param fixed_error Output. Filled with the number of errors fixed in the input.
 * \return 1 if there were uncorrectable errors in the input, 0 otherwise.
 */
int check_and_fix_golay24(char* hex, char* parity, int* fixed_errors);

/**
 * Attempts to correct 20 hex words using the Reed-Solomon(36,20,17) FEC.
 * \param data The packed hex words, each of 6 chars, one after the other.
 * \param parity The corresponding 16 hex words with the parity information.
 * \return 1 if irrecoverable errors have been detected, 0 otherwise.
 */
int check_and_fix_redsolomon_36_20_17(char* data, char* parity);

#ifdef __cplusplus
}
#endif

#endif // __P25P1_CHECK_HDU_H__
