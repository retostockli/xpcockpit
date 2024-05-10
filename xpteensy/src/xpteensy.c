/* This is the xpteensy.c code. It is the main wrapper to libteensy.c, serverdata.c and handleserver.c
   which communicates flight data to/from the X-Plane flight simulator via TCP/IP interface 
   and connects to Teensy Boards with an Ethernet Shield via UDP.

   Copyright (C) 2021-2024  Reto Stockli

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

/* xpteensy headers */
#include "common.h"
#include "xpteensy.h"
#include "ini.h"
#include "libteensy.h"
#include "handleudp.h"
#include "handleserver.h"
#include "serverdata.h"
#include "xplanebeacon.h"
#include "test.h"

// Driver code 
int main(int argc, char **argv) {
  
  /* evaluate command line arguments */
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_teensy(-1);
  }
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  if (ini_signal_handler()<0) exit_teensy(-1);

  /* parse the selected ini file */
  if (ini_read(argv[0],argv[1])<0) exit_teensy(-2);

  /* initialize and start X-Plane Beacon reception */
  if (initialize_beacon_client(verbose)<0) exit_teensy(-3);
 
  /* initialize local dataref structure */
  if (initialize_dataref(verbose)<0) exit_teensy(-4);

  /* initialize TCP/IP interface */
  if (initialize_tcpip_client(verbose)<0) exit_teensy(-5);

  /* initialize teensy I/O data structure */
  if (ini_teensydata()<0) exit_teensy(-5);
  
  /* initialize UDP server */
  if (init_udp_server(teensyserver_ip,teensyserver_port) < 0) exit_teensy(-6);

  /* initialize UDP read thread */
  if (init_udp_receive() < 0) exit_teensy(-7);

  /* initialize teensy input pins */
  if (init_teensy()<0) exit_teensy(-7);

  while (1) {

    /* receive data from SISMO ards */
    if (read_teensy() < 0) exit_teensy(-8);
      
    /* check for TCP/IP connection to X-Plane */
    if (check_xpserver()<0) exit_teensy(-9);
 
    /* receive data from X-Plane via TCP/IP */
    if (receive_xpserver()<0) exit_teensy(-10);
    
    check_aircraft();
 
    /**** User Modules Follow Here ****/

    if (strcmp(argv[1],"test") == 0) {
      test();
    }

    /**** User Modules End Here ****/
    
    /* send data to SISMO ards */
    if (write_teensy() < 0) exit_teensy(-11);

    /* send data to X-Plane via TCP/IP */
    if (send_xpserver()<0) exit_teensy(-12);
    
      /* reset counters and such */
    if (reset_teensydata() < 0) exit_teensy(-13);
    
    usleep(INTERVAL*1000);
  }
  
  return 0; 
} 
