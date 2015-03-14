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

#ifndef NDXN_H
#define NDXN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dsd.h"
#include "nxdn_const.h"

 typedef enum {
    LICH_CHTYPE = 0,
    LICH_FNCHTYPE,
    LICH_CHOPTION,
    LICH_DIRECTION,
    NXDN_SF,    // IDAS Structure
    NXDN_RAN,
    NXDN_MSGSTART,
    NXDN_MSGTYPE,
    NXDN_CCOPTION,
    NXDN_CALLTYPE,
    NXDN_CALLTIMER,
    NXDN_SRCID,
    NXDN_DESTID,
    NXDN_TEMPID,
    NXDN_VISITID,
    NXDN_VISITGRP,
    NXDN_CHAN,
    NXDN_CCHAN1,
    NXDN_CCHAN2,
    NXDN_CIPHERTYPE,
    NXDN_KEYID,
    NXDN_VCALLOPTION,
    NXDN_DCALLOPTION,
    NXDN_SCALLOPTION,
    NXDN_PACKETINFO,
    NXDN_PKTFRNUM,
    NXDN_BLKNUM,
    NXDN_HDRDELAY,
    NXDN_RESPINFO,
    NXDN_ERRBLKFLAG,
    NXDN_CAUSEVD,
    NXDN_CAUSEDISC,
    NXDN_CAUSEMM,
    NXDN_CAUSESS,
    NXDN_REGOPTION,
    NXDN_GRPREGOPTION,
    NXDN_AUTHOPTION,
    NXDN_AUTHPARM,
    NXDN_LOCIDOPTION, // IDAS - Syatem ID Option
    NXDN_LOCID,       // IDAS - System ID
    NXDN_SITEID,
    NXDN_CHANSTRUCT,
    NXDN_SRVINFO,
    NXDN_RESTINFO,
    NXDN_CHANACCESS,
    NXDN_VERSION,
    NXDN_ADJSITECNT,
    NXDN_ADJSITELOCID1,
    NXDN_ADJSITEOPTION1,
    NXDN_ADJSITECCHAN1,   // IDAS- Site Code
    NXDN_ADJSITELOCID2,
    NXDN_ADJSITEOPTION2,
    NXDN_ADJSITECCHAN2,
    NXDN_ADJSITELOCID3,
    NXDN_ADJSITEOPTION3,
    NXDN_ADJSITECCHAN3,
    NXDN_ADJSITELOCID4,
    NXDN_ADJSITEOPTION4,
    NXDN_ADJSITECCHAN4,
    NXDN_CCHANINFO,
    NXDN_IVH,
    NXDN_IVL,
    NXDN_MANNUM,
    NXDN_FAILCODE,
    NXDN_STATUS,
    NXDN_GUFLAG,
    NXDN_DELIVERY,
    NXDN_CTLCMD,
    NXDN_CTLPARM,
    NXDN_TXPERMIT,
    // IDAS
    NXDN_AREA,
    NXDN_SITETYPE,
    NXDN_RPTRSTATE,  // 1 = busy 2 free 4 = EOT 5 = Idle
    NXDN_SPECIALID,  // Special ID
    NXDN_BUSYRPTR, // Repeater in Use or Goto Repeater
    NXDN_FREERPTR1,
    NXDN_FREERPTR2,
    NXDN_CALLOPTION,
    NXDN_DWOPTION,
    NXDN_HOMERPTR1,
    NXDN_HOMERPTR2,
    NXDN_DATASTART,
    // Last Entry
    NXDN_DATAEND,
} nxdn_element_type;

// NXDN ECC
void initCRC6();
unsigned char getCRC6();
void doCRC6(char bit);

void initCRC7();
unsigned char getCRC7();
void doCRC7(char bit);

void initCRC12();
unsigned short getCRC12();
void doCRC12(char bit);

void initNxdnCRC15();
unsigned short getNxdnCRC15();
void doNxdnCRC15(char bit);

void initNxdnCRC16();
unsigned short getNxdnCRC16();
void doNxdnCRC16(char bit);

// NXDN Frames
void processNXDNCac (dsd_opts * opts, dsd_state * state, dsd_frame * frame, unsigned char bits[300]);
void processNXDNFacch1 (dsd_opts * opts, dsd_state * state, dsd_frame * frame, char bits[300]);
void processNXDNFacch2 (dsd_opts * opts, dsd_state * state, dsd_frame * frame, unsigned char bits[348]);
void processNXDNFacch3 (dsd_opts * opts, dsd_state * state, dsd_frame * frame, unsigned char bits[300]);
void processNXDNSacch (dsd_opts * opts, dsd_state * state, dsd_frame * frame, unsigned char bits[300]);

// NXDN Message
void processNXDNMessage (dsd_opts * opts, dsd_state * state, dsd_frame * frame, int offset, bool dualFlag, bool slowFlag);

// NXDN Elements
bool NxdnHasGrpID(dsd_frame * frame, int set);
bool NxdnHasSrcSysID(dsd_frame * frame, int set);
bool NxdnHasDestSysID(dsd_frame * frame, int set);
bool NxdnLICH(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnRAN(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnMessageType(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnRegOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnGrpRegOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnAuthOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnDWOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCCOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCallType(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnSystemID(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnSiteCode(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnServiceInformation(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnSrcID(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnDestID(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnTempID(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnVistorID(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnGrpID(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnVisitorGrpID(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnChannel(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCChannel(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCipherType(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnRptrState(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnBusyRptr(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnFreeRptr(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnVCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnDCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnSCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnStatus(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCauseMM(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCauseVD(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCauseSS(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCauseDisc(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnAdjsiteCount(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnAdjSites(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnCTimer(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnPFNumber(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnBNumber(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnHdrDelay(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnVersion(dsd_frame * frame, int set, dsd_display_format format, char *outStr);
bool NxdnManNum(dsd_frame * frame, int set, dsd_display_format format, char *outStr);

#ifdef __cplusplus
}
#endif

#endif // NXDN_H