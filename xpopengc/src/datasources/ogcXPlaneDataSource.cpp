/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
 
  Copyright (C) 2022 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):
      Reto Stockli
  
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
#include "xplanebeacon.h"
#include "wxrdata.h"
#include "check_aircraft.h"
}


extern int verbosity;
int verbose;

namespace OpenGC
{

extern FLTKRenderWindow* m_pRenderWindow;
  
void XPlaneDataSource::define_server(int port, string ip_address, int radardata)
{
  int n;
  
  // TCP Server (xpserver running as an X-Plane plugin)
  strncpy(XPlaneServerIP,ip_address.c_str(),ip_address.length()); 
  XPlaneServerPort = port;

  wxr_type = radardata;
  wxr_initialized = 0;
 
}

XPlaneDataSource::XPlaneDataSource()
{
  
  printf("XPlaneDataSource constructed\n");

  /* initialize local dataref structure */
  if (initialize_dataref(verbosity) < 0) exit(-8);
 
  /* initialize and start X-Plane Beacon reception */
  if (initialize_beacon_client(verbosity)<0) exit(-8);

  /* initialize TCP/IP interface */
  if (initialize_tcpip_client(verbosity) < 0) exit(-8);
  
  // initialize with default ACF
  SetAcfType(-1);

  verbose = verbosity;
  
}

XPlaneDataSource::~XPlaneDataSource()
{
  printf("Free TCP/IP socket and Dataref Structure \n");
  
  /* cancel tcp/ip connection */
  exit_tcpip_client();

  /* cancel beacon client */
  exit_beacon_client();
  
  /* cancel wxr udp socket */
  if (wxr_type > 0) {
    exit_wxr();
  }
  
  /* free local dataref structure */
  clear_dataref();

}

void XPlaneDataSource::OnIdle()
{

  int connected;
  
  /* check for TCP/IP connection to X-Plane */
  connected = check_xpserver();
  if (connected<0) exit(-9);
  
  /* receive data from X-Plane via TCP/IP */
  if (receive_xpserver()<0) exit(-10);
 
  /* determine Aircraft type based on Tail Number */
  check_aircraft();
  SetAcfType(acf_type);
 
  /* send data to X-Plane via TCP/IP */
  if (send_xpserver()<0) exit(-11);

  /* set avionics dataref to missing if connection was lost
     since this dataref is used to diagnose the connection
     in some of the gauges */
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
  if (connected == 0) {
    if (*avionics_on != INT_MISS) {
      *avionics_on = INT_MISS;
      numreceived = 1;
    }
  }

  /* WXR Data */
  if ((connected==1) && (GetAcfType() >= 0) && (wxr_type > 0)) {
    /* initialize UDP interface to read WXR data */
    init_wxr(XPlaneServerIP);
    
    /* Send WXR Init String */
    write_wxr();
    
    /* WXR Data Reading */
    read_wxr();
  } else {
    /* cancel WXR udp transfer and data structure if X-Plane is connected
       this is needed to re-initialize and re-send the init string with a new connection */
    exit_wxr();
  }
  
} // end "OnIdle()"

} // end namespace OpenGC
