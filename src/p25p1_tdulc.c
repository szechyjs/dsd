/*
 * Copyright (C) 2010 DSD Author
 * GPG Key ID: 0x3F1D7FD0 (74EF 430D F7F2 0A48 FCE6  F630 FAA2 635D 3F1D 7FD0)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "dsd.h"

#include "p25p1_hdu.h"
#include "p25p1_check_hdu.h"
#include "p25p1_check_ldu.h"
#include "p25p1_heuristics.h"

// Uncomment for some verbose debug info
//#define TDULC_DEBUG

/**
 * Reverse the order of bits in a 12-bit word. We need this to accommodate to the expected bit order in
 * some algorithms.
 * \param dodeca The 12-bit word to reverse.
 */
static void
swap_hex_words_bits(char* dodeca)
{
  int j;
  for(j=0; j<6; j++)
    {
      int swap;
      swap = dodeca[j];
      dodeca[j] = dodeca[j+6];
      dodeca[j+6] = swap;
    }
}

/**
 * Reverse the order of bits in a sequence of six pairs of 12-bit words and their parities.
 * \param dodeca_data Pointer to the start of the 12-bit words sequence.
 * \param dodeca_parity Pointer to the parities sequence.
 */
static void
swap_hex_words(char* dodeca_data, char* dodeca_parity)
{
  int i;
  for(i=0; i<6; i++)
    {
      swap_hex_words_bits(dodeca_data + i*12);
      swap_hex_words_bits(dodeca_parity + i*12);
    }
}

/**
 * Read an hex word, its parity bits and attempts to error correct it using the Hamming algorithm.
 */
static void
read_and_correct_dodeca_word (dsd_opts* opts, dsd_state* state, char* dodeca, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index)
{
  char parity[12];
  int fixed_errors;
  int irrecoverable_errors;

  // Read the hex word
  read_word (opts, state, dodeca, 12, status_count, analog_signal_array, analog_signal_index);
  // Read the parity
  read_golay24_parity (opts, state, parity, status_count, analog_signal_array, analog_signal_index);

#ifdef TDULC_DEBUG
  printf("[");
  for (i = 0; i < 12; i++)
    {
      printf("%c", (dodeca[i] == 1)? 'X' : ' ');
    }
  printf("-");
  for (i = 0; i < 12; i++)
    {
      printf("%c", (parity[i] == 1)? 'X' : ' ');
    }
  printf("]");
#endif

  // Use extended golay to error correct the dodeca word
  irrecoverable_errors = check_and_fix_golay_24_12(dodeca, parity, &fixed_errors);

  state->debug_header_errors += fixed_errors;
  if (irrecoverable_errors != 0)
    {
      state->debug_header_critical_errors++;
    }

#ifdef TDULC_DEBUG
  printf(" -> [");
  for (i = 0; i < 12; i++)
    {
      printf("%c", (dodeca[i] == 1)? 'X' : ' ');
    }
  printf("]");
  if (irrecoverable_errors == 0)
    {
      if (fixed_errors > 0)
        {
          printf(" fixed!");
        }
    }
  else
    {
      printf(" IRRECOVERABLE");
    }
  printf("\n");
#endif
}

/**
 * Correct the information in analog_signal_array according with the content of data, which has been
 * error corrected and should be valid.
 * \param data A sequence of 12-bit words.
 * \param count Number of words in the sequence.
 * \param analog_signal_array Pointer to a sequence of AnalogSignal elements, as many as the value of count.
 */
static void
correct_golay_dibits_12(char* data, int count, AnalogSignal* analog_signal_array)
{
  int i, j;
  int analog_signal_index;
  int dibit;
  char parity[12];


  analog_signal_index = 0;

  for (i=count-1; i>=0; i--)
    {
      for (j=0; j<12; j+=2)  // 6 iterations -> 6 dibits
        {
          dibit = (data[i*12+j] << 1) | data[i*12+j+1];
          analog_signal_array[analog_signal_index].corrected_dibit = dibit;

#ifdef HEURISTICS_DEBUG
          if (analog_signal_array[analog_signal_index].dibit != dibit)
            {
              printf("TDULC data word corrected from %i to %i, analog value %i\n",
                      analog_signal_array[analog_signal_index].dibit, dibit, analog_signal_array[analog_signal_index].value);
            }
#endif

          analog_signal_index++;
        }

      // Calculate the golay parity for the hex word
      encode_golay_24_12(data+i*12, parity);

      for (j=0; j<12; j+=2) // 6 iterations -> 6 dibits
        {
          dibit = (parity[j] << 1) | parity[j+1];
          analog_signal_array[analog_signal_index].corrected_dibit = dibit;

#ifdef HEURISTICS_DEBUG
          if (analog_signal_array[analog_signal_index].dibit != dibit)
            {
              printf("TDULC parity corrected from %i to %i, analog value %i\n",
                      analog_signal_array[analog_signal_index].dibit, dibit, analog_signal_array[analog_signal_index].value);
            }
#endif

          analog_signal_index++;
        }
    }
}

void
read_zeros(dsd_opts* opts, dsd_state* state, AnalogSignal* analog_signal_array, unsigned int length,
        int* status_count, int new_sequence)
{
  char* buffer;
  unsigned int i;
  int analog_signal_index;

  analog_signal_index = 0;
  buffer = malloc(length);
  read_dibit_update_analog_data (opts, state, buffer, length, status_count, analog_signal_array, &analog_signal_index);
  free(buffer);
  if (new_sequence)
    {
      analog_signal_array[0].sequence_broken = 1;
    }

  for (i=0;i<length/2;i++)
    {
      analog_signal_array[i].corrected_dibit = 0;
#ifdef HEURISTICS_DEBUG
      if (analog_signal_array[i].corrected_dibit != analog_signal_array[i].dibit)
        {
          printf("TDULC ending zeros corrected from %i to %i, analog value %i\n",
                  analog_signal_array[i].dibit, 0, analog_signal_array[i].value);
        }
#endif
    }

  // We know that all these bits should be zero. Use this information for the heuristics module
  contribute_to_heuristics(state->rf_mod, &(state->p25_heuristics), analog_signal_array, length/2);
}

void
processTDULC (dsd_opts* opts, dsd_state* state)
{
  int i;
  char lcinfo[57], lcformat[9], mfid[9];

  int status_count;

  char dodeca_data[6][12];    // Data in 12-bit words. A total of 6 words.
  char dodeca_parity[6][12];  // Reed-Solomon parity of the data. A total of 6 parity 12-bit words.

  int irrecoverable_errors;

  AnalogSignal analog_signal_array[6*(6+6)+6*(6+6)+10];
  int analog_signal_index;


  analog_signal_index = 0;

  // we skip the status dibits that occur every 36 symbols
  // the first IMBE frame starts 14 symbols before next status
  // so we start counter at 36-14-1 = 21
  status_count = 21;

  for(i=5; i>=0; i--) {
      read_and_correct_dodeca_word (opts, state, &(dodeca_data[i][0]), &status_count, analog_signal_array, &analog_signal_index);
  }

  for(i=5; i>=0; i--) {
      read_and_correct_dodeca_word (opts, state, &(dodeca_parity[i][0]), &status_count, analog_signal_array, &analog_signal_index);
  }

  // Swap the two 6-bit words to accommodate for the expected word order of the Reed-Solomon decoding
  swap_hex_words((char*)dodeca_data, (char*)dodeca_parity);

  // Error correct the hex_data using Reed-Solomon hex_parity
  irrecoverable_errors = check_and_fix_reedsolomon_24_12_13((char*)dodeca_data, (char*)dodeca_parity);

  // Recover the original order
  swap_hex_words((char*)dodeca_data, (char*)dodeca_parity);

  if (irrecoverable_errors == 1)
    {
      state->debug_header_critical_errors++;

      // We can correct (13-1)/2 = 6 errors. If we failed, it means that there were more than 6 errors in
      // these 12+12 words. But take into account that each hex word was already error corrected with
      // Golay 24, which can correct 3 bits on each sequence of (12+12) bits. We could say that there were
      // 7 errors of 4 bits.
      update_error_stats(&state->p25_heuristics, 12*6+12*6, 7*4);

    }
  else
    {
      // Same comments as in processHDU. See there.

      char fixed_parity[6*12];

      // Correct the dibits that we read according with hex_data values
      correct_golay_dibits_12((char*)dodeca_data, 6, analog_signal_array);

      // Generate again the Reed-Solomon parity
      // Now, swap again for Reed-Solomon
      swap_hex_words((char*)dodeca_data, (char*)dodeca_parity);
      encode_reedsolomon_24_12_13((char*)dodeca_data, fixed_parity);
      // Swap again to recover the original order
      swap_hex_words((char*)dodeca_data, fixed_parity);

      // Correct the dibits that we read according with the fixed parity values
      correct_golay_dibits_12(fixed_parity, 6, analog_signal_array+6*(6+6));

      // Once corrected, contribute this information to the heuristics module
      analog_signal_array[0].sequence_broken = 1;
      contribute_to_heuristics(state->rf_mod, &(state->p25_heuristics), analog_signal_array, 6*(6+6)+6*(6+6));
  }


  // Next 10 dibits should be zeros
  // If an irrecoverable error happens, you should start a new sequence since the previous dibit was not set
  read_zeros(opts, state, analog_signal_array + 6*(6+6)+6*(6+6), 20, &status_count, irrecoverable_errors);

  // Next we should find an status dibit
  if (status_count != 35) {
      printf("*** SYNC ERROR\n");
  }

  // trailing status symbol
  {
      int status;
      status = getDibit (opts, state) + '0';
      // TODO: do something useful with the status bits...
  }

  // Put the corrected data into the DSD structures

  lcformat[8] = 0;
  mfid[8] = 0;
  lcinfo[56] = 0;

  lcformat[0] = dodeca_data[5][ 0] + '0';
  lcformat[1] = dodeca_data[5][ 1] + '0';
  lcformat[2] = dodeca_data[5][ 2] + '0';
  lcformat[3] = dodeca_data[5][ 3] + '0';
  lcformat[4] = dodeca_data[5][ 4] + '0';
  lcformat[5] = dodeca_data[5][ 5] + '0';
  lcformat[6] = dodeca_data[5][ 6] + '0';
  lcformat[7] = dodeca_data[5][ 7] + '0';
  mfid[0]     = dodeca_data[5][ 8] + '0';
  mfid[1]     = dodeca_data[5][ 9] + '0';
  mfid[2]     = dodeca_data[5][10] + '0';
  mfid[3]     = dodeca_data[5][11] + '0';

  mfid[4]     = dodeca_data[4][ 0] + '0';
  mfid[5]     = dodeca_data[4][ 1] + '0';
  mfid[6]     = dodeca_data[4][ 2] + '0';
  mfid[7]     = dodeca_data[4][ 3] + '0';
  lcinfo[0]   = dodeca_data[4][ 4] + '0';
  lcinfo[1]   = dodeca_data[4][ 5] + '0';
  lcinfo[2]   = dodeca_data[4][ 6] + '0';
  lcinfo[3]   = dodeca_data[4][ 7] + '0';
  lcinfo[4]   = dodeca_data[4][ 8] + '0';
  lcinfo[5]   = dodeca_data[4][ 9] + '0';
  lcinfo[6]   = dodeca_data[4][10] + '0';
  lcinfo[7]   = dodeca_data[4][11] + '0';

  lcinfo[8]   = dodeca_data[3][ 0] + '0';
  lcinfo[9]   = dodeca_data[3][ 1] + '0';
  lcinfo[10]  = dodeca_data[3][ 2] + '0';
  lcinfo[11]  = dodeca_data[3][ 3] + '0';
  lcinfo[12]  = dodeca_data[3][ 4] + '0';
  lcinfo[13]  = dodeca_data[3][ 5] + '0';
  lcinfo[14]  = dodeca_data[3][ 6] + '0';
  lcinfo[15]  = dodeca_data[3][ 7] + '0';
  lcinfo[16]  = dodeca_data[3][ 8] + '0';
  lcinfo[17]  = dodeca_data[3][ 9] + '0';
  lcinfo[18]  = dodeca_data[3][10] + '0';
  lcinfo[19]  = dodeca_data[3][11] + '0';

  lcinfo[20]  = dodeca_data[2][ 0] + '0';
  lcinfo[21]  = dodeca_data[2][ 1] + '0';
  lcinfo[22]  = dodeca_data[2][ 2] + '0';
  lcinfo[23]  = dodeca_data[2][ 3] + '0';
  lcinfo[24]  = dodeca_data[2][ 4] + '0';
  lcinfo[25]  = dodeca_data[2][ 5] + '0';
  lcinfo[26]  = dodeca_data[2][ 6] + '0';
  lcinfo[27]  = dodeca_data[2][ 7] + '0';
  lcinfo[28]  = dodeca_data[2][ 8] + '0';
  lcinfo[29]  = dodeca_data[2][ 9] + '0';
  lcinfo[30]  = dodeca_data[2][10] + '0';
  lcinfo[31]  = dodeca_data[2][11] + '0';

  lcinfo[32]  = dodeca_data[1][ 0] + '0';
  lcinfo[33]  = dodeca_data[1][ 1] + '0';
  lcinfo[34]  = dodeca_data[1][ 2] + '0';
  lcinfo[35]  = dodeca_data[1][ 3] + '0';
  lcinfo[36]  = dodeca_data[1][ 4] + '0';
  lcinfo[37]  = dodeca_data[1][ 5] + '0';
  lcinfo[38]  = dodeca_data[1][ 6] + '0';
  lcinfo[39]  = dodeca_data[1][ 7] + '0';
  lcinfo[40]  = dodeca_data[1][ 8] + '0';
  lcinfo[41]  = dodeca_data[1][ 9] + '0';
  lcinfo[42]  = dodeca_data[1][10] + '0';
  lcinfo[43]  = dodeca_data[1][11] + '0';

  lcinfo[44]  = dodeca_data[0][ 0] + '0';
  lcinfo[45]  = dodeca_data[0][ 1] + '0';
  lcinfo[46]  = dodeca_data[0][ 2] + '0';
  lcinfo[47]  = dodeca_data[0][ 3] + '0';
  lcinfo[48]  = dodeca_data[0][ 4] + '0';
  lcinfo[49]  = dodeca_data[0][ 5] + '0';
  lcinfo[50]  = dodeca_data[0][ 6] + '0';
  lcinfo[51]  = dodeca_data[0][ 7] + '0';
  lcinfo[52]  = dodeca_data[0][ 8] + '0';
  lcinfo[53]  = dodeca_data[0][ 9] + '0';
  lcinfo[54]  = dodeca_data[0][10] + '0';
  lcinfo[55]  = dodeca_data[0][11] + '0';

  processP25lcw (opts, state, lcformat, mfid, lcinfo);
}
