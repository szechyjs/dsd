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
processNXDNMessage (dsd_opts * opts, dsd_state * state, dsd_frame * frame, int offset, bool dualFlag, bool slowFlag)
{
	int i;
	int elementSet = 0;
    unsigned char *ucptr;
	if(slowFlag)
	{
		elementSet = 2;
		ucptr = frame->raw_sframe_data + offset;
	}
	else
	{
		if(dualFlag)
			elementSet = (frame->element_data_valid[0][NXDN_MSGTYPE])?1:0;
		ucptr = frame->raw_frame_data + offset;
	}
	// Ignore F1 F2 for now
    frame->element_data[elementSet][NXDN_MSGTYPE] = *ucptr & 0x3f;
    frame->element_data_valid[elementSet][NXDN_MSGTYPE] = true;
	++ucptr;
	switch(frame->element_data[elementSet][NXDN_MSGTYPE])
	{
	case 0: // CALL_RESP
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS 
		{
			frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
			++ucptr;
			frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
			frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
			frame->element_data[elementSet][NXDN_FREERPTR1] = *ucptr & 0x1f;
			frame->element_data_valid[elementSet][NXDN_FREERPTR1] = true;
			++ucptr;
			frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_SRCID] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_DESTID] = true;
			ucptr += 2;
			if((frame->element_data[elementSet][NXDN_CCOPTION] & 0x40) && (dualFlag == false))
			{
				frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
				frame->element_data_valid[elementSet][NXDN_LOCID] = true;
				ucptr += 3;
			}
			break;
		}
		break;
	case 1: // VCALL_RESP VCALL 
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_VCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_VCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0: // NexEdge Control Channel
			frame->element_data[elementSet][NXDN_CAUSEVD] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CAUSEVD] = true;
			++ucptr;
			break;
		default:
			frame->element_data[elementSet][NXDN_CIPHERTYPE] = (*ucptr >> 6) & 0x03;
			frame->element_data_valid[elementSet][NXDN_CIPHERTYPE] = true;
			frame->element_data[elementSet][NXDN_KEYID] = *ucptr & 0x3f;
			frame->element_data_valid[elementSet][NXDN_KEYID] = true;
			++ucptr;
			break;
		}
		break;
	case 2: // VCALL_REC_REQ
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_VCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_VCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		if((frame->element_data[elementSet][NXDN_CCOPTION] & 0x40) && (dualFlag == false))
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
		break;
	case 3: // VCALL_CONN_RESP  VCALL_IV
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0: // NexEdge Control Channel
			frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
			++ucptr;
			frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
			frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
			frame->element_data[elementSet][NXDN_VCALLOPTION] = *ucptr & 0x1f;
			frame->element_data_valid[elementSet][NXDN_VCALLOPTION] = true;
			++ucptr;
			frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_SRCID] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_DESTID] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_CAUSEVD] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CAUSEVD] = true;
			++ucptr;
			break;
		default:
			frame->element_data[elementSet][NXDN_IVH] = frame->element_data[elementSet][NXDN_IVL] = 0;
		    for ( i = 0; i < 4; i++)
			{
				frame->element_data[elementSet][NXDN_IVH] << 8;
				frame->element_data[elementSet][NXDN_IVH] |= *(ucptr+i);
			}
			frame->element_data_valid[elementSet][NXDN_IVH] = true;
		    for ( i = 4; i < 8; i++)
			{
				frame->element_data[elementSet][NXDN_IVL] << 8;
				frame->element_data[elementSet][NXDN_IVL] |= *(ucptr+i);
			}
			frame->element_data_valid[elementSet][NXDN_IVL] = true;
			ucptr += 8;
			break;
		}
		break;
	case 4: // VCALL_ASSGN
	case 5: // VCALL_ASSGN_DUP
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_VCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_VCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CALLTIMER] = (*ucptr >> 2) & 0x3f;
		frame->element_data_valid[elementSet][NXDN_CALLTIMER] = true;
		frame->element_data[elementSet][NXDN_CHAN] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
		frame->element_data_valid[elementSet][NXDN_CHAN] = true;
		ucptr += 2;
		if((frame->element_data[0][LICH_CHTYPE] == 0) && (dualFlag == false))
		{
			if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
			{
				frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
				frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			}
			ucptr += 3;
			frame->element_data[elementSet][NXDN_TEMPID] = (*ucptr << 8) | *(ucptr+1);
			if(frame->element_data[elementSet][NXDN_TEMPID]) // If not a filler
				frame->element_data_valid[elementSet][NXDN_TEMPID] = true;
			ucptr += 2;
		}
		break;
	case 6: // CALL_CONN_RESP (IDAS FA3)
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS
		{
			frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
			++ucptr;
			frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
			frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
			frame->element_data[elementSet][NXDN_FREERPTR1] = *ucptr & 0x1f;
			frame->element_data_valid[elementSet][NXDN_FREERPTR1] = true;
			++ucptr;
			frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_SRCID] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_DESTID] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_CAUSEVD] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CAUSEVD] = true;
			++ucptr;
			if((frame->element_data[elementSet][NXDN_CCOPTION] & 0x40) && (dualFlag == false))
			{
				frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
				frame->element_data_valid[elementSet][NXDN_LOCID] = true;
				ucptr += 3;
			}
			break;
		}
		break;
	case 7: // TX_REL_EX
	case 8: // TX_REL
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS
		{
			frame->element_data[elementSet][NXDN_FREERPTR1] = *ucptr & 0x1f;
			frame->element_data_valid[elementSet][NXDN_FREERPTR1] = true;
		}
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		if(frame->element_data[elementSet][NXDN_MSGTYPE] == 7)
		{
			frame->element_data[elementSet][NXDN_TXPERMIT] = (*ucptr >> 7) & 0x01;
			frame->element_data_valid[elementSet][NXDN_TXPERMIT] = true;
			++ucptr;
		}
		break;
	case 9: // DCALL_RESP DCALL (Hdr)
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_DCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_DCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0: // NexEdge Control Channel
			frame->element_data[elementSet][NXDN_CAUSEVD] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CAUSEVD] = true;
			++ucptr;
			break;
		default:
			frame->element_data[elementSet][NXDN_CIPHERTYPE] = (*ucptr >> 6) & 0x03;
			frame->element_data_valid[elementSet][NXDN_CIPHERTYPE] = true;
			frame->element_data[elementSet][NXDN_KEYID] = *ucptr & 0x3f;
			frame->element_data_valid[elementSet][NXDN_KEYID] = true;
			++ucptr;
			frame->element_data[elementSet][NXDN_PACKETINFO] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xfff;
			frame->element_data_valid[elementSet][NXDN_PACKETINFO] = true;
			ucptr += 3;
			if(frame->element_data_valid[0][NXDN_RAN]) // Not IDAS
			{
				frame->element_data[elementSet][NXDN_IVH] = frame->element_data[elementSet][NXDN_IVL] = 0;
				for ( i = 0; i < 4; i++)
				{
					frame->element_data[elementSet][NXDN_IVH] << 8;
					frame->element_data[elementSet][NXDN_IVH] |= *(ucptr+i);
				}
				for ( i = 4; i < 8; i++)
				{
					frame->element_data[elementSet][NXDN_IVL] << 8;
					frame->element_data[elementSet][NXDN_IVL] |= *(ucptr+i);
				}
				frame->element_data_valid[elementSet][NXDN_IVH] = true;
				frame->element_data_valid[elementSet][NXDN_IVL] = true;
				ucptr += 8;
			}
			break;
		}
		break;
	case 10: // DCALL_REC_REQ
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_DCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_DCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		if((frame->element_data[elementSet][NXDN_CCOPTION] & 0x40) && (dualFlag == false))
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
		break;
	case 11: // DCALL (Data)
		frame->element_data[elementSet][NXDN_PKTFRNUM] = (*ucptr >> 4) & 0x0f;
		frame->element_data_valid[elementSet][NXDN_PKTFRNUM] = true;
		frame->element_data[elementSet][NXDN_BLKNUM] = *ucptr & 0x0f;
		frame->element_data_valid[elementSet][NXDN_BLKNUM] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_DATASTART] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATASTART] = true;
		if(frame->element_data_valid[2][NXDN_AREA])
			ucptr += 18;
		else
			ucptr += 20;
		frame->element_data[elementSet][NXDN_DATAEND] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATAEND] = true;
		break;
	case 12: // DCALL_ASSGN DCALL_ACK
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_DCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_DCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		if((frame->element_data[0][LICH_CHTYPE] == 0) && (dualFlag == false))
		{
			frame->element_data[elementSet][NXDN_CALLTIMER] = (*ucptr >> 2) & 0x3f;
			frame->element_data_valid[elementSet][NXDN_CALLTIMER] = true;
			frame->element_data[elementSet][NXDN_CHAN] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
			frame->element_data_valid[elementSet][NXDN_CHAN] = true;
			ucptr += 2;
			if(dualFlag == false)
			{
				if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
				{
					frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
					frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
					frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
					frame->element_data_valid[elementSet][NXDN_LOCID] = true;
				}
				ucptr += 3;
				frame->element_data[elementSet][NXDN_TEMPID] = (*ucptr << 8) | *(ucptr+1);
				if(frame->element_data[elementSet][NXDN_TEMPID]) // If not a filler
					frame->element_data_valid[elementSet][NXDN_TEMPID] = true;
				ucptr += 2;
			}
		}
		else
		{
			frame->element_data[elementSet][NXDN_RESPINFO] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_RESPINFO] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_ERRBLKFLAG] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_ERRBLKFLAG] = true;
			ucptr += 2;
		}
		break;
	case 13: // DCALL_ASSGN_DUP
	case 14: // DCALL_ASSGN
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_DCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_DCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CALLTIMER] = (*ucptr >> 2) & 0x3f;
		frame->element_data_valid[elementSet][NXDN_CALLTIMER] = true;
		frame->element_data[elementSet][NXDN_CHAN] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
		frame->element_data_valid[elementSet][NXDN_CHAN] = true;
		ucptr += 2;
		if((frame->element_data[0][LICH_CHTYPE] == 0) && (dualFlag == false))
		{
			if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
			{
				frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
				frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			}
			ucptr += 3;
			frame->element_data[elementSet][NXDN_TEMPID] = (*ucptr << 8) | *(ucptr+1);
			if(frame->element_data[elementSet][NXDN_TEMPID]) // If not a filler
				frame->element_data_valid[elementSet][NXDN_TEMPID] = true;
			ucptr += 2;
		}
		break;
	case 15: // HEAD_DLY
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_HDRDELAY] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_HDRDELAY] = true;
		ucptr += 2;
		break;
	case 16: // IDLE - Nothing to decode
		break;
	case 17: // DISC
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CAUSEDISC] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CAUSEDISC] = true;
		++ucptr;
		if((frame->element_data[0][LICH_CHTYPE] == 0) && (dualFlag == false))
		{
			if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
			{
				frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
				frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			}
			ucptr += 3;
		}
		break;
	case 24: // SITE_INFO
		frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
		frame->element_data_valid[elementSet][NXDN_LOCID] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_CHANSTRUCT] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_CHANSTRUCT] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_SRVINFO] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRVINFO] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_RESTINFO] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xfff;
		frame->element_data_valid[elementSet][NXDN_RESTINFO] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_CHANACCESS] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xfff;
		frame->element_data_valid[elementSet][NXDN_CHANACCESS] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_VERSION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_VERSION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_ADJSITECNT] = (*ucptr >> 4) & 0x0f;
		frame->element_data_valid[elementSet][NXDN_ADJSITECNT] = true;
		frame->element_data[elementSet][NXDN_CCHAN1] = ((*ucptr << 6) | (*(ucptr+1) >> 2)) & 0x3ff;
		frame->element_data_valid[elementSet][NXDN_CCHAN1] = true;
		frame->element_data[elementSet][NXDN_CCHAN2] = ((*(ucptr+1) << 8) | *(ucptr+2)) & 0x3ff;
		frame->element_data_valid[elementSet][NXDN_CCHAN2] = true;
		ucptr += 3;
		break;
	case 25: // SRV_INFO
		frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
		frame->element_data_valid[elementSet][NXDN_LOCID] = true;
		ucptr += 3;
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS 
		{
			frame->element_data[elementSet][NXDN_SITEID] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_SITEID] = true;
			++ucptr;
		}
		frame->element_data[elementSet][NXDN_SRVINFO] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRVINFO] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_RESTINFO] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xfff;
		frame->element_data_valid[elementSet][NXDN_RESTINFO] = true;
		ucptr += 3;
		break;
	case 26: // CCH_INFO
		frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
		frame->element_data_valid[elementSet][NXDN_LOCID] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_CCHANINFO] = (*ucptr >> 2) & 0x3f;
		frame->element_data_valid[elementSet][NXDN_CCHANINFO] = true;
		frame->element_data[elementSet][NXDN_CCHAN1] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
		frame->element_data_valid[elementSet][NXDN_CCHAN1] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CCHAN2] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
		frame->element_data_valid[elementSet][NXDN_CCHAN2] = true;
		ucptr += 2;
		break;
	case 27: // ADJ_SITE_INFO
		if(frame->element_data_valid[0][NXDN_RAN]) // Type-C 
		{
			frame->element_data[elementSet][NXDN_ADJSITELOCID1] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_ADJSITELOCID1] = true;
			ucptr += 3;
			frame->element_data[elementSet][NXDN_ADJSITEOPTION1] = (*ucptr >> 2) & 0x3f;
			frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION1] = true;
			frame->element_data[elementSet][NXDN_ADJSITECCHAN1] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
			frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN1] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_ADJSITELOCID2] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_ADJSITELOCID2] = true;
			ucptr += 3;
			frame->element_data[elementSet][NXDN_ADJSITEOPTION2] = (*ucptr >> 2) & 0x3f;
			frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION2] = true;
			frame->element_data[elementSet][NXDN_ADJSITECCHAN2] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
			frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN2] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_ADJSITELOCID3] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_ADJSITELOCID3] = true;
			ucptr += 3;
			frame->element_data[elementSet][NXDN_ADJSITEOPTION3] = (*ucptr >> 2) & 0x3f;
			frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION3] = true;
			frame->element_data[elementSet][NXDN_ADJSITECCHAN3] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
			frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN3] = true;
			ucptr += 2;
			if((frame->raw_frame_data_count - offset) > 20)
			{
				frame->element_data[elementSet][NXDN_ADJSITELOCID4] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
				frame->element_data_valid[elementSet][NXDN_ADJSITELOCID4] = true;
				ucptr += 3;
				frame->element_data[elementSet][NXDN_ADJSITEOPTION4] = (*ucptr >> 2) & 0x3f;
				frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION4] = true;
				frame->element_data[elementSet][NXDN_ADJSITECCHAN4] = ((*ucptr << 8) | *(ucptr+1)) & 0x3ff;
				frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN4] = true;
				ucptr += 2;
			}
		}
		else // Type-D
		{
			if(*ucptr & 0x1)
			{
				++ucptr;
				frame->element_data[elementSet][NXDN_ADJSITEOPTION3] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION3] = true;
				frame->element_data[elementSet][NXDN_ADJSITELOCID3] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
				frame->element_data_valid[elementSet][NXDN_ADJSITELOCID3] = true;
				ucptr += 3;
				frame->element_data[elementSet][NXDN_ADJSITECCHAN3] = *ucptr;
				frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN3] = true;
				++ucptr;
				frame->element_data[elementSet][NXDN_ADJSITEOPTION4] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION4] = true;
				frame->element_data[elementSet][NXDN_ADJSITELOCID4] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
				frame->element_data_valid[elementSet][NXDN_ADJSITELOCID4] = true;
				ucptr += 3;
				frame->element_data[elementSet][NXDN_ADJSITECCHAN4] = *ucptr;
				frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN4] = true;
				++ucptr;
			}
			else
			{
				++ucptr;
				frame->element_data[elementSet][NXDN_ADJSITEOPTION1] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION1] = true;
				frame->element_data[elementSet][NXDN_ADJSITELOCID1] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
				frame->element_data_valid[elementSet][NXDN_ADJSITELOCID1] = true;
				ucptr += 3;
				frame->element_data[elementSet][NXDN_ADJSITECCHAN1] = *ucptr;
				frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN1] = true;
				++ucptr;
				frame->element_data[elementSet][NXDN_ADJSITEOPTION2] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_ADJSITEOPTION2] = true;
				frame->element_data[elementSet][NXDN_ADJSITELOCID2] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
				frame->element_data_valid[elementSet][NXDN_ADJSITELOCID2] = true;
				ucptr += 3;
				frame->element_data[elementSet][NXDN_ADJSITECCHAN2] = *ucptr;
				frame->element_data_valid[elementSet][NXDN_ADJSITECCHAN2] = true;
				++ucptr;
			}
		}
		break;
    case 28: // FAIL_STAT_INFO
		frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xffffff;
		frame->element_data_valid[elementSet][NXDN_LOCID] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_FAILCODE] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_FAILCODE] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTIMER] = (*ucptr >> 2) & 0x3f;
		frame->element_data_valid[elementSet][NXDN_CALLTIMER] = true;
		++ucptr;
        break;
    case 32:  // REG_RESP
		frame->element_data[elementSet][NXDN_REGOPTION] = (*ucptr >> 3) & 0x1f;
		frame->element_data_valid[elementSet][NXDN_REGOPTION] = true;
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
		else
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
		frame->element_data_valid[elementSet][NXDN_LOCID] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CAUSEMM] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CAUSEMM] = true;
		++ucptr;
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS
		{
			frame->element_data[elementSet][NXDN_HOMERPTR1] = *ucptr & 0x1f;
			frame->element_data_valid[elementSet][NXDN_HOMERPTR1] = true;
			frame->element_data[elementSet][NXDN_HOMERPTR2] = *ucptr & 0x1f;
			frame->element_data_valid[elementSet][NXDN_HOMERPTR2] = true;
			if(frame->element_data[elementSet][NXDN_REGOPTION] & 0x08)
			{
				frame->element_data[elementSet][NXDN_VISITID] = (*ucptr << 8) | *(ucptr+1);
				frame->element_data_valid[elementSet][NXDN_VISITID] = true;
				ucptr += 2;
				frame->element_data[elementSet][NXDN_VISITGRP] = (*ucptr << 8) | *(ucptr+1);
				frame->element_data_valid[elementSet][NXDN_VISITGRP] = true;
				ucptr += 2;
			}
		}
		else if(frame->element_data[elementSet][NXDN_REGOPTION] & 0x08)
		{
			frame->element_data[elementSet][NXDN_VISITID] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_VISITID] = true;
			ucptr += 2;
			frame->element_data[elementSet][NXDN_VISITGRP] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_VISITGRP] = true;
			ucptr += 2;
		}
        break;
    case 34:  // REG_C_RESP
		frame->element_data[elementSet][NXDN_REGOPTION] = (*ucptr >> 3) & 0x1f;
		frame->element_data_valid[elementSet][NXDN_REGOPTION] = true;
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
		else
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
		frame->element_data_valid[elementSet][NXDN_LOCID] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CAUSEMM] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CAUSEMM] = true;
		++ucptr;
        break;
    case 35:  // REG_COMM
		frame->element_data[elementSet][NXDN_REGOPTION] = (*ucptr >> 3) & 0x1f;
		frame->element_data_valid[elementSet][NXDN_REGOPTION] = true;
		frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
		frame->element_data_valid[elementSet][NXDN_LOCID] = true;
		ucptr += 3;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
        break;
    case 36: // GRP_REG_RESP
		frame->element_data[elementSet][NXDN_GRPREGOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_GRPREGOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CAUSEMM] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CAUSEMM] = true;
		++ucptr;
		if(frame->element_data[elementSet][NXDN_REGOPTION] & 0x40)
		{
			frame->element_data[elementSet][NXDN_VISITGRP] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_VISITGRP] = true;
			ucptr += 2;
		}
        break;
    case 40: // AUTH_INQ_REQ
		frame->element_data[elementSet][NXDN_AUTHOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_AUTHOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_AUTHPARM] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_AUTHPARM] = true;
		ucptr += 2;
		if(frame->element_data[elementSet][NXDN_AUTHOPTION] & 0x40)
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0x7ffff;
			else
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
 		}
       break;
    case 41: // AUTH_INQ_RESP
	    break;
    case 43: // DWR Header  Type-D
		frame->element_data[elementSet][NXDN_DWOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_DWOPTION] = true;
		++ucptr;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		++ucptr;
		frame->element_data[elementSet][NXDN_PACKETINFO] = ((*ucptr << 16) | (*(ucptr+1) << 8) | *(ucptr+2)) & 0xfff;
		frame->element_data_valid[elementSet][NXDN_PACKETINFO] = true;
		ucptr += 3;
	    break;
    case 44: // DWR Data  Type-D
		frame->element_data[elementSet][NXDN_PKTFRNUM] = (*ucptr >> 4) & 0x0f;
		frame->element_data_valid[elementSet][NXDN_PKTFRNUM] = true;
		frame->element_data[elementSet][NXDN_BLKNUM] = *ucptr & 0x0f;
		frame->element_data_valid[elementSet][NXDN_BLKNUM] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_DATASTART] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATASTART] = true;
		ucptr += 18;
		frame->element_data[elementSet][NXDN_DATAEND] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATAEND] = true;
		break;
    case 48: // STAT_INQ_REQ
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_SCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_SCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		if((frame->element_data_valid[0][NXDN_RAN]) && (frame->element_data[elementSet][NXDN_CCOPTION] & 0x40))
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
		break;
    case 49: // STAT_INQ_RESP
    case 50: // STAT_REQ
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		if(frame->element_data[elementSet][NXDN_MSGTYPE] == 50)
		{
			frame->element_data[elementSet][NXDN_SCALLOPTION] = *ucptr & 0x1f;
			frame->element_data_valid[elementSet][NXDN_SCALLOPTION] = true;
		}
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		if(frame->element_data[elementSet][NXDN_MSGTYPE] == 49)
		{
			frame->element_data[elementSet][NXDN_CAUSESS] = *ucptr;
			frame->element_data_valid[elementSet][NXDN_CAUSESS] = true;
		}
		++ucptr;
		frame->element_data[elementSet][NXDN_STATUS] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_STATUS] = true;
		++ucptr;
		if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
        break;
    case 51: // STAT_RESP
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CAUSESS] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CAUSESS] = true;
		++ucptr;
		if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
        break;
    case 52: // REM_CON_REQ
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_GUFLAG] = (*ucptr >> 7) & 0x01;
		frame->element_data_valid[elementSet][NXDN_GUFLAG] = true;
		frame->element_data[elementSet][NXDN_DELIVERY] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_DELIVERY] = true;
		frame->element_data[elementSet][NXDN_CTLCMD] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_CTLCMD] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CTLPARM] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_CTLPARM] = true;
		ucptr += 2;
		if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
        break;
    case 53: // REM_CON_RESP
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_GUFLAG] = (*ucptr >> 7) & 0x01;
		frame->element_data_valid[elementSet][NXDN_GUFLAG] = true;
		frame->element_data[elementSet][NXDN_CTLCMD] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_CTLCMD] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		ucptr++;
		frame->element_data[elementSet][NXDN_CAUSESS] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CAUSESS] = true;
		ucptr++;
		if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
        break;
    case 56: // SDCALL_REQ Hdr
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_DCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_DCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CIPHERTYPE] = (*ucptr >> 6) & 0x03;
		frame->element_data_valid[elementSet][NXDN_CIPHERTYPE] = true;
		frame->element_data[elementSet][NXDN_KEYID] = *ucptr & 0x3f;
		frame->element_data_valid[elementSet][NXDN_KEYID] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_PACKETINFO] = ((*ucptr << 16) | (*(ucptr+1) << 8)) & 0xfff;
		frame->element_data_valid[elementSet][NXDN_PACKETINFO] = true;
		ucptr += 2;
 		if((frame->element_data[0][LICH_CHTYPE] == 0) && (dualFlag == false))
		{
			if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
			{
				frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
				frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
				frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
				frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			}
			ucptr += 3;
		}
       break;
    case 57: // SDCALL_REQ Data
		frame->element_data[elementSet][NXDN_PKTFRNUM] = (*ucptr >> 4) & 0x0f;
		frame->element_data_valid[elementSet][NXDN_PKTFRNUM] = true;
		frame->element_data[elementSet][NXDN_BLKNUM] = *ucptr & 0x0f;
		frame->element_data_valid[elementSet][NXDN_BLKNUM] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_DATASTART] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATASTART] = true;
		if((frame->element_data_valid[0][LICH_CHTYPE] && (frame->element_data[0][LICH_CHTYPE] == 0) && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1)) ||
		   (!frame->element_data_valid[2][NXDN_AREA] && frame->element_data_valid[0][LICH_FNCHTYPE] && (frame->element_data[0][LICH_FNCHTYPE] == 1)))
			ucptr += 14;
		else
			ucptr += 8;
		frame->element_data[elementSet][NXDN_DATAEND] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATAEND] = true;
        break;
    case 58: // SDCALL_IV
		frame->element_data[elementSet][NXDN_IVH] = frame->element_data[elementSet][NXDN_IVL] = 0;
		for ( i = 0; i < 4; i++)
		{
			frame->element_data[elementSet][NXDN_IVH] << 8;
			frame->element_data[elementSet][NXDN_IVH] |= *(ucptr+i);
		}
		frame->element_data_valid[elementSet][NXDN_IVH] = true;
		for ( i = 4; i < 8; i++)
		{
			frame->element_data[elementSet][NXDN_IVL] << 8;
			frame->element_data[elementSet][NXDN_IVL] |= *(ucptr+i);
		}
		frame->element_data_valid[elementSet][NXDN_IVL] = true;
		ucptr += 8;
        break;
    case 59: // SDCALL_RESP
		frame->element_data[elementSet][NXDN_CCOPTION] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CCOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_CALLTYPE] = (*ucptr >> 5) & 0x07;
		frame->element_data_valid[elementSet][NXDN_CALLTYPE] = true;
		frame->element_data[elementSet][NXDN_DCALLOPTION] = *ucptr & 0x1f;
		frame->element_data_valid[elementSet][NXDN_DCALLOPTION] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_SRCID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_SRCID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_DESTID] = (*ucptr << 8) | *(ucptr+1);
		frame->element_data_valid[elementSet][NXDN_DESTID] = true;
		ucptr += 2;
		frame->element_data[elementSet][NXDN_CAUSESS] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_CAUSESS] = true;
		++ucptr;
		if(!frame->element_data_valid[0][NXDN_RAN]) // IDAS
		{
			frame->element_data[elementSet][NXDN_ERRBLKFLAG] = (*ucptr << 8) | *(ucptr+1);
			frame->element_data_valid[elementSet][NXDN_ERRBLKFLAG] = true;
			ucptr += 2;
		}
		else if(frame->element_data[elementSet][NXDN_CCOPTION] & 0x40)
		{
			frame->element_data[elementSet][NXDN_LOCIDOPTION] = (*ucptr >> 3) & 0x1f;
			frame->element_data_valid[elementSet][NXDN_LOCIDOPTION] = true;
			frame->element_data[elementSet][NXDN_LOCID] = ((*ucptr << 21) | (*(ucptr+1) << 13) | (*(ucptr+2) << 5)) & 0xffffff;
			frame->element_data_valid[elementSet][NXDN_LOCID] = true;
			ucptr += 3;
		}
        break;
    case 63: // PROP_FORM
		frame->element_data[elementSet][NXDN_MANNUM] = *ucptr;
		frame->element_data_valid[elementSet][NXDN_MANNUM] = true;
		++ucptr;
		frame->element_data[elementSet][NXDN_DATASTART] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATASTART] = true;
		frame->element_data[elementSet][NXDN_DATAEND] = (slowFlag)?(frame->raw_sframe_data_count):(frame->raw_frame_data_count);
		frame->element_data_valid[elementSet][NXDN_DATAEND] = true;
        break;
    default: // Unknown message
		frame->element_data[elementSet][NXDN_DATASTART] = (slowFlag)?(ucptr - frame->raw_sframe_data):(ucptr - frame->raw_frame_data);
		frame->element_data_valid[elementSet][NXDN_DATASTART] = true;
		frame->element_data[elementSet][NXDN_DATAEND] = (slowFlag)?(frame->raw_sframe_data_count):(frame->raw_frame_data_count);
		frame->element_data_valid[elementSet][NXDN_DATAEND] = true;
        break;
		
	}
}