/* This is the wxrdata.c code which decodes X-Plane Weather data sent by UDP protocol
   TODO: Elevation data in Type 2 data source not yet used

   Copyright (C) 2009 - 2014  Reto Stockli
   Adaptation to Linux compilation by Hans Jansen

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
#include <stdlib.h>  
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>   
#include <string.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>

#include "handleudp.h"
#include "udpdata.h"
#include "wxrdata.h"

float wxr_lonmin_tmp;
float wxr_lonmax_tmp;
float wxr_latmin_tmp;
float wxr_latmax_tmp;
int wxr_pixperlon_tmp;
int wxr_pixperlat_tmp;
int wxr_ncol_tmp;
int wxr_nlin_tmp;
unsigned char **wxr_data_tmp;
int **wxr_height_tmp;


int wxr_type;

float wxr_lonmin;
float wxr_lonmax;
float wxr_latmin;
float wxr_latmax;

int wxr_pixperlon;
int wxr_pixperlat;
int wxr_ncol;
int wxr_nlin;

int wxr_phase;

unsigned char **wxr_data;
int**wxr_height;

/* Type 1: WXR data from X-Plane's Control Pad: we act as server */
/* Type 2: WXR data from X-Plane's regular UDP stream: we act as client */
void init_wxr(int type, char server_ip[]) {

  int n, ret;
  
  wxr_type = type;
  wxr_lonmin_tmp = WXR_MISS;
  wxr_lonmax_tmp = WXR_MISS;
  wxr_latmin_tmp = WXR_MISS;
  wxr_latmax_tmp = WXR_MISS;
  wxr_lonmin = WXR_MISS;
  wxr_lonmax = WXR_MISS;
  wxr_latmin = WXR_MISS;
  wxr_latmax = WXR_MISS;
  
  if (wxr_type == 1) {
    /* initialize UDP socket if needed for WXR data from X-Plane*/
    /* UDP Server Port (OpenGC acts as UDP server for X-Plane as control pad client) */
    strcpy(udpServerIP, server_ip);
    udpServerPort = 48003;
    
    udpReadLeft=0;
    int sendlen = 0; /* no sending */
    int recvlen = 8100; // 'xRAD' plus '\0' (5 bytes) plus 3 integers (12 bytes) plus 61 bytes of radar returns plus \0 = 81 bytes per message
    allocate_udpdata(sendlen,recvlen);
    
    init_udp_server();
    init_udp_receive();

    wxr_phase = 0;

  }

  if (wxr_type == 2) {
    /* initialize UDP socket if needed for WXR data from X-Plane*/
    /* UDP Server Port (X-Plane acts as Server, this here is the client) */
    strcpy(udpServerIP, server_ip);
    udpServerPort = 49000;
 
    if (MAXRADAR > 0) {
      n = (int) log10(MAXRADAR) + 1; // number of characters of MAXRADAR number (converted to string)
    } else {
      n = 1;
    }
  
    int sendlen = 7+n;
    int recvlen = (5+13*MAXRADAR)*100;

    allocate_udpdata(sendlen,recvlen);

    init_udp_client();
    init_udp_receive();

    /* generate send message to initialize UDP transfer */
    sprintf(udpSendBuffer,"RADR %i",MAXRADAR);
    udpSendBuffer[4]='\0';
    udpSendBuffer[6+n]='\0';

    ret = send_udp_to_server();
    printf("Sent UDP Init String to X-Plane with Length: %i \n",ret);

    wxr_phase = 0;

  }

  
}

void read_wxr() {

  /* read buffer and fill it into regular lon/lat array */
	
  int i,j,k,r;
  int nrad;
	
  int lon_deg_west;
  int lat_deg_south;
  int lat_min_south;
  
  float lon;
  float lat;
  float hgt;
  char lev;

  char *wxrBuffer;
  int wxrBufferLen;
  
  if ((wxr_type == 1) || (wxr_type == 2)) {
  
    if (wxr_type == 1) {
      wxrBufferLen = 81;
      nrad = 1;
    } else {
      char *wxrBuffer;
      wxrBufferLen = 5+13*MAXRADAR;
      nrad = MAXRADAR;
    }
    wxrBuffer=(char*) malloc(wxrBufferLen);
	
    while (udpReadLeft > 0) {

      //printf("%i \n ",udpReadLeft);
      
      pthread_mutex_lock(&exit_cond_lock);    
      /* read from start of receive buffer */
      memcpy(wxrBuffer,&udpRecvBuffer[0],wxrBufferLen);
      /* shift remaining read buffer to the left */
      memmove(&udpRecvBuffer[0],&udpRecvBuffer[wxrBufferLen],udpReadLeft-wxrBufferLen);    
      /* decrease read buffer position and counter */
      udpReadLeft -= wxrBufferLen;
      pthread_mutex_unlock(&exit_cond_lock);
	
      for (r=0;r<nrad;r++) {
	
	if (wxr_type == 1) {
	  memcpy(&lon_deg_west,&wxrBuffer[5],sizeof(lon_deg_west));
	  memcpy(&lat_deg_south,&wxrBuffer[5+4],sizeof(lat_deg_south));
	  memcpy(&lat_min_south,&wxrBuffer[5+4+4],sizeof(lat_min_south));
	  lon = lon_deg_west;
	  lat = lat_deg_south;
	  
	  //printf("%i %i %i \n",lon_deg_west,lat_deg_south,lat_min_south);
	} else {
	  memcpy(&lon,&wxrBuffer[5+r*13+0],sizeof(lon));
	  memcpy(&lat,&wxrBuffer[5+r*13+4],sizeof(lat));
	  memcpy(&lev,&wxrBuffer[5+r*13+8],sizeof(lev));
	  memcpy(&hgt,&wxrBuffer[5+r*13+9],sizeof(hgt));
	
	  //printf("%i lon %f lat %f hgt %f lev %d \n",r,lon,lat,hgt,lev);
	}
	  
	if ((wxr_phase == 0) && (lon != 0.0) && (lat != 0.0)) {
	  /* scanning starts from southern latitude and has an inner loop over longitudes from W->E */
	  
	  if (wxr_lonmax_tmp == WXR_MISS) {
	    wxr_lonmin_tmp = lon;
	    wxr_lonmax_tmp = lon;
	    wxr_latmin_tmp = lat;
	    wxr_latmax_tmp = lat;
	  }
	  
	  if (lon > wxr_lonmax_tmp) wxr_lonmax_tmp = lon;
	  if (lon < wxr_lonmin_tmp) wxr_lonmin_tmp = lon;
	  if (lat > wxr_latmax_tmp) wxr_latmax_tmp = lat;
	  if (lat < wxr_latmin_tmp) wxr_latmin_tmp = lat;
	  if ((lon < wxr_lonmax_tmp) && (lat < wxr_latmax_tmp)) {
	    printf("Found WXR Lon/Lat Bounds: %f-%f / %f-%f \n",
		   wxr_lonmin_tmp,wxr_lonmax_tmp,wxr_latmin_tmp,wxr_latmax_tmp);

	    if (wxr_type == 1) {
	      wxr_pixperlon_tmp = 60;
	      wxr_pixperlat_tmp = 60;
	      wxr_ncol_tmp = (wxr_lonmax_tmp - wxr_lonmin_tmp + 1) * wxr_pixperlon_tmp;
	      wxr_nlin_tmp = (wxr_latmax_tmp - wxr_latmin_tmp + 1) * wxr_pixperlat_tmp;
	    } else {
	      /* longitude spacing depending on latitude */
	      wxr_pixperlon_tmp = (int) 60.0*cos(3.1415/180.0*(wxr_latmax_tmp+wxr_latmin_tmp)*0.5);
	      wxr_pixperlat_tmp = 60;
	      wxr_ncol_tmp = (wxr_lonmax_tmp - wxr_lonmin_tmp) * wxr_pixperlon_tmp + 1;
	      wxr_nlin_tmp = (wxr_latmax_tmp - wxr_latmin_tmp) * wxr_pixperlat_tmp + 1;
	    }

	    printf("WXR Data size: %i x %i \n",wxr_ncol_tmp, wxr_nlin_tmp);
	    
	    /* allocate memory for temporary wxr data storage */
	    wxr_data_tmp = (unsigned char**)malloc(wxr_nlin_tmp * sizeof(unsigned char*));
	    wxr_height_tmp = (int**)malloc(wxr_nlin_tmp * sizeof(int*));
	    for (i = 0; i < wxr_nlin_tmp; i++) {
	      wxr_data_tmp[i] = (unsigned char*)malloc(wxr_ncol_tmp * sizeof(unsigned char));
	      wxr_height_tmp[i] = (int*)malloc(wxr_ncol_tmp * sizeof(int));
	    }
	    
	    /* Reset WXR entries to 0 */
	    /* Reset Height to 10'000 m */
	    for (i = 0; i < wxr_nlin_tmp; i++)
	      for (j = 0; j < wxr_ncol_tmp; j++) {
		wxr_data_tmp[i][j] = 0;
		wxr_height_tmp[i][j] = 0;
	      }
	    
	    wxr_phase = 1;
	    
	    /* Reset Max coordinates to later check whether we reached the NE part of the domain
	       Keep Min coordinates for referencing the temporary domain. */
	    wxr_lonmax_tmp = WXR_MISS;
	    wxr_latmax_tmp = WXR_MISS;
     
	  } /* Bounds complete */
	} /* Phase 0 */

	if ((wxr_phase == 1) && (lon != 0.0) && (lat != 0.0)) {

	  if (wxr_lonmax_tmp == WXR_MISS) {
	    wxr_lonmax_tmp = lon;
	    wxr_latmax_tmp = lat;
	  }

	  if (lon > wxr_lonmax_tmp) wxr_lonmax_tmp = lon;
	  if (lat > wxr_latmax_tmp) wxr_latmax_tmp = lat;
	  if ((lon < wxr_lonmax_tmp) && (lat < wxr_latmax_tmp)) {
	    printf("WXR Data reception complete \n");

	    /* free WXR array and recreate it */
	    if (wxr_data) {
	      for (i = 0; i < wxr_nlin; i++)
		free(wxr_data[i]);
	      free(wxr_data);
	    }
	    if (wxr_height) {
	      for (i = 0; i < wxr_nlin; i++)
		free(wxr_height[i]);
	      free(wxr_height);
	    }
	    
	    wxr_pixperlon = wxr_pixperlon_tmp;
	    wxr_pixperlat = wxr_pixperlat_tmp;
	    wxr_lonmin = wxr_lonmin_tmp;
	    wxr_latmin = wxr_latmin_tmp;
	    wxr_ncol = wxr_ncol_tmp;
	    wxr_nlin = wxr_nlin_tmp;
	    wxr_lonmax = wxr_lonmin + wxr_ncol / wxr_pixperlon;
	    wxr_latmax = wxr_latmin + wxr_nlin / wxr_pixperlat;

	    wxr_data = (unsigned char**)malloc(wxr_nlin * sizeof(unsigned char*));
	    wxr_height = (int**)malloc(wxr_nlin * sizeof(int*));
	    for (i = 0; i < wxr_nlin; i++) {
	      wxr_data[i] = (unsigned char*)malloc(wxr_ncol * sizeof(unsigned char));
	      wxr_height[i] = (int*)malloc(wxr_ncol * sizeof(int));
	    }
	    
	    /* copy temporary WXR array to WXR array */
	    for (i = 0; i < wxr_nlin; i++) {
	      for (j = 0; j < wxr_ncol; j++) {
		wxr_data[i][j] = wxr_data_tmp[i][j];
		wxr_height[i][j] = wxr_height_tmp[i][j];
	      }
	    }
		
	    /* Remove Temporary WXR Array */
	    for (i = 0; i < wxr_nlin; i++) {
	      free(wxr_data_tmp[i]);
	      free(wxr_height_tmp[i]);
	    }
	    free(wxr_height_tmp);
	    
	    /* prepare for new scanning of bounds */
	    wxr_lonmin_tmp = WXR_MISS;
	    wxr_lonmax_tmp = WXR_MISS;
	    wxr_latmin_tmp = WXR_MISS;
	    wxr_latmax_tmp = WXR_MISS;
	    
	    wxr_phase = 0;
	      
	  } else {
	    /* Receiving domain to temporary array */

	    if (wxr_type == 1) {
	      /* WXR Levels from 0-9 and No Height Information */
	      /* WXR every arc minute (60 per lon and 60 per lat */
	      
	      if ((lat_min_south != -1) && (lat_min_south != 60)) {
		i = (lat - wxr_latmin_tmp)*wxr_pixperlat_tmp + lat_min_south;
		j = (lon - wxr_lonmin_tmp)*wxr_pixperlon_tmp;
		
		//printf("Reading lon %i lat %i min %i. Indices: %i %i %i \n",
		//	     lon,lat,lat_min_south,j,i,wxrBuffer[5+4+4+4+10]);
		
		for (k = 0; k < wxr_pixperlon_tmp; k++) {
		  wxr_data_tmp[i][j+k] = wxrBuffer[5+4+4+4+k] * 10;
		}
	      }
		
	    } else {
	      /* WXR Levels from 0-100 and Height Information */
	      /* WXR every arc minute for lat, but thinning lon resolution with higher lats */
	      
	      i = (lat - wxr_latmin_tmp)*wxr_pixperlat_tmp;
	      j = (lon - wxr_lonmin_tmp)*wxr_pixperlon_tmp;
	      if ((i>=0)&&(i<wxr_nlin_tmp)&&(j>=0)&&(j<wxr_ncol_tmp)) {
		wxr_data_tmp[i][j] = 0.9 * lev;
		wxr_height_tmp[i][j] = (int) hgt;
	      }
	    }
	      
	  } /* Receiving WXR data or reception finished? */
	    
	} /* Phase 1 */
	  	
      } /* 1..nrad loop */
      
    } /* read left ? */
    
    free(wxrBuffer);     

  } /* Type 1 or Type 2 */
  
}

void exit_wxr() {

  int i;

  /*
  if (wxr_data) {
    for (i = 0; i < wxr_nlin; i++)
      free(wxr_data[i]);
    free(wxr_data);
  }

  if (wxr_data_tmp) {
    for (i = 0; i < wxr_nlin_tmp; i++)
      free(wxr_data_tmp[i]);
    free(wxr_data_tmp);
  }
  */

  /* generate send message to terminate UDP transfer */
  sprintf(udpSendBuffer,"RADR %i",0);
  udpSendBuffer[4]='\0';
  udpSendBuffer[7]='\0';
  
  int ret = send_udp_to_server();
  printf("Sent UDP Exit String to X-Plane with Length: %i \n",ret);

  if (wxr_type == 1) {
      exit_udp_server();
      deallocate_udpdata();
  }
  
  if (wxr_type == 2) {
      exit_udp_client();
      deallocate_udpdata();
  }
  
}
