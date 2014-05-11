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
#include "p25p1_heuristics.h"

int
read_dibit (dsd_opts* opts, dsd_state* state, char* output, int* status_count, int* analog_signal, int* did_read_status)
{
    int dibit;
    int status;

    if (*status_count == 35) {

#ifdef TRACE_DSD
        char prev_prefix = state->debug_prefix;
        state->debug_prefix = 's';
#endif

        // Status bits now
        status = getDibit (opts, state);
        // TODO: do something useful with the status bits...
        if (did_read_status != NULL) {
            *did_read_status = 1;
        }
        *status_count = 1;

#ifdef TRACE_DSD
        state->debug_prefix = prev_prefix;
#endif

    } else {
        if (did_read_status != NULL) {
            *did_read_status = 0;
        }
        (*status_count)++;
    }

    dibit = get_dibit_and_analog_signal (opts, state, analog_signal);
    output[0] = (1 & (dibit >> 1));      // bit 1
    output[1] = (1 & dibit);             // bit 0

    return dibit;
}

void
read_dibit_update_analog_data (dsd_opts* opts, dsd_state* state, char* output, unsigned int count, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index)
{
  unsigned int i;
  unsigned int debug_left, debug_right;

  for (i=0; i<count; i+=2)
    {
      // We read two bits on each call
      int analog_signal;
      int did_read_status;
      int dibit;

      dibit = read_dibit(opts, state, output + i, status_count, &analog_signal, &did_read_status);

      if (analog_signal_array != NULL)
        {
          // Fill up the AnalogSignal struct
          analog_signal_array[*analog_signal_index].value = analog_signal;
          analog_signal_array[*analog_signal_index].dibit = dibit;
          analog_signal_array[*analog_signal_index].sequence_broken = did_read_status;
          (*analog_signal_index)++;
        }
    }
}

void
read_word (dsd_opts* opts, dsd_state* state, char* word, unsigned int length, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index)
{
  read_dibit_update_analog_data(opts, state, word, length, status_count, analog_signal_array, analog_signal_index);
}

void
read_golay24_parity (dsd_opts* opts, dsd_state* state, char* parity, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index)
{
  read_dibit_update_analog_data(opts, state, parity, 12, status_count, analog_signal_array, analog_signal_index);
}

void
read_hamm_parity (dsd_opts* opts, dsd_state* state, char* parity, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index)
{
  // Read 2 dibits = read 4 bits.
  read_dibit_update_analog_data(opts, state, parity, 4, status_count, analog_signal_array, analog_signal_index);
}

/**
 * Corrects a hex (6 bit) word  using the Golay 24 FEC.
 */
static void
correct_hex_word (dsd_state* state, char* hex, char* parity)
{
  int fixed_errors;
  int irrecoverable_errors;

  irrecoverable_errors = check_and_fix_golay_24_6(hex, parity, &fixed_errors);

  state->debug_header_errors += fixed_errors;
  if (irrecoverable_errors != 0)
    {
      state->debug_header_critical_errors++;
    }
}

/**
 * Reads an hex word, its parity bits and attempts to error correct it using the Golay24 algorithm.
 */
static void
read_and_correct_hex_word (dsd_opts* opts, dsd_state* state, char* hex, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index)
{
  char parity[12];

  // Read the hex word
  read_word (opts, state, hex, 6, status_count, analog_signal_array, analog_signal_index);
  // Read the parity
  read_golay24_parity (opts, state, parity, status_count, analog_signal_array, analog_signal_index);
  // Use the Golay24 FEC to correct it. This call modifies the content of hex to fix it, hopefully
  correct_hex_word (state, hex, parity);
}

/**
 * Uses the information from a corrected sequence of hex words to update the AnalogSignal data.
 * The proper Golay 24 parity is calculated from the corrected hex word so we can also fix the Golay parity
 * that we read originally from the signal.
 * \param corrected_hex_data Pointer to a sequence of hex words that has been error corrected and therefore
 * we trust it's correct. Typically this are hex words that has been decoded successfully using a
 * Reed-Solomon variant.
 * \param hex_count The number of hex words in the sequence.
 * \param analog_signal_array A pointer to the AnalogSignal information for the sequence of hex words.
 */
static void
correct_golay_dibits_6(char* corrected_hex_data, int hex_count, AnalogSignal* analog_signal_array)
{
  int i, j;
  int analog_signal_index;
  int dibit;
  char parity[12];


  analog_signal_index = 0;

  for (i=hex_count-1; i>=0; i--)
    {
      for (j=0; j<6; j+=2)  // 3 iterations -> 3 dibits
        {
          // Given the bits, calculates the dibit
          dibit = (corrected_hex_data[i*6+j] << 1) | corrected_hex_data[i*6+j+1];
          // Now we know the dibit we should have read from the signal
          analog_signal_array[analog_signal_index].corrected_dibit = dibit;

#ifdef HEURISTICS_DEBUG
          if (analog_signal_array[analog_signal_index].dibit != dibit)
            {
              printf("HDU data word corrected from %i to %i, analog value %i\n",
                      analog_signal_array[analog_signal_index].dibit, dibit, analog_signal_array[analog_signal_index].value);
            }
#endif

          analog_signal_index++;
        }

      // Calculate the Golay 24 parity for the corrected hex word
      encode_golay_24_6(corrected_hex_data+i*6, parity);

      // Now we know the parity we should have read from the signal. Use this information
      for (j=0; j<12; j+=2) // 6 iterations -> 6 dibits
        {
          // Given the bits, calculates the dibit
          dibit = (parity[j] << 1) | parity[j+1];
          // Now we know the dibit we should have read from the signal
          analog_signal_array[analog_signal_index].corrected_dibit = dibit;

#ifdef HEURISTICS_DEBUG
          if (analog_signal_array[analog_signal_index].dibit != dibit)
            {
              printf("HDU parity corrected from %i to %i, analog value %i\n",
                      analog_signal_array[analog_signal_index].dibit, dibit, analog_signal_array[analog_signal_index].value);
            }
#endif

          analog_signal_index++;
        }
    }
}

/**
 * The important method that processes a full P25 HD unit.
 */
void
processHDU(dsd_opts* opts, dsd_state* state)
{
  char mi[73], mfid[9], algid[9], kid[17], tgid[17], tmpstr[255];
  int i, j;
  long talkgroup;
  int algidhex, kidhex;
  char hex[6];
  int status_count;
  int status;

  char hex_data[20][6];    // Data in hex-words (6 bit words). A total of 20 hex words.
  char hex_parity[16][6];  // Parity of the data, again in hex-word format. A total of 16 parity hex words.

  int irrecoverable_errors;

  AnalogSignal analog_signal_array[20*(3+6)+16*(3+6)];
  int analog_signal_index;

  analog_signal_index = 0;

  // we skip the status dibits that occur every 36 symbols
  // the next status symbol comes in 14 dibits from here
  // so we start counter at 36-14-1 = 21
  status_count = 21;

  // Read 20 hex words, correct them using their Golay 24 parity data.
  for (i=19; i>=0; i--)
    {
      read_and_correct_hex_word (opts, state, hex, &status_count, analog_signal_array, &analog_signal_index);
      // Store the corrected hex word into the hex_data store:
      for (j=0; j<6; j++)
        {
          hex_data[i][j] = hex[j];
        }
    }

  // Read the 16 parity hex word. These are used to FEC the 20 hex words using Reed-Solomon.
  for (i=15; i>=0; i--)
    {
      read_and_correct_hex_word (opts, state, hex, &status_count, analog_signal_array, &analog_signal_index);
      // Store the corrected hex word into the hex_parity store:
      for (j=0; j<6; j++)
        {
          hex_parity[i][j] = hex[j];
        }
    }
  // Don't forget to mark the first element as the start of a new sequence
  analog_signal_array[0].sequence_broken = 1;

  // Use the Reed-Solomon algorithm to correct the data. hex_data is modified in place
  irrecoverable_errors = check_and_fix_redsolomon_36_20_17((char*)hex_data, (char*)hex_parity);
  if (irrecoverable_errors != 0)
    {
      // The hex words failed the Reed-Solomon check. There were too many errors. Still we can use this
      // information to update an estimate of the BER.
      state->debug_header_critical_errors++;

      // We can correct (17-1)/2 = 8 errors. If we failed, it means that there were more than 8 errors in
      // these 20+16 words. But take into account that each hex word was already error corrected with
      // Golay 24, which can correct 3 bits on each sequence of (6+12) bits. We could say that there were
      // 9 errors of 4 bits.
      update_error_stats(&state->p25_heuristics, 20*6+16*6, 9*4);
    }
  else
    {
      // The hex words passed the Reed-Solomon check. This means that very likely they are correct and we
      // can trust that the digitizer did a good job with them. In other words, each analog value was
      // correctly assigned to a dibit. This is extremely useful information for the digitizer and we are
      // going to exploit it.
      char fixed_parity[16*6];

      // Correct the dibits that we did read according with the newly corrected hex_data values
      correct_golay_dibits_6((char*)hex_data, 20, analog_signal_array);

      // Generate again the Reed-Solomon parity for the corrected data
      encode_reedsolomon_36_20_17((char*)hex_data, fixed_parity);

      // Correct the dibits that we read according with the corrected parity values
      correct_golay_dibits_6(fixed_parity, 16, analog_signal_array+20*(3+6));

      // Now we have a bunch of dibits (composed of data and parity of different kinds) that we trust are all
      // correct. We also keep a record of the analog values from where each dibit is coming from.
      // This information is gold for the heuristics module.
      contribute_to_heuristics(state->rf_mod, &(state->p25_heuristics), analog_signal_array, 20*(3+6)+16*(3+6));
    }

  // Now put the corrected data on the DSD structures

  mi[72] = 0;
  mfid[8] = 0;
  algid[8] = 0;
  kid[16] = 0;
  tgid[16] = 0;

  mi[ 0]   = hex_data[19][0] + '0';
  mi[ 1]   = hex_data[19][1] + '0';
  mi[ 2]   = hex_data[19][2] + '0';
  mi[ 3]   = hex_data[19][3] + '0';
  mi[ 4]   = hex_data[19][4] + '0';
  mi[ 5]   = hex_data[19][5] + '0';

  mi[ 6]   = hex_data[18][0] + '0';
  mi[ 7]   = hex_data[18][1] + '0';
  mi[ 8]   = hex_data[18][2] + '0';
  mi[ 9]   = hex_data[18][3] + '0';
  mi[10]   = hex_data[18][4] + '0';
  mi[11]   = hex_data[18][5] + '0';

  mi[12]   = hex_data[17][0] + '0';
  mi[13]   = hex_data[17][1] + '0';
  mi[14]   = hex_data[17][2] + '0';
  mi[15]   = hex_data[17][3] + '0';
  mi[16]   = hex_data[17][4] + '0';
  mi[17]   = hex_data[17][5] + '0';

  mi[18]   = hex_data[16][0] + '0';
  mi[19]   = hex_data[16][1] + '0';
  mi[20]   = hex_data[16][2] + '0';
  mi[21]   = hex_data[16][3] + '0';
  mi[22]   = hex_data[16][4] + '0';
  mi[23]   = hex_data[16][5] + '0';

  mi[24]   = hex_data[15][0] + '0';
  mi[25]   = hex_data[15][1] + '0';
  mi[26]   = hex_data[15][2] + '0';
  mi[27]   = hex_data[15][3] + '0';
  mi[28]   = hex_data[15][4] + '0';
  mi[29]   = hex_data[15][5] + '0';

  mi[30]   = hex_data[14][0] + '0';
  mi[31]   = hex_data[14][1] + '0';
  mi[32]   = hex_data[14][2] + '0';
  mi[33]   = hex_data[14][3] + '0';
  mi[34]   = hex_data[14][4] + '0';
  mi[35]   = hex_data[14][5] + '0';

  mi[36]   = hex_data[13][0] + '0';
  mi[37]   = hex_data[13][1] + '0';
  mi[38]   = hex_data[13][2] + '0';
  mi[39]   = hex_data[13][3] + '0';
  mi[40]   = hex_data[13][4] + '0';
  mi[41]   = hex_data[13][5] + '0';

  mi[42]   = hex_data[12][0] + '0';
  mi[43]   = hex_data[12][1] + '0';
  mi[44]   = hex_data[12][2] + '0';
  mi[45]   = hex_data[12][3] + '0';
  mi[46]   = hex_data[12][4] + '0';
  mi[47]   = hex_data[12][5] + '0';

  mi[48]   = hex_data[11][0] + '0';
  mi[49]   = hex_data[11][1] + '0';
  mi[50]   = hex_data[11][2] + '0';
  mi[51]   = hex_data[11][3] + '0';
  mi[52]   = hex_data[11][4] + '0';
  mi[53]   = hex_data[11][5] + '0';

  mi[54]   = hex_data[10][0] + '0';
  mi[55]   = hex_data[10][1] + '0';
  mi[56]   = hex_data[10][2] + '0';
  mi[57]   = hex_data[10][3] + '0';
  mi[58]   = hex_data[10][4] + '0';
  mi[59]   = hex_data[10][5] + '0';

  mi[60]   = hex_data[ 9][0] + '0';
  mi[61]   = hex_data[ 9][1] + '0';
  mi[62]   = hex_data[ 9][2] + '0';
  mi[63]   = hex_data[ 9][3] + '0';
  mi[64]   = hex_data[ 9][4] + '0';
  mi[65]   = hex_data[ 9][5] + '0';

  mi[66]   = hex_data[ 8][0] + '0';
  mi[67]   = hex_data[ 8][1] + '0';
  mi[68]   = hex_data[ 8][2] + '0';
  mi[69]   = hex_data[ 8][3] + '0';
  mi[70]   = hex_data[ 8][4] + '0';
  mi[71]   = hex_data[ 8][5] + '0';

  mfid[0]  = hex_data[ 7][0] + '0';
  mfid[1]  = hex_data[ 7][1] + '0';
  mfid[2]  = hex_data[ 7][2] + '0';
  mfid[3]  = hex_data[ 7][3] + '0';
  mfid[4]  = hex_data[ 7][4] + '0';
  mfid[5]  = hex_data[ 7][5] + '0';

  mfid[6]  = hex_data[ 6][0] + '0';
  mfid[7]  = hex_data[ 6][1] + '0';
  algid[0] = hex_data[ 6][2] + '0';  // The important algorithm ID. This indicates whether the data is
  algid[1] = hex_data[ 6][3] + '0';  // encrypted and if so what is the encryption algorithm used.
  algid[2] = hex_data[ 6][4] + '0';  // A code 0x80 here means that the data is unencrypted.
  algid[3] = hex_data[ 6][5] + '0';

  algid[4] = hex_data[ 5][0] + '0';
  algid[5] = hex_data[ 5][1] + '0';
  algid[6] = hex_data[ 5][2] + '0';
  algid[7] = hex_data[ 5][3] + '0';
  kid[ 0]  = hex_data[ 5][4] + '0';
  kid[ 1]  = hex_data[ 5][5] + '0';

  kid[ 2]  = hex_data[ 4][0] + '0';  // The encryption key ID
  kid[ 3]  = hex_data[ 4][1] + '0';
  kid[ 4]  = hex_data[ 4][2] + '0';
  kid[ 5]  = hex_data[ 4][3] + '0';
  kid[ 6]  = hex_data[ 4][4] + '0';
  kid[ 7]  = hex_data[ 4][5] + '0';

  kid[ 8]  = hex_data[ 3][0] + '0';
  kid[ 9]  = hex_data[ 3][1] + '0';
  kid[10]  = hex_data[ 3][2] + '0';
  kid[11]  = hex_data[ 3][3] + '0';
  kid[12]  = hex_data[ 3][4] + '0';
  kid[13]  = hex_data[ 3][5] + '0';

  kid[14]  = hex_data[ 2][0] + '0';
  kid[15]  = hex_data[ 2][1] + '0';
  tgid[ 0] = hex_data[ 2][2] + '0';  // Talk group ID
  tgid[ 1] = hex_data[ 2][3] + '0';
  tgid[ 2] = hex_data[ 2][4] + '0';
  tgid[ 3] = hex_data[ 2][5] + '0';

  tgid[ 4] = hex_data[ 1][0] + '0';
  tgid[ 5] = hex_data[ 1][1] + '0';
  tgid[ 6] = hex_data[ 1][2] + '0';
  tgid[ 7] = hex_data[ 1][3] + '0';
  tgid[ 8] = hex_data[ 1][4] + '0';
  tgid[ 9] = hex_data[ 1][5] + '0';

  tgid[10] = hex_data[ 0][0] + '0';
  tgid[11] = hex_data[ 0][1] + '0';
  tgid[12] = hex_data[ 0][2] + '0';
  tgid[13] = hex_data[ 0][3] + '0';
  tgid[14] = hex_data[ 0][4] + '0';
  tgid[15] = hex_data[ 0][5] + '0';

  state->p25kid = strtol(kid, NULL, 2);

  skipDibit (opts, state, 5);
  status = getDibit (opts, state);
  //TODO: Do something useful with the status bits...

  if (opts->p25enc == 1)
    {
      algidhex = strtol (algid, NULL, 2);
      kidhex = strtol (kid, NULL, 2);
      printf ("mi: %s algid: $%x kid: $%x\n", mi, algidhex, kidhex);
    }
  if (opts->p25lc == 1)
    {
      printf ("mfid: %s tgid: %s ", mfid, tgid);
      if (opts->p25tg == 0)
        {
          printf ("\n");
        }
    }

  j = 0;
  if (strcmp (mfid, "10010000") == 0)
    {
      for (i = 4; i < 16; i++)
        {
          if (state->tgcount < 24)
            {
              state->tg[state->tgcount][j] = tgid[i];
            }
          tmpstr[j] = tgid[i];
          j++;
        }
      tmpstr[12] = '0';
      tmpstr[13] = '0';
      tmpstr[14] = '0';
      tmpstr[15] = '0';
    }
  else
    {
      for (i = 0; i < 16; i++)
        {
          if (state->tgcount < 24)
            {
              state->tg[state->tgcount][j] = tgid[i];
            }
          tmpstr[j] = tgid[i];
          j++;
        }
    }
  tmpstr[16] = 0;
  talkgroup = strtol (tmpstr, NULL, 2);
  state->lasttg = talkgroup;
  if (state->tgcount < 24)
    {
      state->tgcount = state->tgcount + 1;
    }
  if (opts->p25tg == 1)
    {
      printf ("tg: %li\n", talkgroup);
    }
}
