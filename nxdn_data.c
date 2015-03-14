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
processNXDNData (dsd_opts * opts, dsd_state * state, dsd_frame * frame)
{
  int i, j, dibit;
  int *dibit_p;
  char *cptr;
  unsigned char bits[350];
  unsigned char *ucptr;

  const int *w, *y;
  const char *pr;
  long long ll;
  dibit_p = state->dibit_buf_p - 8;

  pr = nxdnpr;
  frame->raw_frame_data_count = 0;
  frame->raw_sframe_data_count = 0;
  ucptr = frame->raw_frame_data;
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
  if(frame->element_data[0][LICH_CHTYPE] == 0) // NexEdge Control Channel
  {
    w = ncW;
    y = ncY;
    for (i = 0; i < 150; i++)
      {
        dibit = getDibit (opts, state);
        bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
        pr++;
        bits[*y] = (1 & dibit);        // bit 0
        w++;
        y++;
      }
    processNXDNCac (opts, state, frame, bits);

    for (i = 0; i < 24; i++)
      {
		dibit = getDibit (opts, state);
      }
	return;
  }
  else if((frame->element_data[0][LICH_FNCHTYPE] == 1)) // User Data
  {
	if(frame->element_data[0][LICH_CHTYPE] == 3) // Composite can be in two formats so assume Type-D
	{
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
		if(frame->element_data_valid[2][NXDN_SF]) // Type-D
		{
			w = nfW;
			y = nfY;
			for (i = 0; i < 72; i++)
			{
				dibit = getDibit (opts, state);
				bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
				pr++;
				bits[*y] = (1 & dibit);        // bit 0
				w++;
				y++;
			}
			w = nfW;
			y = nfY;
			for (i = 0; i < 72; i++)
			{
				dibit = getDibit (opts, state);
				bits[*w + 144] = *pr ^ (1 & (dibit >> 1)); // bit 1
				pr++;
				bits[*y + 144] = (1 & dibit);        // bit 0
				w++;
				y++;
			}
			processNXDNFacch3 (opts, state, frame, bits);
		}
		else
		{
			w = nuW;
			y = nuY;
			frame->raw_sframe_data_count = 0;
			pr -= 30; //rewind
			dibit_p = state->dibit_buf_p - 30;  
			for (i = 0; i < 30; i++)
			{
				dibit = *dibit_p;
				dibit_p++;
				if(*pr ^ (state->synctype & 0x1))
				{
					dibit = (dibit ^ 2);
				}
				bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
				pr++;
				bits[*y] = (1 & dibit);        // bit 0
				w++;
				y++;
			}
			for (i = 30; i < 174; i++)
			{
				dibit = getDibit (opts, state);
				bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
				pr++;
				bits[*y] = (1 & dibit);        // bit 0
				w++;
				y++;
			}
			processNXDNFacch2 (opts, state, frame, bits);
		}
	}
	else
	{
		w = nuW;
		y = nuY;
		frame->raw_sframe_data_count = 0;
		for (i = 0; i < 174; i++)
		{
			dibit = getDibit (opts, state);
			bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
			pr++;
			bits[*y] = (1 & dibit);        // bit 0
			w++;
			y++;
		}
		processNXDNFacch2 (opts, state, frame, bits);
	}
	return;
  }
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
  w = nfW;
  y = nfY;
  for (i = 0; i < 72; i++)
    {
        dibit = getDibit (opts, state);
        bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
        pr++;
        bits[*y] = (1 & dibit);        // bit 0
        w++;
        y++;
    }

  processNXDNFacch1 (opts, state, frame, bits);
  if (frame->element_data[0][LICH_CHOPTION] == 0)
    {
	  w = nfW;
	  y = nfY;
      
	  for (i = 0; i < 72; i++)
		{
			dibit = getDibit (opts, state);
			bits[*w] = *pr ^ (1 & (dibit >> 1)); // bit 1
			pr++;
			bits[*y] = (1 & dibit);        // bit 0
			w++;
			y++;
		}
        processNXDNFacch1 (opts, state, frame, bits);
    }
}
