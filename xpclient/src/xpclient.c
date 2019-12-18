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

#include "common.h"
#include "xpclient.h"
#include "serverdata.h"
#include "handleserver.h"

int main (int argc,char **argv)
{
  
  /* server settings */
  memset(server_ip,0,sizeof(server_ip));
  strcpy(server_ip,"127.0.0.1");
//  strcpy(server_ip,"192.168.0.14");
  server_port = 8091;

  /* initialize handler for command-line interrupts (ctrl-c) */
  if (initialize_signal_handler()<0) exit_client(-3);

  /* initialize local dataref structure */
  if (initialize_dataref()<0) exit_client(-4);

  /* initialize TCP/IP interface */
  if (initialize_tcpip()<0) exit_client(-6);

  /* subscribe to specific datarefs */

  // a custom dataref which can be accessed by other plugins/clients
  int *custom = link_dataref_int("xpserver/customdataref");
  // the plane's latitude
  double *latitude = link_dataref_dbl("sim/flightmodel/position/latitude", -3);
  // the EGT of engine 0
  float *egt0 =  link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 0, 0);
  // the EGT of engine 1
  float *egt1 =  link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 1, 0);
 
    
  /*
  unsigned char *acf_tailnum   = link_dataref_byte_arr("sim/aircraft/view/acf_tailnum", 100, -1);  

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
      if (check_server()<0) exit_client(-11);
      
      /* receive data from X-Plane via TCP/IP */
      if (receive_server()<0) exit_client(-12);
     
      /***** start do something with the datarefs *****/

      if (*latitude != FLT_MISS) {
        printf("Latitude: %f\n",*latitude);
      }
 
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


      if (acf_tailnum) {
	printf("ACF_TAILNUM: %s \n",acf_tailnum);
      }
 
      */

      /*
	if (fmc1) {
	printf("FMC1: XXX%sXXX\n",fmc1);
	}
      */
      
      /***** end do something with the datarefs *****/


      /* send data to X-Plane via TCP/IP */
      if (send_server()<0) exit_client(-15);

      /* run usbiocards data exchange (usb and tcp/ip) every second as an example (INTERVAL is 1 ms).
	 In a real application you would run it e.g. every 10 ms to make sure you get a smooth update
	 of flight data in your client and vice versa */
      usleep(INTERVAL*1000);
    
    }

  /* stop sending/receiving datarefs */
  exit_client(0);

}	

/* Exiting */
void exit_client(int ret)
{

  /* cancel tcp/ip connection */
  exit_tcpip();

  /* free local dataref structure */
  clear_dataref();

  printf("Exiting with status %i \n",ret);

  exit(ret);
}


/* signal handler in order to clean up when we're interrupted */
/* from the command line (ctrl-c) */
int initialize_signal_handler(void)
{
  int ret = 0;

  sig_handler = signal(SIGINT, exit_client);
  if (sig_handler == SIG_ERR) {
    printf("Could not establish new signal handler.\n");
    ret = -1;
  }
  return ret;
}
