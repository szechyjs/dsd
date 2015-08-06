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
processAudio (dsd_opts * opts, dsd_state * state)
{

  int i, n;
  float aout_abs, max, gainfactor, gaindelta, maxbuf;

  if (opts->audio_gain == (float) 0)
    {
      // detect max level
      max = 0;
      state->audio_out_temp_buf_p = state->audio_out_temp_buf;
      for (n = 0; n < 160; n++)
        {
          aout_abs = fabsf (*state->audio_out_temp_buf_p);
          if (aout_abs > max)
            {
              max = aout_abs;
            }
          state->audio_out_temp_buf_p++;
        }
      *state->aout_max_buf_p = max;
      state->aout_max_buf_p++;
      state->aout_max_buf_idx++;
      if (state->aout_max_buf_idx > 24)
        {
          state->aout_max_buf_idx = 0;
          state->aout_max_buf_p = state->aout_max_buf;
        }

      // lookup max history
      for (i = 0; i < 25; i++)
        {
          maxbuf = state->aout_max_buf[i];
          if (maxbuf > max)
            {
              max = maxbuf;
            }
        }

      // determine optimal gain level
      if (max > (float) 0)
        {
          gainfactor = ((float) 30000 / max);
        }
      else
        {
          gainfactor = (float) 50;
        }
      if (gainfactor < state->aout_gain)
        {
          state->aout_gain = gainfactor;
          gaindelta = (float) 0;
        }
      else
        {
          if (gainfactor > (float) 50)
            {
              gainfactor = (float) 50;
            }
          gaindelta = gainfactor - state->aout_gain;
          if (gaindelta > ((float) 0.05 * state->aout_gain))
            {
              gaindelta = ((float) 0.05 * state->aout_gain);
            }
        }
      gaindelta /= (float) 160;
    }
  else
    {
      gaindelta = (float) 0;
    }

  if(opts->audio_gain >= 0)
    {
      // adjust output gain
      state->audio_out_temp_buf_p = state->audio_out_temp_buf;
      for (n = 0; n < 160; n++)
        {
          *state->audio_out_temp_buf_p = (state->aout_gain + ((float) n * gaindelta)) * (*state->audio_out_temp_buf_p);
          state->audio_out_temp_buf_p++;
        }
      state->aout_gain += ((float) 160 * gaindelta);
    }

  // copy audio data to output buffer and upsample if necessary
  state->audio_out_temp_buf_p = state->audio_out_temp_buf;
  if (opts->split == 0)
    {
      for (n = 0; n < 160; n++)
        {
          upsample (state, *state->audio_out_temp_buf_p);
          state->audio_out_temp_buf_p++;
          state->audio_out_float_buf_p += 6;
          state->audio_out_idx += 6;
          state->audio_out_idx2 += 6;
        }
      state->audio_out_float_buf_p -= (960 + opts->playoffset);
      // copy to output (short) buffer
      for (n = 0; n < 960; n++)
        {
          if (*state->audio_out_float_buf_p >  32767.0F)
            {
              *state->audio_out_float_buf_p = 32767.0F;
            }
          else if (*state->audio_out_float_buf_p < -32768.0F)
            {
              *state->audio_out_float_buf_p = -32768.0F;
            }
          *state->audio_out_buf_p = (short) *state->audio_out_float_buf_p;
          state->audio_out_buf_p++;
          state->audio_out_float_buf_p++;
        }
      state->audio_out_float_buf_p += opts->playoffset;
    }
  else
    {
      for (n = 0; n < 160; n++)
        {
          if (*state->audio_out_temp_buf_p > 32767.0F)
            {
              *state->audio_out_temp_buf_p = 32767.0F;
            }
          else if (*state->audio_out_temp_buf_p < -32768.0F)
            {
              *state->audio_out_temp_buf_p = -32768.0F;
            }
          *state->audio_out_buf_p = (short) *state->audio_out_temp_buf_p;
          state->audio_out_buf_p++;
          state->audio_out_temp_buf_p++;
          state->audio_out_idx++;
          state->audio_out_idx2++;
        }
    }
}

void
writeSynthesizedVoice (dsd_opts * opts, dsd_state * state)
{
  int n;
  short aout_buf[160];
  short *aout_buf_p;

//  for(n=0; n<160; n++)
//    printf("%d ", ((short*)(state->audio_out_temp_buf))[n]);
//  printf("\n");
  
  aout_buf_p = aout_buf;
  state->audio_out_temp_buf_p = state->audio_out_temp_buf;

  for (n = 0; n < 160; n++)
  {
    if (*state->audio_out_temp_buf_p > (float) 32767)
      {
        *state->audio_out_temp_buf_p = (float) 32767;
      }
    else if (*state->audio_out_temp_buf_p < (float) -32768)
      {
        *state->audio_out_temp_buf_p = (float) -32768;
      }
      *aout_buf_p = (short) *state->audio_out_temp_buf_p;
      aout_buf_p++;
      state->audio_out_temp_buf_p++;
  }

  sf_write_short(opts->wav_out_f, aout_buf, 160);
  /*

  int n;
  short aout_buf[160];
  short *aout_buf_p;
  ssize_t result;

  aout_buf_p = aout_buf;
  state->audio_out_temp_buf_p = state->audio_out_temp_buf;
  for (n = 0; n < 160; n++)
    {
      if (*state->audio_out_temp_buf_p > (float) 32760)
        {
          *state->audio_out_temp_buf_p = (float) 32760;
        }
      else if (*state->audio_out_temp_buf_p < (float) -32760)
        {
          *state->audio_out_temp_buf_p = (float) -32760;
        }
      *aout_buf_p = (short) *state->audio_out_temp_buf_p;
      aout_buf_p++;
      state->audio_out_temp_buf_p++;
    }

  result = write (opts->wav_out_fd, aout_buf, 320);
  fflush (opts->wav_out_f);
  state->wav_out_bytes += 320;
  */
}


void
playSynthesizedVoice (dsd_opts * opts, dsd_state * state)
{
  ssize_t result;

  if (state->audio_out_idx > opts->delay)
    {
      // output synthesized speech to sound card
		if(opts->audio_out_type == 2)
		{
#ifdef USE_PORTAUDIO
			PaError err = paNoError;
			do
			{
				long available = Pa_GetStreamWriteAvailable( opts->audio_out_pa_stream );
				if(available < 0)
					err = available;
				//printf("Frames available: %d\n", available);
				if( err != paNoError )
					break;
				if(available > SAMPLE_RATE_OUT * PA_LATENCY_OUT)
				{
					//It looks like this might not be needed for very small latencies. However, it's definitely needed for a bit larger ones.
					//When PA_LATENCY_OUT == 0.500 I get output buffer underruns if I don't use this. With PA_LATENCY_OUT <= 0.100 I don't see those happen.
					//But with PA_LATENCY_OUT < 0.100 I run the risk of choppy audio and stream errors.
					printf("\nSyncing voice output stream\n");
					err = Pa_StopStream( opts->audio_out_pa_stream );
					if( err != paNoError )
						break;
				}

				err = Pa_IsStreamActive( opts->audio_out_pa_stream );
				if(err == 0)
				{
					printf("Start voice output stream\n");
					err = Pa_StartStream( opts->audio_out_pa_stream );
				}
				else if(err == 1)
				{
					err = paNoError;
				}
				if( err != paNoError )
					break;

				//printf("write stream %d\n", state->audio_out_idx);
				err = Pa_WriteStream( opts->audio_out_pa_stream, (state->audio_out_buf_p - state->audio_out_idx), state->audio_out_idx );
				if( err != paNoError )
					break;
			} while(0);

			if( err != paNoError )
			{
				fprintf( stderr, "An error occured while using the portaudio output stream\n" );
				fprintf( stderr, "Error number: %d\n", err );
				fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
			}

#endif
		}
		else
			result = write (opts->audio_out_fd, (state->audio_out_buf_p - state->audio_out_idx), (state->audio_out_idx * 2));
      state->audio_out_idx = 0;
    }

  if (state->audio_out_idx2 >= 800000)
    {
      state->audio_out_float_buf_p = state->audio_out_float_buf + 100;
      state->audio_out_buf_p = state->audio_out_buf + 100;
      memset (state->audio_out_float_buf, 0, 100 * sizeof (float));
      memset (state->audio_out_buf, 0, 100 * sizeof (short));
      state->audio_out_idx2 = 0;
    }
}

#ifdef USE_PORTAUDIO
int getPADevice(char* dev, int input, PaStream** stream)
{
	int devnum = atoi(dev + 3);
	printf("Using portaudio device %d.\n", devnum);

    PaError err;

	int numDevices = Pa_GetDeviceCount();
    if( numDevices < 0 )
    {
        printf( "ERROR: Pa_GetDeviceCount returned 0x%x\n", numDevices );
        err = numDevices;
        goto error;
    }
	if( devnum >= numDevices)
    {
        printf( "ERROR: Requested device %d is larger than number of devices.\n", devnum );
        return(1);
    }
	
	const   PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo( devnum );

    /* print device name */
#ifdef WIN32
    {   /* Use wide char on windows, so we can show UTF-8 encoded device names */
        wchar_t wideName[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, deviceInfo->name, -1, wideName, MAX_PATH-1);
        wprintf( L"Name                        = %s\n", wideName );
    }
#else
    printf( "Name                        = %s\n", deviceInfo->name );
#endif
	if((input == 1) && (deviceInfo->maxInputChannels == 0))
	{
		printf( "ERROR: Requested device %d is not an input device.\n", devnum );
		return(1);
	}
	if((input == 0) && (deviceInfo->maxOutputChannels == 0))
	{
		printf( "ERROR: Requested device %d is not an output device.\n", devnum );
		return(1);
	}

	//Create stream parameters
	PaStreamParameters parameters;
    parameters.device = devnum;
    parameters.channelCount = 1;       /* mono */
    parameters.sampleFormat = paInt16; //Shorts
    parameters.suggestedLatency = (input == 1) ? PA_LATENCY_IN : PA_LATENCY_OUT;
    parameters.hostApiSpecificStreamInfo = NULL;

	//Open stream
	err = Pa_OpenStream(
            stream,
            (input == 1) ? &parameters : NULL,
            (input == 0) ? &parameters : NULL,
            (input == 1) ? SAMPLE_RATE_IN : SAMPLE_RATE_OUT,
            PA_FRAMES_PER_BUFFER,
            paClipOff,
            NULL /*callback*/,
            NULL );
    if( err != paNoError ) goto error;

	return 0;

error:
	fprintf( stderr, "An error occured while initializing a portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
#endif

void
openAudioOutDevice (dsd_opts * opts, int speed)
{
  // get info of device/file
	if(strncmp(opts->audio_out_dev, "pa:", 3) == 0)
	{
		opts->audio_out_type = 2;
#ifdef USE_PORTAUDIO
		int err = getPADevice(opts->audio_out_dev, 0, &opts->audio_out_pa_stream);
		if(err != 0)
			exit(err);
#else
		printf("Error, Portaudio support not compiled.\n");
		exit(1);
#endif
	}
	else
	{
		struct stat stat_buf;
  if(stat(opts->audio_out_dev, &stat_buf) != 0)
    {
      printf("Error, couldn't open %s\n", opts->audio_out_dev);
      exit(1);
    }

  if( !(S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)))
    {
      // this is not a device
      printf("Error, %s is not a device. use -w filename for wav output.\n", opts->audio_out_dev);
      exit(1);
    }
#ifdef SOLARIS
  sample_info_t aset, aget;

  opts->audio_out_fd = open (opts->audio_out_dev, O_WRONLY);
  if (opts->audio_out_fd == -1)
    {
      printf ("Error, couldn't open %s\n", opts->audio_out_dev);
      exit (1);
    }

  // get current
  ioctl (opts->audio_out_fd, AUDIO_GETINFO, &aset);

  aset.record.sample_rate = speed;
  aset.play.sample_rate = speed;
  aset.record.channels = 1;
  aset.play.channels = 1;
  aset.record.precision = 16;
  aset.play.precision = 16;
  aset.record.encoding = AUDIO_ENCODING_LINEAR;
  aset.play.encoding = AUDIO_ENCODING_LINEAR;

  if (ioctl (opts->audio_out_fd, AUDIO_SETINFO, &aset) == -1)
    {
      printf ("Error setting sample device parameters\n");
      exit (1);
    }
#endif

#if defined(BSD) && !defined(__APPLE__)

  int fmt;

  opts->audio_out_fd = open (opts->audio_out_dev, O_WRONLY);
  if (opts->audio_out_fd == -1)
    {
      printf ("Error, couldn't open %s\n", opts->audio_out_dev);
      opts->audio_out = 0;
      exit(1);
    }

  fmt = 0;
  if (ioctl (opts->audio_out_fd, SNDCTL_DSP_RESET) < 0)
    {
      printf ("ioctl reset error \n");
    }
  fmt = speed;
  if (ioctl (opts->audio_out_fd, SNDCTL_DSP_SPEED, &fmt) < 0)
    {
      printf ("ioctl speed error \n");
    }
  fmt = 0;
  if (ioctl (opts->audio_out_fd, SNDCTL_DSP_STEREO, &fmt) < 0)
    {
      printf ("ioctl stereo error \n");
    }
  fmt = AFMT_S16_LE;
  if (ioctl (opts->audio_out_fd, SNDCTL_DSP_SETFMT, &fmt) < 0)
    {
      printf ("ioctl setfmt error \n");
    }

#endif
	}
  printf ("Audio Out Device: %s\n", opts->audio_out_dev);
}

void
openAudioInDevice (dsd_opts * opts)
{
  // get info of device/file
	if(strncmp(opts->audio_in_dev, "-", 1) == 0)
	{
		opts->audio_in_type = 1;
		opts->audio_in_file_info = calloc(1, sizeof(SF_INFO));
		opts->audio_in_file_info->samplerate=48000;
		opts->audio_in_file_info->channels=1;
		opts->audio_in_file_info->seekable=0;
		opts->audio_in_file_info->format=SF_FORMAT_RAW|SF_FORMAT_PCM_16|SF_ENDIAN_LITTLE;
		opts->audio_in_file = sf_open_fd(fileno(stdin), SFM_READ, opts->audio_in_file_info, 0);

		if(opts->audio_in_file == NULL) {
			printf ("Error, couldn't open stdin with libsndfile: %s\n", sf_strerror(NULL));
			exit(1);
		}
	}
	else if(strncmp(opts->audio_in_dev, "pa:", 2) == 0)
	{
		opts->audio_in_type = 2;
#ifdef USE_PORTAUDIO
		int err = getPADevice(opts->audio_in_dev, 1, &opts->audio_in_pa_stream);
		if(err != 0)
			exit(err);

		if (opts->split == 0)
		{
			int err = getPADevice(opts->audio_in_dev, 0, &opts->audio_out_pa_stream);
			if(err != 0)
				exit(err);
		}

#else
		printf("Error, Portaudio support not compiled.\n");
		exit(1);
#endif
	}
	else
	{
  struct stat stat_buf;
  if (stat(opts->audio_in_dev, &stat_buf) != 0)
    {
      printf("Error, couldn't open %s\n", opts->audio_in_dev);
      exit(1);
    }
  if (S_ISREG(stat_buf.st_mode))
    {
      // is this a regular file? then process with libsndfile.
      opts->audio_in_type = 1;
      opts->audio_in_file_info = calloc(1, sizeof(SF_INFO));
      opts->audio_in_file_info->channels = 1;
      opts->audio_in_file = sf_open(opts->audio_in_dev, SFM_READ, opts->audio_in_file_info);

      if(opts->audio_in_file == NULL)
        {
          printf ("Error, couldn't open file %s\n", opts->audio_in_dev);
          exit(1);
        }
    }
  else
    {
      // this is a device, use old handling
      opts->audio_in_type = 0;
#ifdef SOLARIS
    sample_info_t aset, aget;
    int rgain;
  
    rgain = 64;
  
    if (opts->split == 1)
      {
        opts->audio_in_fd = open (opts->audio_in_dev, O_RDONLY);
      }
    else
      {
        opts->audio_in_fd = open (opts->audio_in_dev, O_RDWR);
      }
    if (opts->audio_in_fd == -1)
      {
        printf ("Error, couldn't open %s\n", opts->audio_in_dev);
        exit(1);
      }
  
    // get current
    ioctl (opts->audio_in_fd, AUDIO_GETINFO, &aset);
  
    aset.record.sample_rate = SAMPLE_RATE_IN;
    aset.play.sample_rate = SAMPLE_RATE_IN;
    aset.record.channels = 1;
    aset.play.channels = 1;
    aset.record.precision = 16;
    aset.play.precision = 16;
    aset.record.encoding = AUDIO_ENCODING_LINEAR;
    aset.play.encoding = AUDIO_ENCODING_LINEAR;
    aset.record.port = AUDIO_LINE_IN;
    aset.record.gain = rgain;
  
    if (ioctl (opts->audio_in_fd, AUDIO_SETINFO, &aset) == -1)
      {
        printf ("Error setting sample device parameters\n");
        exit (1);
      }
#endif

#if defined(BSD) && !defined(__APPLE__)
    int fmt;
  
    if (opts->split == 1)
      {
        opts->audio_in_fd = open (opts->audio_in_dev, O_RDONLY);
      }
    else
      {
        opts->audio_in_fd = open (opts->audio_in_dev, O_RDWR);
      }
  
    if (opts->audio_in_fd == -1)
      {
        printf ("Error, couldn't open %s\n", opts->audio_in_dev);
        opts->audio_out = 0;
      }
  
    fmt = 0;
    if (ioctl (opts->audio_in_fd, SNDCTL_DSP_RESET) < 0)
      {
        printf ("ioctl reset error \n");
      }
    fmt = SAMPLE_RATE_IN;
    if (ioctl (opts->audio_in_fd, SNDCTL_DSP_SPEED, &fmt) < 0)
      {
        printf ("ioctl speed error \n");
      }
    fmt = 0;
    if (ioctl (opts->audio_in_fd, SNDCTL_DSP_STEREO, &fmt) < 0)
      {
        printf ("ioctl stereo error \n");
      }
    fmt = AFMT_S16_LE;
    if (ioctl (opts->audio_in_fd, SNDCTL_DSP_SETFMT, &fmt) < 0)
      {
        printf ("ioctl setfmt error \n");
      }
#endif
  }
  }

  if (opts->split == 1)
    {
      printf ("Audio In Device: %s\n", opts->audio_in_dev);
    }
  else
    {
      printf ("Audio In/Out Device: %s\n", opts->audio_in_dev);
    }
}
