
#ifndef GOLAY24_HPP_7a68240afda9406facf81fcad3851111
#define GOLAY24_HPP_7a68240afda9406facf81fcad3851111

/**
 * Based on the work of Mr Hank Wallace.
 *
 * Code adapted from http://www.aqdi.com/golay.htm
 *
 * Tried before with the implementation from ITPP library but couldn't make it produce the same outputs
 * expected from the P25 transmissions that I have tested. This implementation does work.
 */

#include <stdio.h>
#include <assert.h>

#define POLY  0xAE3

class Golay24
{
private:
    unsigned int golay(unsigned int cw)
    /* This function calculates [23,12] Golay codewords.
       The format of the returned int is
       [checkbits(11),data(12)]. */
    {
      int i;
      unsigned int c;
      cw&=0xfffl;
      c=cw; /* save original codeword */
      for (i=1; i<=12; i++)  /* examine each data bit */
        {
          if (cw & 1)        /* test data bit */
            cw^=POLY;        /* XOR polynomial */
          cw>>=1;            /* shift intermediate result */
        }
      return((cw<<12)|c);    /* assemble codeword */
    }

    int parity(unsigned int cw)
    /* This function checks the overall parity of codeword cw.
       If parity is even, 0 is returned, else 1. */
    {
      unsigned char p;

      /* XOR the bytes of the codeword */
      p=*(unsigned char*)&cw;
      p^=*((unsigned char*)&cw+1);
      p^=*((unsigned char*)&cw+2);

      /* XOR the halves of the intermediate result */
      p=p ^ (p>>4);
      p=p ^ (p>>2);
      p=p ^ (p>>1);

      /* return the parity result */
      return(p & 1);
    }

    unsigned int syndrome(unsigned int cw)
    /* This function calculates and returns the syndrome
       of a [23,12] Golay codeword. */
    {
      int i;
      cw&=0x7fffffl;
      for (i=1; i<=12; i++)  /* examine each data bit */
        {
          if (cw & 1)        /* test data bit */
            cw^=POLY;        /* XOR polynomial */
          cw>>=1;            /* shift intermediate result */
        }
      return(cw<<12);        /* value pairs with upper bits of cw */
    }

    int weight(unsigned int cw)
    /* This function calculates the weight of
       23 bit codeword cw. */
    {
      int bits,k;

      /* nibble weight table */
      const char wgt[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

      bits=0; /* bit counter */
      k=0;
      /* do all bits, six nibbles max */
      while ((k<6) && (cw))
        {
          bits=bits+wgt[cw & 0xf];
          cw>>=4;
          k++;
        }

      return(bits);
    }

    unsigned int rotate_left(unsigned int cw, int n)
    /* This function rotates 23 bit codeword cw left by n bits. */
    {
      int i;

      if (n != 0)
        {
          for (i=1; i<=n; i++)
            {
              if ((cw & 0x400000l) != 0)
                cw=(cw << 1) | 1;
              else
                cw<<=1;
            }
        }

      return(cw & 0x7fffffl);
    }

    unsigned int rotate_right(unsigned int cw, int n)
    /* This function rotates 23 bit codeword cw right by n bits. */
    {
      int i;

      if (n != 0)
        {
          for (i=1; i<=n; i++)
            {
              if ((cw & 1) != 0)
                cw=(cw >> 1) | 0x400000l;
              else
                cw>>=1;
            }
        }

      return(cw & 0x7fffffl);
    }

public:
    unsigned int correct(unsigned int cw, int *errs, unsigned int *errors_detected)
    /* This function corrects Golay [23,12] codeword cw, returning the
       corrected codeword. This function will produce the corrected codeword
       for three or fewer errors. It will produce some other valid Golay
       codeword for four or more errors, possibly not the intended
       one. *errs is set to the number of bit errors corrected. */
    {
      unsigned char
        w;                /* current syndrome limit weight, 2 or 3 */
      unsigned int
        mask;             /* mask for bit flipping */
      int
        i,j;              /* index */
      unsigned int
        s,                /* calculated syndrome */
        cwsaver;          /* saves initial value of cw */

      cwsaver=cw;         /* save */
      *errs=0;
      *errors_detected = 0;

      w=3;                /* initial syndrome weight threshold */
      j=-1;               /* -1 = no trial bit flipping on first pass */
      mask=1;
      while (j<23) /* flip each trial bit */
        {
          if (j != -1) /* toggle a trial bit */
            {
              if (j>0) /* restore last trial bit */
                {
                  cw=cwsaver ^ mask;
                  mask+=mask; /* point to next bit */
                }
              cw=cwsaver ^ mask; /* flip next trial bit */
              w=2; /* lower the threshold while bit diddling */
            }

          s=syndrome(cw); /* look for errors */
          if (s) /* errors exist */
            {
              (*errors_detected)++;
              for (i=0; i<23; i++) /* check syndrome of each cyclic shift */
                {
                  if ((*errs=weight(s)) <= w) /* syndrome matches error pattern */
                    {
                      cw=cw ^ s;              /* remove errors */
                      cw=rotate_right(cw,i);  /* unrotate data */
                      return(s=cw);
                    }
                  else
                    {
                      cw=rotate_left(cw,1);   /* rotate to next pattern */
                      s=syndrome(cw);         /* calc new syndrome */
                    }
                }
              j++; /* toggle next trial bit */
            }
          else
            {
              return(cw); /* return corrected codeword */
            }
        }

      return(cwsaver); /* return original if no corrections */
    } /* correct */

    unsigned int encode(unsigned int data)
    {
        unsigned int codeword =golay(data); /* make a test codeword */
        if (parity(codeword)) {
          codeword^=0x800000l;
        }

        return codeword;
    }

    unsigned int encode23(unsigned int data)
    {
        return golay(data); /* make a test codeword */
    }

    int decode(int *errs, unsigned int *cw)
    /* This function decodes codeword *cw. Here error correction is attempted,
       with *errs set to the number of
       bits corrected, and returning 0 if no errors exist, or 1 if parity errors
       exist. */
    {
      unsigned int parity_bit;
      unsigned int detected_errors;
      parity_bit=*cw & 0x800000l; /* save parity bit */
      *cw&=~0x800000l;            /* remove parity bit for correction */

      *cw=correct(*cw, errs, &detected_errors);     /* correct up to three bits */
      *cw|=parity_bit;            /* restore parity bit */

      /* check for 4 bit errors */
      if (parity(*cw))            /* odd parity is an error */
        return(1);

      return(0); /* no errors */
    } /* decode */

    int detect(int *errs, unsigned int cw)
    /* This function decodes codeword cw. Here error detection is performed on cw,
       returning 0 if no errors exist, 1 if an overall parity error exists, and
       2 if a codeword error exists. */
    {
      *errs=0;
      if (parity(cw)) /* odd parity is an error */
        {
          *errs=1;
          return(1);
        }
      if (syndrome(cw))
        {
          *errs=1;
          return(2);
        }
      else
        return(0); /* no errors */

    } /* decode */
};

/**
 * Convenience class that adapts Mr Wallace's implementation to the input/output format of DSD.
 * DSD works with data stored in char arrays where each element represents a bit (0 and 1 values).
 * The original implementation works with codewords that pack the 24 bits of information (12 data and
 * another 12 of parity) in an integer.
 */
class DSDGolay24 : public Golay24
{
public:
    unsigned int adapt_to_codeword(const char* word, unsigned int length, const char* parity)
    {
        unsigned int codeword = 0;

        // Data needs to be packed with the 12 bits of parity as the most significant
        // bits and 12 bits of data as the less significant. All these discovered by trial and error.
        for (unsigned int i=0; i<12; i++) {
            assert(parity[11-i] == 0 || parity[11-i] == 1);
            codeword <<= 1;
            codeword |= parity[11-i];
        }
        for (unsigned int i=0; i<length; i++) {
            char bit = word[length-1-i];
            assert(bit == 0 || bit == 1);
            codeword <<= 1;
            codeword |= bit;
        }
        // We only have length bits of data. We fill up the less significant bits of codeword with zeros.
        if (length < 12) {
            codeword <<= (12 - length);
        }

        return codeword;
    }

    void adapt_to_word(unsigned int codeword, char* word, unsigned int length)
    {
        // put back in word the bits from codeword
        for (unsigned int i=0, mask = 1<<(12-length); i<length; i++, mask<<=1) {
            word[i] = (codeword & mask) != 0? 1 : 0;
        }
    }

    unsigned int adapt_from_word(char* word, unsigned int length)
    {
        unsigned int codeword = 0;

        // encode the hex bits into a codeword
        for (unsigned int i=0; i<length; i++) {
            int bit = word[length-1-i];
            assert(bit == 0 || bit == 1);
            codeword <<= 1;
            codeword |= bit;
        }

        // We only have length bits of data. We fill up the less significant bits of codeword with zeros.
        if (length < 12) {
            codeword <<= (12-length);
        }

        return codeword;
    }

    /**
     * Important method that takes an hex word, a 12 bit parity word and uses the Golay24 implementation
     * to error correct it.
     * \param hex The data to error correct, packed in an array of 6 chars, representing a bit each.
     * \param parity The 12 bits of parity packed in an array of chars.
     * \param fixed_errors Output argument, returns the number of errors detected and fixed.
     * \return 1 if we were unable to correct the hex word. Too many errors detected.
     *           in this case the original data remains unchanged.
     *         0 if the data was successfully error corrected.
     */
    int decode_6(char* hex, const char* parity, int* fixed_errors)
    {
        unsigned int codeword = adapt_to_codeword(hex, 6, parity);
        // codeword now contains:
        // bits  0- 5: zeros
        // bits  6-11: hex bits
        // bits 12-23: golay (24,12) parity bits

        // bits are ordered from left to right, so bit 0 is the most significant and bit 23 is the less.

        int uncorrectable_errors = Golay24::decode(fixed_errors, &codeword);

        // codeword is now hopefully fixed

        // If there are uncorrectable errors and the fixed proposal includes ones
        // in bits 0-5 then probably the fix is not good and we discard it.
        // Bits 0-5 should always be zero, if there is a problem it's on the other bits
        // from 6 to 24.
        if (uncorrectable_errors == 1 && (codeword & 0x3f) != 0) {
            // discard, don't touch hex
        } else {
            // put it back into our hex format
            adapt_to_word(codeword, hex, 6);
            uncorrectable_errors = 0;
        }

        return uncorrectable_errors;
    }

    int decode_12(char* dodeca, const char* parity, int* fixed_errors)
    {
        unsigned int codeword = adapt_to_codeword(dodeca, 12, parity);
        // codeword contains:
        // bits  0-11: dodeca bits
        // bits 12-23: golay (24,12) parity bits

        // bits are ordered from left to right, so bit 0 is the most significant and bit 23 is the less.

        int uncorrectable_errors = Golay24::decode(fixed_errors, &codeword);

        // codeword is now hopefully fixed

        // If there are uncorrectable errors and the fixed proposal includes ones
        // in bits 0-5 then probably the fix is not good and we discard it.
        // Bits 0-5 should always be zero, if there is a problem it's on the other bits
        // from 6 to 24.
        if (uncorrectable_errors == 1 && (codeword & 0x3f) != 0) {
            // discard, don't touch hex
        } else {
            // put it back into our hex format
            adapt_to_word(codeword, dodeca, 12);
            uncorrectable_errors = 0;
        }

        return uncorrectable_errors;
    }

    void encode_6(char* hex, char* out_parity)
    {
        unsigned int data = adapt_from_word(hex, 6);
        unsigned int codeword = Golay24::encode(data);

        // Fill up the parity
        for (unsigned int i=0, mask = 1<<12; i<12; i++, mask<<=1) {
            out_parity[i] = (codeword & mask) != 0? 1 : 0;
        }
    }

    void encode_12(char* dodeca, char* out_parity)
    {
        unsigned int data = adapt_from_word(dodeca, 12);
        unsigned int codeword = Golay24::encode(data);

        // Fill up the parity
        for (unsigned int i=0, mask = 1<<12; i<12; i++, mask<<=1) {
            out_parity[i] = (codeword & mask) != 0? 1 : 0;
        }
    }
};

#endif // GOLAY24_HPP_7a68240afda9406facf81fcad3851111
