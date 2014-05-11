
#include "dsd.h"
#include "p25p1_const.h"
#include "p25p1_hdu.h"
#include "p25p1_check_ldu.h"
#include "p25p1_heuristics.h"

#ifdef TRACE_DSD
static void
debug_write_label_imbe(dsd_state* state, unsigned int cc, int bitindex, char bit)
{
  float left, right;

  if (state->debug_label_imbe_file == NULL)
    {
      state->debug_label_imbe_file = fopen ("pp_label_imbe.txt", "w");
    }
  left = (state->debug_sample_index-10) / 48000.0F;
  right = (state->debug_sample_index) / 48000.0F;
  fprintf(state->debug_label_imbe_file, "%f\t%f\tC_%i[%i]=%c\n", left, right, cc, bitindex, (bit + '0'));
}
#endif

/**
 * Logs the IMBE's c0-c6 words.
 */
static void
debug_log_imbe(char imbe_fr[8][23])
{
  int i, j;
  printf("    ");

  for (j=0; j<4; j++)
    {
        printf("{");
        for (i=0; i<23; i++)
          {
            if (i>0)
              {
                printf(", ");
              }
            printf("%c", (imbe_fr[j][i] + '0'));
          }
        printf("}; ");
    }

  for (j=4; j<7; j++)
    {
      printf("{");
      for (i=0; i<15; i++)
        {
          if (i>0)
            {
              printf(", ");
            }
          printf("%c", (imbe_fr[j][i] + '0'));
        }
      printf("}; ");
    }

  printf("\n");
}

void
process_IMBE (dsd_opts* opts, dsd_state* state, int* status_count)
{
  int j, dibit, status;
  char imbe_fr[8][23];
  const int *w, *x, *y, *z;

  w = iW;
  x = iX;
  y = iY;
  z = iZ;

#ifdef TRACE_DSD
  state->debug_prefix = 'I';
#endif

  for (j = 0; j < 72; j++)
    {
      if (*status_count == 35)
        {
          // Skip the status symbol
#ifdef TRACE_DSD
          state->debug_prefix = 's';
#endif
          status = getDibit (opts, state);
          // TODO: do something useful with the status bits...
          *status_count = 1;

#ifdef TRACE_DSD
          state->debug_prefix = 'I';
#endif
        }
      else
        {
          (*status_count)++;
        }
      dibit = getDibit (opts, state);
      imbe_fr[*w][*x] = (1 & (dibit >> 1)); // bit 1
      imbe_fr[*y][*z] = (1 & dibit);        // bit 0

#ifdef TRACE_DSD
      if (*w == 0) {
          debug_write_label_imbe(state, 0, *x, (1 & (dibit >> 1)));
      }
      if (*y == 0) {
          debug_write_label_imbe(state, 0, *z, (1 & dibit));
      }
#endif

      w++;
      x++;
      y++;
      z++;
    }

#ifdef TRACE_DSD
  state->debug_prefix = '\0';
#endif

  if (state->p25kid == 0 || opts->unmute_encrypted_p25 == 1)
    {
      {
          // Check for a non-standard c0 transmitted
          // This is explained here: https://github.com/szechyjs/dsd/issues/24
          char non_standard_word[23] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0};
          int match = 1;
          unsigned int i;
          for (i=0; i<23; i++) {
              if (imbe_fr[0][i] != non_standard_word[i]) {
                  match = 0;
                  break;
              }
          }

          if (match) {
              // Skip this particular value. If we let it pass it will be signaled as an erroneus IMBE
              printf("(Non-standard IMBE c0 detected, skipped)");
          } else {
              processMbeFrame (opts, state, imbe_fr, NULL, NULL);
          }
      }
    }
}

// Uncomment this line for verbose information on the error correction of the LDU bits.
//#define LDU_DEBUG

void
read_and_correct_hex_word (dsd_opts* opts, dsd_state* state, char* hex, int* status_count,
        AnalogSignal* analog_signal_array, int* analog_signal_index)
{
  char parity[4];
  int error_count;

  // Read the hex word
  read_word (opts, state, hex, 6, status_count, analog_signal_array, analog_signal_index);
  // Read the parity
  read_hamm_parity (opts, state, parity, status_count, analog_signal_array, analog_signal_index);

#ifdef LDU_DEBUG
  printf("[");
  for (i = 0; i < 6; i++)
    {
      printf("%c", (hex[i] == 1)? 'X' : ' ');
    }
  printf("-");
  for (i = 0; i < 4; i++)
    {
      printf("%c", (parity[i] == 1)? 'X' : ' ');
    }
  printf("]");
#endif

  // Use Hamming to error correct the hex word
  error_count = check_and_fix_hamming_10_6_3(hex, parity);

  if (error_count == 1)
    {
      state->debug_header_errors++;
    }
  else if (error_count ==2)
    {
      state->debug_header_critical_errors++;
    }

#ifdef LDU_DEBUG
  printf(" -> [");
  for (i = 0; i < 6; i++)
    {
      printf("%c", (hex[i] == 1)? 'X' : ' ');
    }
  printf("]");
  if (error_count == 1)
    {
      printf(" fixed!");
    }
  else if (error_count ==2)
    {
      printf(" IRRECOVERABLE");
    }
  printf("\n");
#endif
}

void
correct_hamming_dibits(char* data, int count, AnalogSignal* analog_signal_array)
{
  char parity[4];
  int i, j;
  int analog_signal_index;

  analog_signal_index = 0;

  for (i=count-1; i>=0; i--)
    {
      // Take the next 3 dibits (1 hex word) as they are
      for (j=0; j<6; j+=2)  // 3 iterations -> 3 dibits
        {
          int dibit = (data[i*6+j] << 1) | data[i*6+j+1];
          // This dibit is the correct value we should have read in the first place
          analog_signal_array[analog_signal_index].corrected_dibit = dibit;

#ifdef HEURISTICS_DEBUG
          if (analog_signal_array[analog_signal_index].dibit != dibit)
            {
              printf("LDU word corrected from %i to %i, analog value %i\n",
                      analog_signal_array[analog_signal_index].dibit, dibit, analog_signal_array[analog_signal_index].value);
            }
#endif

          analog_signal_index++;
        }

      // The next two dibits are calculated has the hamming parity of the hex word
      encode_hamming_10_6_3(data+i*6, parity);

      for (j=0; j<4; j+=2)  // 2 iterations -> 2 dibits
        {
          int dibit = (parity[j] << 1) | parity[j+1];
          // Again, this dibit is the correct value we should have read in the first place
          analog_signal_array[analog_signal_index].corrected_dibit = dibit;

#ifdef HEURISTICS_DEBUG
          if (analog_signal_array[analog_signal_index].dibit != dibit)
            {
              printf("LDU-HM parity corrected from %i to %i, analog value %i\n",
                      analog_signal_array[analog_signal_index].dibit, dibit, analog_signal_array[analog_signal_index].value);
            }
#endif

          analog_signal_index++;
        }
    }
}

void
debug_ldu_header(dsd_state* state)
{
#ifdef TRACE_DSD
  float s = state->debug_sample_index / 48000.0F;
  printf("Start of LDU at sample %f\n", s);
#endif

  debug_print_heuristics(&state->p25_heuristics);
}

