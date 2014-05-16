#ifndef __P25P1_CHECK_NID_H__
#define __P25P1_CHECK_NID_H__

#ifdef __cplusplus
extern "C" {
#endif

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
int check_NID(char* bch_code, int* new_nac, char* new_duid, unsigned char parity);

#ifdef __cplusplus
}
#endif

#endif // __P25P1_CHECK_NID_H__
