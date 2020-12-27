/* This is the xpsismo.c code. It is the main wrapper to the libsismo.c framework
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

// Driver code 
int main(int argc, char **argv) {

  int card = 0;
  
  /* evaluate command line arguments */
  argv++; 
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_sismo(-1);
  }

  /* parse the xpusb.ini file */
  if (ini_read(*argv)<0) exit_sismo(-2);
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  if (ini_signal_handler()<0) exit_sismo(-3);

  /* initialize sismo I/O data structure */
  if (ini_sismodata()<0) exit_sismo(-4);
  
  /* initialize UDP server */
  if (init_udp_server(sismoserver_ip,sismoserver_port) < 0) exit_sismo(-5);

  /* initialize UDP read thread */
  if (init_udp_receive() < 0) exit_sismo(-6);

  while (1) {

    if (read_sismo() < 0) exit_sismo(-7);

    
    if (sismo[card].inputs_changed[13] || sismo[card].inputs_changed[15]) {
      printf("%i %i changed: %i %i \n",sismo[card].inputs[13][0],sismo[card].inputs[15][0],
	     sismo[card].inputs_changed[13],sismo[card].inputs_changed[15]);
      //sismo[card].outputs[1] = sismo[card].inputs[13][0];
      //sismo[card].outputs_changed[1] = 1;
    }

    
    if (write_sismo() < 0) exit_sismo(-9);
	  
    if (reset_sismodata() < 0) exit_sismo(-10);
    usleep(INTERVAL*1000);
  }
  
  exit_udp();
  
  return 0; 
} 