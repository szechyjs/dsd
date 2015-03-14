#include "dsd.h"
#include "dsd_display.h"
#include "nxdn.h"

void
printFrameSync (dsd_opts * opts, dsd_state * state)
{
  char *frametype;
  char *modulation;
  if (opts->verbose > 0)
    {
	  switch(state->synctype)
	  {
	  case 0:
		if(state->lastsynctype != -1)
		{
			frametype = " +P25p1    ";
		}
		else if(state->carrier == 1)
		{
			frametype = "(+P25p1)   ";
		}
		break;
	  case 1:
		if(state->lastsynctype != -1)
		{
			frametype = " -P25p1    ";
		}
		else if(state->carrier == 1)
		{
			frametype = "(-P25p1)   ";
		}
		break;
	  case 2:
	  case 4:
		if(state->lastsynctype != -1)
		{
			frametype = " +X2-TDMA  ";
		}
		else if(state->carrier == 1)
		{
			frametype = "(+X2-TDMA) ";
		}
		break;
	  case 3:
	  case 5:
		if(state->lastsynctype != -1)
		{
			frametype = " -X2-TDMA  ";
		}
		else if(state->carrier == 1)
		{
			frametype = "(-X2-TDMA) ";
		}
		break;
	  case 6:
		frametype = " +D-STAR   ";
		break;
	  case 7:
		frametype = " -D-STAR   ";
		break;
	  case 8:
	  case 16:
        if (state->samplesPerSymbol == 20)
			frametype = " +NXDN48   ";
		else
			frametype = " +NXDN96   ";
		break;
	  case 9:
	  case 17:
        if (state->samplesPerSymbol == 20)
			frametype = " -NXDN48   ";
		else
			frametype = " -NXDN96   ";
		break;
	  case 10:
	  case 12:
		if(state->lastsynctype != -1)
		{
			frametype = " +DMR      ";
		}
		else if(state->carrier == 1)
		{
			frametype = "(+DMR)     ";
		}
		break;
	  case 11:
	  case 13:
		if(state->lastsynctype != -1)
		{
			frametype = " -DMR      ";
		}
		else if(state->carrier == 1)
		{
			frametype = "(-DMR)     ";
		}
		break;
	  case 14:
		frametype = " -ProVoice ";
		break;
	  case 15:
		frametype = " -ProVoice ";
		break;
	  case 18:
		frametype = " +D-STAR_HD   ";
		break;
	  case 19:
		frametype = " -D-STAR_HD   ";
		break;
	  default:
		if((state->lastsynctype != -1) && (state->carrier == 1))
		{
			printf ("Sync: no sync\n");
		}
		return;
	  }
      printf ("Sync: %s ", frametype);
    }
  if (opts->verbose > 2)
    {
      printf ("o: %4i ", state->offset + 1);
    }
  if (opts->verbose > 1)
    {
		switch(state->rf_mod)
		{
		case 0:
          modulation = "C4FM";
		  break;
		case 1:
          modulation = "QPSK";
		  break;
		 case 2:
          modulation = "GFSK";
		  break;
		 default:
		  modulation = "????";
        }
      printf ("mod: %s ", modulation);
    }
  if (opts->verbose > 2)
    {
      printf ("g: %f ", state->aout_gain);
    }
}
void
displayMBEErrors (dsd_opts * opts, dsd_state * state)
{
	// save display of error string until later
	strcat(state->err_str2, state->err_str);
	*state->err_str = '\0';
}
void
displayState(dsd_opts * opts, dsd_state * state)
{
    if (opts->errorbars == 1)
    {
		printFrameSync(opts, state);
	    if (state->synctype < 0)
			return;
		if (opts->verbose > 0)
		{
			printf ("inlvl: %2i%% ", (int) state->max / 164);
		}
	}
}
void
displayFrame(dsd_opts * opts, dsd_state * state, dsd_frame * frame)
{
  int i, j;
  bool vFlag = false;
  char tmpStr[257];
  if (opts->errorbars == 1)
    {
	  switch(frame->frame_type)
	  {
	  case FR_TYPE_NXDN_48_VOICE:
	  case FR_TYPE_NXDN_96_VOICE:
        vFlag = true;
        // ** Fall Thru ** //
      case FR_TYPE_NXDN_48_DATA:
      case FR_TYPE_NXDN_96_DATA:
		if(opts->verbose < 1)
			printf("NXDN%s ",((frame->frame_type == FR_TYPE_NXDN_48_VOICE) || (frame->frame_type == FR_TYPE_NXDN_48_DATA))?"48":"96");
		if(NxdnLICH( frame, 0, 0, tmpStr))
			printf("%s ", tmpStr);
		if(NxdnRAN( frame, 0, 0, tmpStr))
			printf("%s ", tmpStr);
        if(vFlag)
        {
          printf("VOICE e:%s ", state->err_str2);
          *state->err_str = '\0';
          *state->err_str2 = '\0';
        }
        else
        {
          *state->err_str = '\0';
          *state->err_str2 = '\0';
          for (i = 0; i < 2; i++)
          {
            if(NxdnMessageType( frame, i, 0, tmpStr))
            {
              if(i == 1)
                printf( "| ");
              printf("%-16s ", tmpStr);
            }
            if(NxdnCCOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCallOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnRegOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnGrpRegOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnAuthOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnDWOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCallType( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnVisitorGrpID( frame, i, 0, tmpStr))
            {
              printf("%s", tmpStr);
              if(NxdnGrpID( frame, i, 0, tmpStr))
                printf("[%s]", tmpStr);
              printf(" ");
            }
            else if(NxdnGrpID( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnDestID( frame, i, 0, tmpStr))
            {
              printf("%s", tmpStr);
              if(NxdnHasDestSysID( frame, i) && NxdnSystemID( frame, i, 0, tmpStr))
                printf("[%s]", tmpStr);
              printf(" ");
            }
            if(NxdnVistorID( frame, i, 0, tmpStr) || NxdnTempID( frame, i, 0, tmpStr))
            {
              printf("%s[", tmpStr);
              if(NxdnHasSrcSysID( frame, i) && NxdnSystemID( frame, i, 0, tmpStr))
              {
                printf("%s ", tmpStr);
              }
              NxdnSrcID( frame, i, 0, tmpStr);
              printf("%s] ", tmpStr);
            }
            else if(NxdnSrcID( frame, i, 0, tmpStr))
            {
              printf("%s", tmpStr);
              if(NxdnHasSrcSysID( frame, i) && NxdnSystemID( frame, i, 0, tmpStr))
                printf("[%s]", tmpStr);
              printf(" ");
            }
            if(!NxdnHasDestSysID( frame, i) && !NxdnHasSrcSysID( frame, i) && NxdnSystemID( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnSiteCode( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnChannel( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCipherType( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnVCallOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnDCallOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnSCallOption( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnServiceInformation( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCChannel( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnAdjSites( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnAdjsiteCount( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnStatus( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCauseMM( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCauseVD( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCauseSS( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCauseDisc( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnCTimer( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnPFNumber( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnBNumber( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnHdrDelay( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnVersion( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(NxdnManNum( frame, i, 0, tmpStr))
              printf("%s ", tmpStr);
            if(frame->element_data_valid[i][NXDN_DATASTART])
            {
              printf("Data: ");
              for (j = frame->element_data[i][NXDN_DATASTART]; j < frame->element_data[i][NXDN_DATAEND]; j++)
              {
                printf("%02X ", frame->raw_frame_data[j]);
              }
            }
          }
        }
        printf ("\n");
        if(frame->element_data_valid[2][NXDN_MSGTYPE] || frame->element_data_valid[2][NXDN_AREA])
        {
          printf ("\t\t\t  SACCH: ");
          if(NxdnRptrState( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnSiteCode( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnMessageType( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCCOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCallOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnRegOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnGrpRegOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnAuthOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnDWOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCallType( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnVisitorGrpID( frame, 2, 0, tmpStr))
          {
            printf("%s", tmpStr);
            if(NxdnGrpID( frame, 2, 0, tmpStr))
                printf("[%s]", tmpStr);
            printf(" ");
          }
          else if(NxdnGrpID( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnDestID( frame, 2, 0, tmpStr))
          {
            printf("%s", tmpStr);
            if(NxdnHasDestSysID( frame, 2) && NxdnSystemID( frame, 2, 0, tmpStr))
                printf("[%s]", tmpStr);
            printf(" ");
          }
          if(NxdnVistorID( frame, 2, 0, tmpStr) || NxdnTempID( frame, 2, 0, tmpStr))
          {
            printf("%s[", tmpStr);
            if(NxdnHasSrcSysID( frame, 2) && NxdnSystemID( frame, 2, 0, tmpStr))
            {
                printf("%s ", tmpStr);
            }
            NxdnSrcID( frame, 2, 0, tmpStr);
            printf("%s] ", tmpStr);
          }
          else if(NxdnSrcID( frame, 2, 0, tmpStr))
          {
            printf("%s", tmpStr);
            if(NxdnHasSrcSysID( frame, 2) && NxdnSystemID( frame, 2, 0, tmpStr))
                printf("[%s]", tmpStr);
            printf(" ");
          }
          if(!NxdnHasDestSysID( frame, 2) && !NxdnHasSrcSysID( frame, 2) && NxdnSystemID( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnChannel( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnBusyRptr( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCipherType( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnFreeRptr( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnVCallOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnDCallOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnSCallOption( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnStatus( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCauseMM( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCauseVD( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCauseSS( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCauseDisc( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnCTimer( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnPFNumber( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnBNumber( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnHdrDelay( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(NxdnManNum( frame, 2, 0, tmpStr))
            printf("%s ", tmpStr);
          if(frame->element_data_valid[2][NXDN_DATASTART])
          {
            printf("Data: ");
            for (i = frame->element_data[2][NXDN_DATASTART]; i < frame->element_data[2][NXDN_DATAEND]; i++)
            {
                printf("%02X ", frame->raw_sframe_data[i]);
            }
          }
          printf ("\n");
        }
        fflush (stdout);
	    break;
      }
    }
}
void
displayActivity(dsd_opts * opts, dsd_frame * frame, dsd_display * display)
{
    int i, j, set, index;
	unsigned char *ucptr;
    bool vFlag = false;
    switch(frame->frame_type)
    {
      case FR_TYPE_NXDN_48_VOICE:
      case FR_TYPE_NXDN_96_VOICE:
        vFlag = true;
        // ** Fall Thru ** //
      case FR_TYPE_NXDN_48_DATA:
      case FR_TYPE_NXDN_96_DATA:
        for( set = 0; set < 2; set++)
        {
          if(frame->element_data_valid[set][NXDN_MSGTYPE] && (frame->element_data[set][NXDN_MSGTYPE] == 63)) // Prop Message
          {
            if(frame->element_data_valid[set][NXDN_MANNUM] && (frame->element_data[set][NXDN_MANNUM] == 104) && frame->element_data_valid[set][NXDN_DATASTART])
            {
              index = frame->element_data[set][NXDN_DATASTART];
              switch(frame->raw_frame_data[index++])
              {
              case 0x82:
				if(frame->raw_frame_data[index++] == 0x4)
				{
					int pos = frame->raw_frame_data[index++];
					if((pos%16) == 4)
					{
						pos >>= 4;
						--pos;
						if((pos >= 0) && (pos <= 3))
						for(i = 0; i < 4; i++)
						{
							display->alias[(pos*4)+i] = frame->raw_frame_data[index++];
						}
						int checksum = display->alias[14] << 8 | display->alias[15];
						j = 0;
						for(i = 0; i <14; i++)
						{
							j += display->alias[i];
						}
						if ((j == checksum) && display->alias[0])
						{
							printf("\t\t\t\t  Alias:%-14s \n", display->alias);
							display->alias[0] = display->alias[14] = display->alias[15] = 0;
						}
					}
				}
				
              default:
				break;
              }
              for (; i < frame->element_data[2][NXDN_DATAEND]; i++)
              {
				printf("%02X ", frame->raw_sframe_data[i]);
              }
          }
        }
      }
      if(frame->element_data_valid[2][NXDN_MSGTYPE] && (frame->element_data[2][NXDN_MSGTYPE] == 63)) // Prop Message
      {
		if(frame->element_data_valid[2][NXDN_MANNUM] && (frame->element_data[2][NXDN_MANNUM] == 104) && frame->element_data_valid[2][NXDN_DATASTART])
		{
		    index = frame->element_data[2][NXDN_DATASTART];
		    switch(frame->raw_sframe_data[index++])
			{
			case 0x82:
				if(frame->raw_sframe_data[index++] == 0x4)
				{
					int pos = frame->raw_sframe_data[index++];
					if((pos%16) == 4)
					{
						pos >>= 4;
						--pos;
						for(i = 0; i < 4; i++)
						{
							display->alias[(pos*4)+i] = frame->raw_sframe_data[index++];
						}
						int checksum = display->alias[14] << 8 | display->alias[15];
						j = 0;
						for(i = 0; i <14; i++)
						{
							j += display->alias[i];
						}
						if ((j == checksum) && display->alias[0])
						{
							printf("\t\t\t\t  Alias:%-14s \n", display->alias);
							display->alias[0] = 0;
						}
					}
				}
				
			default:
				break;
			}
			for (; i < frame->element_data[2][NXDN_DATAEND]; i++)
			{
				printf("%02X ", frame->raw_sframe_data[i]);
			}
		}
      }
      break;
    }
}