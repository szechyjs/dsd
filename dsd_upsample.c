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

void
upsample (dsd_state * state, float invalue)
{

  int i, j, sum;
  float *outbuf1, a, b, c, d;
  float phasestart, phaseend, resamplestart, resampleend, phasestep, resampleamp2;
  float resamplemin, resamplemax;

  if (state->audio_out_idx2 > 23)
    {
      outbuf1 = state->audio_out_float_buf_p;
      outbuf1--;
      c = *outbuf1;
      outbuf1 -= 6;
      b = *outbuf1;
      outbuf1 -= 6;
      a = *outbuf1;
      d = (invalue);
      resamplemin = 32767;
      resamplemax = -32768;
      if (a < resamplemin)
        {
          resamplemin = a;
        }
      if (a > resamplemax)
        {
          resamplemax = a;
        }
      if (b < resamplemin)
        {
          resamplemin = b;
        }
      if (b > resamplemax)
        {
          resamplemax = b;
        }
      if (c < resamplemin)
        {
          resamplemin = c;
        }
      if (c > resamplemax)
        {
          resamplemax = c;
        }
      if (d < resamplemin)
        {
          resamplemin = d;
        }
      if (d > resamplemax)
        {
          resamplemax = d;
        }
      resampleamp2 = ((float) resamplemax - (float) resamplemin) / (float) 2;
#ifdef UPSAMPLE_DEBUG
      printf ("a:%f b:%f c:%f d:%f resamplemin:%f resamplemax:%f resampleamp2:%f\n", a, b, c, d, resamplemin, resamplemax, resampleamp2);
#endif
      if (resampleamp2 == 0)
        {
          resamplestart = (float) 0;
          resampleend = (float) 0;
        }
      else
        {
          resamplestart = (((float) b - (float) resamplemin) / resampleamp2) - (float) 1.0;
          resampleend = (((float) c - (float) resamplemin) / resampleamp2) - (float) 1.0;
        }

      if (resamplestart < -1)
        {
          resamplestart = (float) -1;
        }
      else if (resamplestart > (float) 1)
        {
          resamplestart = (float) 1;
        }
      if (resampleend < -1)
        {
          resampleend = (float) -1;
        }
      else if (resampleend > (float) 1)
        {
          resampleend = (float) 1;
        }

      phasestart = asinf (resamplestart);
      phaseend = asinf (resampleend);
      phasestep = (phaseend - phasestart) / 6;
#ifdef UPSAMPLE_DEBUG
      printf ("resamplestart:%f, resampleend:%f, phasestart:%f, phaseend:%f, phasestep:%f\n", resamplestart, resampleend, phasestart, phaseend, phasestep);
#endif

      // rewrite samples between b and c
      outbuf1 = state->audio_out_float_buf_p;
      outbuf1 -= 6;
      for (i = 1; i < 6; i++)
        {
          *outbuf1 = (resamplemin + resampleamp2 + (resampleamp2 * sin (phasestart + (phasestep * (float) i))));
#ifdef UPSAMPLE_DEBUG
          printf ("audio_out_idx2:%i outbuf:%f\n", state->audio_out_idx2, *outbuf1);
#endif
          outbuf1++;
        }
#ifdef UPSAMPLE_DEBUG
      printf ("(c):%f\n", *outbuf1);
#endif
      // write d
      outbuf1 += 6;
      *outbuf1 = d;
#ifdef UPSAMPLE_DEBUG
      printf ("audio_out_idx2:%i outbuf(d):%f\n", state->audio_out_idx2, *outbuf1);
#endif
      outbuf1++;

      // smooth
      outbuf1 -= 36;
      for (j = 0; j < 12; j++)
        {
          for (i = 0; i < 12; i++)
            {
              sum = 0;
              outbuf1--;
              sum += *outbuf1;
              outbuf1++;
              sum += *outbuf1;
              outbuf1++;
              sum += *outbuf1;
              outbuf1--;
              *outbuf1 = (sum / (float) 3);
              outbuf1++;
            }
          outbuf1 -= 11;
        }
    }
  else
    {
      outbuf1 = state->audio_out_float_buf_p;
      outbuf1--;
      c = *outbuf1;
      *outbuf1 = ((invalue * (float) 0.166) + (c * (float) 0.834));
      outbuf1++;
      *outbuf1 = ((invalue * (float) 0.332) + (c * (float) 0.668));
      outbuf1++;
      *outbuf1 = ((invalue * (float) 0.5) + (c * (float) 0.5));
      outbuf1++;
      *outbuf1 = ((invalue * (float) 0.668) + (c * (float) 0.332));
      outbuf1++;
      *outbuf1 = ((invalue * (float) 0.834) + (c * (float) 0.166));
      outbuf1++;
      *outbuf1 = invalue;
    }
}
