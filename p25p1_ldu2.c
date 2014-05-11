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
processLDU2 (dsd_opts * opts, dsd_state * state)
{
  // extracts IMBE frames from LDU frame
  int i;
  char mi[73], algid[9], kid[17];
  char lsd1[9], lsd2[9];
  int algidhex, kidhex;

  int status_count;

  char hex_data[16][6];    // Data in hex-words (6 bit words). A total of 16 hex words.
  char hex_parity[8][6];   // Parity of the data, again in hex-word format. A total of 12 parity hex words.

  int irrecoverable_errors;

  AnalogSignal analog_signal_array[16*(3+2)+8*(3+2)];
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
  read_and_correct_hex_word (opts, state, &(hex_data[15][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[14][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[13][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[12][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[0*5].sequence_broken = 1;

  // IMBE 3
#ifdef TRACE_DSD
  state->debug_prefix_2 = '2';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 3
  read_and_correct_hex_word (opts, state, &(hex_data[11][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[10][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 9][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 8][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[4*5].sequence_broken = 1;

  // IMBE 4
#ifdef TRACE_DSD
  state->debug_prefix_2 = '3';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 4
  read_and_correct_hex_word (opts, state, &(hex_data[ 7][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 6][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 5][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 4][0]), &status_count, analog_signal_array, &analog_signal_index);
  analog_signal_array[8*5].sequence_broken = 1;

  // IMBE 5
#ifdef TRACE_DSD
  state->debug_prefix_2 = '4';
#endif
  process_IMBE (opts, state, &status_count);

  // Read data after IMBE 5
  read_and_correct_hex_word (opts, state, &(hex_data[ 3][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 2][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 1][0]), &status_count, analog_signal_array, &analog_signal_index);
  read_and_correct_hex_word (opts, state, &(hex_data[ 0][0]), &status_count, analog_signal_array, &analog_signal_index);
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
  irrecoverable_errors = check_and_fix_reedsolomon_24_16_9((char*)hex_data, (char*)hex_parity);
  if (irrecoverable_errors == 1)
    {
      state->debug_header_critical_errors++;

      // We can correct (9-1)/2 = 4 errors. If we failed, it means that there were more than 4 errors in
      // these 12+12 words. But take into account that each hex word was already error corrected with
      // Hamming(10,6,3), which can correct 1 bits on each sequence of (6+4) bits. We could say that there
      // were 5 errors of 2 bits.
      update_error_stats(&state->p25_heuristics, 12*6+12*6, 5*2);
    }
  else
    {
      // Same comments as in processHDU. See there.

      char fixed_parity[8*6];

      // Correct the dibits that we read according with hex_data values
      correct_hamming_dibits((char*)hex_data, 16, analog_signal_array);

      // Generate again the Reed-Solomon parity
      encode_reedsolomon_24_16_9((char*)hex_data, fixed_parity);

      // Correct the dibits that we read according with the fixed parity values
      correct_hamming_dibits(fixed_parity, 8, analog_signal_array+16*(3+2));

      // Once corrected, contribute this information to the heuristics module
      contribute_to_heuristics(state->rf_mod, &(state->p25_heuristics), analog_signal_array, 16*(3+2)+8*(3+2));
    }


#ifdef HEURISTICS_DEBUG
  printf("(audio errors, header errors, critical header errors) (%i,%i,%i)\n",
          state->debug_audio_errors, state->debug_header_errors, state->debug_header_critical_errors);
#endif

  // Now put the corrected data into the DSD structures

  mi[72] = 0;
  algid[8] = 0;
  kid[16] = 0;
  lsd1[8] = 0;
  lsd2[8] = 0;

  mi[ 0]   = hex_data[15][0] + '0';
  mi[ 1]   = hex_data[15][1] + '0';
  mi[ 2]   = hex_data[15][2] + '0';
  mi[ 3]   = hex_data[15][3] + '0';
  mi[ 4]   = hex_data[15][4] + '0';
  mi[ 5]   = hex_data[15][5] + '0';

  mi[ 6]   = hex_data[14][0] + '0';
  mi[ 7]   = hex_data[14][1] + '0';
  mi[ 8]   = hex_data[14][2] + '0';
  mi[ 9]   = hex_data[14][3] + '0';
  mi[10]   = hex_data[14][4] + '0';
  mi[11]   = hex_data[14][5] + '0';

  mi[12]   = hex_data[13][0] + '0';
  mi[13]   = hex_data[13][1] + '0';
  mi[14]   = hex_data[13][2] + '0';
  mi[15]   = hex_data[13][3] + '0';
  mi[16]   = hex_data[13][4] + '0';
  mi[17]   = hex_data[13][5] + '0';

  mi[18]   = hex_data[12][0] + '0';
  mi[19]   = hex_data[12][1] + '0';
  mi[20]   = hex_data[12][2] + '0';
  mi[21]   = hex_data[12][3] + '0';
  mi[22]   = hex_data[12][4] + '0';
  mi[23]   = hex_data[12][5] + '0';

  mi[24]   = hex_data[11][0] + '0';
  mi[25]   = hex_data[11][1] + '0';
  mi[26]   = hex_data[11][2] + '0';
  mi[27]   = hex_data[11][3] + '0';
  mi[28]   = hex_data[11][4] + '0';
  mi[29]   = hex_data[11][5] + '0';

  mi[30]   = hex_data[10][0] + '0';
  mi[31]   = hex_data[10][1] + '0';
  mi[32]   = hex_data[10][2] + '0';
  mi[33]   = hex_data[10][3] + '0';
  mi[34]   = hex_data[10][4] + '0';
  mi[35]   = hex_data[10][5] + '0';

  mi[36]   = hex_data[ 9][0] + '0';
  mi[37]   = hex_data[ 9][1] + '0';
  mi[38]   = hex_data[ 9][2] + '0';
  mi[39]   = hex_data[ 9][3] + '0';
  mi[40]   = hex_data[ 9][4] + '0';
  mi[41]   = hex_data[ 9][5] + '0';

  mi[42]   = hex_data[ 8][0] + '0';
  mi[43]   = hex_data[ 8][1] + '0';
  mi[44]   = hex_data[ 8][2] + '0';
  mi[45]   = hex_data[ 8][3] + '0';
  mi[46]   = hex_data[ 8][4] + '0';
  mi[47]   = hex_data[ 8][5] + '0';

  mi[48]   = hex_data[ 7][0] + '0';
  mi[49]   = hex_data[ 7][1] + '0';
  mi[50]   = hex_data[ 7][2] + '0';
  mi[51]   = hex_data[ 7][3] + '0';
  mi[52]   = hex_data[ 7][4] + '0';
  mi[53]   = hex_data[ 7][5] + '0';

  mi[54]   = hex_data[ 6][0] + '0';
  mi[55]   = hex_data[ 6][1] + '0';
  mi[56]   = hex_data[ 6][2] + '0';
  mi[57]   = hex_data[ 6][3] + '0';
  mi[58]   = hex_data[ 6][4] + '0';
  mi[59]   = hex_data[ 6][5] + '0';

  mi[60]   = hex_data[ 5][0] + '0';
  mi[61]   = hex_data[ 5][1] + '0';
  mi[62]   = hex_data[ 5][2] + '0';
  mi[63]   = hex_data[ 5][3] + '0';
  mi[64]   = hex_data[ 5][4] + '0';
  mi[65]   = hex_data[ 5][5] + '0';

  mi[66]   = hex_data[ 4][0] + '0';
  mi[67]   = hex_data[ 4][1] + '0';
  mi[68]   = hex_data[ 4][2] + '0';
  mi[69]   = hex_data[ 4][3] + '0';
  mi[70]   = hex_data[ 4][4] + '0';
  mi[71]   = hex_data[ 4][5] + '0';

  algid[0] = hex_data[ 3][0] + '0';
  algid[1] = hex_data[ 3][1] + '0';
  algid[2] = hex_data[ 3][2] + '0';
  algid[3] = hex_data[ 3][3] + '0';
  algid[4] = hex_data[ 3][4] + '0';
  algid[5] = hex_data[ 3][5] + '0';

  algid[6] = hex_data[ 2][0] + '0';
  algid[7] = hex_data[ 2][1] + '0';
  kid[0]   = hex_data[ 2][2] + '0';
  kid[1]   = hex_data[ 2][3] + '0';
  kid[2]   = hex_data[ 2][4] + '0';
  kid[3]   = hex_data[ 2][5] + '0';

  kid[4]   = hex_data[ 1][0] + '0';
  kid[5]   = hex_data[ 1][1] + '0';
  kid[6]   = hex_data[ 1][2] + '0';
  kid[7]   = hex_data[ 1][3] + '0';
  kid[8]   = hex_data[ 1][4] + '0';
  kid[9]   = hex_data[ 1][5] + '0';

  kid[10]  = hex_data[ 0][0] + '0';
  kid[11]  = hex_data[ 0][1] + '0';
  kid[12]  = hex_data[ 0][2] + '0';
  kid[13]  = hex_data[ 0][3] + '0';
  kid[14]  = hex_data[ 0][4] + '0';
  kid[15]  = hex_data[ 0][5] + '0';


  if (opts->p25enc == 1)
    {
      algidhex = strtol (algid, NULL, 2);
      kidhex = strtol (kid, NULL, 2);
      printf ("mi: %s algid: $%x kid: $%x\n", mi, algidhex, kidhex);
    }
}
