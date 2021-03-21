/* This is the xparduino.c code. It is the main wrapper to libarduino.c, serverdata.c and handleserver.c
   which communicates flight data to/from the X-Plane flight simulator via TCP/IP interface 
   and connects to Arduino Boards with an Ethernet Shield via UDP.

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
#include "xparduino.h"
#include "ini.h"
#include "libarduino.h"
#include "handleudp.h"
#include "handleserver.h"
#include "serverdata.h"
#include "check_aircraft.h"
#include "test.h"
#include "compass.h"

int acf_type;

// Driver code 
int main(int argc, char **argv) {
  
  /* evaluate command line arguments */
  argv++; 
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_arduino(-1);
  }

  /* initialize local dataref structure */
  if (initialize_dataref()<0) exit_arduino(-1);

  /* parse the selected ini file */
  if (ini_read(*argv)<0) exit_arduino(-2);
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  if (ini_signal_handler()<0) exit_arduino(-3);

  /* initialize TCP/IP interface */
  if (initialize_tcpip()<0) exit_arduino(-4);

  /* initialize arduino I/O data structure */
  if (ini_arduinodata()<0) exit_arduino(-5);
  
  /* initialize UDP server */
  if (init_udp_server(arduinoserver_ip,arduinoserver_port) < 0) exit_arduino(-6);

  /* initialize UDP read thread */
  if (init_udp_receive() < 0) exit_arduino(-7);

  /* initialize arduino input pins */
  if (init_arduino()<0) exit_arduino(-7);

  while (1) {

    /* receive data from SISMO ards */
    if (read_arduino() < 0) exit_arduino(-8);
      
    /* check for TCP/IP connection to X-Plane */
    if (check_server()<0) exit_arduino(-9);
 
    /* receive data from X-Plane via TCP/IP */
    if (receive_server()<0) exit_arduino(-10);
    
    check_aircraft();
 
    /**** User Modules Follow Here ****/

    if (strcmp(*argv,"test") == 0) {
      test();
    }
    if (strcmp(*argv,"compass") == 0) {
      compass();
    }

    /**** User Modules End Here ****/
    
    /* send data to SISMO ards */
    if (write_arduino() < 0) exit_arduino(-11);

    /* send data to X-Plane via TCP/IP */
    if (send_server()<0) exit_arduino(-12);
    
      /* reset counters and such */
    if (reset_arduinodata() < 0) exit_arduino(-13);
    
    usleep(INTERVAL*1000);
  }
  
  return 0; 
} 
