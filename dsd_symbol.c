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

int
getSymbol (dsd_opts * opts, dsd_state * state, int have_sync)
{
  short sample;
  int i, sum, symbol, count;
  ssize_t result;

  sum = 0;
  count = 0;
  for (i = 0; i < state->samplesPerSymbol; i++)
    {
      // timing control
      if ((i == 0) && (have_sync == 0))
        {
          if (state->samplesPerSymbol == 20)
            {
              if ((state->jitter >= 7) && (state->jitter <= 10))
                {
                  i--;
                }
              else if ((state->jitter >= 11) && (state->jitter <= 14))
                {
                  i++;
                }
            }
          else if (state->rf_mod == 1)
            {
              if ((state->jitter >= 0) && (state->jitter < state->symbolCenter))
                {
                  i++;          // fall back
                }
              else if ((state->jitter > state->symbolCenter) && (state->jitter < 10))
                {
                  i--;          // catch up
                }
            }
          else if (state->rf_mod == 2)
            {
              if ((state->jitter >= state->symbolCenter - 1) && (state->jitter <= state->symbolCenter))
                {
                  i--;
                }
              else if ((state->jitter >= state->symbolCenter + 1) && (state->jitter <= state->symbolCenter + 2))
                {
                  i++;
                }
            }
          else if (state->rf_mod == 0)
            {
              if ((state->jitter > 0) && (state->jitter <= state->symbolCenter))
                {
                  i--;          // catch up
                }
              else if ((state->jitter > state->symbolCenter) && (state->jitter < state->samplesPerSymbol))
                {
                  i++;          // fall back
                }
            }
          state->jitter = -1;
        }

      // Read the new sample from the input
      if(opts->audio_in_type == 0) {
          result = read (opts->audio_in_fd, &sample, 2);
      }
      else if (opts->audio_in_type == 1) {
          result = sf_read_short(opts->audio_in_file, &sample, 1);
          if(result == 0) {
              cleanupAndExit (opts, state);
          }
      }
	  else
	  {
#ifdef USE_PORTAUDIO
		PaError err = Pa_ReadStream( opts->audio_in_pa_stream, &sample, 1 );
		if( err != paNoError )
		{
    			fprintf( stderr, "An error occured while using the portaudio input stream\n" );
    			fprintf( stderr, "Error number: %d\n", err );
    			fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		}
#endif
	  }

#ifdef TRACE_DSD
      state->debug_sample_index++;
#endif

      // printf("res: %zd\n, offset: %lld", result, sf_seek(opts->audio_in_file, 0, SEEK_CUR));
      if (opts->use_cosine_filter)
        {
          if (state->lastsynctype >= 10 && state->lastsynctype <= 13)
              sample = dmr_filter(sample);
          else if (state->lastsynctype == 8 || state->lastsynctype == 9 ||
                 state->lastsynctype == 16 || state->lastsynctype == 17)
            {
              if(state->samplesPerSymbol == 20)
                {
                  sample = nxdn_filter(sample);
                }
              else // the 12.5KHz NXDN filter is the same as the DMR filter
                {
                  sample = dmr_filter(sample);
                }
            }
        }

      if ((sample > state->max) && (have_sync == 1) && (state->rf_mod == 0))
        {
          sample = state->max;
        }
      else if ((sample < state->min) && (have_sync == 1) && (state->rf_mod == 0))
        {
          sample = state->min;
        }

      if (sample > state->center)
        {
          if (state->lastsample < state->center)
            {
              state->numflips += 1;
            }
          if (sample > (state->maxref * 1.25))
            {
              if (state->lastsample < (state->maxref * 1.25))
                {
                  state->numflips += 1;
                }
              if ((state->jitter < 0) && (state->rf_mod == 1))
                {               // first spike out of place
                  state->jitter = i;
                }
              if ((opts->symboltiming == 1) && (have_sync == 0) && (state->lastsynctype != -1))
                {
                  printf ("O");
                }
            }
          else
            {
              if ((opts->symboltiming == 1) && (have_sync == 0) && (state->lastsynctype != -1))
                {
                  printf ("+");
                }
              if ((state->jitter < 0) && (state->lastsample < state->center) && (state->rf_mod != 1))
                {               // first transition edge
                  state->jitter = i;
                }
            }
        }
      else
        {                       // sample < 0
          if (state->lastsample > state->center)
            {
              state->numflips += 1;
            }
          if (sample < (state->minref * 1.25))
            {
              if (state->lastsample > (state->minref * 1.25))
                {
                  state->numflips += 1;
                }
              if ((state->jitter < 0) && (state->rf_mod == 1))
                {               // first spike out of place
                  state->jitter = i;
                }
              if ((opts->symboltiming == 1) && (have_sync == 0) && (state->lastsynctype != -1))
                {
                  printf ("X");
                }
            }
          else
            {
              if ((opts->symboltiming == 1) && (have_sync == 0) && (state->lastsynctype != -1))
                {
                  printf ("-");
                }
              if ((state->jitter < 0) && (state->lastsample > state->center) && (state->rf_mod != 1))
                {               // first transition edge
                  state->jitter = i;
                }
            }
        }
      if (state->samplesPerSymbol == 20)
        {
          if ((i >= 9) && (i <= 11))
            {
              sum += sample;
              count++;
            }
        }
      if (state->samplesPerSymbol == 5)
        {
          if (i == 2)
            {
              sum += sample;
              count++;
            }
        }
      else
        {
          if (state->rf_mod == 0)
            {
              // 0: C4FM modulation

              if ((i >= state->symbolCenter - 1) && (i <= state->symbolCenter + 2))
                {
                  sum += sample;
                  count++;
                }

#ifdef TRACE_DSD
              if (i == state->symbolCenter - 1) {
                  state->debug_sample_left_edge = state->debug_sample_index - 1;
              }
              if (i == state->symbolCenter + 2) {
                  state->debug_sample_right_edge = state->debug_sample_index - 1;
              }
#endif
            }
          else
            {
              // 1: QPSK modulation
              // 2: GFSK modulation
              // Note: this has been changed to use an additional symbol to the left
              // On the p25_raw_unencrypted.flac it is evident that the timing
              // comes one sample too late.
              // This change makes a significant improvement in the BER, at least for
              // this file.
              //if ((i == state->symbolCenter) || (i == state->symbolCenter + 1))
              if ((i == state->symbolCenter - 1) || (i == state->symbolCenter + 1))
                {
                  sum += sample;
                  count++;
                }

#ifdef TRACE_DSD
              //if (i == state->symbolCenter) {
              if (i == state->symbolCenter - 1) {
                  state->debug_sample_left_edge = state->debug_sample_index - 1;
              }
              if (i == state->symbolCenter + 1) {
                  state->debug_sample_right_edge = state->debug_sample_index - 1;
              }
#endif
            }
        }


      state->lastsample = sample;
    }   // for

  symbol = (sum / count);

  if ((opts->symboltiming == 1) && (have_sync == 0) && (state->lastsynctype != -1))
    {
      if (state->jitter >= 0)
        {
          printf (" %i\n", state->jitter);
        }
      else
        {
          printf ("\n");
        }
    }

#ifdef TRACE_DSD
  if (state->samplesPerSymbol == 10) {
      float left, right;
      if (state->debug_label_file == NULL) {
          state->debug_label_file = fopen ("pp_label.txt", "w");
      }
      left = state->debug_sample_left_edge / SAMPLE_RATE_IN;
      right = state->debug_sample_right_edge / SAMPLE_RATE_IN;
      if (state->debug_prefix != '\0') {
          if (state->debug_prefix == 'I') {
              fprintf(state->debug_label_file, "%f\t%f\t%c%c %i\n", left, right, state->debug_prefix, state->debug_prefix_2, symbol);
          } else {
              fprintf(state->debug_label_file, "%f\t%f\t%c %i\n", left, right, state->debug_prefix, symbol);
          }
      } else {
          fprintf(state->debug_label_file, "%f\t%f\t%i\n", left, right, symbol);
      }
  }
#endif


  state->symbolcnt++;
  return (symbol);
}
