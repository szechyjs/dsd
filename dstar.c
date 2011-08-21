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

/*
 * Note: D-STAR support is very incomplete and does not work yet
 * It is unknown if the ambe3600x2250 decoder is even compatible with D-STAR 
 * voice frames.  If it is then the interleave pattern needs to be determined.
 * GMSK modulation optimizations will also required to get a usable bit error
 * rate.  This was included mainly as an example of how other systmes might
 * be supported.
 */

#include "dsd.h"
#include "dstar_const.h"

void
processDSTAR (dsd_opts * opts, dsd_state * state)
{
  // extracts AMBE frames from D-STAR voice frame
  int i, j, dibit;
  char ambe_fr[4][24];
  char dstar[300];
  const int *w, *x;

  if (opts->errorbars == 1)
    {
      printf ("e:");
    }

#ifdef DSTAR_DUMP
  printf ("\n");
#endif

  for (j = 0; j < 210; j++)     // 210 is very long but used for debugging
    {
      // voice frame
      w = dW;
      x = dX;
      for (i = 0; i < 72; i++)
        {
          dibit = getDibit (opts, state);
          dstar[i] = dibit + 48;
          ambe_fr[*w][*x] = (1 & dibit);
          w++;
          x++;
        }
      dstar[i] = 32;
      processMbeFrame (opts, state, NULL, ambe_fr, NULL);

      //  data frame
      for (i = 73; i < 97; i++)
        {
          dstar[i] = getDibit (opts, state) + 48;
        }
      dstar[i] = 0;

#ifdef DSTAR_DUMP
      printf ("%s\n", dstar);
#endif
    }
  if (opts->errorbars == 1)
    {
      printf ("\n");
    }
}
