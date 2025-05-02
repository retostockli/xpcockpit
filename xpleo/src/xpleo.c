/* This is the xpleo.c code. It communicates flight data to/from the X-Plane flight simulator via TCP/IP interface 
   and connects to the Leo Bodnar USB Joystick interfaces.

   Copyright (C) 2025  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* system headers */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

/* xpusb headers */
#include "common.h"
#include "xpusb.h"
#include "libiocards.h"
#include "serverdata.h"
#include "handleserver.h"
#include "xplanebeacon.h"

/* headers to user space modules */
#include "bu0836_test.h"
#include "b737_yokerudder.h"

/* Main program for data exchange between X-Plane and the OpenCockpits IOCARDS */
/* All input/output/motor/server addresses are zero-based (e.g. range from 0..ninputs-1) */
int main (int argc, char **argv)
{

  /* Loop interval [ms] */
  interval = 5;
  
  /* print License terms */
  print_license();
  
  /* evaluate command line arguments */
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_xpleo(-1);
  }

  /* parse the xpleo.ini file */
  if (read_ini(argv[0],argv[1])<0) exit_xpleo(-1);
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  if (initialize_signal_handler()<0) exit_xpleo(-2);

  /* initialize and start X-Plane Beacon reception */
  if (initialize_beacon_client(verbose)<0) exit_xpleo(-3);
 
  /* initialize local dataref structure */
  if (initialize_dataref(verbose)<0) exit_xpleo(-4);

  /* initialize TCP/IP interface */
  if (initialize_tcpip_client(verbose)<0) exit_xpleo(-5);
  
  /* initialize USB/HID interface */
  if (initialize_usb()<0) exit_xpleo(-7);
  
  /* initialize Leo Bodnar's devices */
  if (initialize_leo()<0) exit_xpleo(-8);

  /* initialize Leo Bodnar I/O data */
  if (initialize_leodata()<0) exit_xpleo(-5);
  
  /* main loop */
  /* run until an error occurs or the signal handler deals with ctrl-c */
  for (;;)
    {
      //gettimeofday(&tval_before, NULL);

      /* receive analog axis and digital input data from BU0836X card */
      if (receive_bu0836()<0) exit_xpleo(-12);
      
      /* check for TCP/IP connection to X-Plane */
      if (check_xpserver()<0) exit_xpleo(-13);
      
      /* receive data from X-Plane via TCP/IP */
      if (receive_xpserver()<0) exit_xpleo(-14);

      check_aircraft();

      /*** user-space modules start here ***/
      if (strcmp("boeing737",argv[1]) == 0) {
	b737_yokerudder();
      }
      if (strcmp(argv[1],"test") == 0) {
	bu0836_test();
      }
      /*** user-space modules end here ***/
      
      /* send data to X-Plane via TCP/IP */
      if (send_xpserver()<0) exit_xpleo(-23);
      
      /* save a copy of the current I/O state */
      if (copy_leodata()<0) exit_xpleo(-24);
     
      /* run xpleo data exchange (usb and tcp/ip) every 1000 microseconds */
      usleep(interval*1000);

    }

}	

/* Exiting */
void exit_xpleo(int ret)
{

  /* cancel USB connection */
  terminate_usb();

  /* cancel tcp/ip connection */
  exit_tcpip_client();

  /* cancel beacon client */
  exit_beacon_client();

  /* free local dataref structure */
  clear_dataref();

  if (ret != 2) {
    printf("Exiting with status %i \n",ret);
    exit(ret);
  } else {
    /* A signal 2 means CTRL-C by user. All is fine */
    exit(0);
  }
}


/* signal handler in order to clean up when we're interrupted */
/* from the command line (ctrl-c) or from kill on command line */
int initialize_signal_handler(void)
{
  int ret = 0;

  if (signal(SIGINT, exit_xpleo) == SIG_ERR) {
    printf("Could not establish new interrupt signal handler.\n");
    ret = -1;
  }
  if (signal(SIGTERM, exit_xpleo) == SIG_ERR) {
    printf("Could not establish new termination signal handler.\n");
    ret = -1;
  }
  return ret;
}
