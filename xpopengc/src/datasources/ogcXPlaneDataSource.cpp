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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#ifdef WIN
#include <winsock2.h>
#else
#include <sys/socket.h> 
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "ogcFLTKRenderWindow.h"
#include "ogcDataSource.h"
#include "ogcXPlaneDataSource.h"

extern "C" {
#include "handleserver.h"
#include "wxrdata.h"
#include "common.h"
}

extern int verbosity;

namespace OpenGC
{

extern FLTKRenderWindow* m_pRenderWindow;
  
void XPlaneDataSource::define_server(int port, string ip_address, int radardata)
{
  int n;

  printf("Contacting xpserver plugin at IP %s Port %i %i \n",ip_address.c_str(), port,radardata);
  // TCP Server (xpserver running as an X-Plane plugin)
  strncpy(server_ip,ip_address.c_str(),ip_address.length()); 
  server_port = port;

  /* initialize TCP/IP interface */
  if (initialize_tcpip() < 0) exit(-8);

  /* initialize UDP interface to read WXR data */
  init_wxr(radardata,server_ip);

}

XPlaneDataSource::XPlaneDataSource()
{
  
  printf("XPlaneDataSource constructed\n");

  /* initialize local dataref structure */
  if (initialize_dataref() < 0) exit(-8);
  
  // initialize with default ACF
  SetAcfType(0);

  verbose = verbosity;
  
}

XPlaneDataSource::~XPlaneDataSource()
{
  printf("Free TCP/IP socket and Dataref Structure \n");
  
  /* cancel tcp/ip connection */
  exit_tcpip();

  /* cancel wxr udp socket */
  exit_wxr();
  
  /* free local dataref structure */
  clear_dataref();

}

void XPlaneDataSource::OnIdle()
{

  int ret;
  
  /* check for TCP/IP connection to X-Plane */
  if (check_server()<0) exit(-9);
  
  /* receive data from X-Plane via TCP/IP */
  if (receive_server()<0) exit(-10);
 
  /* determine Aircraft type based on Tail Number */
  unsigned char *tailnum = link_dataref_byte_arr("sim/aircraft/view/acf_tailnum",  40, -1);
  if (tailnum) {
    //printf("ACF_TAILNUM: %s \n",tailnum);
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
  
  /* send data to X-Plane via TCP/IP */
  if (send_server()<0) exit(-11);

  /* WXR Data Reading */
  read_wxr();
  
} // end "OnIdle()"

} // end namespace OpenGC
