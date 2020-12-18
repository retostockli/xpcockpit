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
#include <signal.h>
#include <sys/time.h>

/* xpsismo headers */
#include "common.h"
#include "xpsismo.h"
#include "ini.h"
#include "libsismo.h"
#include "handleudp.h"

// Driver code 
int main(int argc, char **argv) {
 
  /* evaluate command line arguments */
  argv++; 
  argc--;
  if (argc != 1) {
    printf("Invalid number of arguments. Please only specify the initialization name. This is the prefix of one of the initialization file names found in the source subdirectory inidata/ or in the installation subdirectory share/.\n");
    exit_xpsismo(-1);
  }

  /* parse the xpusb.ini file */
  if (read_ini(*argv)<0) exit_xpsismo(-2);
  
  /* initialize handler for command-line interrupts (ctrl-c) */
  if (initialize_signal_handler()<0) exit_xpsismo(-3);

  /* initialize UDP server */
  if (init_udp_server(sismoserver_ip,sismoserver_port) < 0) exit_xpsismo(-4);

  /* initialize UDP read thread */
  if (init_udp_receive() < 0) exit_xpsismo(-5);

  while (1) {

    if (read_sismo() < 0) exit_xpsismo(-6);

    usleep(INTERVAL*1000);
  }
  
  exit_udp();
  
  return 0; 
} 

/* Exiting */
void exit_xpsismo(int ret)
{
  if (ret != 2) {
    printf("Exiting with status %i \n",ret);
    exit(ret);
  } else {
    /* A signal 2 means CTRL-C by user. All is fine */
    exit(0);
  }
}


/* signal handler in order to clean up when we're interrupted */
/* from the command line (ctrl-c) */
int initialize_signal_handler(void)
{
  int ret = 0;
  
  if (signal(SIGINT, exit_xpsismo) == SIG_ERR) {
    printf("Could not establish new signal handler.\n");
    ret = -1;
  }
  return ret;
}
