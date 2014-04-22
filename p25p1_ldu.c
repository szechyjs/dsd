
#include "dsd.h"
#include "p25p1_const.h"
#include "p25p1_hdu.h"

// separate imbe frames and deinterleave
void
process_IMBE (dsd_opts* opts, dsd_state* state, int* status_count)
{
  int j, dibit, status;
  char imbe_fr[8][23];
  const int *w, *x, *y, *z;

  w = iW;
  x = iX;
  y = iY;
  z = iZ;

  for (j = 0; j < 72; j++)
    {
      if (*status_count == 35)
        {
          status = getDibit (opts, state);
          // TODO: do something useful with the status bits...
          *status_count = 1;
        }
      else
        {
          (*status_count)++;
        }
      dibit = getDibit (opts, state);
      imbe_fr[*w][*x] = (1 & (dibit >> 1)); // bit 1
      imbe_fr[*y][*z] = (1 & dibit);        // bit 0
      w++;
      x++;
      y++;
      z++;
    }

  if (state->p25kid == 0 || opts->unmute_encrypted_p25 == 1)
    {
      processMbeFrame (opts, state, imbe_fr, NULL, NULL);
    }
}

/**
 * Read 4 bits from the stream of data. Those are the parity bits that follow each hex word.
 */
static void
read_hamm_parity (dsd_opts* opts, dsd_state* state, char* parity, int* status_count)
{
  // Read 2 dibits = read 4 bits.
  read_dibit(opts, state, parity, status_count);
  read_dibit(opts, state, parity + 2, status_count);
}

//#define __LDU_DEBUG__

/**
 * Read an hex word, its parity bits and attempts to error correct it using the Hamming algorithm.
 */
void
read_and_correct_hex_word (dsd_opts* opts, dsd_state* state, char* hex, int* status_count)
{
  char parity[4];
  int i;

  // Read the hex word
  read_hex (opts, state, hex, status_count);
  // Read the parity
  read_hamm_parity (opts, state, parity, status_count);

#ifdef __LDU_DEBUG__
  printf("[");
  for (i = 0; i < 6; i++) {
      printf("%c", (hex[i] == 1)? 'X' : ' ');
  }
  printf("-");
  for (i = 0; i < 4; i++) {
      printf("%c", (parity[i] == 1)? 'X' : ' ');
  }
  printf("]");
#endif

  // Use Hamming to error correct the hex word
  int error_count = check_and_fix_hamming_10_12_13(hex, parity);

  if (error_count == 1) {
      state->debug_header_errors++;
  } else if (error_count ==2) {
      state->debug_header_critical_errors++;
  }

#ifdef __LDU_DEBUG__
  printf(" -> [");
  for (i = 0; i < 6; i++) {
      printf("%c", (hex[i] == 1)? 'X' : ' ');
  }
  printf("]");
  if (error_count == 1) {
      printf(" fixed!");
  } else if (error_count ==2) {
      printf(" IRRECOVERABLE");
  }
  printf("\n");
#endif
}
