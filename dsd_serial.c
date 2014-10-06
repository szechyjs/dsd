#ifndef _WIN32
#include <termios.h>
#endif
#include "dsd.h"

void
openSerial (dsd_opts * opts, dsd_state * state)
{

#ifdef USE_WINDOWS_SERIAL
  DCB dcb = {0};

  opts->serial_h = CreateFile (opts->serial_dev, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
  if (opts->serial_h == INVALID_HANDLE_VALUE)
    {
      printf ("Error, couldn't open %s\n", opts->serial_dev);
      exit (1);
    }

   if (!GetCommState(opts->serial_h, &dcb))
    {
      printf ("Error, couldn't get settings from %s\n", opts->serial_dev);
      exit (1);
    }
   dcb.BaudRate = CBR_115200 ;
   switch (opts->serial_baud)
    {
    case 1200:
      dcb.BaudRate = CBR_1200;
    case 2400:
      dcb.BaudRate = CBR_2400;
    case 4800:
      dcb.BaudRate = CBR_4800;
    case 9600:
      dcb.BaudRate = CBR_9600;
      break;
    case 19200:
      dcb.BaudRate = CBR_19200;
      break;
    case 38400:
      dcb.BaudRate = CBR_38400;
      break;
    case 57600:
      dcb.BaudRate = CBR_57600;
      break;
    case 115200:
      dcb.BaudRate = CBR_115200;
      break;
    }
    if (!SetCommState(opts->serial_h, &dcb))
    {
      printf ("Error, couldn't set settings to %s\n", opts->serial_dev);
      exit (1);
    }
#else
  struct termios tty;
  speed_t baud;

  printf ("Opening serial port %s and setting baud to %i\n", opts->serial_dev, opts->serial_baud);
  opts->serial_fd = open (opts->serial_dev, O_WRONLY);
  if (opts->serial_fd == -1)
    {
      printf ("Error, couldn't open %s\n", opts->serial_dev);
      exit (1);
    }

  tty.c_cflag = 0;

  baud = B115200;
  switch (opts->serial_baud)
    {
    case 1200:
      baud = B1200;
    case 2400:
      baud = B2400;
    case 4800:
      baud = B4800;
    case 9600:
      baud = B9600;
      break;
    case 19200:
      baud = B19200;
      break;
    case 38400:
      baud = B38400;
      break;
    case 57600:
      baud = B57600;
      break;
    case 115200:
      baud = B115200;
      break;
    case 230400:
      baud = B230400;
      break;
    }
  if (opts->serial_baud > 0)
    {
      cfsetospeed (&tty, baud);
      cfsetispeed (&tty, baud);
    }

  tty.c_cflag |= (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_iflag = IGNBRK;
  tty.c_lflag = 0;
  tty.c_oflag = 0;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_cflag &= ~(PARENB | PARODD);
  tty.c_cflag &= ~CSTOPB;
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 5;

  tcsetattr (opts->serial_fd, TCSANOW, &tty);
#endif

}

void
resumeScan (dsd_opts * opts, dsd_state * state)
{

  char cmd[16];
#ifdef USE_WINDOWS_SERIAL
  BOOL result;

  if (opts->serial_h != INVALID_HANDLE_VALUE)
    {
      sprintf (cmd, "\rKEY00\r");
      result = WriteFile (opts->serial_h, cmd, 7, NULL, NULL);
      cmd[0] = 2;
      cmd[1] = 75;
      cmd[2] = 15;
      cmd[3] = 3;
      cmd[4] = 93;
      cmd[5] = 0;
      result = WriteFile (opts->serial_h, cmd, 5, NULL, NULL);
      state->numtdulc = 0;
    }
#else
  ssize_t result;

  if (opts->serial_fd > 0)
    {
      sprintf (cmd, "\rKEY00\r");
      result = write (opts->serial_fd, cmd, 7);
      cmd[0] = 2;
      cmd[1] = 75;
      cmd[2] = 15;
      cmd[3] = 3;
      cmd[4] = 93;
      cmd[5] = 0;
      result = write (opts->serial_fd, cmd, 5);
      state->numtdulc = 0;
    }
#endif

}
