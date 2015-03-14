/*
 * Copyright (C) 2014 Eric A. Cottrell <eric.c.boston@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "nxdn.h"

void
processNXDNVoice (dsd_opts * opts, dsd_state * state, dsd_frame * frame)
{
  int i, j, dibit;
  int *dibit_p;
  char *cptr;
  unsigned char bits[300];
  char lich[9];
  short chType, fnChType, chOption, direction; // LICH bits
  char sacch[60];
  char ambe_fr[4][24];
  const int *w, *x, *y, *z;
  const char *pr;
  unsigned char *ucptr;

  dibit_p = state->dibit_buf_p - 8;

  pr = nxdnpr;
  frame->raw_frame_data_count = 0;
   frame->raw_sframe_data_count = 0;
   ucptr = frame->raw_frame_data + frame->raw_frame_data_count;
  for(i = 0; i < 128; i++)
  {
	frame->element_data_valid[0][i] = false;
	frame->element_data_valid[1][i] = false;
	frame->element_data_valid[2][i] = false;
  }
  for (i = 0; i < 8; i++)
    {
      dibit = *dibit_p;
      dibit_p++;
      if(*pr ^ (state->synctype & 0x1))
	    {
          dibit = (dibit ^ 2);
	    }
	  pr++;
	  *ucptr <<= 1;
      *ucptr |= (1 & (dibit >> 1));
    }
  frame->element_data[0][LICH_CHTYPE] = (*ucptr >> 6) & 0x03;
  frame->element_data_valid[0][LICH_CHTYPE] = true;
  frame->element_data[0][LICH_FNCHTYPE] = (*ucptr >> 4) & 0x03;
  frame->element_data_valid[0][LICH_FNCHTYPE] = true;
  frame->element_data[0][LICH_CHOPTION] = (*ucptr >> 2) & 0x03;
  frame->element_data_valid[0][LICH_CHOPTION] = true;
  frame->element_data[0][LICH_DIRECTION] = (*ucptr >> 1) & 0x01;
  frame->element_data_valid[0][LICH_DIRECTION] = true;
  ++frame->raw_frame_data_count;
  ++ucptr;
  w = nsW;
  y = nsY;

    for (i = 0; i < 30; i++)
      {
        dibit = getDibit (opts, state);
        bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
        pr++;
        bits[*y] = (1 & dibit);        // bit 0
        w++;
        y++;
      }
    processNXDNSacch (opts, state, frame, bits);
  
  for (j = 0; j < 4; j++)
    {
      w = nW;
      x = nX;
      y = nY;
      z = nZ;
      for (i = 0; i < 36; i++)
        {
          dibit = getDibit (opts, state);
#ifdef NXDN_DUMP
          printf ("%c", dibit + 48);
#endif
          ambe_fr[*w][*x] = *pr ^ (1 & (dibit >> 1));   // bit 1
          pr++;
          ambe_fr[*y][*z] = (1 & dibit);        // bit 0
          w++;
          x++;
          y++;
          z++;
        }
      processMbeFrame (opts, state, NULL, ambe_fr, NULL);
#ifdef NXDN_DUMP
      printf (" ");
#endif
    }
}
