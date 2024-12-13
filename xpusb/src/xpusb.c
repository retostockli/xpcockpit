/* This is the xpusb.c code. It is the main wrapper to the libiocards.c framework
   which communicates flight data to/from the X-Plane flight simulator via TCP/IP interface 
   and connects to the OpenCockpits IOCARDS hardware via USB interface.

   Copyright (C) 2009 - 2014  Reto Stockli
   Adaptation to QPAC A320 - 2015 - Hans Jansen

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
#include "iocard_test.h"
#include "usbkeys_test.h"
#include "usbservos_test.h"
#include "dcmotorsplus_test.h"
#include "bu0836_test.h"

#include "check_aircraft.h"

#include "a320_pedestal_mip.h"	
#include "a320_overhead.h"	
#include "a320_mcdu_keys.h"	
#include "a320_chrono.h"

#include "b737_mcp.h"
#include "b737_efis.h"
#include "b737_pedestal.h"
#include "b737_throttle.h"
#include "b737_yokerudder.h"
#include "b737_overhead_fwd.h"

void a320_chrono (void);

/* Main program for data exchange between X-Plane and the OpenCockpits IOCARDS */
/* All input/output/motor/server addresses are zero-based (e.g. range from 0..ninputs-1) */
int main (int argc, char **argv)
{
 
  //struct timeval tval_before, tval_after, tval_result;

  /* print License terms */
  print_license();
  
  /* evaluate command line arguments */
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_xpusb(-1);
  }

  /* parse the xpusb.ini file */
  if (read_ini(argv[0],argv[1])<0) exit_xpusb(-1);
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  if (initialize_signal_handler()<0) exit_xpusb(-2);

  /* initialize and start X-Plane Beacon reception */
  if (initialize_beacon_client(verbose)<0) exit_xpusb(-3);
 
  /* initialize local dataref structure */
  if (initialize_dataref(verbose)<0) exit_xpusb(-4);

  /* initialize TCP/IP interface */
  if (initialize_tcpip_client(verbose)<0) exit_xpusb(-5);
  
  /* initialize USB/HID interface */
  if (initialize_usb()<0) exit_xpusb(-7);
  
  /* initialize OpenCockpits and Leo Bodnar's devices */
  if (initialize_iocards()<0) exit_xpusb(-8);
  
//  /* initialize OpenCockpits Chrono-A320 module */
//  if (initialize_chrono320() < 0) exit_xpusb(-15);

  /* initialize IOCards I/O data */
  if (initialize_iocardsdata()<0) exit_xpusb(-5);
  
  /* main loop */
  /* run until an error occurs or the signal handler deals with ctrl-c */
  for (;;)
    {
      //gettimeofday(&tval_before, NULL);

      /* receive data from MASTERCARD via USB */
      if (receive_mastercard()<0) exit_xpusb(-9);

      /* receive data from IOCardS-Keys */
      if (receive_keys()<0) exit_xpusb(-10);

      /* receive analog axis data from IOCard-USBServos */
      if (receive_axes()<0) exit_xpusb(-11);

      /* receive analog axis and digital input data from BU0836X card */
      if (receive_bu0836()<0) exit_xpusb(-12);
      
      /* check for TCP/IP connection to X-Plane */
      if (check_xpserver()<0) exit_xpusb(-13);
      
      /* receive data from Chrono-A320 module */
      if (receive_chrono320()<0) exit_xpusb(-16);
      
      /* receive data from X-Plane via TCP/IP */
      if (receive_xpserver()<0) exit_xpusb(-14);

      check_aircraft();
      
      /*** user-space modules begin here ***/
      if (strcmp(argv[1],"a320chrono") == 0) {
	a320_chrono();
      }
      if (strcmp(argv[1],"a320usb") == 0) {
	a320_overhead();
	a320_pedestal_mip();
	a320_chrono();
	a320_mcdu_keys();
      }
      if (strcmp("boeing737",argv[1]) == 0) {
	//	if (acf_type >= 0) {
	  b737_throttle();
	  b737_yokerudder();
	  b737_overhead_fwd();
	  //	}
      }
      if (strcmp("boeing737yokerudder",argv[1]) == 0) {
	b737_yokerudder();
      }
      if (strcmp("boeing737throttle",argv[1]) == 0) {
	b737_throttle();
      }
      if (strcmp("boeing737pedestal",argv[1]) == 0) {
	b737_pedestal();
      }
      if (strcmp("boeing737mcp",argv[1]) == 0) {
	b737_mcp();
	b737_efis();
      }
      if (strcmp("boeing737ovhfwd",argv[1]) == 0) {
	b737_overhead_fwd();
      }
      if (strcmp(argv[1],"default") == 0) {
	iocard_test();
      }
      if (strcmp(argv[1],"test") == 0) {
	//iocard_test();
	// usbkeys_test();
	//usbservos_test();
	// dcmotorsplus_test();
	bu0836_test();
	// dcmotorsplus_test();
      }
      /*** user-space modules end here ***/
      
      /* send data to MASTERCARD via USB */
      if (send_mastercard()<0) exit_xpusb(-20);
     
      /* send servo data to USBServos card */
      if (send_servos()<0) exit_xpusb(-21);

      /* send motor, servo and output data to DCMotors PLUS card */
      if (send_motors()<0) exit_xpusb(-22);
      
      /* send data from Chrono-A320 module */
      if (send_chrono320()<0) exit_xpusb(-25);
      
      /* send data to X-Plane via TCP/IP */
      if (send_xpserver()<0) exit_xpusb(-23);
      
      /* save a copy of the current I/O state */
      if (copy_iocardsdata()<0) exit_xpusb(-24);
     
      /* run xpusb data exchange (usb and tcp/ip) every 1000 microseconds */
      usleep(INTERVAL*1000);

    }

}	

/* Exiting */
void exit_xpusb(int ret)
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

  if (signal(SIGINT, exit_xpusb) == SIG_ERR) {
    printf("Could not establish new interrupt signal handler.\n");
    ret = -1;
  }
  if (signal(SIGTERM, exit_xpusb) == SIG_ERR) {
    printf("Could not establish new termination signal handler.\n");
    ret = -1;
  }
  return ret;
}
