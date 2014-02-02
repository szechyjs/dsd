#ifndef __CHECK_NID_H__
#define __CHECK_NID_H__

#ifdef __cplusplus 
extern "C" {
#endif

int check_NID(char* bch_code, int* new_nac, char* new_duid, unsigned char parity);

#ifdef __cplusplus 
}
#endif

#endif // __CHECK_NID_H__
