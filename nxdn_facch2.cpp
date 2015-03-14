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

#include <itpp/comm/punct_convcode.h>

#include "nxdn.h"

using namespace itpp;

static int initFlag = 0;
static Punctured_Convolutional_Code pcc;
static ivec generator(2);
static bmat puncture_matrix = "1 1 1 1 1 1 1;1 0 1 1 1 0 1";

void
processNXDNFacch2 (dsd_opts * opts, dsd_state * state, dsd_frame * frame, unsigned char bits[348])
{
  int msgStart;
  long long ll;
  char tmpStr[81];
  int i, j;
  vec input(348);
  bvec output(199);
  short msgType;
  unsigned char *ucptr;

  if(initFlag == 0)
  {
    generator(0)=023;
    generator(1)=035;
    pcc.set_generator_polynomials(generator, 5);
    pcc.set_method(Tail);
    pcc.set_puncture_matrix(puncture_matrix);
    pcc.set_truncation_length(20);
    initFlag = 1;
  }
  pcc.reset();
    // Convert to itpp soft decision mapping (0 = 1 and 1 = -1)
    for(unsigned int i=0; i<348; i++) {
        input[i] = (bits[i])?-1.0:1.0;
    }
    pcc.decode_tail(input, output);
    initNxdnCRC15();
    for(i = 0; i < 199; i++)
    {
      doNxdnCRC15((output[i])?1:0);
    }
    if(getNxdnCRC15() != 0)
      return;
	// 184 data bits 23 octets
    ucptr = frame->raw_frame_data + frame->raw_frame_data_count;
	*ucptr = 0;
	for (i = 0; i < 184; i++)
	{
	  if((i%8) == 0)
	  {
		if(i != 0)
		{
			++frame->raw_frame_data_count;
			++ucptr;
		}
		else
		{
			msgStart = frame->raw_frame_data_count;
		}
	    *ucptr = 0;
	  }
	  *ucptr <<= 1;
      *ucptr |= output[i];
	}
	++frame->raw_frame_data_count;
	++ucptr;
	processNXDNMessage (opts, state, frame, msgStart, false, false);
}
