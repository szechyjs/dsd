
#ifndef P25P1_CHECK_HDU_H_f5f079faf2d64cf5831e1da1ab83b9ba
#define P25P1_CHECK_HDU_H_f5f079faf2d64cf5831e1da1ab83b9ba

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Attempts to correct an hex (6-bit) word using the Golay(24,12,8) FEC.
 * \param hex The 6-bit word to error correct, given as a char array of 6 elements, one bit each.
 * \param parity A 12-bit word with the Golay parity data, given as a char array of 12 elements.
 * \param fixed_error Output. Filled with the number of errors fixed in the input.
 * \return 1 if there were uncorrectable errors in the input, 0 otherwise.
 */
int check_and_fix_golay_24_6(char* hex, char* parity, int* fixed_errors);

/**
 * Attempts to correct a 12-bit word using the Golay(24,12,8) FEC.
 * \param dodeca The 12-bit word to error correct, given as a char array of 12 elements, one bit each.
 * \param parity A 12-bit word with the Golay parity data, given as a char array of 12 elements.
 * \param fixed_error Output. Filled with the number of errors fixed in the input.
 * \return 1 if there were uncorrectable errors in the input, 0 otherwise.
 */
int check_and_fix_golay_24_12(char* dodeca, char* parity, int* fixed_errors);

/**
 * Encodes an hex word using the Golay(24,12,8) FEC.
 * \param hex A 6-bit word to encode.
 * \param out_parity The address to be filled with the calculated Golay parity.
 */
void encode_golay_24_6(char* hex, char* out_parity);

/**
 * Encodes an 12-bitword using the Golay(24,12,8) FEC.
 * \param dodeca A 12-bit word to encode.
 * \param out_parity The address to be filled with the calculated Golay parity.
 */
void encode_golay_24_12(char* dodeca, char* out_parity);

/**
 * Attempts to correct 20 hex words using the Reed-Solomon(36,20,17) FEC.
 * \param data The packed hex words, each of 6 chars, one after the other.
 * \param parity The corresponding 16 hex words with the parity information.
 * \return 1 if irrecoverable errors have been detected, 0 otherwise.
 */
int check_and_fix_redsolomon_36_20_17(char* data, char* parity);

/**
 * Calculates the Reed-Solomon parity of 20 words of 6 bits each.
 * \param hex_data The address with the words whose parity we want to calculate.
 * \param fixed_parity The address to be filled with the calculated parity. 16 words of 6 bits are produced.
 */
void encode_reedsolomon_36_20_17(char* hex_data, char* fixed_parity);

#ifdef __cplusplus
}
#endif

#endif // P25P1_CHECK_HDU_H_f5f079faf2d64cf5831e1da1ab83b9ba
