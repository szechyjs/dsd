# Copyright (C) 2010 DSD Author
# GPG Key ID: 0x3F1D7FD0 (74EF 430D F7F2 0A48 FCE6  F630 FAA2 635D 3F1D 7FD0)
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

CC = gcc
CFLAGS = -O2 -Wall
INCLUDES = -I. -I/usr/local/include -I/usr/include
LIBS = -L/usr/local/lib -lm -lmbe 
INSTALL=install
AR=ar
RANLIB=ranlib
DEST_BASE=/usr/local
DEST_INC=${DEST_BASE}/include
DEST_LIB=${DEST_BASE}/lib
DEST_BIN=${DEST_BASE}/bin
OBJS = dsd_main.o dsd_symbol.o dsd_dibit.o dsd_frame_sync.o dsd_file.o dsd_audio.o dsd_serial.o dsd_frame.o dsd_mbe.o dsd_upsample.o p25p1_hdu.o p25p1_ldu1.o p25p1_ldu2.o p25p1_tdulc.o p25_lcw.o x2tdma_voice.o x2tdma_data.o dstar.o nxdn_voice.o nxdn_data.o dmr_voice.o dmr_data.o provoice.o

all: dsd

build: all

dsd_main.o: dsd_main.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_main.c -o dsd_main.o

dsd_symbol.o: dsd_symbol.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_symbol.c -o dsd_symbol.o

dsd_dibit.o: dsd_dibit.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_dibit.c -o dsd_dibit.o

dsd_frame_sync.o: dsd_frame_sync.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_frame_sync.c -o dsd_frame_sync.o

dsd_file.o: dsd_file.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_file.c -o dsd_file.o

dsd_audio.o: dsd_audio.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_audio.c -o dsd_audio.o

dsd_serial.o: dsd_serial.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_serial.c -o dsd_serial.o

dsd_frame.o: dsd_frame.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_frame.c -o dsd_frame.o

dsd_mbe.o: dsd_mbe.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_mbe.c -o dsd_mbe.o

dsd_upsample.o: dsd_upsample.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dsd_upsample.c -o dsd_upsample.o

p25p1_hdu.o: p25p1_hdu.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c p25p1_hdu.c -o p25p1_hdu.o

p25p1_ldu1.o: p25p1_ldu1.c p25p1_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c p25p1_ldu1.c -o p25p1_ldu1.o

p25p1_ldu2.o: p25p1_ldu2.c p25p1_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c p25p1_ldu2.c -o p25p1_ldu2.o

p25p1_tdulc.o: p25p1_tdulc.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c p25p1_tdulc.c -o p25p1_tdulc.o

p25_lcw.o: p25_lcw.c dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c p25_lcw.c -o p25_lcw.o

x2tdma_voice.o: x2tdma_voice.c x2tdma_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c x2tdma_voice.c -o x2tdma_voice.o

x2tdma_data.o: x2tdma_data.c x2tdma_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c x2tdma_data.c -o x2tdma_data.o

dstar.o: dstar.c dstar_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dstar.c -o dstar.o

nxdn48_voice.o: nxdn48_voice.c nxdn_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c nxdn48_voice.c -o nxdn48_voice.o

nxdn_voice.o: nxdn_voice.c nxdn_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c nxdn_voice.c -o nxdn_voice.o

nxdn_data.o: nxdn_data.c nxdn_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c nxdn_data.c -o nxdn_data.o

dmr_voice.o: dmr_voice.c dmr_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dmr_voice.c -o dmr_voice.o

dmr_data.o: dmr_data.c dmr_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c dmr_data.c -o dmr_data.o

provoice.o: provoice.c provoice_const.h dsd.h config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c provoice.c -o provoice.o

dsd: $(OBJS) dsd.h config.h
	$(CC) $(CFLAGS) $(LIBS) -o dsd $(OBJS)

clean:
	rm -f *.o
	rm -f dsd

install: dsd
	$(INSTALL) dsd $(DEST_BIN)
	chgrp uucp $(DEST_BIN)/dsd
	chmod g+s $(DEST_BIN)/dsd
