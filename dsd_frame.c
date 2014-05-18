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
#if !defined(NULL)
#define NULL 0
#endif

#include "p25p1_check_nid.h"

void
printFrameInfo (dsd_opts * opts, dsd_state * state)
{

  int level;

  level = (int) state->max / 164;
  if (opts->verbose > 0)
    {
      printf ("inlvl: %2i%% ", level);
    }
  if (state->nac != 0)
    {
      printf ("nac: %4X ", state->nac);
    }

  if (opts->verbose > 1)
    {
      printf ("src: %8i ", state->lastsrc);
    }
  printf ("tg: %5i ", state->lasttg);
}

void
processFrame (dsd_opts * opts, dsd_state * state)
{

  int i, j, dibit;
  char duid[3];
  char nac[13];
  int level;

  char status_0;
  char bch_code[63];
  int index_bch_code;
  unsigned char parity;
  char v;
  int new_nac;
  char new_duid[3];
  int check_result;

  nac[12] = 0;
  duid[2] = 0;
  j = 0;

  if (state->rf_mod == 1)
    {
      state->maxref = (int)(state->max * 0.80F);
      state->minref = (int)(state->min * 0.80F);
    }
  else
    {
      state->maxref = state->max;
      state->minref = state->min;
    }

  if ((state->synctype == 8) || (state->synctype == 9))
    {
      state->rf_mod = 2;
      state->nac = 0;
      state->lastsrc = 0;
      state->lasttg = 0;
      if (opts->errorbars == 1)
        {
          if (opts->verbose > 0)
            {
              level = (int) state->max / 164;
              printf ("inlvl: %2i%% ", level);
            }
        }
      state->nac = 0;
      if ((opts->mbe_out_dir[0] != 0) && (opts->mbe_out_f == NULL))
        {
          openMbeOutFile (opts, state);
        }
      sprintf (state->fsubtype, " VOICE        ");
      processNXDNVoice (opts, state);
      return;
    }
  else if ((state->synctype == 16) || (state->synctype == 17))
    {
      state->rf_mod = 2;
      state->nac = 0;
      state->lastsrc = 0;
      state->lasttg = 0;
      if (opts->errorbars == 1)
        {
          if (opts->verbose > 0)
            {
              level = (int) state->max / 164;
              printf ("inlvl: %2i%% ", level);
            }
        }
      state->nac = 0;
      if ((opts->mbe_out_dir[0] != 0) && (opts->mbe_out_f == NULL))
        {
          openMbeOutFile (opts, state);
        }
      sprintf (state->fsubtype, " DATA         ");
      processNXDNData (opts, state);
      return;
    }
  else if ((state->synctype == 6) || (state->synctype == 7))
    {
      state->nac = 0;
      state->lastsrc = 0;
      state->lasttg = 0;
      if (opts->errorbars == 1)
        {
          if (opts->verbose > 0)
            {
              level = (int) state->max / 164;
              printf ("inlvl: %2i%% ", level);
            }
        }
      state->nac = 0;
      if ((opts->mbe_out_dir[0] != 0) && (opts->mbe_out_f == NULL))
        {
          openMbeOutFile (opts, state);
        }
      sprintf (state->fsubtype, " VOICE        ");
      processDSTAR (opts, state);
      return;
    }
  else if ((state->synctype == 18) || (state->synctype == 19))
    {
      state->nac = 0;
      state->lastsrc = 0;
      state->lasttg = 0;
      if (opts->errorbars == 1)
        {
          if (opts->verbose > 0)
            {
              level = (int) state->max / 164;
              printf ("inlvl: %2i%% ", level);
            }
        }
      state->nac = 0;
      if ((opts->mbe_out_dir[0] != 0) && (opts->mbe_out_f == NULL))
        {
          openMbeOutFile (opts, state);
        }
      sprintf (state->fsubtype, " DATA         ");
      processDSTAR_HD (opts, state);
      return;
    }

  else if ((state->synctype >= 10) && (state->synctype <= 13))
    {
      state->nac = 0;
      state->lastsrc = 0;
      state->lasttg = 0;
      if (opts->errorbars == 1)
        {
          if (opts->verbose > 0)
            {
              level = (int) state->max / 164;
              printf ("inlvl: %2i%% ", level);
            }
        }
      if ((state->synctype == 11) || (state->synctype == 12))
        {
          if ((opts->mbe_out_dir[0] != 0) && (opts->mbe_out_f == NULL))
            {
              openMbeOutFile (opts, state);
            }
          sprintf (state->fsubtype, " VOICE        ");
          processDMRvoice (opts, state);
        }
      else
        {
          closeMbeOutFile (opts, state);
          state->err_str[0] = 0;
          processDMRdata (opts, state);
        }
      return;
    }
  else if ((state->synctype >= 2) && (state->synctype <= 5))
    {
      state->nac = 0;
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
        }
      if ((state->synctype == 3) || (state->synctype == 4))
        {
          if ((opts->mbe_out_dir[0] != 0) && (opts->mbe_out_f == NULL))
            {
              openMbeOutFile (opts, state);
            }
          sprintf (state->fsubtype, " VOICE        ");
          processX2TDMAvoice (opts, state);
        }
      else
        {
          closeMbeOutFile (opts, state);
          state->err_str[0] = 0;
          processX2TDMAdata (opts, state);
        }
      return;
    }
  else if ((state->synctype == 14) || (state->synctype == 15))
    {
      state->nac = 0;
      state->lastsrc = 0;
      state->lasttg = 0;
      if (opts->errorbars == 1)
        {
          if (opts->verbose > 0)
            {
              level = (int) state->max / 164;
              printf ("inlvl: %2i%% ", level);
            }
        }
      if ((opts->mbe_out_dir[0] != 0) && (opts->mbe_out_f == NULL))
        {
          openMbeOutFile (opts, state);
        }
      sprintf (state->fsubtype, " VOICE        ");
      processProVoice (opts, state);
      return;
    }
  else
    {
      // Read the NAC, 12 bits
      j = 0;
      index_bch_code = 0;
      for (i = 0; i < 6; i++)
        {
          dibit = getDibit (opts, state);

          v = 1 & (dibit >> 1); // bit 1
          nac[j] = v + '0';
          j++;
          bch_code[index_bch_code] = v;
          index_bch_code++;

          v = 1 & dibit;        // bit 0
          nac[j] = v + '0';
          j++;
          bch_code[index_bch_code] = v;
          index_bch_code++;
        }
      state->nac = strtol (nac, NULL, 2);

      // Read the DUID, 4 bits
      for (i = 0; i < 2; i++)
        {
          dibit = getDibit (opts, state);
          duid[i] = dibit + '0';

          bch_code[index_bch_code] = 1 & (dibit >> 1);  // bit 1
          index_bch_code++;
          bch_code[index_bch_code] = 1 & dibit;         // bit 0
          index_bch_code++;
        }

      // Read the BCH data for error correction of NAC and DUID
      for (i = 0; i < 3; i++)
        {
          dibit = getDibit (opts, state);

          bch_code[index_bch_code] = 1 & (dibit >> 1);  // bit 1
          index_bch_code++;
          bch_code[index_bch_code] = 1 & dibit;         // bit 0
          index_bch_code++;
        }
      // Intermission: read the status dibit
      status_0 = getDibit (opts, state) + '0';
      // ... continue reading the BCH error correction data
      for (i = 0; i < 20; i++)
        {
          dibit = getDibit (opts, state);

          bch_code[index_bch_code] = 1 & (dibit >> 1);  // bit 1
          index_bch_code++;
          bch_code[index_bch_code] = 1 & dibit;         // bit 0
          index_bch_code++;
        }

      // Read the parity bit
      dibit = getDibit (opts, state);
      bch_code[index_bch_code] = 1 & (dibit >> 1);      // bit 1
      parity = (1 & dibit);     // bit 0

      // Check if the NID is correct
      check_result = check_NID (bch_code, &new_nac, new_duid, parity);
      if (check_result) {
          if (new_nac != state->nac) {
              // NAC fixed by error correction
              state->nac = new_nac;
              state->debug_header_errors++;
          }
          if (strcmp(new_duid, duid) != 0) {
              // DUID fixed by error correction
              //printf("Fixing DUID %s -> %s\n", duid, new_duid);
              duid[0] = new_duid[0];
              duid[1] = new_duid[1];
              state->debug_header_errors++;
          }
      } else {
          // Check of NID failed and unable to recover its value
          //printf("NID error\n");
          duid[0] = 'E';
          duid[1] = 'E';
          state->debug_header_critical_errors++;
      }
    }

  if (strcmp (duid, "00") == 0)
    {
      // Header Data Unit
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" HDU\n");
        }
      if (opts->mbe_out_dir[0] != 0)
        {
          closeMbeOutFile (opts, state);
          openMbeOutFile (opts, state);
        }
      mbe_initMbeParms (state->cur_mp, state->prev_mp, state->prev_mp_enhanced);
      state->lastp25type = 2;
      sprintf (state->fsubtype, " HDU          ");
      processHDU (opts, state);
    }
  else if (strcmp (duid, "11") == 0)
    {
      // Logical Link Data Unit 1
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" LDU1  ");
        }
      if (opts->mbe_out_dir[0] != 0)
        {
          if (opts->mbe_out_f == NULL)
            {
              openMbeOutFile (opts, state);
            }
        }
      state->lastp25type = 1;
      sprintf (state->fsubtype, " LDU1         ");
      state->numtdulc = 0;
      processLDU1 (opts, state);
    }
  else if (strcmp (duid, "22") == 0)
    {
      // Logical Link Data Unit 2
      if (state->lastp25type != 1)
        {
          if (opts->errorbars == 1)
            {
              printFrameInfo (opts, state);
              printf (" Ignoring LDU2 not preceeded by LDU1\n");
            }
          state->lastp25type = 0;
          sprintf (state->fsubtype, "              ");
        }
      else
        {
          if (opts->errorbars == 1)
            {
              printFrameInfo (opts, state);
              printf (" LDU2  ");
            }
          if (opts->mbe_out_dir[0] != 0)
            {
              if (opts->mbe_out_f == NULL)
                {
                  openMbeOutFile (opts, state);
                }
            }
          state->lastp25type = 2;
          sprintf (state->fsubtype, " LDU2         ");
          state->numtdulc = 0;
          processLDU2 (opts, state);
        }
    }
  else if (strcmp (duid, "33") == 0)
    {
      // Terminator with subsequent Link Control
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" TDULC\n");
        }
      if (opts->mbe_out_dir[0] != 0)
        {
          closeMbeOutFile (opts, state);
        }
      mbe_initMbeParms (state->cur_mp, state->prev_mp, state->prev_mp_enhanced);
      state->lasttg = 0;
      state->lastsrc = 0;
      state->lastp25type = 0;
      state->err_str[0] = 0;
      sprintf (state->fsubtype, " TDULC        ");
      state->numtdulc++;
      if ((opts->resume > 0) && (state->numtdulc > opts->resume))
        {
          resumeScan (opts, state);
        }
      processTDULC (opts, state);
      state->err_str[0] = 0;
    }
  else if (strcmp (duid, "03") == 0)
    {
      // Terminator without subsequent Link Control
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" TDU\n");
        }
      if (opts->mbe_out_dir[0] != 0)
        {
          closeMbeOutFile (opts, state);
        }
      mbe_initMbeParms (state->cur_mp, state->prev_mp, state->prev_mp_enhanced);
      state->lasttg = 0;
      state->lastsrc = 0;
      state->lastp25type = 0;
      state->err_str[0] = 0;
      sprintf (state->fsubtype, " TDU          ");

      processTDU (opts, state);
    }
  else if (strcmp (duid, "13") == 0)
    {
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" TSDU\n");
        }
      if (opts->resume > 0)
        {
          resumeScan (opts, state);
        }
      state->lasttg = 0;
      state->lastsrc = 0;
      state->lastp25type = 3;
      sprintf (state->fsubtype, " TSDU         ");

      // Now processing NID

      skipDibit (opts, state, 328-25);
    }
  else if (strcmp (duid, "30") == 0)
    {
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" PDU\n");
        }
      if (opts->resume > 0)
        {
          resumeScan (opts, state);
        }
      if (opts->mbe_out_dir[0] != 0)
        {
          if (opts->mbe_out_f == NULL)
            {
              openMbeOutFile (opts, state);
            }
        }
      state->lastp25type = 4;
      sprintf (state->fsubtype, " PDU          ");
    }
  // try to guess based on previous frame if unknown type
  else if (state->lastp25type == 1)
    {
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf ("(LDU2) ");
        }
      if (opts->mbe_out_dir[0] != 0)
        {
          if (opts->mbe_out_f == NULL)
            {
              openMbeOutFile (opts, state);
            }
        }
      //state->lastp25type = 0;
      // Guess that the state is LDU2
      state->lastp25type = 2;
      sprintf (state->fsubtype, "(LDU2)        ");
      state->numtdulc = 0;
      processLDU2 (opts, state);
    }
  else if (state->lastp25type == 2)
    {
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf ("(LDU1) ");
        }
      if (opts->mbe_out_dir[0] != 0)
        {
          if (opts->mbe_out_f == NULL)
            {
              openMbeOutFile (opts, state);
            }
        }
      //state->lastp25type = 0;
      // Guess that the state is LDU1
      state->lastp25type = 1;
      sprintf (state->fsubtype, "(LDU1)        ");
      state->numtdulc = 0;
      processLDU1 (opts, state);
    }
  else if (state->lastp25type == 3)
    {
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" (TSDU)\n");
        }
      //state->lastp25type = 0;
      // Guess that the state is TSDU
      state->lastp25type = 3;
      sprintf (state->fsubtype, "(TSDU)        ");

      // Now processing NID

      skipDibit (opts, state, 328-25);
    }
  else if (state->lastp25type == 4)
    {
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" (PDU)\n");
        }
      state->lastp25type = 0;
    }
  else
    {
      state->lastp25type = 0;
      sprintf (state->fsubtype, "              ");
      if (opts->errorbars == 1)
        {
          printFrameInfo (opts, state);
          printf (" duid:%s *Unknown DUID*\n", duid);
        }
    }
}
