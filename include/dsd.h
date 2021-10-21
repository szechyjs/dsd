#ifndef DSD_H
#define DSD_H
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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef SOLARIS
#include <sys/audioio.h>
#endif
#if defined(BSD) && !defined(__APPLE__)
#include <sys/soundcard.h>
#endif
#include <math.h>
#include <mbelib.h>
#include <sndfile.h>

#include "p25p1_heuristics.h"


#define SAMPLE_RATE_IN 48000
#define SAMPLE_RATE_OUT 8000

#ifdef USE_PORTAUDIO
#include "portaudio.h"
#define PA_FRAMES_PER_BUFFER 64
//Buffer needs to be large enough to prevent input buffer overruns while DSD is doing other struff (like outputting voice)
//else you get skipped samples which result in incomplete/erronous decodes and a mountain of error messages.
#define PA_LATENCY_IN 0.500
//Buffer needs to be large enough to prevent output buffer underruns while DSD is doing other stuff (like decoding input)
//else you get choppy audio and in 'extreme' cases errors.
//Buffer also needs to be as small as possible so we don't have a lot of audio delay.
#define PA_LATENCY_OUT 0.100
#endif

/*
 * global variables
 */
extern int exitflag;


typedef struct
{
  int onesymbol;
  char mbe_in_file[1024];
  FILE *mbe_in_f;
  int errorbars;
  int datascope;
  int symboltiming;
  int verbose;
  int p25enc;
  int p25lc;
  int p25status;
  int p25tg;
  int scoperate;
  char audio_in_dev[1024];
  int audio_in_fd;
  SNDFILE *audio_in_file;
  SF_INFO *audio_in_file_info;
#ifdef USE_PORTAUDIO
  PaStream* audio_in_pa_stream;
#endif
  int audio_in_type; // 0 for device, 1 for file, 2 for portaudio
  char audio_out_dev[1024];
  int audio_out_fd;
  SNDFILE *audio_out_file;
  SF_INFO *audio_out_file_info;
#ifdef USE_PORTAUDIO
  PaStream* audio_out_pa_stream;
#endif
  int audio_out_type; // 0 for device, 1 for file, 2 for portaudio
  int split;
  int playoffset;
  char mbe_out_dir[1024];
  char mbe_out_file[1024];
  char mbe_out_path[1024];
  FILE *mbe_out_f;
  float audio_gain;
  int audio_out;
  char wav_out_file[1024];
  SNDFILE *wav_out_f;
  //int wav_out_fd;
  int serial_baud;
  char serial_dev[1024];
  int serial_fd;
  int resume;
  int frame_dstar;
  int frame_x2tdma;
  int frame_p25p1;
  int frame_nxdn48;
  int frame_nxdn96;
  int frame_dmr;
  int frame_provoice;
  int mod_c4fm;
  int mod_qpsk;
  int mod_gfsk;
  int uvquality;
  int inverted_x2tdma;
  int inverted_dmr;
  int mod_threshold;
  int ssize;
  int msize;
  int playfiles;
  int delay;
  int use_cosine_filter;
  int unmute_encrypted_p25;
} dsd_opts;

typedef struct
{
  int *dibit_buf;
  int *dibit_buf_p;
  int repeat;
  short *audio_out_buf;
  short *audio_out_buf_p;
  float *audio_out_float_buf;
  float *audio_out_float_buf_p;
  float audio_out_temp_buf[160];
  float *audio_out_temp_buf_p;
  int audio_out_idx;
  int audio_out_idx2;
  //int wav_out_bytes;
  int center;
  int jitter;
  int synctype;
  int min;
  int max;
  int lmid;
  int umid;
  int minref;
  int maxref;
  int lastsample;
  int sbuf[128];
  int sidx;
  int maxbuf[1024];
  int minbuf[1024];
  int midx;
  char err_str[64];
  char fsubtype[16];
  char ftype[16];
  int symbolcnt;
  int rf_mod;
  int numflips;
  int lastsynctype;
  int lastp25type;
  int offset;
  int carrier;
  char tg[25][16];
  int tgcount;
  int lasttg;
  int lastsrc;
  int nac;
  int errs;
  int errs2;
  int mbe_file_type;
  int optind;
  int numtdulc;
  int firstframe;
  char slot0light[8];
  char slot1light[8];
  float aout_gain;
  float aout_max_buf[200];
  float *aout_max_buf_p;
  int aout_max_buf_idx;
  int samplesPerSymbol;
  int symbolCenter;
  char algid[9];
  char keyid[17];
  int currentslot;
  mbe_parms *cur_mp;
  mbe_parms *prev_mp;
  mbe_parms *prev_mp_enhanced;
  int p25kid;

  unsigned int debug_audio_errors;
  unsigned int debug_header_errors;
  unsigned int debug_header_critical_errors;

  // Last dibit read
  int last_dibit;

  // Heuristics state data for +P5 signals
  P25Heuristics p25_heuristics;

  // Heuristics state data for -P5 signals
  P25Heuristics inv_p25_heuristics;

#ifdef TRACE_DSD
  char debug_prefix;
  char debug_prefix_2;
  unsigned int debug_sample_index;
  unsigned int debug_sample_left_edge;
  unsigned int debug_sample_right_edge;
  FILE* debug_label_file;
  FILE* debug_label_dibit_file;
  FILE* debug_label_imbe_file;
#endif

} dsd_state;

/*
 * Frame sync patterns
 */
#define INV_P25P1_SYNC "333331331133111131311111"
#define P25P1_SYNC     "111113113311333313133333"

#define X2TDMA_BS_VOICE_SYNC "113131333331313331113311"
#define X2TDMA_BS_DATA_SYNC  "331313111113131113331133"
#define X2TDMA_MS_DATA_SYNC  "313113333111111133333313"
#define X2TDMA_MS_VOICE_SYNC "131331111333333311111131"

#define DSTAR_HD       "131313131333133113131111"
#define INV_DSTAR_HD   "313131313111311331313333"
#define DSTAR_SYNC     "313131313133131113313111"
#define INV_DSTAR_SYNC "131313131311313331131333"

#define NXDN_MS_DATA_SYNC      "313133113131111333"
#define INV_NXDN_MS_DATA_SYNC  "131311331313333111"
#define NXDN_MS_VOICE_SYNC     "313133113131113133"
#define INV_NXDN_MS_VOICE_SYNC "131311331313331311"
#define INV_NXDN_BS_DATA_SYNC  "131311331313333131"
#define NXDN_BS_DATA_SYNC      "313133113131111313"
#define INV_NXDN_BS_VOICE_SYNC "131311331313331331"
#define NXDN_BS_VOICE_SYNC     "313133113131113113"

#define DMR_BS_DATA_SYNC  "313333111331131131331131"
#define DMR_BS_VOICE_SYNC "131111333113313313113313"
#define DMR_MS_DATA_SYNC  "311131133313133331131113"
#define DMR_MS_VOICE_SYNC "133313311131311113313331"

#define INV_PROVOICE_SYNC    "31313111333133133311331133113311"
#define PROVOICE_SYNC        "13131333111311311133113311331133"
#define INV_PROVOICE_EA_SYNC "13313133113113333311313133133311"
#define PROVOICE_EA_SYNC     "31131311331331111133131311311133"

/*
 * function prototypes
 */
void processDMRdata (dsd_opts * opts, dsd_state * state);
void processDMRvoice (dsd_opts * opts, dsd_state * state);
void processAudio (dsd_opts * opts, dsd_state * state);
void writeSynthesizedVoice (dsd_opts * opts, dsd_state * state);
void playSynthesizedVoice (dsd_opts * opts, dsd_state * state);
void openAudioOutDevice (dsd_opts * opts, int speed);
void openAudioInDevice (dsd_opts * opts);

int getDibit (dsd_opts * opts, dsd_state * state);
int get_dibit_and_analog_signal (dsd_opts * opts, dsd_state * state, int * out_analog_signal);

void skipDibit (dsd_opts * opts, dsd_state * state, int count);
void saveImbe4400Data (dsd_opts * opts, dsd_state * state, char *imbe_d);
void saveAmbe2450Data (dsd_opts * opts, dsd_state * state, char *ambe_d);
int readImbe4400Data (dsd_opts * opts, dsd_state * state, char *imbe_d);
int readAmbe2450Data (dsd_opts * opts, dsd_state * state, char *ambe_d);
void openMbeInFile (dsd_opts * opts, dsd_state * state);
void closeMbeOutFile (dsd_opts * opts, dsd_state * state);
void openMbeOutFile (dsd_opts * opts, dsd_state * state);
void openWavOutFile (dsd_opts * opts, dsd_state * state);
void closeWavOutFile (dsd_opts * opts, dsd_state * state);
void printFrameInfo (dsd_opts * opts, dsd_state * state);
void processFrame (dsd_opts * opts, dsd_state * state);
void printFrameSync (dsd_opts * opts, dsd_state * state, char *frametype, int offset, char *modulation);
int getFrameSync (dsd_opts * opts, dsd_state * state);
int comp (const void *a, const void *b);
void noCarrier (dsd_opts * opts, dsd_state * state);
void initOpts (dsd_opts * opts);
void initState (dsd_state * state);
void usage ();
void liveScanner (dsd_opts * opts, dsd_state * state);
void cleanupAndExit (dsd_opts * opts, dsd_state * state);
void sigfun (int sig);
int main (int argc, char **argv);
void playMbeFiles (dsd_opts * opts, dsd_state * state, int argc, char **argv);
void processMbeFrame (dsd_opts * opts, dsd_state * state, char imbe_fr[8][23], char ambe_fr[4][24], char imbe7100_fr[7][24]);
void openSerial (dsd_opts * opts, dsd_state * state);
void resumeScan (dsd_opts * opts, dsd_state * state);
int getSymbol (dsd_opts * opts, dsd_state * state, int have_sync);
void upsample (dsd_state * state, float invalue);
void processDSTAR (dsd_opts * opts, dsd_state * state);
void processNXDNVoice (dsd_opts * opts, dsd_state * state);
void processNXDNData (dsd_opts * opts, dsd_state * state);
void processP25lcw (dsd_opts * opts, dsd_state * state, char *lcformat, char *mfid, char *lcinfo);
void processHDU (dsd_opts * opts, dsd_state * state);
void processLDU1 (dsd_opts * opts, dsd_state * state);
void processLDU2 (dsd_opts * opts, dsd_state * state);
void processTDU (dsd_opts * opts, dsd_state * state);
void processTDULC (dsd_opts * opts, dsd_state * state);
void processProVoice (dsd_opts * opts, dsd_state * state);
void processX2TDMAdata (dsd_opts * opts, dsd_state * state);
void processX2TDMAvoice (dsd_opts * opts, dsd_state * state);
void processDSTAR_HD (dsd_opts * opts, dsd_state * state);
short dmr_filter(short sample);
short nxdn_filter(short sample);

#endif // DSD_H
