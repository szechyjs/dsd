#include "dsd.h"
#include "options.h"

void
processNXDNData (dsd_opts * opts, dsd_state * state)
{
  int i, dibit;

  if (Options::Instance().GetErrorBars())
    {
      printf ("DATA    ");
    }

  for (i = 0; i < 30; i++)
    {
      dibit = getDibit (opts, state);
#ifdef NXDN_DUMP
      printf ("%c", dibit + 48);
#endif
    }
#ifdef NXDN_DUMP
  printf (" ");
#endif

  for (i = 0; i < 144; i++)
    {
      dibit = getDibit (opts, state);
#ifdef NXDN_DUMP
      printf ("%c", dibit + 48);
#endif
    }

  if (Options::Instance().GetErrorBars())
    {
      printf ("\n");
    }
}
