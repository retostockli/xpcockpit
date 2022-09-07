/* This is the xpclient code. It is a sample client for the idea of an Open Source Data Interface 
   for X-Plane which communicates flight data to/from the X-Plane flight 
   simulator.

   This code here requires the related X-Plane plugin xpserver to operate.

   Copyright (C) 2009 - 2019 Reto Stockli

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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>

#include "xpclient.h"
#include "serverdata.h"
#include "handleserver.h"
#include "xplanebeacon.h"

int main (int argc,char **argv)
{
  
  /* server settings */
  memset(XPlaneServerIP,0,sizeof(XPlaneServerIP));
  //strcpy(XPlaneServerIP,"127.0.0.1");
  XPlaneServerPort = 8091;
  memset(clientname,0,sizeof(clientname));
  strcpy(clientname,"xpclient");

  int verbose=1;  
  int interval=500;  /* poll interval in milliseconds: set between 1-999 milliseconds
			 (>= 1000 ms will not work with some usleep implementations) */

  /* initialize handler for command-line interrupts (ctrl-c) */
  if (initialize_signal_handler()<0) exit_xpclient(-3);

  /* initialize and start X-Plane Beacon reception */
  if (initialize_beacon_client(verbose)<0) exit_xpclient(-4);
  
  /* initialize local dataref structure */
  if (initialize_dataref(verbose)<0) exit_xpclient(-6);

  /* initialize TCP/IP interface */
  if (initialize_tcpip_client(verbose)<0) exit_xpclient(-7);
  
  /* subscribe to specific datarefs */

  // a custom dataref which can be accessed by other plugins/clients
  int *custom = link_dataref_int("xpserver/customdataref");
  // the plane's latitude
  double *latitude = link_dataref_dbl("sim/flightmodel/position/latitude", -3);
  // the EGT of engine 0
  float *egt0 =  link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 0, 0);
  // the EGT of engine 1
  float *egt1 =  link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 1, 0);
 
    
  unsigned char *acf_tailnum   = link_dataref_byte_arr("sim/aircraft/view/acf_tailnum", 100, -1);  

  int *nav_shows_dem = link_dataref_int("xpserver/EFIS_capt_terr");

  
  /*
  unsigned char *fmc1 = link_dataref_byte_arr("laminar/B738/fmc1/Line02_X", 40,-1);

  float *test = link_dataref_flt("x737/cockpit/fireSupPanel/fireWarnTestSw_f",-3);
  
  float *test = link_dataref_flt("FJCC/UFMC/V1",-3);

  float *present = link_dataref_flt("FJCC/UFMC/PRESENT",-1);
  float *lon = link_dataref_flt("FJCC/UFMC/Waypoint/Lon",-3);
  float *lat = link_dataref_flt("FJCC/UFMC/Waypoint/Lat",-3);
  int *idx = link_dataref_int("FJCC/UFMC/Waypoint/Index");
  int *nidx = link_dataref_int("FJCC/UFMC/Waypoint/Number_of_Waypoints");
  */

  /*
  float *bearing = link_dataref_flt_arr("sim/cockpit2/tcas/indicators/relative_bearing_degs",20,19,-2);

  double *x1 = link_dataref_dbl("sim/multiplayer/position/plane19_x",-2);
  double *x = link_dataref_dbl("sim/flightmodel/position/local_x",-2);
  double *y1 = link_dataref_dbl("sim/multiplayer/position/plane19_y",-2);
  double *y = link_dataref_dbl("sim/flightmodel/position/local_y",-2);
  double *z1 = link_dataref_dbl("sim/multiplayer/position/plane19_z",-2);
  double *z = link_dataref_dbl("sim/flightmodel/position/local_z",-2);
  
  float *framerate = link_dataref_flt("sim/operation/misc/frame_rate_period",FLT_MISS);

  int *n1 = link_dataref_int("laminar/B738/autopilot/n1_status");

  float *parkbrake = link_dataref_flt("sim/flightmodel/controls/parkbrake",-2);
  */

  /* main loop */
  /* run until an error occurs or the signal handler deals with ctrl-c */
  for (;;)
    {

      /* check for TCP/IP connection to X-Plane */
      if (check_xpserver()<0) exit_xpclient(-11);
      
      /* receive data from X-Plane via TCP/IP */
      if (receive_xpserver()<0) exit_xpclient(-12);
    
      /***** start do something with the datarefs *****/

      if (*latitude != FLT_MISS) {
        printf("Latitude: %f\n",*latitude);
      }

      printf("TERR: %i \n",*nav_shows_dem);

  *nav_shows_dem = 0;
      
      if (*egt0 != FLT_MISS) {
	printf("EGT Engine 1: %f\n",*egt0);
      }
      if (*egt1 != FLT_MISS) {
	printf("EGT Engine 2: %f\n",*egt1);
      }

      if (*custom != INT_MISS) {
	printf("CUSTOM VALUE: %i \n",*custom);
	*custom = *custom + 1;
      } else {
	*custom = 1;
      }

      if (acf_tailnum) {
	printf("ACF TAILNUM: %s \n",acf_tailnum);
      }
       
      /*
      if (*framerate != FLT_MISS) {
	printf("framerate %f \n",1. / *framerate);
      }

      if (*bearing != FLT_MISS) {
	printf("bearing %f \n",*bearing);
      }
      if (*x != FLT_MISS) {
	printf("x %f y %f z %f \n",*x1,*y1,*z1);
	printf("dx %f dy %f dz %f \n",*x1-*x,*y1-*y,*z1-*z);
      }

      if (*n1 != INT_MISS) {
	printf("N1 %i\n",*n1);
      }
     
      if (*parkbrake != FLT_MISS) {
	printf("Parkbrake %f\n",*parkbrake);
	*parkbrake = 0.0;
      }
      */
      
      /*
      
      if (*nidx != INT_MISS) {
	*idx = 0;
	printf("N: %i \n",*nidx);
      }

      if (*lon != FLT_MISS) {
	printf("lon: %f \n",*lon);
	*lon = 1.0;
      }


      */

      /*
	if (fmc1) {
	printf("FMC1: XXX%sXXX\n",fmc1);
	}
      */
      
      /***** end do something with the datarefs *****/


      /* send data to X-Plane via TCP/IP */
      if (send_xpserver()<0) exit_xpclient(-15);
 
      /* run usbiocards data exchange (usb and tcp/ip) every second as an example (interval is 1 ms).
	 In a real application you would run it e.g. every 10 ms to make sure you get a smooth update
	 of flight data in your client and vice versa */
      usleep(interval*1000);
    
    }

  /* stop sending/receiving datarefs */
  exit_xpclient(0);

}	

/* Exiting */
void exit_xpclient(int ret)
{

  /* cancel tcp/ip connection */
  exit_tcpip_client();

  /* cancel beacon client */
  exit_beacon_client();

  /* free local dataref structure */
  clear_dataref();

  printf("Exiting with status %i \n",ret);

  exit(ret);
}


/* signal handler in order to clean up when we're interrupted */
/* from the command line (ctrl-c) or from kill on command line */
int initialize_signal_handler(void)
{
  int ret = 0;

  if (signal(SIGINT, exit_xpclient) == SIG_ERR) {
    printf("Could not establish new interrupt signal handler.\n");
    ret = -1;
  }
  if (signal(SIGTERM, exit_xpclient) == SIG_ERR) {
    printf("Could not establish new termination signal handler.\n");
    ret = -1;
  }
  return ret;
}
