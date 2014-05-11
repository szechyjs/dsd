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
#include "p25p1_const.h"

#include "p25p1_ldu.h"
#include "p25p1_check_ldu.h"
#include "p25p1_hdu.h"


void
processLDU1 (dsd_opts* opts, dsd_state* state)
{
  // extracts IMBE frames from LDU frame
  int i;
  char lcformat[9], mfid[9], lcinfo[57];
  char lsd1[9], lsd2[9];

  int status_count;

  char hex_data[12][6];    // Data in hex-words (6 bit words). A total of 12 hex words.
  char hex_parity[12][6];  // Parity of the data, again in hex-word format. A total of 12 parity hex words.

  int irrecoverable_errors;

  AnalogSignal analog_signal_array[12*(3+2)+12*(3+2)];
  int analog_signal_index;


  analog_signal_index = 0;

  // we skip the status dibits that occur every 36 symbols
  // the first IMBE frame starts 14 symbols before next status
  // so we start counter at 36-14-1 = 21
  status_count = 21;

  if (opts->errorbars == 1)
    {
      printf ("e:");
    }

  // IMBE 1
#ifdef TRACE_DSD
  state->debug_prefix_2 = '0';
#endif
  process_IMBE (opts, state, &status_count);

  // IMBE 2
#ifdef TRACE_DSD
  state->debug_prefix_2 = '1';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 2
  read_and_correct_hex_word (opts, state, &(hex_data[11][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[10][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 9][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 8][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[0*5].sequence_broken = 1;

  // IMBE 3
#ifdef TRACE_DSD
  state->debug_prefix_2 = '2';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 3
  read_and_correct_hex_word (opts, state, &(hex_data[ 7][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 6][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 5][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 4][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[4*5].sequence_broken = 1;

  // IMBE 4
#ifdef TRACE_DSD
  state->debug_prefix_2 = '3';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 4
  read_and_correct_hex_word (opts, state, &(hex_data[ 3][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 2][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 1][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 0][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[8*5].sequence_broken = 1;

  // IMBE 5
#ifdef TRACE_DSD
  state->debug_prefix_2 = '4';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 5
  read_and_correct_hex_word (opts, state, &(hex_parity[11][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[10][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 9][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 8][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[12*5].sequence_broken = 1;

  // IMBE 6
#ifdef TRACE_DSD
  state->debug_prefix_2 = '5';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 6
  read_and_correct_hex_word (opts, state, &(hex_parity[ 7][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 6][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 5][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 4][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[16*5].sequence_broken = 1;

  // IMBE 7
#ifdef TRACE_DSD
  state->debug_prefix_2 = '6';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 7
  read_and_correct_hex_word (opts, state, &(hex_parity[ 3][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 2][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 1][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_parity[ 0][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[20*5].sequence_broken = 1;

  // IMBE 8
#ifdef TRACE_DSD
  state->debug_prefix_2 = '7';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 8: LSD (low speed data)
  {
    char lsd[8];
    char cyclic_parity[8];

    for (i=0; i<=6; i+=2)
      {
        read_dibit(opts, state, lsd+i, &status_count, NULL, NULL);
      }
    for (i=0; i<=6; i+=2)
      {
        read_dibit(opts, state, cyclic_parity+i, &status_count, NULL, NULL);
      }
    for (i=0; i<8; i++)
      {
        lsd1[i] = lsd[i] + '0';
      }

    for (i=0; i<=6; i+=2)
      {
        read_dibit(opts, state, lsd+i, &status_count, NULL, NULL);
      }
    for (i=0; i<=6; i+=2)
      {
        read_dibit(opts, state, cyclic_parity+i, &status_count, NULL, NULL);
      }
    for (i=0; i<8; i++)
      {
        lsd2[i] = lsd[i] + '0';
      }

    // TODO: error correction of the LSD bytes...
    // TODO: do something useful with the LSD bytes...
  }

  // IMBE 9
#ifdef TRACE_DSD
  state->debug_prefix_2 = '8';
#endif
  process_IMBE (opts, state, &status_count);

  if (opts->errorbars == 1)
    {
      printf ("\n");
    }

  if (opts->p25status == 1)
    {
      printf ("lsd1: %s lsd2: %s\n", lsd1, lsd2);
    }

  // trailing status symbol
  {
      int status;
      status = getDibit (opts, state) + '0';
      // TODO: do something useful with the status bits...
  }

  // Error correct the hex_data using Reed-Solomon hex_parity
  irrecoverable_errors = check_and_fix_reedsolomon_24_12_13((char*)hex_data, (char*)hex_parity);
  if (irrecoverable_errors == 1)
    {
      state->debug_header_critical_errors++;

      // We can correct (13-1)/2 = 6 errors. If we failed, it means that there were more than 6 errors in
      // these 12+12 words. But take into account that each hex word was already error corrected with
      // Hamming(10,6,3), which can correct 1 bits on each sequence of (6+4) bits. We could say that there
      // were 7 errors of 2 bits.
      update_error_stats(&state->p25_heuristics, 12*6+12*6, 7*2);
    }
  else
    {
      // Same comments as in processHDU. See there.

      char fixed_parity[12*6];

      // Correct the dibits that we read according with hex_data values
      correct_hamming_dibits((char*)hex_data, 12, analog_signal_array);

      // Generate again the Reed-Solomon parity
      encode_reedsolomon_24_12_13((char*)hex_data, fixed_parity);

      // Correct the dibits that we read according with the fixed parity values
      correct_hamming_dibits(fixed_parity, 12, analog_signal_array+12*(3+2));

      // Once corrected, contribute this information to the heuristics module
      contribute_to_heuristics(state->rf_mod, &(state->p25_heuristics), analog_signal_array, 12*(3+2)+12*(3+2));
    }

#ifdef HEURISTICS_DEBUG
  printf("(audio errors, header errors, critical header errors) (%i,%i,%i)\n",
          state->debug_audio_errors, state->debug_header_errors, state->debug_header_critical_errors);
#endif

  // Now put the corrected data into the DSD structures

  lcformat[8] = 0;
  mfid[8] = 0;
  lcinfo[56] = 0;
  lsd1[8] = 0;
  lsd2[8] = 0;

  lcformat[0] = hex_data[11][0] + '0';
  lcformat[1] = hex_data[11][1] + '0';
  lcformat[2] = hex_data[11][2] + '0';
  lcformat[3] = hex_data[11][3] + '0';
  lcformat[4] = hex_data[11][4] + '0';
  lcformat[5] = hex_data[11][5] + '0';

  lcformat[6] = hex_data[10][0] + '0';
  lcformat[7] = hex_data[10][1] + '0';
  mfid[0]     = hex_data[10][2] + '0';
  mfid[1]     = hex_data[10][3] + '0';
  mfid[2]     = hex_data[10][4] + '0';
  mfid[3]     = hex_data[10][5] + '0';

  mfid[4]     = hex_data[ 9][0] + '0';
  mfid[5]     = hex_data[ 9][1] + '0';
  mfid[6]     = hex_data[ 9][2] + '0';
  mfid[7]     = hex_data[ 9][3] + '0';
  lcinfo[0]   = hex_data[ 9][4] + '0';
  lcinfo[1]   = hex_data[ 9][5] + '0';

  lcinfo[2]   = hex_data[ 8][0] + '0';
  lcinfo[3]   = hex_data[ 8][1] + '0';
  lcinfo[4]   = hex_data[ 8][2] + '0';
  lcinfo[5]   = hex_data[ 8][3] + '0';
  lcinfo[6]   = hex_data[ 8][4] + '0';
  lcinfo[7]   = hex_data[ 8][5] + '0';

  lcinfo[8]   = hex_data[ 7][0] + '0';
  lcinfo[9]   = hex_data[ 7][1] + '0';
  lcinfo[10]  = hex_data[ 7][2] + '0';
  lcinfo[11]  = hex_data[ 7][3] + '0';
  lcinfo[12]  = hex_data[ 7][4] + '0';
  lcinfo[13]  = hex_data[ 7][5] + '0';

  lcinfo[14]  = hex_data[ 6][0] + '0';
  lcinfo[15]  = hex_data[ 6][1] + '0';
  lcinfo[16]  = hex_data[ 6][2] + '0';
  lcinfo[17]  = hex_data[ 6][3] + '0';
  lcinfo[18]  = hex_data[ 6][4] + '0';
  lcinfo[19]  = hex_data[ 6][5] + '0';

  lcinfo[20]  = hex_data[ 5][0] + '0';
  lcinfo[21]  = hex_data[ 5][1] + '0';
  lcinfo[22]  = hex_data[ 5][2] + '0';
  lcinfo[23]  = hex_data[ 5][3] + '0';
  lcinfo[24]  = hex_data[ 5][4] + '0';
  lcinfo[25]  = hex_data[ 5][5] + '0';

  lcinfo[26]  = hex_data[ 4][0] + '0';
  lcinfo[27]  = hex_data[ 4][1] + '0';
  lcinfo[28]  = hex_data[ 4][2] + '0';
  lcinfo[29]  = hex_data[ 4][3] + '0';
  lcinfo[30]  = hex_data[ 4][4] + '0';
  lcinfo[31]  = hex_data[ 4][5] + '0';

  lcinfo[32]  = hex_data[ 3][0] + '0';
  lcinfo[33]  = hex_data[ 3][1] + '0';
  lcinfo[34]  = hex_data[ 3][2] + '0';
  lcinfo[35]  = hex_data[ 3][3] + '0';
  lcinfo[36]  = hex_data[ 3][4] + '0';
  lcinfo[37]  = hex_data[ 3][5] + '0';

  lcinfo[38]  = hex_data[ 2][0] + '0';
  lcinfo[39]  = hex_data[ 2][1] + '0';
  lcinfo[40]  = hex_data[ 2][2] + '0';
  lcinfo[41]  = hex_data[ 2][3] + '0';
  lcinfo[42]  = hex_data[ 2][4] + '0';
  lcinfo[43]  = hex_data[ 2][5] + '0';

  lcinfo[44]  = hex_data[ 1][0] + '0';
  lcinfo[45]  = hex_data[ 1][1] + '0';
  lcinfo[46]  = hex_data[ 1][2] + '0';
  lcinfo[47]  = hex_data[ 1][3] + '0';
  lcinfo[48]  = hex_data[ 1][4] + '0';
  lcinfo[49]  = hex_data[ 1][5] + '0';

  lcinfo[50]  = hex_data[ 0][0] + '0';
  lcinfo[51]  = hex_data[ 0][1] + '0';
  lcinfo[52]  = hex_data[ 0][2] + '0';
  lcinfo[53]  = hex_data[ 0][3] + '0';
  lcinfo[54]  = hex_data[ 0][4] + '0';
  lcinfo[55]  = hex_data[ 0][5] + '0';

  processP25lcw (opts, state, lcformat, mfid, lcinfo);
}
