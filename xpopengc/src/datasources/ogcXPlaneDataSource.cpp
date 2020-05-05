/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcXPlaneDataSource.cpp,v $

  Copyright (C) 2001-2 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):
      Reto Stockli

  Last modification:
    Date:      $Date: 2015/10/06 $
    Version:   $Revision: $
    Author:    $Author: stockli $
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================*/

#include <stdio.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <math.h>

#include "ogcFLTKRenderWindow.h"
#include "ogcDataSource.h"
#include "ogcXPlaneDataSource.h"

extern "C" {
#include "handleserver.h"
#include "handleudp.h"
#include "common.h"
}

extern int verbosity;

namespace OpenGC
{

extern FLTKRenderWindow* m_pRenderWindow;

void signal_handler(int sigraised)
{
  printf("Interrupted ... exiting \n");
  exit(0);
}
  
void XPlaneDataSource::define_server(int port, string ip_address, int maxradar)
{
  int n;

  printf("Contacting xpserver plugin at IP %s Port %i \n",ip_address.c_str(), port);
  // TCP Server (xpserver running as an X-Plane plugin)
  strncpy(server_ip,ip_address.c_str(),ip_address.length()); 
  server_port = port;

  // UDP Server (native X-Plane UDP server)
  strcpy(udpServerIP,ip_address.c_str());
  udpServerPort = 49000;
  m_maxradar = maxradar;

  /* initialize TCP/IP interface */
  if (initialize_tcpip() < 0) exit(-8);

  /* initialize UDP socket if needed for WXR data from X-Plane*/
  if (m_maxradar > 0 ) {
    n = (int) log10(m_maxradar) + 1; // number of characters of MAXRADAR
    
    int sendlen = 7+n; // RADR plus \0 plus the # of radar returns plus \0
    int recvlen = 5+13*m_maxradar+1; // RADR plux 13 bytes * # of radar returns plus \0
    
    allocate_udpdata(sendlen,recvlen);
    
    if (init_udp() < 0) exit(-8);

    /* generate send message */
    snprintf(udpSendBuffer, sendlen,"RADR %i",m_maxradar);
    udpSendBuffer[4]='\0';
    udpSendBuffer[6+n]='\0';

  }
}

XPlaneDataSource::XPlaneDataSource()
{
/* TCP CLIENT VARS */					  
  sig_t sig_handler;
  
  printf("XPlaneDataSource constructed\n");

  /* initialize local dataref structure */
  if (initialize_dataref() < 0) exit(-8);

  // Set up a signal handler so we can clean up when we're interrupted from the command line
  sig_handler = signal(SIGINT, signal_handler);
  if (sig_handler == SIG_ERR) {
    printf("Could not establish new signal handler.\n");
  }
  
  // initialize with default ACF
  SetAcfType(0);

  // No UDP request sent yet
  m_sent = false;

  verbose = verbosity;
  
}

XPlaneDataSource::~XPlaneDataSource()
{
  printf("Free TCP/IP socket and Dataref Structure \n");
  
  /* cancel tcp/ip connection */
  exit_tcpip();

  /* cancel udp socket */
  exit_udp();
  
  /* free local dataref structure */
  clear_dataref();

}

void XPlaneDataSource::OnIdle()
{

  int ret;
 
  /* determine Aircraft type based on Tail Number */
  unsigned char *tailnum = link_dataref_byte_arr("sim/aircraft/view/acf_tailnum",  40, -1);
  if (tailnum) {
    // printf("ACF_TAILNUM: %s \n",tailnum);
    if (strcmp((const char*) tailnum,"ZB738")==0) {
      SetAcfType(3); // ZIBO
    } else if (strcmp((const char*) tailnum,"NN816N")==0) {
      SetAcfType(2); // Laminar 737
    } else if (strcmp((const char*) tailnum,"OY-GRL")==0) {
      SetAcfType(1); // x737
    } else if (strcmp((const char*) tailnum,"D-ATUC")==0) {
      SetAcfType(1); // x737
    } else {
      SetAcfType(0); // other
    } 
  } else {
    SetAcfType(0);
  }
  
  /* check for TCP/IP connection to X-Plane */
  if (check_server()<0) exit(-9);
  
  /* receive data from X-Plane via TCP/IP */
  if (receive_server()<0) exit(-10);

  /* send data to X-Plane via TCP/IP */
  if (send_server()<0) exit(-11);

  /* request WXR data if needed */
  if (m_maxradar > 0) {

    
    if (!m_sent) {
      // request radar only once at start, you will then get the data forever
      ret = send_udp();
      printf("Sent Request for WXR Data to X-Plane: %i \n",ret);
      m_sent = true;
    }
   
    if (m_sent) {
      ret = recv_udp();

      if (ret > 0) {
	// READING DONE IN NAV GAUGE
      } else {
	//printf("No WXR return %i \n",ret);
	memset(udpRecvBuffer,0,udpRecvBufferLen);
      }
      
    }

  }
  
} // end "OnIdle()"

} // end namespace OpenGC
