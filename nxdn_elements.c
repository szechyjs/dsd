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

static const char *unknown[2] = {"U?", "Unknown"};
static const char nullstr[] = "";
static const char blank1[] = " ";
static const char blank2[] = "  ";
static const char blank3[] = "   ";

// MSG TYPE
static const char callresp[] = "CALL_RESP";
static const char callreq[] = "CALL_REQ";
static const char vcallreq[] = "VCALL_REQ";
static const char vcallresp[] = "VCALL_RESP";
static const char vcall[] = "VCALL";
static const char vcallrecresp[] = "VCALL_REC_RESP";
static const char vcallrecreq[] = "VCALL_REC_REQ";
static const char vcallconnreq[] = "VCALL_CONN_REQ";
static const char vcallconnresp[] = "VCALL_CONN_RESP";
static const char vcalliv[] = "VCALL_IV";
static const char vcallassgn[] = "VCALL_ASSGN";
static const char vcallassgndup[] = "VCALL_ASSGN_DUP";
static const char callconnresp[] = "CALL_CONN_RESP";
static const char txrelex[] = "TX_REL_EX";
static const char txrel[] = "TX_REL";
static const char dcallreq[] = "DCALL_REQ";
static const char dcallresp[] = "DCALL_RESP";
static const char dcallhdr[] = "DCALL Hdr";
static const char dcallrecresp[] = "DCALL_REC_RESP";
static const char dcallrecreq[] = "DCALL_REC_REQ";
static const char dcalldata[] = "DCALL Data";
static const char dcallack[] = "DCALL_ACK";
static const char dcallassgndup[] = "DCALL_ASSGN_DUP";
static const char dcallassgn[] = "DCALL_ASSGN";
static const char headdly[] = "HEAD_DLY";
static const char idle[] = "IDLE";
static const char disc[] = "DISC";
static const char discreq[] = "DISC_REQ";
static const char siteinfo[] = "SITE_INFO";
static const char srvinfo[] = "SRV_INFO";
static const char cchinfo[] = "CCH_INFO";
static const char adjsiteinfo[] = "ADJ_SITE_INFO";
static const char failstatinfo[] = "FAIL_STAT_INFO";
static const char regresp[] = "REG_RESP";
static const char regreq[] = "REG_REQ";
static const char regcresp[] = "REG_C_RESP";
static const char regcreq[] = "REG_C_REQ";
static const char regcomm[] = "REG_COMM";
static const char grpregresp[] = "GRP_REG_RESP";
static const char grpregreq[] = "GRP_REG_REQ";
static const char authinqreq[] = "AUTH_INQ_REQ";
static const char authinqresp[] = "AUTH_INQ_RESP";
static const char dwrhdr[] = "DWR Hdr";
static const char dwrsysdata[] = "DWR Data";
static const char dwrack[] = "DWR_ACK";
static const char statinqreq[] = "STAT_INQ_REQ";
static const char statinqresp[] = "STAT_INQ_RESP";
static const char statreq[] = "STAT_REQ";
static const char statresp[] = "STAT_RESP";
static const char remconreq[] = "REM_CON_REQ";
static const char remconresp[] = "REM_CON_RESP";
static const char sdcallreqhdr[] = "SDCALL_REQ Hdr";
static const char sdcallreqdata[] = "SDCALL_REQ Data";
static const char sdcalliv[] = "SDCALL_IV";
static const char sdcallresp[] = "SDCALL_RESP";
static const char propform[] = "PROP_FORM";

typedef struct {
bool validsys[3];
bool validin[4];
const char *indesc[4];
bool validout[4];
const char *outdesc[4];
} msgtype;

msgtype msgtypes[64] =
{
// System Valid             Input Valid                     Input Strings                                          Output Valid                   Output Strings
// Conv  Type-C Type D      RCCH   RTCH   RDCH   RTCH_C     RCCH           RTCH           RDCH           RTCH_C             RCCH   RTCH   RDCH   RTCH_C     RCCH           RTCH           RDCH           RTCH_C
{{ false, false,  true }, { false, false, false,  true }, { nullstr,       nullstr,       nullstr,       callreq       }, { false, false, false,  true }, { nullstr,       nullstr,       nullstr,       callresp      }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { vcallreq,      vcall,         vcall,         vcall         }, {  true,  true,  true,  true }, { vcallresp,     vcall,         vcall,         vcall         }},
{{  true,  true, false }, {  true, false, false, false }, { vcallrecresp,  nullstr,       nullstr,       nullstr       }, {  true,  true,  true, false }, { vcallrecreq,   vcallrecreq,   vcallrecreq,   nullstr       }},
{{  true,  true, false }, {  true,  true,  true, false }, { vcallconnreq,  vcalliv,       vcalliv,       nullstr       }, {  true,  true,  true, false }, { vcallconnresp, vcalliv,       vcalliv,       nullstr       }},
{{  true,  true,  true }, { false, false, false,  true }, { nullstr,       nullstr,       nullstr,       vcallreq      }, {  true,  true,  true,  true }, { vcallassgn,    vcallassgn,    vcallassgn,    vcallassgn    }},
{{  true,  true, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true,  true,  true, false }, { vcallassgndup, vcallassgndup, vcallassgndup, nullstr       }},
{{ false, false,  true }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false,  true }, { nullstr,       nullstr,       nullstr,       callconnresp  }},
{{  true,  true,  true }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false,  true,  true,  true }, { nullstr,       txrelex,       txrelex,       txrelex       }},

{{  true,  true,  true }, { false,  true,  true,  true }, { nullstr,       txrel,         txrel,         txrel         }, { false,  true,  true,  true }, { nullstr,       txrel,         txrel,         txrel         }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { dcallreq,      dcallhdr,      dcallhdr,      dcallhdr      }, {  true,  true,  true,  true }, { dcallresp,     dcallhdr,      dcallhdr,      dcallhdr      }},
{{  true,  true, false }, {  true, false, false, false }, { dcallrecresp,  nullstr,       nullstr,       nullstr       }, {  true,  true,  true, false }, { dcallrecreq,   dcallrecreq,   dcallrecreq,   nullstr       }},
{{  true,  true,  true }, { false,  true,  true,  true }, { nullstr,       dcalldata,     dcalldata,     dcalldata     }, { false,  true,  true,  true }, { nullstr,       dcalldata,     dcalldata,     dcalldata     }},
{{  true,  true,  true }, { false,  true,  true,  true }, { nullstr,       dcallack,      dcallack,      dcallack      }, { false,  true,  true,  true }, { nullstr,       dcallack,      dcallack,      dcallack      }},
{{  true,  true, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true,  true,  true, false }, { dcallassgndup, dcallassgndup, dcallassgndup, nullstr       }},
{{  true,  true, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true,  true,  true, false }, { dcallassgn,    dcallassgn,    dcallassgn,    nullstr       }},
{{  true,  true,  true }, { false,  true,  true,  true }, { nullstr,       headdly,       headdly,       headdly       }, { false,  true,  true,  true }, { nullstr,       headdly,       headdly,       headdly       }},

{{  true,  true,  true }, { false,  true,  true,  true }, { nullstr,       idle,          idle,          idle          }, {  true,  true,  true,  true }, { idle,          idle,          idle,          idle          }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { discreq,       discreq,       discreq,       discreq       }, {  true,  true,  true,  true }, { disc,          disc,          disc,          disc          }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},

{{ false,  true, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true, false, false, false }, { siteinfo,      nullstr,       nullstr,       nullstr       }},
{{  true,  true,  true }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true,  true,  true,  true }, { srvinfo,       srvinfo,       srvinfo,       srvinfo       }},
{{  true,  true, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true,  true,  true, false }, { cchinfo,       cchinfo,       cchinfo,       nullstr       }},
{{  true,  true,  true }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true,  true,  true,  true }, { adjsiteinfo,   adjsiteinfo,   adjsiteinfo,   adjsiteinfo   }},
{{  true,  true, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false,  true,  true, false }, { nullstr,       failstatinfo,  failstatinfo,  nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},

{{ false,  true,  true }, {  true, false, false,  true }, { regreq,        nullstr,       nullstr,       regreq        }, {  true, false, false,  true }, { regresp,       nullstr,       nullstr,       regresp       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false,  true,  true }, {  true, false, false,  true }, { regcreq,       nullstr,       nullstr,       regcreq       }, {  true, false, false,  true }, { regcresp,      nullstr,       nullstr,       regcresp      }},
{{ false,  true, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, {  true, false, false, false }, { regcomm,       nullstr,       nullstr,       nullstr       }},
{{ false,  true,  true }, {  true, false, false,  true }, { grpregreq,     nullstr,       nullstr,       grpregreq     }, {  true, false, false,  true }, { grpregresp,    nullstr,       nullstr,       grpregresp    }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},

{{  true,  true,  true }, {  true,  true,  true,  true }, { authinqreq,    authinqreq,    authinqreq,    authinqreq    }, {  true,  true,  true,  true }, { authinqreq,    authinqreq,    authinqreq,    authinqreq    }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { authinqresp,   authinqresp,   authinqresp,   authinqresp   }, {  true,  true,  true,  true }, { authinqresp,   authinqresp,   authinqresp,   authinqresp   }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false,  true }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false,  true }, { nullstr,       nullstr,       nullstr,       dwrhdr        }},
{{ false, false,  true }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false,  true }, { nullstr,       nullstr,       nullstr,       dwrsysdata    }},
{{ false, false,  true }, { false, false, false,  true }, { nullstr,       nullstr,       nullstr,       dwrack        }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},

{{  true,  true,  true }, {  true,  true,  true,  true }, { statinqreq,    statinqreq,    statinqreq,    statinqreq    }, {  true,  true,  true,  true }, { statinqreq,    statinqreq,    statinqreq,    statinqreq    }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { statinqresp,   statinqresp,   statinqresp,   statinqresp   }, {  true,  true,  true,  true }, { statinqresp,   statinqresp,   statinqresp,   statinqresp   }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { statreq,       statreq,       statreq,       statreq       }, {  true,  true,  true,  true }, { statreq,       statreq,       statreq,       statreq       }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { statresp,      statresp,      statresp,      statresp      }, {  true,  true,  true,  true }, { statresp,      statresp,      statresp,      statresp      }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { remconreq,     remconreq,     remconreq,     remconreq     }, {  true,  true,  true,  true }, { remconreq,     remconreq,     remconreq,     remconreq     }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { remconresp,    remconresp,    remconresp,    remconresp    }, {  true,  true,  true,  true }, { remconresp,    remconresp,    remconresp,    remconresp    }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},

{{  true,  true,  true }, {  true,  true,  true,  true }, { sdcallreqhdr,  sdcallreqhdr,  sdcallreqhdr,  sdcallreqhdr  }, {  true,  true,  true,  true }, { sdcallreqhdr,  sdcallreqhdr,  sdcallreqhdr,  sdcallreqhdr  }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { sdcallreqdata, sdcallreqdata, sdcallreqdata, sdcallreqdata }, {  true,  true,  true,  true }, { sdcallreqdata, sdcallreqdata, sdcallreqdata, sdcallreqdata }},
{{  true,  true, false }, {  true,  true,  true, false }, { sdcalliv,      sdcalliv,      sdcalliv,      nullstr       }, {  true,  true,  true, false }, { sdcalliv,      sdcalliv,      sdcalliv,      nullstr       }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { sdcallresp,    sdcallresp,    sdcallresp,    sdcallresp    }, {  true,  true,  true,  true }, { sdcallresp,    sdcallresp,    sdcallresp,    sdcallresp    }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{ false, false, false }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }, { false, false, false, false }, { nullstr,       nullstr,       nullstr,       nullstr       }},
{{  true,  true,  true }, {  true,  true,  true,  true }, { propform,      propform,      propform,      propform      }, {  true,  true,  true,  true }, { propform,      propform,      propform,      propform      }},
};

// CC Option
static const char *emergency[2] = {"E! ", "Emergency! "};
static const char *intersystem[2] = {"IS ", "Inter-System "};
static const char *priority[2] = {"Pr ", "Priority "};
static const char *t_type[2] = {"MS ", "Mult-System "};

static long NxdnUnknown(long value, dsd_display_format format, char *outStr)
{
	switch (format)
	{
	case DISP_FMT_BRIEF:
	case DISP_FMT_VERBOSE:
		sprintf(outStr,"%s:%ld", unknown[format], value);
		break;
	default:
		sprintf(outStr,"%ld", value );
	
	}
	return value;
}

bool NxdnHasGrpID(dsd_frame * frame, int set)
{
	if((frame->element_data_valid[set][NXDN_CALLTYPE] && (frame->element_data[set][NXDN_CALLTYPE] < 2)) ||
       (frame->element_data_valid[set][NXDN_REGOPTION]) || (frame->element_data_valid[set][NXDN_GRPREGOPTION]) ||
       (frame->element_data_valid[set][NXDN_GUFLAG] && (frame->element_data[set][NXDN_GUFLAG] == 0)))
		return true;
	return false;
}
bool NxdnHasSrcSysID(dsd_frame * frame, int set)
{
	if(frame->element_data_valid[set][NXDN_LOCIDOPTION] && ((frame->element_data[set][NXDN_LOCIDOPTION] & 0x18) == 0x10))
		return true;
	return false;
}
bool NxdnHasDestSysID(dsd_frame * frame, int set)
{
	if(frame->element_data_valid[set][NXDN_LOCIDOPTION] && ((frame->element_data[set][NXDN_LOCIDOPTION] & 0x18) == 0x08))
		return true;
	return false;
}
bool
NxdnLICH(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
			sprintf(outStr, "T-C Ctl%s", (frame->element_data[0][LICH_DIRECTION])?"->":"<-");
			return true;
		case 1:
			sprintf(outStr, "T-C Tfc%s", (frame->element_data[0][LICH_DIRECTION])?"->":"<-");
			return true;
		case 2:
			sprintf(outStr, "%s", (frame->element_data[0][LICH_DIRECTION])?"Rptr->":"Dir/Rptr<-");
			return true;
		case 3:
			if(frame->element_data_valid[0][NXDN_RAN]) // Type-C single
			{
				sprintf(outStr, "T-C Cmp%s", (frame->element_data[0][LICH_DIRECTION])?"->":"<-");
			}
			else
			{
				sprintf(outStr, "T-D Cmp%s", (frame->element_data[0][LICH_DIRECTION])?"->":"<-");
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool
NxdnRAN(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_RAN] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		switch (frame->element_data[set][NXDN_RAN])
		{
		case 0:
			if(frame->element_data[0][LICH_CHTYPE] == 2) // Zero for CRS means no RAN
			{
				*outStr = '\0';
				return false;
			}
		/* Fall Through */
		default:
			sprintf(outStr,"RAN:%ld", frame->element_data[set][NXDN_RAN]);
	
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool
NxdnMessageType(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	const char *ccptr;
	if(frame->element_data_valid[set][NXDN_MSGTYPE] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		msgtype *mtptr = &msgtypes[frame->element_data[set][NXDN_MSGTYPE]];
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			if(frame->element_data[0][LICH_DIRECTION]) // Outbound
			{
				if(mtptr->validout[frame->element_data[0][LICH_CHTYPE]])
				{
					sprintf(outStr,"%s", mtptr->outdesc[frame->element_data[0][LICH_CHTYPE]]);
					return true;
				}
			}
			else
			{
				if(mtptr->validin[frame->element_data[0][LICH_CHTYPE]])
				{
					sprintf(outStr,"%s", mtptr->indesc[frame->element_data[0][LICH_CHTYPE]]);
					return true;
				}
			}
			break;
		default:
			break;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnRegOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_REGOPTION] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			if(frame->element_data[set][NXDN_REGOPTION] & 0x18)
			{
				sprintf(outStr, "%s%s",(frame->element_data[set][NXDN_REGOPTION] & 0x10)?emergency[format]:nullstr,(frame->element_data[set][NXDN_REGOPTION] & 0x08)?intersystem[format]:nullstr);
				return true;
			}
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnGrpRegOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_GRPREGOPTION] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			if(frame->element_data[set][NXDN_GRPREGOPTION] & 0xC0)
			{
				sprintf(outStr, "%s%s",(frame->element_data[set][NXDN_GRPREGOPTION] & 0x80)?emergency[format]:nullstr,(frame->element_data[set][NXDN_GRPREGOPTION] & 0x40)?intersystem[format]:nullstr);
				return true;
			}
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnAuthOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_AUTHOPTION] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			if(frame->element_data[set][NXDN_AUTHOPTION] & 0xB0)
			{
				sprintf(outStr, "%s%s%s",(frame->element_data[set][NXDN_AUTHOPTION] & 0x80)?emergency[format]:nullstr,(frame->element_data[set][NXDN_AUTHOPTION] & 0x20)?intersystem[format]:nullstr,(frame->element_data[set][NXDN_AUTHOPTION] & 0x10)?t_type[format]:nullstr);
				return true;
			}
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnDWOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_DWOPTION] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			if(frame->element_data[set][NXDN_DWOPTION] & 0x80)
			{
				sprintf(outStr, "%s",(frame->element_data[set][NXDN_DWOPTION] & 0x80)?emergency[format]:nullstr);
				return true;
			}
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnCCOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CCOPTION] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			if(frame->element_data[set][NXDN_CCOPTION] & 0xF0)
			{
				sprintf(outStr, "%s%s%s%s",(frame->element_data[set][NXDN_CCOPTION] & 0x80)?emergency[format]:nullstr,(frame->element_data[set][NXDN_CCOPTION] & 0x40)?intersystem[format]:nullstr,(frame->element_data[set][NXDN_CCOPTION] & 0x20)?priority[format]:nullstr,(frame->element_data[set][NXDN_CCOPTION] & 0x10)?t_type[format]:nullstr);
				return true;
			}
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CALLOPTION] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 3:
			if(frame->element_data[set][NXDN_CALLOPTION] & 0x07)
			{
				sprintf(outStr, "%s%s%s",(frame->element_data[set][NXDN_CALLOPTION] & 0x04)?emergency[format]:nullstr,(frame->element_data[set][NXDN_CALLOPTION] & 0x01)?priority[format]:nullstr,(frame->element_data[set][NXDN_CALLOPTION] & 0x02)?t_type[format]:nullstr);
				return true;
			}
		default:
			break;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnCallType(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CALLTYPE] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			switch(frame->element_data[set][NXDN_CALLTYPE])
			{
			case 0:
				sprintf(outStr,"AC");
				break;
			case 1:
				sprintf(outStr,"GC");
				break;
			case 2:
				sprintf(outStr,"_C");
				break;
			case 3:
				sprintf(outStr,"SC");
				break;
			case 4:
				sprintf(outStr,"IC");
				break;
			case 6:
				sprintf(outStr,"PC");
				break;
			case 7:
				sprintf(outStr,"SC");
				break;
			default:
				sprintf(outStr,"R?:%ld",frame->element_data[set][NXDN_CALLTYPE]);
				break;
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnSystemID(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	const char *ccptr;
	if(frame->element_data_valid[set][NXDN_LOCID] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		long l = frame->element_data[set][NXDN_LOCID];
		long sysid;
		long site;
		char c;
		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			switch((l >> 22) & 0x3)
			{
			case 0:
				sysid = (l >> 12) & 0x3ff;
				site = l & 0xfff;
			    c = 'G';
				break;
			case 2:
				sysid = (l >> 8) & 0x3fff;
				site = l & 0xff;
				c = 'R';
				break;
			case 1:
				sysid = (l >> 5) & 0x1ffff;
				site = l & 0x1f;
				c = 'L';
				break;
			default:
				sprintf(outStr,"Sys:R?(%ld)", l);
				return true;
				break;
			}
			if(sysid > 0L)
			{
				if(site > 0L)
					sprintf(outStr,"Sys:%c%ld Site:%ld", c, sysid, site);
				else
					sprintf(outStr,"Sys:%c%ld", c, sysid);
				
				return true;
			}
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
			if(l == 0)
				sprintf(outStr,"Sys:None");
			else
			{
				sprintf(outStr,"Sys:%ld-%ld", (l >> 15) & 0xf, l & 0x7fff);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnSiteCode(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_SITEID] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		if(frame->element_data[set][NXDN_SITEID] > 0)
		{
			sprintf(outStr,"Site:%ld", frame->element_data[set][NXDN_SITEID]);
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnServiceInformation(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	const char *ccptr;
	if(frame->element_data_valid[set][NXDN_SRVINFO] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		switch(frame->element_data[0][LICH_CHTYPE])
		{
		case 0:
		case 1:
		case 2:
		case 3:
			if(frame->element_data[0][LICH_DIRECTION] && (frame->element_data[set][NXDN_SRVINFO] & 0xFFF0)) // Outbound
			{
				sprintf(outStr, "%s%s%s%s%s%s%s%s%s%s%s%s", (frame->element_data[set][NXDN_SRVINFO] & 0x8000)?"MS ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x4000)?"IS ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x2000)?"LR ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x1000)?"GR ":nullstr,
				                                                        (frame->element_data[set][NXDN_SRVINFO] & 0x0800)?"Au ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x0400)?"CC ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x0200)?"VC ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x0100)?"DC ":nullstr,
				                                                        (frame->element_data[set][NXDN_SRVINFO] & 0x0080)?"SD ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x0040)?"SR ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x0020)?"PN ":nullstr,(frame->element_data[set][NXDN_SRVINFO] & 0x0010)?"IN ":nullstr);
				return true;
			}
			break;
		default:
			break;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnSrcID(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_SRCID] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			switch (frame->element_data[set][NXDN_SRCID])
			{
			case 0: // Null Group
				*outStr = '\0';
				return false;
				break;
			case 0xfff0:
				sprintf(outStr,"SI:T-TC");
				break;
			case 0xfff1:
				sprintf(outStr,"SI:T-PSTN");
				break;
			case 0xfff2:
				sprintf(outStr,"SI:Sp02");
				break;
			case 0xfff3:
				sprintf(outStr,"SI:Sp03");
				break;
			case 0xfff4:
				sprintf(outStr,"SI:Sp04");
				break;
			case 0xfff5:
				sprintf(outStr,"SI:C-PSTN");
				break;
			case 0xfff6:
				sprintf(outStr,"SI:Sp06");
				break;
			case 0xfff7:
				sprintf(outStr,"SI:Sp07");
				break;
			case 0xfff8:
				sprintf(outStr,"SI:Sp08");
				break;
			case 0xfff9:
				sprintf(outStr,"SI:Sp09");
				break;
			case 0xfffa:
				sprintf(outStr,"SI:Sp10");
				break;
			case 0xfffb:
				sprintf(outStr,"SI:Sp11");
				break;
			case 0xfffc:
				sprintf(outStr,"SI:Sp12");
				break;
			case 0xfffd:
				sprintf(outStr,"SI:Sp13");
				break;
			case 0xfffe:
				sprintf(outStr,"SI:Sp14");
				break;
			case 0xffff:
				sprintf(outStr,"SI:All");
				break;
			default:
				sprintf(outStr,"SI:%ld", frame->element_data[set][NXDN_SRCID]);
		
			}
			return true;
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
		
		    int prefix = (frame->element_data[set][NXDN_SRCID] >> 11) & 0x1f;
			int id = frame->element_data[set][NXDN_SRCID] & 0x7ff;
			switch (id)
			{
			case 0: // Null Group
			case 2041:
			case 2044:
			case 2045:
			case 2046:
				*outStr = '\0';
				return false;
				break;
			case 2042:
				sprintf(outStr,"SI:%d-PSTN", prefix);
				break;
			case 2043:
				sprintf(outStr,"SI:%d-TC", prefix);
				break;
			case 2048:
				sprintf(outStr,"SI:%d-All", prefix);
				break;
			default:
				sprintf(outStr,"SI:%d-%s%d", prefix, (id > 2000)?"Sp":"", (id > 2000)?id-2000:id);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnDestID(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	
	if(frame->element_data_valid[set][NXDN_DESTID] && !NxdnHasGrpID(frame, set) && frame->element_data_valid[0][LICH_CHTYPE])
	{
		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			switch (frame->element_data[set][NXDN_DESTID])
			{
			case 0: // Null Group
				*outStr = '\0';
				return false;
				break;
			case 0xfff0:
				sprintf(outStr,"DI:T-TC");
				break;
			case 0xfff1:
				sprintf(outStr,"DI:T-PSTN");
				break;
			case 0xfff2:
				sprintf(outStr,"DI:Sp02");
				break;
			case 0xfff3:
				sprintf(outStr,"DI:Sp03");
				break;
			case 0xfff4:
				sprintf(outStr,"DI:Sp04");
				break;
			case 0xfff5:
				sprintf(outStr,"DI:C-PSTN");
				break;
			case 0xfff6:
				sprintf(outStr,"DI:Sp06");
				break;
			case 0xfff7:
				sprintf(outStr,"DI:Sp07");
				break;
			case 0xfff8:
				sprintf(outStr,"DI:Sp08");
				break;
			case 0xfff9:
				sprintf(outStr,"DI:Sp09");
				break;
			case 0xfffa:
				sprintf(outStr,"DI:Sp10");
				break;
			case 0xfffb:
				sprintf(outStr,"DI:Sp11");
				break;
			case 0xfffc:
				sprintf(outStr,"DI:Sp12");
				break;
			case 0xfffd:
				sprintf(outStr,"DI:Sp13");
				break;
			case 0xfffe:
				sprintf(outStr,"DI:Sp15");
				break;
			case 0xffff:
				sprintf(outStr,"DI:All");
				break;
			default:
				sprintf(outStr,"DI:%ld", frame->element_data[set][NXDN_DESTID]);
		
			}
			return true;
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
		
		    int prefix = (frame->element_data[set][NXDN_DESTID] >> 11) & 0x1f;
			int id = frame->element_data[set][NXDN_DESTID] & 0x7ff;
			switch (id)
			{
			case 0: // Null Group
			case 2041:
			case 2044:
			case 2045:
			case 2046:
				*outStr = '\0';
				return false;
				break;
			case 2042:
				sprintf(outStr,"DI:%d-PSTN", prefix);
				break;
			case 2043:
				sprintf(outStr,"DI:%d-TC", prefix);
				break;
			case 2048:
				sprintf(outStr,"DI:%d-All", prefix);
				break;
			default:
				sprintf(outStr,"DI:%d-%s%d", prefix, (id > 2000)?"Sp":"", (id > 2000)?id-2000:id);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnTempID(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_TEMPID] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			switch (frame->element_data[set][NXDN_TEMPID])
			{
			case 0: // Null Group
				*outStr = '\0';
				return false;
				break;
			case 0xfff0:
				sprintf(outStr,"TI:T-TC");
				break;
			case 0xfff1:
				sprintf(outStr,"TI:T-PSTN");
				break;
			case 0xfff2:
				sprintf(outStr,"TI:Sp02");
				break;
			case 0xfff3:
				sprintf(outStr,"TI:Sp03");
				break;
			case 0xfff4:
				sprintf(outStr,"TI:Sp04");
				break;
			case 0xfff5:
				sprintf(outStr,"TI:C-PSTN");
				break;
			case 0xfff6:
				sprintf(outStr,"TI:Sp06");
				break;
			case 0xfff7:
				sprintf(outStr,"TI:Sp07");
				break;
			case 0xfff8:
				sprintf(outStr,"TI:Sp08");
				break;
			case 0xfff9:
				sprintf(outStr,"TI:Sp09");
				break;
			case 0xfffa:
				sprintf(outStr,"TI:Sp10");
				break;
			case 0xfffb:
				sprintf(outStr,"TI:Sp11");
				break;
			case 0xfffc:
				sprintf(outStr,"TI:Sp12");
				break;
			case 0xfffd:
				sprintf(outStr,"TI:Sp13");
				break;
			case 0xfffe:
				sprintf(outStr,"TI:Sp14");
				break;
			case 0xffff:
				sprintf(outStr,"TI:All");
				break;
			default:
				sprintf(outStr,"TI:%ld", frame->element_data[set][NXDN_TEMPID]);
		
			}
			return true;
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
		
		    int prefix = (frame->element_data[set][NXDN_TEMPID] >> 11) & 0x1f;
			int id = frame->element_data[set][NXDN_TEMPID] & 0x7ff;
			switch (id)
			{
			case 0: // Null Group
			case 2041:
			case 2044:
			case 2045:
			case 2046:
				*outStr = '\0';
				return false;
				break;
			case 2042:
				sprintf(outStr,"TI:%d-PSTN", prefix);
				break;
			case 2043:
				sprintf(outStr,"TI:%d-TC", prefix);
				break;
			case 2048:
				sprintf(outStr,"TI:%d-All", prefix);
				break;
			default:
				sprintf(outStr,"TI:%d-%s%d", prefix, (id > 2000)?"Sp":"", (id > 2000)?id-2000:id);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnVistorID(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_VISITID] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			switch (frame->element_data[set][NXDN_VISITID])
			{
			case 0: // Null Group
				*outStr = '\0';
				return false;
				break;
			case 0xfff0:
				sprintf(outStr,"VI:T-TC");
				break;
			case 0xfff1:
				sprintf(outStr,"VI:T-PSTN");
				break;
			case 0xfff2:
				sprintf(outStr,"VI:Sp02");
				break;
			case 0xfff3:
				sprintf(outStr,"VI:Sp03");
				break;
			case 0xfff4:
				sprintf(outStr,"VI:Sp04");
				break;
			case 0xfff5:
				sprintf(outStr,"VI:C-PSTN");
				break;
			case 0xfff6:
				sprintf(outStr,"VI:Sp06");
				break;
			case 0xfff7:
				sprintf(outStr,"VI:Sp07");
				break;
			case 0xfff8:
				sprintf(outStr,"VI:Sp08");
				break;
			case 0xfff9:
				sprintf(outStr,"VI:Sp09");
				break;
			case 0xfffa:
				sprintf(outStr,"VI:Sp10");
				break;
			case 0xfffb:
				sprintf(outStr,"VI:Sp11");
				break;
			case 0xfffc:
				sprintf(outStr,"VI:Sp12");
				break;
			case 0xfffd:
				sprintf(outStr,"VI:Sp13");
				break;
			case 0xfffe:
				sprintf(outStr,"VI:Sp14");
				break;
			case 0xffff:
				sprintf(outStr,"VI:All");
				break;
			default:
				sprintf(outStr,"VI:%ld", frame->element_data[set][NXDN_VISITID]);
		
			}
			return true;
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
		
		    int prefix = (frame->element_data[set][NXDN_VISITID] >> 11) & 0x1f;
			int id = frame->element_data[set][NXDN_VISITID] & 0x7ff;
			switch (id)
			{
			case 0: // Null Group
			case 2041:
			case 2044:
			case 2045:
			case 2046:
				*outStr = '\0';
				return false;
				break;
			case 2042:
				sprintf(outStr,"VI:%d-PSTN", prefix);
				break;
			case 2043:
				sprintf(outStr,"VI:%d-TC", prefix);
				break;
			case 2048:
				sprintf(outStr,"VI:%d-All", prefix);
				break;
			default:
				sprintf(outStr,"VI:%d-%s%d", prefix, (id > 2000)?"Sp":"", (id > 2000)?id-2000:id);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnGrpID(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_DESTID] && NxdnHasGrpID(frame, set) && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			switch (frame->element_data[set][NXDN_DESTID])
			{
			case 0: // Null Group
				*outStr = '\0';
				return false;
				break;
			case 0xfff0:
				sprintf(outStr,"GI:R?");
				break;
			case 0xfff1:
				sprintf(outStr,"GI:Sp01");
				break;
			case 0xfff2:
				sprintf(outStr,"GI:Sp02");
				break;
			case 0xfff3:
				sprintf(outStr,"GI:Sp03");
				break;
			case 0xfff4:
				sprintf(outStr,"GI:Sp04");
				break;
			case 0xfff5:
				sprintf(outStr,"GI:Sp05");
				break;
			case 0xfff6:
				sprintf(outStr,"GI:Sp06");
				break;
			case 0xfff7:
				sprintf(outStr,"GI:Sp07");
				break;
			case 0xfff8:
				sprintf(outStr,"GI:Sp08");
				break;
			case 0xfff9:
				sprintf(outStr,"GI:Sp09");
				break;
			case 0xfffa:
				sprintf(outStr,"GI:Sp10");
				break;
			case 0xfffb:
				sprintf(outStr,"GI:Sp11");
				break;
			case 0xfffc:
				sprintf(outStr,"GI:Sp12");
				break;
			case 0xfffd:
				sprintf(outStr,"GI:Sp13");
				break;
			case 0xfffe:
				sprintf(outStr,"GI:Sp14");
				break;
			case 0xffff:
				sprintf(outStr,"GI:All");
				break;
			default:
				sprintf(outStr,"GI:%ld", frame->element_data[set][NXDN_DESTID]);
		
			}
			return true;
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
		
		    int prefix = (frame->element_data[set][NXDN_DESTID] >> 11) & 0x1f;
			int id = frame->element_data[set][NXDN_DESTID] & 0x7ff;
			switch (id)
			{
			case 0: // Null Group
			case 2041:
			case 2044:
			case 2045:
			case 2046:
				*outStr = '\0';
				return false;
				break;
			case 2042:
				sprintf(outStr,"GI:%d-PSTN", prefix);
				break;
			case 2043:
				sprintf(outStr,"GI:%d-TC", prefix);
				break;
			case 2047:
				sprintf(outStr,"GI:%d-All", prefix);
				break;
			default:
				sprintf(outStr,"GI:%d-%s%d", prefix, (id > 2000)?"Sp":"", (id > 2000)?id-2000:id);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnVisitorGrpID(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_VISITGRP] && frame->element_data_valid[0][LICH_CHTYPE])
	{
		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			switch (frame->element_data[set][NXDN_VISITGRP])
			{
			case 0: // Null Group
				*outStr = '\0';
				return false;
				break;
			case 0xfff0:
				sprintf(outStr,"VGI:R?");
				break;
			case 0xfff1:
				sprintf(outStr,"VGI:Sp01");
				break;
			case 0xfff2:
				sprintf(outStr,"VGI:Sp02");
				break;
			case 0xfff3:
				sprintf(outStr,"VGI:Sp03");
				break;
			case 0xfff4:
				sprintf(outStr,"VGI:Sp04");
				break;
			case 0xfff5:
				sprintf(outStr,"VGI:Sp05");
				break;
			case 0xfff6:
				sprintf(outStr,"VGI:Sp06");
				break;
			case 0xfff7:
				sprintf(outStr,"VGI:Sp07");
				break;
			case 0xfff8:
				sprintf(outStr,"VGI:Sp08");
				break;
			case 0xfff9:
				sprintf(outStr,"VGI:Sp09");
				break;
			case 0xfffa:
				sprintf(outStr,"VGI:Sp10");
				break;
			case 0xfffb:
				sprintf(outStr,"VGI:Sp11");
				break;
			case 0xfffc:
				sprintf(outStr,"VGI:Sp12");
				break;
			case 0xfffd:
				sprintf(outStr,"VGI:Sp13");
				break;
			case 0xfffe:
				sprintf(outStr,"VGI:Sp14");
				break;
			case 0xffff:
				sprintf(outStr,"VGI:All");
				break;
			default:
				sprintf(outStr,"VGI:%ld", frame->element_data[set][NXDN_VISITGRP]);
		
			}
			return true;
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
		
		    int prefix = (frame->element_data[set][NXDN_VISITGRP] >> 11) & 0x1f;
			int id = frame->element_data[set][NXDN_VISITGRP] & 0x7ff;
			switch (id)
			{
			case 0: // Null Group
			case 2041:
			case 2044:
			case 2045:
			case 2046:
				*outStr = '\0';
				return false;
				break;
			case 2042:
				sprintf(outStr,"VGI:%d-PSTN", prefix);
				break;
			case 2043:
				sprintf(outStr,"VGI:%d-TC", prefix);
				break;
			case 2047:
				sprintf(outStr,"VGI:%d-All", prefix);
				break;
			default:
				sprintf(outStr,"VGI:%d-%s%d", prefix, (id > 2000)?"Sp":"", (id > 2000)?id-2000:id);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnChannel(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CHAN] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short Chan = frame->element_data[set][NXDN_CHAN];
		switch (Chan)
		{
		case 0:
			*outStr = '\0';
			return false;
		default:
			sprintf(outStr,"Ch:%d", Chan);
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnCChannel(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short Chan1 = 0;
		short Chan2 = 0;
		if(frame->element_data_valid[set][NXDN_CCHAN1])
			Chan1 = frame->element_data[set][NXDN_CCHAN1];
		if(frame->element_data_valid[set][NXDN_CCHAN2])
			Chan2 = frame->element_data[set][NXDN_CCHAN2];
		if((Chan1 | Chan2) > 0)
		{
			if((Chan2 > 0) && (Chan2 != Chan1))
				sprintf(outStr,"CCh:[%d %d]", Chan1, Chan2);
			else	
				sprintf(outStr,"CCh:%d", Chan1);
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnCipherType(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CIPHERTYPE] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		short i = frame->element_data[set][NXDN_CIPHERTYPE];
		switch (i)
		{
		case 0:
			sprintf(outStr,"Clear");
			break;
		case 1:
			sprintf(outStr,"Scrambled");
			break;
		case 2:
			if(frame->element_data_valid[set][NXDN_KEYID])
				sprintf(outStr,"DES(%ld)", frame->element_data[set][NXDN_KEYID]);
			else
				sprintf(outStr,"DES");
			break;
		case 3:
			if(frame->element_data_valid[set][NXDN_KEYID])
				sprintf(outStr,"AES(%ld)", frame->element_data[set][NXDN_KEYID]);
			else
				sprintf(outStr,"AES");
			break;
		default:
			*outStr = '\0';
			return false;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnRptrState(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_RPTRSTATE] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short RpS = frame->element_data[set][NXDN_RPTRSTATE];
		switch (RpS) // 1 = busy 2 free 4 = EOT 5 = Idle
		{
		case 1:
			sprintf(outStr,"BUSY");
			break;
		case 4:
			sprintf(outStr,"HALT");
			break;
		case 5:
			sprintf(outStr,"IDLE");
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnBusyRptr(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_BUSYRPTR] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short BRp = frame->element_data[set][NXDN_BUSYRPTR];
		switch (BRp)
		{
		case 0:
			if(frame->element_data_valid[set][NXDN_GUFLAG] && (frame->element_data[set][NXDN_GUFLAG] == 1))
				sprintf(outStr,"REG_COMM");
			else
			{
				*outStr = '\0';
				return false;
			}
			break;
		case 31:
			sprintf(outStr,"EOT");
			break;
		default:
			sprintf(outStr,"GoToRp:%d", BRp);
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnFreeRptr(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	
	if(frame->element_data_valid[set][NXDN_FREERPTR1] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short FRp1 = frame->element_data[set][NXDN_FREERPTR1];
		short FRp2 = frame->element_data[set][NXDN_FREERPTR2];
		switch (FRp1)
		{
		case 0:
			sprintf(outStr,"FRp:AllBusy");
			return true;
		case 31: // Reserved
			break;
		default:
			if(frame->element_data_valid[set][NXDN_FREERPTR2] && FRp2 > 0 && FRp2 < 31 && (FRp2 != FRp1))
			{
				sprintf(outStr,"FRp:%d & %d", FRp1, FRp2);
			}
			else
			{
				sprintf(outStr,"FRp:%d", FRp1);
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnVCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_VCALLOPTION] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short i = frame->element_data[set][NXDN_VCALLOPTION];
		switch (i & 0x17)
		{
		case 0:
			sprintf(outStr,"HD48EHR");
			break;
		case 2:
			sprintf(outStr,"HD96EHR");
			break;
		case 3:
			sprintf(outStr,"HD96EFR");
			break;
		case 16:
			sprintf(outStr,"FD48EHR");
			break;
		case 18:
			sprintf(outStr,"FD96EHR");
			break;
		case 19:
			sprintf(outStr,"FD96EFR");
			break;
		default:
			*outStr = '\0';
			return false;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnDCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_DCALLOPTION] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short i = frame->element_data[set][NXDN_DCALLOPTION];
		switch (i & 0x17)
		{
		case 0:
			sprintf(outStr,"HD48");
			break;
		case 2:
			sprintf(outStr,"HD96");
			break;
		case 16:
			sprintf(outStr,"FD48");
			break;
		case 18:
			sprintf(outStr,"FD96");
			break;
		default:
			*outStr = '\0';
			return false;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnSCallOption(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_SCALLOPTION] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION])
	{
		short i = frame->element_data[set][NXDN_SCALLOPTION];
		switch (i & 0x0B)
		{
		case 0:
			sprintf(outStr,"Uncnfm");
			break;
		case 8:
			sprintf(outStr,"Cnfm");
			break;
			break;
		default:
			*outStr = '\0';
			return false;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnStatus(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_STATUS])
	{
	    int status = frame->element_data[set][NXDN_STATUS];
		switch (status)
		{
		case 0: // Null Status
			*outStr = '\0';
			return false;
			break;
		case 0xd0:
			sprintf(outStr,"PAGE");
			break;
		case 0xE0:
			sprintf(outStr,"Emergency!");
			break;
		case 0xE1:
			sprintf(outStr,"Emergency(Man Down)!");
			break;
		case 0xE2:
			sprintf(outStr,"Emergency(termination)");
			break;
		case 0xE3:
			sprintf(outStr,"Emergency(Stationary)!");
			break;
		case 0xE4:
			sprintf(outStr,"Emergency(Motion)!");
		case 0xE5:
			sprintf(outStr,"Emergency(Lone Worker)!");
		default:
			sprintf(outStr,"%s%d", (status > 0xd0)?"R?:":"Status:", status);
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnCauseMM(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CAUSEMM])
	{
		short i = frame->element_data[set][NXDN_CAUSEMM];
		switch (i & 0x7F)
		{
		case 0x01:
			sprintf(outStr,"CMM:Accepted");
			break;
		case 0x04:
			sprintf(outStr,"CMM:Failure(Grp)");
			break;
		case 0x05:
			sprintf(outStr,"CMM:Refused(Grp)");
			break;
		case 0x06:
			sprintf(outStr,"CMM:Failure");
			break;
		case 0x08:
			sprintf(outStr,"CMM:Refused");
			break;
		case 0x51:
			sprintf(outStr,"CMM:Failure(Net)");
			break;
		case 0x52:
			sprintf(outStr,"CMM:Failure(Temp)");
			break;
		case 0x53:
			sprintf(outStr,"CMM:Failure(Congestion)");
			break;
		case 0x5F:
			sprintf(outStr,"CMM:Failure[Resource]");
			break;
		case 0x60:
			sprintf(outStr,"CMM:Failure(Service)");
			break;
		case 0x6F:
			sprintf(outStr,"CMM:Failure[Service]");
			break;
		case 0x70:
			sprintf(outStr,"CMM:Failure(Missing Info)");
			break;
		case 0x71:
			sprintf(outStr,"CMM:Failure(Invalid Info)");
			break;
		case 0x7F:
			sprintf(outStr,"CMM:Failure[Procedure]");
			break;
		default:
			sprintf(outStr,"CMM:R?(%d)", i & 0x7F);
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnCauseVD(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CAUSEVD])
	{
		short i = frame->element_data[set][NXDN_CAUSEVD];
		switch (i & 0x7F)
		{
		case 0x10:
			sprintf(outStr,"CVD:Accepted");
			break;
		case 0x11:
			sprintf(outStr,"CVD:Failure(Grp Permission)");
			break;
		case 0x12:
		case 0x13:
			sprintf(outStr,"CVD:Failure(Unit Permission)");
			break;
		case 0x14:
			sprintf(outStr,"CVD:Failure(Unit Registration)");
			break;
		case 0x15:
			sprintf(outStr,"CVD:Failure(Response)");
			break;
		case 0x16:
			sprintf(outStr,"CVD:Refused(Call)");
			break;
		case 0x18:
			sprintf(outStr,"CVD:Busy(Unit)");
			break;
		case 0x19:
			sprintf(outStr,"CVD:Busy(Grp)");
			break;
		case 0x1a:
			sprintf(outStr,"CVD:Busy");
			break;
		case 0x1c:
			sprintf(outStr,"CVD:Failure(Registration)");
			break;
		case 0x1d:
			sprintf(outStr,"CVD:Failure(Grp Registration)");
			break;
		case 0x1e:
			sprintf(outStr,"CVD:Failure(Queue Interruption)");
			break;
		case 0x20:
			sprintf(outStr,"CVD:Permission(calling unit)");
			break;
		case 0x21:
			sprintf(outStr,"CVD:Permission(called unit)");
			break;
		case 0x22:
			sprintf(outStr,"CVD:Permission(group)");
			break;
		case 0x30:
			sprintf(outStr,"CVD:Channel(all in use)");
			break;
		case 0x31:
			sprintf(outStr,"CVD:Phone(all in use)");
			break;
		case 0x32:
			sprintf(outStr,"CVD:Alerting(called unit)");
			break;
		case 0x33:
			sprintf(outStr,"CVD:Alerting(phone)");
			break;
		case 0x38:
			sprintf(outStr,"CVD:Busy(called unit)");
			break;
		case 0x39:
			sprintf(outStr,"CVD:Busy(called group)");
			break;
		case 0x3f:
			sprintf(outStr,"CVD:Failure(Queue)");
			break;
		case 0x50:
			sprintf(outStr,"CVD:Failure(no channel)");
			break;
		case 0x51:
			sprintf(outStr,"CVD:Failure(Net)");
			break;
		case 0x52:
			sprintf(outStr,"CVD:Failure(Temp)");
			break;
		case 0x53:
			sprintf(outStr,"CVD:Failure(Congestion)");
			break;
		case 0x5F:
			sprintf(outStr,"CVD:Failure[Resource]");
			break;
		case 0x60:
			sprintf(outStr,"CVD:Service(Unavailable)");
			break;
		case 0x61:
			sprintf(outStr,"CVD:Service(Unsupported)");
			break;
		case 0x6F:
			sprintf(outStr,"CVD:Service(Failure)");
			break;
		case 0x70:
			sprintf(outStr,"CVD:Failure(Missing Info)");
			break;
		case 0x71:
			sprintf(outStr,"CVD:Failure(Invalid Info)");
			break;
		case 0x7F:
			sprintf(outStr,"CVD:Failure[Procedure]");
			break;
		default:
			sprintf(outStr,"CVD:R?(%d)", i & 0x7F);
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnCauseSS(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CAUSESS])
	{
		short i = frame->element_data[set][NXDN_CAUSESS];
		switch (i & 0x7F)
		{
		case 0x01:
			sprintf(outStr,"CSS:ACK(Rx)");
			break;
		case 0x02:
			sprintf(outStr,"CSS:ACK(Tx)");
			break;
		case 0x08:
			sprintf(outStr,"CSS:NACK(Retry)");
			break;
		case 0x09:
			sprintf(outStr,"CSS:NACK(Memory Full)");
			break;
		case 0x0a:
			sprintf(outStr,"CSS:NACK(Abort)");
			break;
		case 0x10:
			sprintf(outStr,"CSS:Accepted");
			break;
		case 0x11:
			sprintf(outStr,"CSS:Failure(Grp Permission)");
			break;
		case 0x12:
		case 0x13:
			sprintf(outStr,"CSS:Failure(Unit Permission)");
			break;
		case 0x14:
			sprintf(outStr,"CSS:Failure(Unit Registration)");
			break;
		case 0x15:
			sprintf(outStr,"CSS:Failure(Response)");
			break;
		case 0x16:
			sprintf(outStr,"CSS:Refused(Call)");
			break;
		case 0x18:
			sprintf(outStr,"CSS:Busy(Unit)");
			break;
		case 0x19:
			sprintf(outStr,"CSS:Busy(Grp)");
			break;
		case 0x1a:
			sprintf(outStr,"CSS:Busy");
			break;
		case 0x1c:
			sprintf(outStr,"CSS:Failure(Registration)");
			break;
		case 0x1d:
			sprintf(outStr,"CSS:Failure(Grp Registration)");
			break;
		case 0x20:
			sprintf(outStr,"CSS:Permission(calling unit)");
			break;
		case 0x21:
			sprintf(outStr,"CSS:Permission(called unit)");
			break;
		case 0x22:
			sprintf(outStr,"CSS:Permission(group)");
			break;
		case 0x50:
			sprintf(outStr,"CSS:Failure(no channel)");
			break;
		case 0x51:
			sprintf(outStr,"CSS:Failure(Net)");
			break;
		case 0x52:
			sprintf(outStr,"CSS:Failure(Temp)");
			break;
		case 0x53:
			sprintf(outStr,"CSS:Failure(Congestion)");
			break;
		case 0x5F:
			sprintf(outStr,"CSS:Failure[Resource]");
			break;
		case 0x60:
			sprintf(outStr,"CSS:Service(Unavailable)");
			break;
		case 0x61:
			sprintf(outStr,"CSS:Service(Unsupported)");
			break;
		case 0x6F:
			sprintf(outStr,"CSS:Service(Failure)");
			break;
		case 0x70:
			sprintf(outStr,"CSS:Failure(Missing Info)");
			break;
		case 0x71:
			sprintf(outStr,"CSS:Failure(Invalid Info)");
			break;
		case 0x7F:
			sprintf(outStr,"CSS:Failure[Procedure]");
			break;
		default:
			sprintf(outStr,"CSS:R?(%d)", i & 0x7F);
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnCauseDisc(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CAUSEDISC])
	{
		short i = frame->element_data[set][NXDN_CAUSEDISC];
		switch (i & 0x7F)
		{
		case 0x10:
			sprintf(outStr,"CDis:User");
			break;
		case 0x11:
			sprintf(outStr,"CDis:PSTN");
			break;
		case 0x14:
			sprintf(outStr,"CDis:Timer");
		case 0x1f:
			sprintf(outStr,"CDis:Unit");
			break;
		case 0x20:
			sprintf(outStr,"CDis:TC(Timer)");
			break;
		case 0x2F:
			sprintf(outStr,"CDis:TC");
			break;
		case 0x50:
			sprintf(outStr,"CDis:Failure(no channel)");
			break;
		case 0x51:
			sprintf(outStr,"CDis:Failure(Network)");
			break;
		case 0x52:
			sprintf(outStr,"CDis:Failure(Temp)");
			break;
		case 0x53:
			sprintf(outStr,"CDis:Failure(Congestion)");
			break;
		case 0x5F:
			sprintf(outStr,"CDis:Failure[Resource]");
			break;
		case 0x60:
			sprintf(outStr,"CDis:Service(Unavailable)");
			break;
		case 0x61:
			sprintf(outStr,"CDis:Service(Unsupported)");
			break;
		case 0x6F:
			sprintf(outStr,"CDis:Service(Failure)");
			break;
		case 0x70:
			sprintf(outStr,"CDis:Failure(Missing Info)");
			break;
		case 0x71:
			sprintf(outStr,"CDis:Failure(Invalid Info)");
			break;
		case 0x7F:
			sprintf(outStr,"CDis:Failure[Procedure]");
			break;
		default:
			sprintf(outStr,"CDis:R?(%d)", i & 0x7F);
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnAdjsiteCount(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_ADJSITECNT] && frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		short Count = frame->element_data[set][NXDN_ADJSITECNT];
		switch (Count)
		{
		case 0:
			*outStr = '\0';
			return false;
		default:
			sprintf(outStr,"AjS#:%d", Count);
			break;
		}
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnAdjSites(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	int i;
	bool strFlag = false;
	bool validFlag = false;
	char tmpStr[512];
	*outStr = '\0';
	if(frame->element_data_valid[0][LICH_CHTYPE] && frame->element_data_valid[0][LICH_DIRECTION] && (frame->element_data[0][LICH_DIRECTION] == 1))
	{
		int i;
		long l = frame->element_data[set][NXDN_LOCID];
		long sysid;
		long site;
		int  chan;
		int  option;
		char c;

		if((frame->element_data[0][LICH_CHTYPE] < 3) || frame->element_data_valid[0][NXDN_RAN]) // Type-C or single
		{
			for(i = 0; i < 4; i++)
			{
				switch(i)
				{
				case 0:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID1] || !frame->element_data_valid[set][NXDN_ADJSITEOPTION1])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION1] & 0x0F;
					l = frame->element_data[set][NXDN_ADJSITELOCID1];
					if((option == 0) || (l == 0L))
						continue;
					chan = frame->element_data[set][NXDN_ADJSITECCHAN1];
					break;
				case 1:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID2] || !frame->element_data_valid[set][NXDN_ADJSITEOPTION2])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION2] & 0x0F;
					l = frame->element_data[set][NXDN_ADJSITELOCID2];
					if((option == 0) || (l == 0L))
						continue;
					chan = frame->element_data[set][NXDN_ADJSITECCHAN2];
					break;
				case 2:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID3] || !frame->element_data_valid[set][NXDN_ADJSITEOPTION3])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION3] & 0x0F;
					l = frame->element_data[set][NXDN_ADJSITELOCID3];
					if((option == 0) || (l == 0L))
						continue;
					chan = frame->element_data[set][NXDN_ADJSITECCHAN3];
					break;
				case 3:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID4] || !frame->element_data_valid[set][NXDN_ADJSITEOPTION4])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION4] & 0x0F;
					l = frame->element_data[set][NXDN_ADJSITELOCID4];
					if((option == 0) || (l == 0L))
						continue;
					chan = frame->element_data[set][NXDN_ADJSITECCHAN4];
					break;
				}
				switch((l >> 22) & 0x3)
				{
				case 0:
					sysid = (l >> 12) & 0x3ff;
					site = l & 0xfff;
					c = 'G';
					break;
				case 2:
					sysid = (l >> 8) & 0x3fff;
					site = l & 0xff;
					c = 'R';
					break;
				case 1:
					sysid = (l >> 5) & 0x1ffff;
					site = l & 0x1f;
					c = 'L';
					break;
				default:
					sysid = 0L;
					sprintf(tmpStr,"AjS%d[Sys:R?(%ld)]", option, l);
					if(!strFlag)
					{
						strcpy(outStr, tmpStr);
						strFlag = true;
					}
					else
					{
						strcat(outStr, " ");
						strcat(outStr, tmpStr);
					}
					break;
				}
				if(sysid > 0L)
				{
					sprintf(tmpStr,"AjS%d:[Sys:%c%ld Site:%ld CCh:%d]", option, c, sysid, site, chan);
					if(!strFlag)
					{
						strcpy(outStr, tmpStr);
						strFlag = true;
					}
					else
					{
						strcat(outStr, " ");
						strcat(outStr, tmpStr);
					}
				}
			}
		}
		else if (frame->element_data_valid[2][NXDN_AREA]) // Type D
		{
			for(i = 0; i < 4; i++)
			{
				switch(i)
				{
				case 0:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID1])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION1];
					l = frame->element_data[set][NXDN_ADJSITELOCID1];
					chan = frame->element_data[set][NXDN_ADJSITECCHAN1];
					break;
				case 1:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID2])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION2];
					l = frame->element_data[set][NXDN_ADJSITELOCID2];
					chan = frame->element_data[set][NXDN_ADJSITECCHAN2];
					break;
				case 2:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID3])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION3];
					l = frame->element_data[set][NXDN_ADJSITELOCID3];
					chan = frame->element_data[set][NXDN_ADJSITECCHAN3];
					break;
				case 3:
					if(!frame->element_data_valid[set][NXDN_ADJSITELOCID4])
						continue;
					validFlag = true;
					option = frame->element_data[set][NXDN_ADJSITEOPTION4];
					l = frame->element_data[set][NXDN_ADJSITELOCID4];
					chan = frame->element_data[set][NXDN_ADJSITECCHAN4];
					break;
				}
				if(l > 0L)
				{
					sprintf(tmpStr,"AjS%d:[Sys:%ld-%ld Site:%d%s]", i+1, (l >> 15) & 0xf, l & 0x7fff, chan, (option & 0x10)?" Isolated":"");
				}
				else
				{
					sprintf(tmpStr,"AjS%d:None", i+1);
				}
				if(!strFlag)
				{
					strcpy(outStr, tmpStr);
					strFlag = true;
				}
				else
				{
					strcat(outStr, " ");
					strcat(outStr, tmpStr);
				}
			}
		}
	}
	if(validFlag && !strFlag)
	{
		sprintf(outStr,"AjS:None");
		strFlag = true;
	}
	return strFlag;
}
bool NxdnCTimer(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_CALLTIMER])
	{
		short Timer = frame->element_data[set][NXDN_CALLTIMER] * 15;
		if(Timer > 180)
			Timer += (Timer - 180);
		if(Timer <= 600)
		{
			switch (Timer)
			{
			case 0:
				*outStr = '\0';
				return false;
			default:
				sprintf(outStr,"CTmr:%hd", Timer);
				break;
			}
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnPFNumber(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_PKTFRNUM])
	{
		sprintf(outStr,"PFN:%ld", frame->element_data[set][NXDN_PKTFRNUM]);
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnBNumber(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_BLKNUM])
	{
		sprintf(outStr,"BlkN:%ld", frame->element_data[set][NXDN_BLKNUM]);
		return true;
	}
	*outStr = '\0';
	return false;
}
bool NxdnHdrDelay(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_HDRDELAY])
	{
		if(frame->element_data[set][NXDN_HDRDELAY] < 1024)
		{
			sprintf(outStr,"FrCt:%ld", frame->element_data[set][NXDN_HDRDELAY]);
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnVersion(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_VERSION])
	{
		if(frame->element_data[set][NXDN_VERSION] < 256)
		{
			sprintf(outStr,"Ver:%ld", frame->element_data[set][NXDN_VERSION]);
			return true;
		}
	}
	*outStr = '\0';
	return false;
}
bool NxdnManNum(dsd_frame * frame, int set, dsd_display_format format, char *outStr)
{
	if(frame->element_data_valid[set][NXDN_MANNUM])
	{
		if(frame->element_data[set][NXDN_MANNUM] < 256)
		{
			sprintf(outStr,"MN:%ld", frame->element_data[set][NXDN_MANNUM]);
			return true;
		}
	}
	*outStr = '\0';
	return false;
}


