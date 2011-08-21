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

  // adjust output gain
  state->audio_out_temp_buf_p = state->audio_out_temp_buf;
  for (n = 0; n < 160; n++)
    {
      *state->audio_out_temp_buf_p = (state->aout_gain + ((float) n * gaindelta)) * (*state->audio_out_temp_buf_p);
      state->audio_out_temp_buf_p++;
    }
  state->aout_gain += ((float) 160 * gaindelta);

  // copy audio datat to output buffer and upsample if necessary
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
          if (*state->audio_out_float_buf_p > (float) 32760)
            {
              *state->audio_out_float_buf_p = (float) 32760;
            }
          else if (*state->audio_out_float_buf_p < (float) -32760)
            {
              *state->audio_out_float_buf_p = (float) -32760;
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
          if (*state->audio_out_temp_buf_p > (float) 32760)
            {
              *state->audio_out_temp_buf_p = (float) 32760;
            }
          else if (*state->audio_out_temp_buf_p < (float) -32760)
            {
              *state->audio_out_temp_buf_p = (float) -32760;
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
}

void
playSynthesizedVoice (dsd_opts * opts, dsd_state * state)
{

  ssize_t result;

  if (state->audio_out_idx > opts->delay)
    {
      // output synthesized speech to sound card
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

void
openAudioOutDevice (dsd_opts * opts, int speed)
{

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

#ifdef BSD

  int fmt;

  opts->audio_out_fd = open (opts->audio_out_dev, O_WRONLY);
  if (opts->audio_out_fd == -1)
    {
      printf ("Error, couldn't open %s\n", opts->audio_out_dev);
      opts->audio_out = 0;
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

  printf ("Audio Out Device: %s\n", opts->audio_out_dev);
}

void
openAudioInDevice (dsd_opts * opts)
{

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
      printf ("Error, couldn't open /dev/audio\n");
    }

  // get current
  ioctl (opts->audio_in_fd, AUDIO_GETINFO, &aset);

  aset.record.sample_rate = 48000;
  aset.play.sample_rate = 48000;
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

#ifdef BSD
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
  fmt = 48000;
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

  if (opts->split == 1)
    {
      printf ("Audio In Device: %s\n", opts->audio_in_dev);
    }
  else
    {
      printf ("Audio In/Out Device: %s\n", opts->audio_in_dev);
    }
}
