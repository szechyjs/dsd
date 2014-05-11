
#ifndef P25P1_CHECK_LDU_H_c1734445a67e47caa25673d7a4ce7520
#define P25P1_CHECK_LDU_H_c1734445a67e47caa25673d7a4ce7520

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Attempts to error correct a hex (6-bit) word using the Hamming(10,6,3) FEC.
 * \param hex The 6-bit word to error correct.
 * \param parity The parity to use in the error correction. 4 bits.
 * \return The estimated error count. It might be erroneous.
 */
int check_and_fix_hamming_10_6_3(char* hex, char* parity);

/**
 * Encodes an hex word with the Hamming(10,6,3) FEC.
 * \param hex The 6-bit word to error correct.
 * \param out_parity The address where to store the calculated parity. 4 bytes are used, one for each bit.
 */
void encode_hamming_10_6_3(char* hex, char* out_parity);

/**
 * Attempts to correct 12 hex words using the Reed-Solomon(24,12,13) FEC.
 * \param data The packed hex words, each of 6 chars, one after the other.
 * \param parity The corresponding 12 hex words with the parity information.
 * \return 1 if irrecoverable errors have been detected, 0 otherwise.
 */
int check_and_fix_reedsolomon_24_12_13(char* data, char* parity);

/**
 * Encodes 12 hex words with the  Reed-Solomon(24,12,13) FEC.
 * \param hex_data The packed 12 hex words to encode.
 * \fixed_parity The address were to store the 12 hex words with the Reed-Solomon parity.
 */
void encode_reedsolomon_24_12_13(char* hex_data, char* fixed_parity);

/**
 * Attempts to correct 16 hex words using the Reed-Solomon(24,16,9) FEC.
 * \param data The packed 16 hex words, each of 6 chars, one after the other.
 * \param parity The corresponding 8 hex words with the parity information.
 * \return 1 if irrecoverable errors have been detected, 0 otherwise.
 */
int check_and_fix_reedsolomon_24_16_9(char* data, char* parity);

/**
 * Encodes 12 hex words with the  Reed-Solomon(24,16,9) FEC.
 * \param hex_data The packed 16 hex words to encode.
 * \fixed_parity The address were to store the 8 hex words with the Reed-Solomon parity.
 */
void encode_reedsolomon_24_16_9(char* hex_data, char* fixed_parity);

#ifdef __cplusplus
}
#endif

#endif // P25P1_CHECK_LDU_H_c1734445a67e47caa25673d7a4ce7520
