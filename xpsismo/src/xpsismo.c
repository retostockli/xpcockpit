/* This is the xpsismo.c code. It is the main wrapper to libsismo.c, serverdata.c and handleserver.c
   which communicates flight data to/from the X-Plane flight simulator via TCP/IP interface 
   and connects to the SISMO SOLUCIONES hardware via UDP.

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

/* xpsismo headers */
#include "common.h"
#include "xpsismo.h"
#include "ini.h"
#include "libsismo.h"
#include "handleudp.h"
#include "handleserver.h"
#include "serverdata.h"
#include "check_aircraft.h"
#include "test.h"
#include "b737_mcp.h"
#include "b737_efis.h"
#include "b737_mip.h"

int acf_type;

// Driver code 
int main(int argc, char **argv) {
  
  /* evaluate command line arguments */
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_sismo(-1);
  }

  /* initialize local dataref structure */
  if (initialize_dataref()<0) exit_sismo(-1);

  /* parse the selected ini file */
  if (ini_read(argv[0],argv[1])<0) exit_sismo(-2);
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  if (ini_signal_handler()<0) exit_sismo(-3);

  /* initialize TCP/IP interface */
  if (initialize_tcpip()<0) exit_sismo(-4);

  /* initialize sismo I/O data structure */
  if (ini_sismodata()<0) exit_sismo(-5);
  
  /* initialize UDP server */
  if (init_udp_server(sismoserver_ip,sismoserver_port) < 0) exit_sismo(-6);

  /* initialize UDP read thread */
  if (init_udp_receive() < 0) exit_sismo(-7);

  while (1) {

    /* receive data from SISMO cards */
    if (read_sismo() < 0) exit_sismo(-8);
      
    /* check for TCP/IP connection to X-Plane */
    if (check_server()<0) exit_sismo(-9);
 
    /* receive data from X-Plane via TCP/IP */
    if (receive_server()<0) exit_sismo(-10);
    
    check_aircraft();
 
    /**** User Modules Follow Here ****/

    if (strcmp(argv[1],"test") == 0) {
      //      if (acf_type >= 0) {
	test();
	//      }
    }
    if (strcmp("boeing737mcp",argv[1]) == 0) {
      if (acf_type >= 0) {
	b737_mcp();
	b737_efis(0); /* captain */
	b737_efis(1); /* copilot */
      }
    }
    if (strcmp("boeing737mip",argv[1]) == 0) {
      if (acf_type >= 0) {
	b737_mip();
      }
    }
    if (strcmp("boeing737",argv[1]) == 0) {
      if (acf_type >= 0) {
	b737_mcp();
	b737_efis(0); /* captain */
	b737_efis(1); /* copilot */
	b737_mip();
      }
    }

    /**** User Modules End Here ****/
    
    /* send data to SISMO cards */
    if (write_sismo() < 0) exit_sismo(-11);

    /* send data to X-Plane via TCP/IP */
    if (send_server()<0) exit_sismo(-12);
    
      /* reset counters and such */
    if (reset_sismodata() < 0) exit_sismo(-13);
    
    usleep(INTERVAL*1000);
  }
  
  return 0; 
} 
