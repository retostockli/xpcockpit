/* This is the xppi.c code. It is the main wrapper to serverdata.c and handleserver.c
   which communicates flight data to/from the X-Plane flight simulator via TCP/IP interface 
   and connects to the Rasperry PI GPIO.

   Copyright (C) 2021  Reto Stockli

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
#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/time.h>

/* xparduino headers */
#include "common.h"
#include "xppi.h"
#include "ini.h"
#include "handleserver.h"
#include "serverdata.h"
#include "check_aircraft.h"
#include "xplanebeacon.h"
#include "test.h"
#include "b737_fmc.h"
#include "b737_compass.h"
#include "b737_awm.h"
#include "b737_yawdamper.h"

// Driver code 
int main(int argc, char **argv) {
  
  /* evaluate command line arguments */
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_pi(-1);
  }


  /* parse the selected ini file */
  if (ini_read(argv[0],argv[1])<0) exit_pi(-2);

  /* initialize and start X-Plane Beacon reception */
  if (initialize_beacon_client(verbose)<0) exit_pi(-3);
 
  /* initialize local dataref structure */
  if (initialize_dataref(verbose)<0) exit_pi(-4);

  /* initialize TCP/IP interface */
  if (initialize_tcpip_client(verbose)<0) exit_pi(-5);

  /* initialize wiringPi Library */
  if (init_pi()<0) exit_pi(-4);
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  /* needs to be put after initalizing pi */
  if (ini_signal_handler()<0) exit_pi(-1);
 
  /* initialize modules */
  if (strcmp(argv[1],"test") == 0) {
    if (test_init()<0) exit_pi(-5);
  }
  if (strcmp(argv[1],"boeing737mip") == 0) {
    if (b737_compass_init()<0) exit_pi(-5);
    if (b737_awm_init()<0) exit_pi(-5);
    if (b737_yawdamper_init()<0) exit_pi(-5);
  }
  if (strcmp(argv[1],"boeing737compass") == 0) {
    if (b737_compass_init()<0) exit_pi(-5);
  }
  if (strcmp(argv[1],"boeing737awm") == 0) {
    if (b737_awm_init()<0) exit_pi(-5);
  }
  if (strcmp(argv[1],"boeing737yawdamper") == 0) {
    if (b737_yawdamper_init()<0) exit_pi(-5);
  }
  if ((strcmp(argv[1],"boeing737fmc") == 0) ||
      (strcmp(argv[1],"boeing737fmc1") == 0) ||
      (strcmp(argv[1],"boeing737fmc2") == 0)) {
    if (b737_fmc_init()<0) exit_pi(-5);
  }

  while (1) {
      
    /* check for TCP/IP connection to X-Plane */
    if (check_xpserver()<0) exit_pi(-9);
 
    /* receive data from X-Plane via TCP/IP */
    if (receive_xpserver()<0) exit_pi(-10);
    
    check_aircraft();
 
    /**** User Modules Follow Here ****/

    if (strcmp(argv[1],"test") == 0) {
      test();
    }
    if (strcmp(argv[1],"boeing737mip") == 0) {
      b737_compass();
      b737_awm();
      b737_yawdamper();
    }
    if (strcmp(argv[1],"boeing737compass") == 0) {
      b737_compass();
    }
    if (strcmp(argv[1],"boeing737awm") == 0) {
      b737_awm();
    }
    if (strcmp(argv[1],"boeing737yawdamper") == 0) {
      b737_yawdamper();
    }

    if ((strcmp(argv[1],"boeing737fmc") == 0) ||
        (strcmp(argv[1],"boeing737fmc1") == 0) ||
        (strcmp(argv[1],"boeing737fmc2") == 0)) {
      b737_fmc();
    }

    /**** User Modules End Here ****/
 
    /* send data to X-Plane via TCP/IP */
    if (send_xpserver()<0) exit_pi(-12);
   
    usleep(INTERVAL*1000);
  }
  
  return 0; 
} 
