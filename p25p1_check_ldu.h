#ifndef __P25P1_CHECK_LDU_H__
#define __P25P1_CHECK_LDU_H__

#ifdef __cplusplus
extern "C" {
#endif

int check_and_fix_hamming_10_12_13(char* hex, char* parity);

/**
 * Attempts to correct 12 hex words using the Reed-Solomon(24,12,13) FEC.
 * \param data The packed hex words, each of 6 chars, one after the other.
 * \param parity The corresponding 12 hex words with the parity information.
 * \return 1 if irrecoverable errors have been detected, 0 otherwise.
 */
int check_and_fix_redsolomon_24_12_13(char* data, char* parity);

/**
 * Attempts to correct 16 hex words using the Reed-Solomon(24,16,9) FEC.
 * \param data The packed hex words, each of 6 chars, one after the other.
 * \param parity The corresponding 8 hex words with the parity information.
 * \return 1 if irrecoverable errors have been detected, 0 otherwise.
 */
int check_and_fix_redsolomon_24_16_9(char* data, char* parity);

#ifdef __cplusplus
}
#endif

#endif // __P25P1_CHECK_LDU_H__
