/* This is the wxrdata.c code which decodes X-Plane Weather data sent by UDP protocol
   TODO: Elevation data in Type 2 data source not yet used

   Copyright (C) 2009 - 2022 Reto Stockli
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
#include <sys/time.h>
#include <stdbool.h>
#include <unistd.h>   
#include <string.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>

#include "xplanewxr.h"
#include "wxrdata.h"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define WXR_CHECK_INTERVAL 30.0


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
int wxr_initialized;

unsigned char **wxr_data;
int **wxr_height;
int wxr_newdata;
int wxr_firstread;
int wxr_firstsend;

struct timeval wxr_t2;
struct timeval wxr_t1;

/* Type 1: WXR data from X-Plane's Control Pad: we act as server */
/* Type 2: WXR data from X-Plane's regular UDP stream: we act as client */
void init_wxr(char server_ip[]) {

  if (wxr_initialized == 0) {

    if (strcmp(server_ip,"")!=0) {
  
      int n;
  
      wxr_lonmin_tmp = WXR_MISS;
      wxr_lonmax_tmp = WXR_MISS;
      wxr_latmin_tmp = WXR_MISS;
      wxr_latmax_tmp = WXR_MISS;
      wxr_lonmin = WXR_MISS;
      wxr_lonmax = WXR_MISS;
      wxr_latmin = WXR_MISS;
      wxr_latmax = WXR_MISS;
      wxr_newdata = 0;
      wxr_firstread = 0;
      wxr_firstsend = 0;
  
      if (wxr_type == 1) {
	/* initialize UDP socket if needed for WXR data from X-Plane*/
	/* UDP Server Port (OpenGC acts as UDP server for X-Plane as control pad client) */
	strcpy(wxrServerIP, server_ip);
	wxrServerPort = 48003;
    
	wxrReadLeft=0;
	int sendlen = 0; /* no sending */
	int recvlen = 8100; // 'xRAD' plus '\0' (5 bytes) plus 3 integers (12 bytes) plus 61 bytes of radar returns plus \0 = 81 bytes per message
	allocate_wxrdata(sendlen,recvlen);
    
	init_wxr_server();
	init_wxr_receive();

      }

      if (wxr_type == 2) {
	/* initialize UDP socket if needed for WXR data from X-Plane*/
	/* UDP Server Port (X-Plane acts as Server, this here is the client) */
	strcpy(wxrServerIP, server_ip);
	wxrServerPort = 49000;
 
	if (MAXRADAR > 0) {
	  n = (int) log10(MAXRADAR) + 1; // number of characters of MAXRADAR number (converted to string)
	} else {
	  n = 1;
	}
  
	int sendlen = 7+n;
	//	int recvlen = (5+13*MAXRADAR)*100;
	int recvlen = 50000;

	allocate_wxrdata(sendlen,recvlen);

	init_wxr_client();
	init_wxr_receive();

      }

      wxr_phase = 0;
      wxr_initialized = 1;
  
      /* get current time of day */
      gettimeofday(&wxr_t1,NULL);

    }
  }
  
}

void write_wxr() {

  int ret, n, j;

  /* if we did not receive any WXR data for X seconds, send init string again */

      
    /* get current time of day */
    gettimeofday(&wxr_t2,NULL);

    float dt = ((wxr_t2.tv_sec - wxr_t1.tv_sec) + (wxr_t2.tv_usec - wxr_t1.tv_usec) / 1000000.0);
    
    if (dt > WXR_CHECK_INTERVAL) {
      
      printf("We did receive WXR data for %i seconds, send init string to X-Plane\n",
	     (int) WXR_CHECK_INTERVAL);
      
      /* new time reference for next call is current time */
      /* will be reset if we successfully read WXR data from x-plane */
      wxr_t1.tv_sec = wxr_t2.tv_sec;
      wxr_t1.tv_usec = wxr_t2.tv_usec;
      
      wxr_firstsend = 0;
      
      if (wxr_data_tmp != NULL) {
	/* Remove Temporary WXR Array */
	for (j = 0; j < wxr_nlin_tmp; j++) {
	  free(wxr_data_tmp[j]);
	}
	free(wxr_data_tmp);
	wxr_data_tmp = NULL;
      }

      if (wxr_height_tmp != NULL) {
	/* Remove Temporary WXR Array */
	for (j = 0; j < wxr_nlin_tmp; j++) {
	  free(wxr_height_tmp[j]);
	}
	free(wxr_height_tmp);
	wxr_height_tmp = NULL;
      }
 	
      /* prepare for new scanning of bounds */
      wxr_lonmin_tmp = WXR_MISS;
      wxr_lonmax_tmp = WXR_MISS;
      wxr_latmin_tmp = WXR_MISS;
      wxr_latmax_tmp = WXR_MISS;
      
      wxr_phase = 0;
      wxr_newdata = 0;
      
    }
  
  if ((wxr_type == 2) && (wxr_firstsend == 0) && (wxr_initialized)) {
 
    if (MAXRADAR > 0) {
      n = (int) log10(MAXRADAR) + 1; // number of characters of MAXRADAR number (converted to string)
    } else {
      n = 1;
    }
 
    /* generate send message to initialize UDP transfer */
    sprintf(wxrSendBuffer,"RADR %i",MAXRADAR);
    wxrSendBuffer[4]='\0';
    wxrSendBuffer[6+n]='\0';

    ret = send_wxr_to_server();
    printf("Sent WXR Init String to X-Plane with length: %i \n",ret);

    wxr_firstsend = 1;
    wxr_firstread = 0;
    
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
  short hgt2;
  char lev;

  char *wxrBuffer;
  int wxrBufferLen;
  
  if (((wxr_type == 1) || (wxr_type == 2)) && (wxr_initialized)) {
  
    if (wxr_type == 1) {
      wxrBufferLen = 81;
      nrad = 1;
    } else {
      if (wxr_is_xp12) {
	wxrBufferLen = 5+18*MAXRADAR;
      } else {
	wxrBufferLen = 5+13*MAXRADAR;
      }
      nrad = MAXRADAR;
    }
    wxrBuffer=(char*) malloc(wxrBufferLen);
	
    while (wxrReadLeft > 0) {

      /* get current time of day */
      /* update timer if we receive WXR data */
      gettimeofday(&wxr_t1,NULL);
      
      // printf("%i \n",wxrReadLeft);
      
      if (wxr_firstread == 0) {
	printf("This client is receiving WXR data from X-Plane!\n");
	wxr_firstread = 1;
      }
      
      pthread_mutex_lock(&wxr_exit_cond_lock);    
      /* read from start of receive buffer */
      memcpy(wxrBuffer,&wxrRecvBuffer[0],wxrBufferLen);
      /* shift remaining read buffer to the left */
      memmove(&wxrRecvBuffer[0],&wxrRecvBuffer[wxrBufferLen],wxrReadLeft-wxrBufferLen);    
      /* decrease read buffer position and counter */
      wxrReadLeft -= wxrBufferLen;
      pthread_mutex_unlock(&wxr_exit_cond_lock);
	
      for (r=0;r<nrad;r++) {
	
	if (wxr_type == 1) {
	  memcpy(&lon_deg_west,&wxrBuffer[5],sizeof(lon_deg_west));
	  memcpy(&lat_deg_south,&wxrBuffer[5+4],sizeof(lat_deg_south));
	  memcpy(&lat_min_south,&wxrBuffer[5+4+4],sizeof(lat_min_south));
	  lon = lon_deg_west;
	  lat = lat_deg_south;
	  
	  //printf("%i %i %i \n",lon_deg_west,lat_deg_south,lat_min_south);
	} else {
	  /* XP11
	     the four chars RADR and a NULL. Then 13 Bytes:
	     float lon                       longitude of the radar point
	     float lat                       latitude of course
	     char storm_level_0_100         precip level, 0 to 100
	     float storm_height_meters       the storm tops in meters MSL
	  */

	  /* XP12
	     the four chars RADR, and a fifth byte that is internal-use. Then 18 Bytes:
	     float lon                       longitude of the scan pointa
	     float lat                       latitude of the scan point
	     float bases_meters              the cloud basees in meters MSL
	     float tops_meters               the cloud tops in meters MSL
	     char clouds_ratio              cloud ratio this lat and lon ratio
	     char precip_ratio              precip ratio at this lat and lon
	  */
	  if (wxr_is_xp12) {
	    memcpy(&lon,&wxrBuffer[5+r*18+0],sizeof(lon));
	    memcpy(&lat,&wxrBuffer[5+r*18+4],sizeof(lat));
	    memcpy(&lev,&wxrBuffer[5+r*18+17],sizeof(lev));
	    memcpy(&hgt,&wxrBuffer[5+r*18+13],sizeof(hgt));
	  } else {
	    memcpy(&lon,&wxrBuffer[5+r*13+0],sizeof(lon));
	    memcpy(&lat,&wxrBuffer[5+r*13+4],sizeof(lat));
	    memcpy(&lev,&wxrBuffer[5+r*13+8],sizeof(lev));
	    memcpy(&hgt,&wxrBuffer[5+r*13+9],sizeof(hgt));
	  }
	  printf("%i lon %f lat %f hgt %f lev %i \n",r,lon,lat,hgt,lev);
	  
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
	    printf("Found WXR Lon/Lat Bounds: %f to %f / %f to %f \n",
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
	    for (j = 0; j < wxr_nlin_tmp; j++)
	      for (i = 0; i < wxr_ncol_tmp; i++) {
		wxr_data_tmp[j][i] = 0;
		wxr_height_tmp[j][i] = 0;
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
	    if (wxr_data != NULL) {
	      for (j = 0; j < wxr_nlin; j++)
		free(wxr_data[j]);
	      free(wxr_data);
	      wxr_data = NULL;
	    }
	    if (wxr_height != NULL) {
	      for (j = 0; j < wxr_nlin; j++)
		free(wxr_height[j]);
	      free(wxr_height);
	      wxr_height = NULL;
	    }
	    
	    wxr_pixperlon = WXR_UPSCALE * wxr_pixperlon_tmp;
	    wxr_pixperlat = WXR_UPSCALE * wxr_pixperlat_tmp;
	    wxr_lonmin = wxr_lonmin_tmp;
	    wxr_latmin = wxr_latmin_tmp;
	    wxr_ncol = WXR_UPSCALE * wxr_ncol_tmp;
	    wxr_nlin = WXR_UPSCALE * wxr_nlin_tmp;
	    wxr_lonmax = wxr_lonmin + wxr_ncol / wxr_pixperlon;
	    wxr_latmax = wxr_latmin + wxr_nlin / wxr_pixperlat;

	    wxr_data = (unsigned char**)malloc(wxr_nlin * sizeof(unsigned char*));
	    wxr_height = (int**)malloc(wxr_nlin * sizeof(int*));
	    for (j = 0; j < wxr_nlin; j++) {
	      wxr_data[j] = (unsigned char*)malloc(wxr_ncol * sizeof(unsigned char));
	      wxr_height[j] = (int*)malloc(wxr_ncol * sizeof(int));
	    }
	    

	    if (WXR_UPSCALE > 1) {
	      /* interpolate temporary WXR array into WXR array */
	      //nearest_uchar(wxr_data_tmp, wxr_data, wxr_ncol_tmp, wxr_nlin_tmp, wxr_ncol, wxr_nlin);
	      //nearest_int(wxr_height_tmp, wxr_height, wxr_ncol_tmp, wxr_nlin_tmp, wxr_ncol, wxr_nlin);
	      //bilinear_uchar(wxr_data_tmp, wxr_data, wxr_ncol_tmp, wxr_nlin_tmp, wxr_ncol, wxr_nlin);
	      //bilinear_int(wxr_height_tmp, wxr_height, wxr_ncol_tmp, wxr_nlin_tmp, wxr_ncol, wxr_nlin);
	      bicubic_uchar(wxr_data_tmp, wxr_data, wxr_ncol_tmp, wxr_nlin_tmp, wxr_ncol, wxr_nlin);
	      bicubic_int(wxr_height_tmp, wxr_height, wxr_ncol_tmp, wxr_nlin_tmp, wxr_ncol, wxr_nlin);
	    } else {
	      /* copy temporary WXR array to WXR array */
	      for (j = 0; j < wxr_nlin; j++) {
		for (i = 0; i < wxr_ncol; i++) {
		  wxr_data[j][i] = wxr_data_tmp[j][i];
		  wxr_height[j][i] = wxr_height_tmp[j][i];
		}
	      }
	    }
		
	    /* Remove Temporary WXR Array */
	    for (j = 0; j < wxr_nlin_tmp; j++) {
	      free(wxr_data_tmp[j]);
	      free(wxr_height_tmp[j]);
	    }
	    free(wxr_data_tmp);
	    free(wxr_height_tmp);
	    wxr_data_tmp = NULL;
	    wxr_height_tmp = NULL;

	    //printf("Temporary WXR arrays deleted\n");
	    
	    /* prepare for new scanning of bounds */
	    wxr_lonmin_tmp = WXR_MISS;
	    wxr_lonmax_tmp = WXR_MISS;
	    wxr_latmin_tmp = WXR_MISS;
	    wxr_latmax_tmp = WXR_MISS;
	    
	    wxr_phase = 0;

	    wxr_newdata = 1;
	      
	  } else {
	    /* Receiving domain to temporary array */

	    if (wxr_type == 1) {
	      /* WXR Levels from 0-9 and No Height Information */
	      /* WXR every arc minute (60 per lon and 60 per lat */
	      
	      if ((lat_min_south != -1) && (lat_min_south != 60)) {
		j = (lat - wxr_latmin_tmp)*wxr_pixperlat_tmp + lat_min_south;
		i = (lon - wxr_lonmin_tmp)*wxr_pixperlon_tmp;
		
		//printf("Reading lon %i lat %i min %i. Indices: %i %i %i \n",
		//	     lon,lat,lat_min_south,j,i,wxrBuffer[5+4+4+4+10]);
		
		for (k = 0; k < wxr_pixperlon_tmp; k++) {
		  wxr_data_tmp[j][i+k] = wxrBuffer[5+4+4+4+k] * 10;
		  wxr_height_tmp[i][i+k] = 30000; /* no height information in type 1 data, assume 30000 feet */
		}
	      }
		
	    } else {
	      /* WXR Levels from 0-100 and storm top height information (feet a.s.l.) */
	      /* WXR every arc minute for lat, but thinning lon resolution with higher lats */
	      
	      j = (lat - wxr_latmin_tmp)*wxr_pixperlat_tmp;
	      i = (lon - wxr_lonmin_tmp)*wxr_pixperlon_tmp;
	      if ((j>=0)&&(j<wxr_nlin_tmp)&&(i>=0)&&(i<wxr_ncol_tmp)) {
		wxr_data_tmp[j][i] = 0.9 * lev;
		wxr_height_tmp[j][i] = (int) (hgt * 3.28);
	      }
	    }
	      
	  } /* Receiving WXR data or reception finished? */
	    
	} /* Phase 1 */
	  	
      } /* 1..nrad loop */
      
    } /* read left ? */
    
    free(wxrBuffer);
    wxrBuffer = NULL;

  } /* Type 1 or Type 2 */
  
}

void exit_wxr() {

  if (wxr_initialized) {
  

    /*
      if (wxr_data != NULL) {
      for (j = 0; j < wxr_nlin; j++)
      free(wxr_data[j]);
      free(wxr_data);
      wxr_data = NULL;
      }

      if (wxr_data_tmp != NULL) {
      for (j = 0; j < wxr_nlin_tmp; j++)
      free(wxr_data_tmp[j]);
      free(wxr_data_tmp);
      wxr_data_tmp = NULL;
      }
    */

    if (wxr_type == 1) {
      exit_wxr_server();
      deallocate_wxrdata();
    }
  
    if (wxr_type == 2) {

      /* generate send message to terminate UDP transfer */
      sprintf(wxrSendBuffer,"RADR %i",0);
      wxrSendBuffer[4]='\0';
      wxrSendBuffer[7]='\0';
    
      int ret = send_wxr_to_server();
      printf("Sent WXR Exit String to X-Plane with Length: %i \n",ret);

      exit_wxr_client();
      printf("WXR Client Exited\n");
      deallocate_wxrdata();
    }

    wxr_initialized = 0;
    
  }
  
}

void nearest_uchar(unsigned char **data, unsigned char **newData, int width, int height, int newWidth, int newHeight)
{
  int i,j;
  int x,y;
  //  unsigned char cc;
  float tx = (float)(width-1) / newWidth;
  float ty =  (float)(height-1) / newHeight;
  
  for(j=0;j<newHeight;j++) {
    for(i=0;i<newWidth;i++) {
      x = ceil(tx*i); /* NN x index of input data */
      y = ceil(ty*j); /* NN y index of input data */
      newData[j][i] = data[y][x];             
    }
  }
}

void nearest_int(int **data, int **newData, int width, int height, int newWidth, int newHeight)
{
  int i,j;
  int x,y;
  //  int cc;
  float tx = (float)(width-1) / newWidth;
  float ty =  (float)(height-1) / newHeight;
  
  for(j=0;j<newHeight;j++) {
    for(i=0;i<newWidth;i++) {
      x = ceil(tx*i); /* NN x index of input data */
      y = ceil(ty*j); /* NN y index of input data */
      newData[j][i] = data[y][x];             
    }
  }
}

void bilinear_uchar(unsigned char **data, unsigned char **newData, int width, int height, int newWidth, int newHeight)
{
  int x,y;
  float tx = (float)(width-1)/newWidth;
  float ty = (float)(height-1)/newHeight;
  float x_diff, y_diff;
  int i,j;

  for(j=0;j<newHeight;j++) {
    for(i=0;i<newWidth;i++) {
      x = (int)(tx * i);
      y = (int)(ty * j);
             
      x_diff = ((tx * i) -x);
      y_diff = ((ty * j) -y);
     
      newData[j][i] =
	(float)data[y][x]*(1.0-x_diff)*(1.0-y_diff)
	+(float)data[y][x+1]*(1.0-y_diff)*(x_diff)
	+(float)data[y+1][x]*(y_diff)*(1.0-x_diff)
	+(float)data[y+1][x+1]*(y_diff)*(x_diff);  
    }
  } 
}

void bilinear_int(int **data, int **newData, int width, int height, int newWidth, int newHeight)
{
  int x,y;
  float tx = (float)(width-1)/newWidth;
  float ty = (float)(height-1)/newHeight;
  float x_diff, y_diff;
  int i,j;

  for(j=0;j<newHeight;j++) {
    for(i=0;i<newWidth;i++) {
      x = (int)(tx * i);
      y = (int)(ty * j);
             
      x_diff = ((tx * i) -x);
      y_diff = ((ty * j) -y);
     
      newData[j][i] =
	(float)data[y][x]*(1.0-x_diff)*(1.0-y_diff)
	+(float)data[y][x+1]*(1.0-y_diff)*(x_diff)
	+(float)data[y+1][x]*(y_diff)*(1.0-x_diff)
	+(float)data[y+1][x+1]*(y_diff)*(x_diff);  
    }
  } 
}

void bicubic_uchar(unsigned char **data, unsigned char **newData, int width, int height, int newWidth, int newHeight)
{    
  float Cc;
  float C[4];
  float d0,d2,d3;
  float a0,a1,a2,a3;
  int i,j;
  int jj;
  int x,y;
  float dx,dy;
        
  float tx = (float)(width-1)/newWidth;
  float ty = (float)(height-1)/newHeight;
        
  for(j=0;j<newHeight;j++) {
    for(i=0;i<newWidth;i++) {
      x = (int)(tx * i);
      y = (int)(ty * j);
           
      dx = tx*i-x;
      dy = ty*j-y;
                      
      for(jj=0;jj<=3;jj++) {
	a0 = (float)data[min(max(y-1+jj,0),height-1)][x];                                  
	d0 = (float)data[min(max(y-1+jj,0),height-1)][max(x-1,0)] - a0;
	d2 = (float)data[min(max(y-1+jj,0),height-1)][min(x+1,width-1)] - a0;
	d3 = (float)data[min(max(y-1+jj,0),height-1)][min(x+2,width-1)] - a0;
	a1 = -1.0/3.0*d0 + d2 -1.0/6.0*d3;
	a2 =  1.0/2.0*d0 + 1.0/2.0*d2;
	a3 = -1.0/6.0*d0 - 1.0/2.0*d2 + 1.0/6.0*d3;
	C[jj] = a0 + a1*dx + a2*dx*dx + a3*dx*dx*dx;
      }
	
      d0 = C[0]-C[1];
      d2 = C[2]-C[1];
      d3 = C[3]-C[1];
      a0 = C[1];
      a1 = -1.0/3.0*d0 + d2 -1.0/6.0*d3;
      a2 =  1.0/2.0*d0 + 1.0/2.0*d2;
      a3 = -1.0/6.0*d0 - 1.0/2.0*d2 + 1.0/6.0*d3;
      Cc = a0 + a1*dy + a2*dy*dy + a3*dy*dy*dy;
      if((int)Cc>255) Cc=255.0;
      if((int)Cc<0) Cc=0.0;
      newData[j][i] = Cc;
    }    
  }
}

void bicubic_int(int **data, int **newData, int width, int height, int newWidth, int newHeight)
{    
  float Cc;
  float C[4];
  float d0,d2,d3;
  float a0,a1,a2,a3;
  int i,j;
  int jj;
  int x,y;
  float dx,dy;
        
  float tx = (float)(width-1)/newWidth;
  float ty = (float)(height-1)/newHeight;
        
  for(j=0;j<newHeight;j++) {
    for(i=0;i<newWidth;i++) {
      x = (int)(tx * i);
      y = (int)(ty * j);
           
      dx = tx*i-x;
      dy = ty*j-y;
                      
      for(jj=0;jj<=3;jj++) {
	a0 = (float)data[min(max(y-1+jj,0),height-1)][x];                                  
	d0 = (float)data[min(max(y-1+jj,0),height-1)][max(x-1,0)] - a0;
	d2 = (float)data[min(max(y-1+jj,0),height-1)][min(x+1,width-1)] - a0;
	d3 = (float)data[min(max(y-1+jj,0),height-1)][min(x+2,width-1)] - a0;
	a1 = -1.0/3.0*d0 + d2 -1.0/6.0*d3;
	a2 =  1.0/2.0*d0 + 1.0/2.0*d2;
	a3 = -1.0/6.0*d0 - 1.0/2.0*d2 + 1.0/6.0*d3;
	C[jj] = a0 + a1*dx + a2*dx*dx + a3*dx*dx*dx;
      }
	
      d0 = C[0]-C[1];
      d2 = C[2]-C[1];
      d3 = C[3]-C[1];
      a0 = C[1];
      a1 = -1.0/3.0*d0 + d2 -1.0/6.0*d3;
      a2 =  1.0/2.0*d0 + 1.0/2.0*d2;
      a3 = -1.0/6.0*d0 - 1.0/2.0*d2 + 1.0/6.0*d3;
      Cc = a0 + a1*dy + a2*dy*dy + a3*dy*dy*dy;
      newData[j][i] = Cc;
    }    
  }
}
