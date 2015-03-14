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


// CRC-6
static unsigned char cCRC6;
void initCRC6()
{
  cCRC6 = 0x3F;
}
unsigned char getCRC6()
{
  return cCRC6;
}
void doCRC6(char bit)
{
   char shiftFlag = 0;
   if(cCRC6 & 0x20)
     shiftFlag = 1;
   cCRC6 <<= 1;
   cCRC6 &= 0x3F;
   if (bit ^ shiftFlag)
     cCRC6 ^= 0x27;
}
// CRC-7
static unsigned char cCRC7;
void initCRC7()
{
  cCRC7 = 0x7f;
}
unsigned char getCRC7()
{
  return cCRC7;
}
void doCRC7(char bit)
{
   char shiftFlag = 0;
   if(cCRC7 & 0x40)
     shiftFlag = 1;
   cCRC7 <<= 1;
   cCRC7 &= 0x7F;
   if (bit ^ shiftFlag)
     cCRC7 ^= 0x09;
}
// CRC-12
static unsigned short cCRC12;
void initCRC12()
{
  cCRC12 = 0xfff;
}
unsigned short getCRC12()
{
  return cCRC12;
}
void doCRC12(char bit)
{
   char shiftFlag = 0;
   if(cCRC12 & 0x800)
     shiftFlag = 1;
   cCRC12 <<= 1;
   cCRC12 &= 0xFFF;
   if (bit ^ shiftFlag)
     cCRC12 ^= 0x80F;
}

// CRC-15
static unsigned short cCRC15;
void initNxdnCRC15()
{
  cCRC15 = 0xefff;
}
unsigned short getNxdnCRC15()
{
  return cCRC15;
}
void doNxdnCRC15(char bit)
{
   char shiftFlag = 0;
   if(cCRC15 & 0x4000)
     shiftFlag = 1;
   cCRC15 <<= 1;
   cCRC12 &= 0xEFFF;
    if (shiftFlag ^ bit)
     cCRC15 ^= 0x4CC4;
}

// CRC-16
static unsigned short cCRC16;
void initNxdnCRC16()
{
  cCRC16 = 0xffff;
}
unsigned short getNxdnCRC16()
{
  return cCRC16;
}
void doNxdnCRC16(char bit)
{
   char shiftFlag = 0;
   if(cCRC16 & 0x8000)
     shiftFlag = 1;
   cCRC16 <<= 1;
    if (shiftFlag ^ bit)
     cCRC16 ^= 0x1021;
}

