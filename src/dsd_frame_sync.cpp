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
#include "options.h"

void
printFrameSync (dsd_opts * opts, dsd_state * state, const char *frametype, int offset, char *modulation)
{
  int verbose = Options::Instance().GetVerboseLevel();
  if (verbose > 0)
    {
      printf ("Sync: %s ", frametype);
    }
  if (verbose > 2)
    {
      printf ("o: %4i ", offset);
    }
  if (verbose > 1)
    {
      printf ("mod: %s ", modulation);
    }
  if (verbose > 2)
    {
      printf ("g: %f ", state->aout_gain);
    }
}

int
getFrameSync (dsd_opts * opts, dsd_state * state)
{
  /* detects frame sync and returns frame type
   *  0 = +P25p1
   *  1 = -P25p1
   *  2 = +X2-TDMA (non inverted signal data frame)
   *  3 = -X2-TDMA (inverted signal voice frame)
   *  4 = +X2-TDMA (non inverted signal voice frame)
   *  5 = -X2-TDMA (inverted signal data frame)
   *  6 = +D-STAR
   *  7 = -D-STAR
   *  8 = +NXDN (non inverted voice frame)
   *  9 = -NXDN (inverted voice frame)
   * 10 = +DMR (non inverted signal data frame)
   * 11 = -DMR (inverted signal voice frame)
   * 12 = +DMR (non inverted signal voice frame)
   * 13 = -DMR (inverted signal data frame)
   * 14 = +ProVoice
   * 15 = -ProVoice
   * 16 = +NXDN (non inverted data frame)
   * 17 = -NXDN (inverted data frame)
   * 18 = +D-STAR_HD
   * 19 = -D-STAR_HD
   */


  int i, j, t, o, dibit, sync, symbol, synctest_pos, lastt;
  char synctest[25];
  char synctest18[19];
  char synctest32[33];
  char modulation[8];
  char *synctest_p;
  char synctest_buf[10240];
  int lmin, lmax, lidx;
  int lbuf[24], lbuf2[24];
  int lsum;
  char spectrum[64];

  bool errorbars = Options::Instance().GetErrorBars();

  for (i = 18; i < 24; i++)
    {
      lbuf[i] = 0;
      lbuf2[i] = 0;
    }

  // detect frame sync
  t = 0;
  synctest[24] = 0;
  synctest18[18] = 0;
  synctest32[32] = 0;
  synctest_pos = 0;
  synctest_p = synctest_buf + 10;
  sync = 0;
  lmin = 0;
  lmax = 0;
  lidx = 0;
  lastt = 0;
  state->numflips = 0;
  if ((Options::Instance().GetSymbolTiming()) && (state->carrier == 1))
    {
      printf ("\nSymbol Timing:\n");
    }
  while (sync == 0)
    {
      t++;
      symbol = getSymbol (opts, state, 0);

      lbuf[lidx] = symbol;
      state->sbuf[state->sidx] = symbol;
      if (lidx == 23)
        {
          lidx = 0;
        }
      else
        {
          lidx++;
        }
      if (state->sidx == (opts->ssize - 1))
        {
          state->sidx = 0;
        }
      else
        {
          state->sidx++;
        }

      if (lastt == 23)
        {
          lastt = 0;
          if (state->numflips > opts->mod_threshold)
            {
              if (Options::Instance().GetModQpsk())
                {
                  state->rf_mod = 1;
                }
            }
          else if (state->numflips > 18)
            {
              if (Options::Instance().GetModGfsk())
                {
                  state->rf_mod = 2;
                }
            }
          else
            {
              if (Options::Instance().GetModC4fm())
                {
                  state->rf_mod = 0;
                }
            }
          state->numflips = 0;
        }
      else
        {
          lastt++;
        }

      if (state->dibit_buf_p > state->dibit_buf + 900000)
        {
    	  state->dibit_buf_p = state->dibit_buf + 200;
        }

      //determine dibit state
      if (symbol > 0)
        {
          *state->dibit_buf_p = 1;
          state->dibit_buf_p++;
          dibit = 49;               // '1'
        }
      else
        {
          *state->dibit_buf_p = 3;
          state->dibit_buf_p++;
          dibit = 51;               // '3'
        }

      *synctest_p = dibit;
      if (t >= 18)
        {
          for (i = 0; i < 24; i++)
            {
              lbuf2[i] = lbuf[i];
            }
          qsort (lbuf2, 24, sizeof (int), comp);
          lmin = (lbuf2[2] + lbuf2[3] + lbuf2[4]) / 3;
          lmax = (lbuf2[21] + lbuf2[20] + lbuf2[19]) / 3;

          if (state->rf_mod == 1)
            {
              state->minbuf[state->midx] = lmin;
              state->maxbuf[state->midx] = lmax;
              if (state->midx == (opts->msize - 1))
                {
                  state->midx = 0;
                }
              else
                {
                  state->midx++;
                }
              lsum = 0;
              for (i = 0; i < opts->msize; i++)
                {
                  lsum += state->minbuf[i];
                }
              state->min = lsum / opts->msize;
              lsum = 0;
              for (i = 0; i < opts->msize; i++)
                {
                  lsum += state->maxbuf[i];
                }
              state->max = lsum / opts->msize;
              state->center = ((state->max) + (state->min)) / 2;
              state->maxref = (int)((state->max) * 0.80F);
              state->minref = (int)((state->min) * 0.80F);
            }
          else
            {
              state->maxref = state->max;
              state->minref = state->min;
            }

          if (state->rf_mod == 0)
            {
              sprintf (modulation, "C4FM");
            }
          else if (state->rf_mod == 1)
            {
              sprintf (modulation, "QPSK");
            }
          else if (state->rf_mod == 2)
            {
              sprintf (modulation, "GFSK");
            }

          if (Options::Instance().GetDataScope())
            {
              if (lidx == 0)
                {
                  for (i = 0; i < 64; i++)
                    {
                      spectrum[i] = 0;
                    }
                  for (i = 0; i < 24; i++)
                    {
                      o = (lbuf2[i] + 32768) / 1024;
                      spectrum[o]++;
                    }
                  if (state->symbolcnt > (4800 / opts->scoperate))
                    {
                      state->symbolcnt = 0;
                      printf ("\n");
                      printf ("Demod mode:     %s                Nac:                     %4X\n", modulation, state->nac);
                      printf ("Frame Type:    %s        Talkgroup:            %7i\n", state->ftype, state->lasttg);
                      printf ("Frame Subtype: %s       Source:          %12i\n", state->fsubtype, state->lastsrc);
                      printf ("TDMA activity:  %s %s     Voice errors: %s\n", state->slot0light, state->slot1light, state->err_str);
                      printf ("+----------------------------------------------------------------+\n");
                      for (i = 0; i < 10; i++)
                        {
                          printf ("|");
                          for (j = 0; j < 64; j++)
                            {
                              if (i == 0)
                                {
                                  if ((j == ((state->min) + 32768) / 1024) || (j == ((state->max) + 32768) / 1024))
                                    {
                                      printf ("#");
                                    }
                                  else if (j == (state->center + 32768) / 1024)
                                    {
                                      printf ("!");
                                    }
                                  else
                                    {
                                      if (j == 32)
                                        {
                                          printf ("|");
                                        }
                                      else
                                        {
                                          printf (" ");
                                        }
                                    }
                                }
                              else
                                {
                                  if (spectrum[j] > 9 - i)
                                    {
                                      printf ("*");
                                    }
                                  else
                                    {
                                      if (j == 32)
                                        {
                                          printf ("|");
                                        }
                                      else
                                        {
                                          printf (" ");
                                        }
                                    }
                                }
                            }
                          printf ("|\n");
                        }
                      printf ("+----------------------------------------------------------------+\n");
                    }
                }
            }

          strncpy (synctest, (synctest_p - 23), 24);
          if (Options::Instance().GetFrameP25p1())
            {
              if (strcmp (synctest, P25P1_SYNC) == 0)
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, " P25 Phase 1 ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, " +P25p1    ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = 0;
                  return (0);
                }
              if (strcmp (synctest, INV_P25P1_SYNC) == 0)
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, " P25 Phase 1 ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, " -P25p1    ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = 1;
                  return (1);
                }
            }
          if (Options::Instance().GetFrameX2tdma())
            {
              if ((strcmp (synctest, X2TDMA_BS_DATA_SYNC) == 0) || (strcmp (synctest, X2TDMA_MS_DATA_SYNC) == 0))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + (lmax)) / 2;
                  state->min = ((state->min) + (lmin)) / 2;
                  if (!Options::Instance().GetInvertedX2tdma())
                    {
                      // data frame
                      sprintf (state->ftype, " X2-TDMA     ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " +X2-TDMA  ", synctest_pos + 1, modulation);
                        }
                      state->lastsynctype = 2;
                      return (2);
                    }
                  else
                    {
                      // inverted voice frame
                      sprintf (state->ftype, " X2-TDMA     ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " -X2-TDMA  ", synctest_pos + 1, modulation);
                        }
                      if (state->lastsynctype != 3)
                        {
                          state->firstframe = 1;
                        }
                      state->lastsynctype = 3;
                      return (3);
                    }
                }
              if ((strcmp (synctest, X2TDMA_BS_VOICE_SYNC) == 0) || (strcmp (synctest, X2TDMA_MS_VOICE_SYNC) == 0))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  if (!Options::Instance().GetInvertedX2tdma())
                    {
                      // voice frame
                      sprintf (state->ftype, " X2-TDMA     ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " +X2-TDMA  ", synctest_pos + 1, modulation);
                        }
                      if (state->lastsynctype != 4)
                        {
                          state->firstframe = 1;
                        }
                      state->lastsynctype = 4;
                      return (4);
                    }
                  else
                    {
                      // inverted data frame
                      sprintf (state->ftype, " X2-TDMA     ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " -X2-TDMA  ", synctest_pos + 1, modulation);
                        }
                      state->lastsynctype = 5;
                      return (5);
                    }
                }
            }
          if (Options::Instance().GetFrameDmr())
            {
              if ((strcmp (synctest, DMR_MS_DATA_SYNC) == 0) || (strcmp (synctest, DMR_BS_DATA_SYNC) == 0))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + (lmax)) / 2;
                  state->min = ((state->min) + (lmin)) / 2;
                  if (!Options::Instance().GetInvertedDmr())
                    {
                      // data frame
                      sprintf (state->ftype, " DMR         ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " +DMR      ", synctest_pos + 1, modulation);
                        }
                      state->lastsynctype = 10;
                      return (10);
                    }
                  else
                    {
                      // inverted voice frame
                      sprintf (state->ftype, " DMR         ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " -DMR      ", synctest_pos + 1, modulation);
                        }
                      if (state->lastsynctype != 11)
                        {
                          state->firstframe = 1;
                        }
                      state->lastsynctype = 11;
                      return (11);
                    }
                }
              if ((strcmp (synctest, DMR_MS_VOICE_SYNC) == 0) || (strcmp (synctest, DMR_BS_VOICE_SYNC) == 0))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  if (!Options::Instance().GetInvertedDmr())
                    {
                      // voice frame
                      sprintf (state->ftype, " DMR         ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " +DMR      ", synctest_pos + 1, modulation);
                        }
                      if (state->lastsynctype != 12)
                        {
                          state->firstframe = 1;
                        }
                      state->lastsynctype = 12;
                      return (12);
                    }
                  else
                    {
                      // inverted data frame
                      sprintf (state->ftype, " DMR         ");
                      if (errorbars == 1)
                        {
                          printFrameSync (opts, state, " -DMR      ", synctest_pos + 1, modulation);
                        }
                      state->lastsynctype = 13;
                      return (13);
                    }
                }
            }
          if (Options::Instance().GetFrameProvoice())
            {
              strncpy (synctest32, (synctest_p - 31), 32);
              if ((strcmp (synctest32, PROVOICE_SYNC) == 0) || (strcmp (synctest32, PROVOICE_EA_SYNC) == 0))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, " ProVoice    ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, " -ProVoice ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = 14;
                  return (14);
                }
              else if ((strcmp (synctest32, INV_PROVOICE_SYNC) == 0) || (strcmp (synctest32, INV_PROVOICE_EA_SYNC) == 0))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, " ProVoice    ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, " -ProVoice ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = 15;
                  return (15);
                }

            }
          if (Options::Instance().GetFrameNxdn96() || Options::Instance().GetFrameNxdn48())
            {
              strncpy (synctest18, (synctest_p - 17), 18);
              if ((strcmp (synctest18, NXDN_BS_VOICE_SYNC) == 0) || (strcmp (synctest18, NXDN_MS_VOICE_SYNC) == 0))
                {
                  if ((state->lastsynctype == 8) || (state->lastsynctype == 16))
                    {
                      state->carrier = 1;
                      state->offset = synctest_pos;
                      state->max = ((state->max) + lmax) / 2;
                      state->min = ((state->min) + lmin) / 2;
                      if (state->samplesPerSymbol == 20)
                        {
                          sprintf (state->ftype, " NXDN48      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " +NXDN48   ", synctest_pos + 1, modulation);
                            }
                        }
                      else
                        {
                          sprintf (state->ftype, " NXDN96      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " +NXDN96   ", synctest_pos + 1, modulation);
                            }
                        }
                      state->lastsynctype = 8;
                      return (8);
                    }
                  else
                    {
                      state->lastsynctype = 8;
                    }
                }
              else if ((strcmp (synctest18, INV_NXDN_BS_VOICE_SYNC) == 0) || (strcmp (synctest18, INV_NXDN_MS_VOICE_SYNC) == 0))
                {
                  if ((state->lastsynctype == 9) || (state->lastsynctype == 17))
                    {
                      state->carrier = 1;
                      state->offset = synctest_pos;
                      state->max = ((state->max) + lmax) / 2;
                      state->min = ((state->min) + lmin) / 2;
                      if (state->samplesPerSymbol == 20)
                        {
                          sprintf (state->ftype, " NXDN48      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " -NXDN48   ", synctest_pos + 1, modulation);
                            }
                        }
                      else
                        {
                          sprintf (state->ftype, " NXDN96      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " -NXDN96   ", synctest_pos + 1, modulation);
                            }
                        }
                      state->lastsynctype = 9;
                      return (9);
                    }
                  else
                    {
                      state->lastsynctype = 9;
                    }
                }
              else if ((strcmp (synctest18, NXDN_BS_DATA_SYNC) == 0) || (strcmp (synctest18, NXDN_MS_DATA_SYNC) == 0))
                {
                  if ((state->lastsynctype == 8) || (state->lastsynctype == 16))
                    {
                      state->carrier = 1;
                      state->offset = synctest_pos;
                      state->max = ((state->max) + lmax) / 2;
                      state->min = ((state->min) + lmin) / 2;
                      if (state->samplesPerSymbol == 20)
                        {
                          sprintf (state->ftype, " NXDN48      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " +NXDN48   ", synctest_pos + 1, modulation);
                            }
                        }
                      else
                        {
                          sprintf (state->ftype, " NXDN96      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " +NXDN96   ", synctest_pos + 1, modulation);
                            }
                        }
                      state->lastsynctype = 16;
                      return (16);
                    }
                  else
                    {
                      state->lastsynctype = 16;
                    }
                }
              else if ((strcmp (synctest18, INV_NXDN_BS_DATA_SYNC) == 0) || (strcmp (synctest18, INV_NXDN_MS_DATA_SYNC) == 0))
                {
                  if ((state->lastsynctype == 9) || (state->lastsynctype == 17))
                    {
                      state->carrier = 1;
                      state->offset = synctest_pos;
                      state->max = ((state->max) + lmax) / 2;
                      state->min = ((state->min) + lmin) / 2;
                      sprintf (state->ftype, " NXDN        ");
                      if (state->samplesPerSymbol == 20)
                        {
                          sprintf (state->ftype, " NXDN48      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " -NXDN48   ", synctest_pos + 1, modulation);
                            }
                        }
                      else
                        {
                          sprintf (state->ftype, " NXDN96      ");
                          if (errorbars == 1)
                            {
                              printFrameSync (opts, state, " -NXDN96   ", synctest_pos + 1, modulation);
                            }
                        }
                      state->lastsynctype = 17;
                      return (17);
                    }
                  else
                    {
                      state->lastsynctype = 17;
                    }
                }
            }
          if (Options::Instance().GetFrameDstar())
            {
              if (strcmp (synctest, DSTAR_SYNC) == 0)
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, " D-STAR      ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, " +D-STAR   ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = 6;
                  return (6);
                }
              if (strcmp (synctest, INV_DSTAR_SYNC) == 0)
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, " D-STAR      ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, " -D-STAR   ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = 7;
                  return (7);
                }
              if (strcmp (synctest, DSTAR_HD) == 0)
                 {
                   state->carrier = 1;
                   state->offset = synctest_pos;
                   state->max = ((state->max) + lmax) / 2;
                   state->min = ((state->min) + lmin) / 2;
                   sprintf (state->ftype, " D-STAR_HD   ");
                   if (errorbars == 1)
                     {
                       printFrameSync (opts, state, " +D-STAR_HD   ", synctest_pos + 1, modulation);
                     }
                   state->lastsynctype = 18;
                   return (18);
                 }
              if (strcmp (synctest, INV_DSTAR_HD) == 0)
                {
                   state->carrier = 1;
                   state->offset = synctest_pos;
                   state->max = ((state->max) + lmax) / 2;
                   state->min = ((state->min) + lmin) / 2;
                   sprintf (state->ftype, " D-STAR_HD   ");
                   if (errorbars == 1)
                     {
                       printFrameSync (opts, state, " -D-STAR_HD   ", synctest_pos + 1, modulation);
                     }
                   state->lastsynctype = 19;
                   return (19);
                 }

            }

          if ((t == 24) && (state->lastsynctype != -1))
            {
              if ((state->lastsynctype == 0) && ((state->lastp25type == 1) || (state->lastp25type == 2)))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + (lmax)) / 2;
                  state->min = ((state->min) + (lmin)) / 2;
                  sprintf (state->ftype, "(P25 Phase 1)");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, "(+P25p1)   ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = -1;
                  return (0);
                }
              else if ((state->lastsynctype == 1) && ((state->lastp25type == 1) || (state->lastp25type == 2)))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, "(P25 Phase 1)");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, "(-P25p1)   ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = -1;
                  return (1);
                }
              else if ((state->lastsynctype == 3) && ((strcmp (synctest, X2TDMA_BS_VOICE_SYNC) != 0) || (strcmp (synctest, X2TDMA_MS_VOICE_SYNC) != 0)))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, "(X2-TDMA)    ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, "(-X2-TDMA) ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = -1;
                  return (3);
                }
              else if ((state->lastsynctype == 4) && ((strcmp (synctest, X2TDMA_BS_DATA_SYNC) != 0) || (strcmp (synctest, X2TDMA_MS_DATA_SYNC) != 0)))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, "(X2-TDMA)    ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, "(+X2-TDMA) ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = -1;
                  return (4);
                }
              else if ((state->lastsynctype == 11) && ((strcmp (synctest, DMR_BS_VOICE_SYNC) != 0) || (strcmp (synctest, DMR_MS_VOICE_SYNC) != 0)))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, "(DMR)        ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, "(-DMR)     ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = -1;
                  return (11);
                }
              else if ((state->lastsynctype == 12) && ((strcmp (synctest, DMR_BS_DATA_SYNC) != 0) || (strcmp (synctest, DMR_MS_DATA_SYNC) != 0)))
                {
                  state->carrier = 1;
                  state->offset = synctest_pos;
                  state->max = ((state->max) + lmax) / 2;
                  state->min = ((state->min) + lmin) / 2;
                  sprintf (state->ftype, "(DMR)        ");
                  if (errorbars == 1)
                    {
                      printFrameSync (opts, state, "(+DMR)     ", synctest_pos + 1, modulation);
                    }
                  state->lastsynctype = -1;
                  return (12);
                }
            }
        }

      if (exitflag == 1)
        {
          cleanupAndExit (opts, state);
        }

      if (synctest_pos < 10200)
        {
          synctest_pos++;
          synctest_p++;
        }
      else
        {
          // buffer reset
          synctest_pos = 0;
          synctest_p = synctest_buf;
          noCarrier (opts, state);
        }

      if (state->lastsynctype != 1)
        {
          if (synctest_pos >= 1800)
            {
              if ((errorbars == 1) && (opts->verbose > 1) && (state->carrier == 1))
                {
                  printf ("Sync: no sync\n");
                }
              noCarrier (opts, state);
              return (-1);
            }
        }
    }

  return (-1);
}
