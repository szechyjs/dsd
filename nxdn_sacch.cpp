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
static bmat puncture_matrix = "1 1 1;1 1 0";

void
processNXDNSacch (dsd_opts * opts, dsd_state * state, dsd_frame * frame, unsigned char bits[300])
{
  int msgStart;
  long long ll;
  char tmpStr[81];
  int i, j;
  vec input(60);
  bvec output(32);
  static bvec smsg(72);
  short msgType, st, ran, id, area, rptr;
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
    for(unsigned int i=0; i<60; i++) {
        input[i] = (bits[i])?-1.0:1.0;
    }
    pcc.decode_tail(input, output);

  switch (frame->element_data[0][LICH_CHTYPE])
  {
  case 0:
	// Not valid on CAC
	return;
  case 3: // Composite, used for both IDAS Type-D truck and NexEdge Type-C single frequency.
	// IDAS is the most common so try that first
    initCRC7();
    for(i = 0; i < 32; i++)
    {
      doCRC7((output[i])?1:0);
    }
    if(getCRC7() == 0x00) // Good CRC so assume this is IDAS
    {
		// 25 data bits 3 octets and one bit
		frame->raw_sframe_data_count = 0;
		ucptr = frame->raw_sframe_data;
		*ucptr = 0;
		for (i = 0; i < 25; i++)
		{
		if((i%8) == 0)
		{
			if(i != 0)
			{
				++frame->raw_sframe_data_count;
				++ucptr;
			}
			else
			{
				msgStart = frame->raw_sframe_data_count;
			}
			*ucptr = 0;
		}
		*ucptr <<= 1;
		*ucptr |= output[i];
		}
		++frame->raw_sframe_data_count;
		++ucptr;
		// Decode the SCCH here
		ucptr = frame->raw_sframe_data + msgStart;
		frame->element_data[2][NXDN_SF] = (*ucptr >> 6) & 0x03;
		frame->element_data_valid[2][NXDN_SF] = true;
		frame->element_data[2][NXDN_AREA] = (*ucptr >> 5) & 0x01;
		frame->element_data_valid[2][NXDN_AREA] = true;
		if(frame->element_data[0][LICH_DIRECTION] == 1) // Outbound Messages
		{
			switch(frame->element_data[2][NXDN_SF])
			{
			case 3:  // Info1 -- Comm options and free repeaters
				frame->element_data[2][NXDN_FREERPTR1] = *ucptr & 0x1f;
				frame->element_data_valid[2][NXDN_FREERPTR1] = true;
				++ucptr;
				frame->element_data[2][NXDN_FREERPTR2] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[2][NXDN_FREERPTR2] = true;
				frame->element_data[2][NXDN_CALLOPTION] = *ucptr & 0x07;
				frame->element_data_valid[2][NXDN_CALLOPTION] = true;
				++ucptr;
				frame->element_data[2][NXDN_CIPHERTYPE] = (*ucptr >> 6) & 0x03;
				frame->element_data_valid[2][NXDN_CIPHERTYPE] = true;
				frame->element_data[2][NXDN_KEYID] = *ucptr & 0x3f;
				frame->element_data_valid[2][NXDN_KEYID] = true;
				++ucptr;
				break;
			case 2:  // Info2 -- Receiving ID
			case 0:  // 
			    j = ((*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ff; // extract id first
				switch(j)
				{
				case 2046: // Idle Repeater
				case 2045: // Halt Repeater
				case 2044: // Free Repeater
					frame->element_data[2][NXDN_RPTRSTATE] = j - 2042;
					frame->element_data_valid[2][NXDN_RPTRSTATE] = true;
					frame->element_data[2][NXDN_FREERPTR1] = *ucptr & 0x1f;
					frame->element_data_valid[2][NXDN_FREERPTR1] = true;
					++ucptr;
					frame->element_data[2][NXDN_FREERPTR2] = (*ucptr >> 3) & 0x1f;
					frame->element_data_valid[2][NXDN_FREERPTR2] = true;
					ucptr += 2;
					break;
				case 2041: // Site ID
					frame->element_data[2][NXDN_SITETYPE] = (*ucptr >> 3) & 0x3;
					frame->element_data_valid[2][NXDN_SITETYPE] = true;
					frame->element_data[2][NXDN_SITEID] = ((*ucptr << 5) | (*(ucptr+1) >> 3)) & 0xff;
					frame->element_data_valid[2][NXDN_SITEID] = true;
					ucptr += 2;
					break;
				default: // Busy Repeater
					if(j > 2001) // Special ID
					{
						frame->element_data[2][NXDN_SPECIALID] = j;
						frame->element_data_valid[2][NXDN_SPECIALID] = true;
					}
					frame->element_data[2][NXDN_BUSYRPTR] = *ucptr & 0x1f;
					frame->element_data_valid[2][NXDN_BUSYRPTR] = true;
					++ucptr;
					frame->element_data[2][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
					frame->element_data_valid[2][NXDN_DESTID] = true;
					ucptr += 2;
					frame->element_data[2][NXDN_GUFLAG] = *ucptr & 0x01;
					frame->element_data_valid[2][NXDN_GUFLAG] = true;
					++ucptr;
				}
				break;
			case 1:  // Info3 -- Sending ID and free repeater
				frame->element_data[2][NXDN_RPTRSTATE] = 1;
				frame->element_data_valid[2][NXDN_RPTRSTATE] = true;
				frame->element_data[2][NXDN_FREERPTR1] = *ucptr & 0x1f;
				frame->element_data_valid[2][NXDN_FREERPTR1] = true;
				++ucptr;
				frame->element_data[2][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
				frame->element_data_valid[2][NXDN_SRCID] = true;
				ucptr += 2;
				break;
			}
		}
		else
		{
		}
		break; // all done
	}
	// else fall through to Type-C format
  default:
    initCRC6();
    for(i = 0; i < 32; i++)
    {
      doCRC6((output[i])?1:0);
    }
    if(getCRC6() != 0x00)
      return;
    st = (output[0] << 1) | output[1];
    frame->element_data[2][NXDN_SF] = st;
    frame->element_data_valid[2][NXDN_SF] = true;
    ran = 0;
    for (i = 2; i < 8; i++)
    {
      ran <<= 1;
      ran |= output[i];
    }
    frame->element_data[0][NXDN_RAN] = ran;
    frame->element_data_valid[0][NXDN_RAN] = true;
    if(frame->element_data[0][LICH_FNCHTYPE] == 0) // Non-Superframe
    {
      for(i=0; i < 18; i++)
      {
        smsg[i] = output[i+8];
        smsg[i+18] = 0;
        smsg[i+36] = 0;
        smsg[i+54] = 0;
        
      }
    }
    else
    {
      for(i=0; i < 18; i++)
      {
        smsg[i+(3-(int)st)*18] = output[i+8];
      }
    }
    if((st == 0) || (frame->element_data[0][LICH_FNCHTYPE] == 0))
    {
      // 72 data bits 9 octets
      frame->raw_sframe_data_count = 0;
      ucptr = frame->raw_sframe_data;
      *ucptr = 0;
      for (i = 0; i < 72; i++)
      {
        if((i%8) == 0)
        {
          if(i != 0)
          {
			++frame->raw_sframe_data_count;
			++ucptr;
          }
          else
          {
			msgStart = frame->raw_sframe_data_count;
          }
          *ucptr = 0;
        }
        *ucptr <<= 1;
        *ucptr |= smsg[i];
      }
      ++frame->raw_sframe_data_count;
      ++ucptr;
      processNXDNMessage (opts, state, frame, msgStart, false, true);
      return;
    }
  }
}
