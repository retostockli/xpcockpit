/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcTerrainData.cpp,v $

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>    // std::min,max
#include <cmath>
#include "ogcTerrainData.h"

namespace OpenGC
{

  using namespace std;

  TerrainData
  ::TerrainData()
  {
    m_nlon = -1; // make nlon dynamic depending on latitude (we need more lons at higher lats)
    m_nlat = 5;

    m_pplon = 120; // number of pixels per degree longitude
    m_pplat = 120; // number of pixels per degree latitude

    dem_miss = -500; // missing value (ocean?)

    /* will be defined by current aircraft coordinates */
    m_lonmin = -9999;
    m_lonmax = -9999;
    m_latmin = -9999;
    m_latmax = -9999;
    m_ncol = -9999;
    m_nlin = -9999;
    
    /* allocate 1x1 deg buffer for reading DEM chunks */
    dem_data_1deg = (short int**)malloc(m_pplat * sizeof(short int*));
    for (int j = 0; j < m_pplat; j++) {
      dem_data_1deg[j] = (short int*)malloc(m_pplon * sizeof(short int));
    }
   
  }

  TerrainData
  ::~TerrainData()
  {
    int j;
    
    if (dem_data_1deg) {
      for (j = 0; j < m_pplat; j++)
	free(dem_data_1deg[j]);
    }
    if (dem_data) {
      for (j = 0; j < m_nlin; j++)
	free(dem_data[j]);
    }
  }

  bool
  TerrainData
  ::CheckFiles()
  {

    bool success = true;
    char filename[100];
    ifstream ifile;

    for (int f=0;f<16;f++) {
      sprintf(filename,"%s/%c%s",m_PathToDEM.c_str(),f+97,"10g");
      ifile.open(filename);
      if(ifile) {
	printf("DEM File exists: %s \n",filename);
	ifile.close();
      } else {
	printf("DEM File doesn't exist: %s \n",filename);
	success = false;
      }
    }

    if (!success) printf("DEM Dataset incomplete. Please Check!\n");
    
    return success;
  }

  void
  TerrainData
  ::CreateColorTable()
  {

    int np = 12;
    int nx = 256;
    int p0,p1;
    int x0,x1,xp;
    unsigned char y0,y1;
    unsigned char dem_colors [12][3] =
      { {19,100,12},
	{43,95,38},
	{46,150,28},
	{30,181,4},
	{114,211,37},
	{220,220,73},
	{230,215,73},
	{190,146,72},
	{166,80,29},
	{138,27,15},
	{184,123,124},
	{220,220,220} };

    for (int i=0;i<256;i++) {
      p0=i*(np-1)/nx;
      p1=p0+1;
      xp=i;
      x0=p0*nx/(np-1);
      x1=p1*nx/(np-1);

      //printf("%i %i %i %i %i %i \n",i,p0,p1,x0,xp,x1);

      for (int c=0;c<3;c++) {
	y0 = dem_colors[p0][c];
	y1 = dem_colors[p1][c];
	dem_colortable[i][c] = (float) y0 + (((float) y1 - (float) y0)/ ((float) x1 - (float) x0)) *  ((float) xp - (float) x0);
      }

    }
    
    
    /* index 0 is water */
    dem_colortable[0][0] = 0;
    dem_colortable[0][1] = 100;
    dem_colortable[0][2] = 200;

  }

  
  void
  TerrainData
  ::CalcBounds(double lon, double lat, int *lonmin, int *lonmax, int *latmin, int *latmax)
  {
    /* these are the outer bounds of the area to be read */
    
    if ((lon>=-180.0) && (lon<=180.0) && (lat>=-90.0) && (lat<=90.0)) {
      /* limit to 179/89 because lon = 179 will read lons 179-180 and lat 89 will read lats 89-90 */
      int curlon = min((int) lon,179);
      int curlat = min((int) lat,89);

      int m_nlon = min((((float) ((m_nlat-1)/2)) / max(cos((float) curlat * M_PI / 180.0),0.01) + 1.0)*2.0,360.0);
      
      /* TODO: HANDLE Dateline and Poles */
      *lonmin = max(min(curlon - (m_nlon-1)/2,179),-180);
      *lonmax = max(min(curlon + (m_nlon-1)/2+1,180),-179);
      *latmin = max(min(curlat - (m_nlat-1)/2,89),-90);
      *latmax = max(min(curlat + (m_nlat-1)/2+1,90),-89);

      /*
      printf("Current lon/lat: %i %i \n",curlon,curlat);
      printf("Lonmin/Lonmax: %i %i \n",*lonmin,*lonmax);
      printf("Latmin/Latmax: %i %i \n",*latmin,*latmax);
      printf("nlon/nlat: %i %i \n",m_nlon,m_nlat);
      */
    }
      
  }
  
  void
  TerrainData
  ::ReadDEMLonLat(int lon, int lat)
  {
    // lon and lat are the LL corner of the degree square
    // so lon=5, lat=47 covers the area of 5-6E and 47-48N
    
    // 4 j tiles are arranged from N->S by 40, 50, 50, 40 degrees latitude
    // 4 i tiles are arranged from W->E by 90 degrees each

    if ((lon != -9999) && (lat != -9999)) {
    
      int i,j,f,x,y,l;
      int latmax, lat1;
      ifstream ifile;
      short buffer[m_pplon];
      char filename[100];

      printf("Reading DEM for lon=%i, lat=%i \n",lon,lat);
      
      lat1=lat+1; // We need UL corner
    
      if (lat1>50) {
	j=0;
	latmax = 90;
      } else if (lat1>0) {
	j=1;
	latmax = 50;
      } else if (lat1>-50) {
	j=2;
	latmax = 0;
      } else {
	j=3;
	latmax = -50;
      }
      i = min((lon+180)/90,3);

      f = i + (j*4);

      x = (lon - (i*90-180))*m_pplon;
      y = (latmax - lat1)*m_pplat;

      //printf("%i %i \n",x,y);
    
      sprintf(filename,"%s/%c%s",m_PathToDEM.c_str(),f+97,"10g");
      ifile.open (filename, ios::in | ios::binary);
      for (l=0;l<m_pplat;l++) {
	ifile.seekg (2*(x+(y+l)*90*m_pplon));
	ifile.read ((char*)&buffer, sizeof(buffer));
	//printf("%i %i %i %i \n",buffer[0],buffer[1],buffer[2],buffer[3]);
	memcpy(dem_data_1deg[m_pplat-l-1],buffer,sizeof(buffer));
      }
      ifile.close();

    }
    
  }

  void
  TerrainData
  ::ReadDEM(int lonmin, int lonmax, int latmin, int latmax)
  {
    int j;
    int x,y;
    int u,v;
    bool exists;
    short int **tmp_data = NULL;
    int nlon, nlat;
    int ncol, nlin;

    /* new requested dimension */
    nlon = (lonmax - lonmin);
    nlat = (latmax - latmin);
    ncol = nlon * m_pplon;
    nlin = nlat * m_pplat;

    /* check if we already had a DEM stored, use what can be used from it */
    if (dem_data) {
      exists = true;
      /* create temporary storage to hold previous DEM */
      tmp_data = (short int**)malloc(m_nlin * sizeof(short int*));
      for (j = 0; j < m_nlin; j++) {
	tmp_data[j] = (short int*)malloc(m_ncol * sizeof(short int));
	memcpy(&tmp_data[j][0],&dem_data[j][0],m_ncol * sizeof(short int));
      }
      
      for (int j = 0; j < m_nlin; j++) {
	free(dem_data[j]);
      }
      free(dem_data);
    } else {
      exists = false;
    }
      
    /* allocate buffer for reading DEM */
    dem_data = (short int**)malloc(nlin * sizeof(short int*));
    for (j = 0; j < nlin; j++) {
      dem_data[j] = (short int*)malloc(ncol * sizeof(short int));
    }

    for (int lat=latmin;lat<latmax;lat++) {
      for (int lon=lonmin;lon<lonmax;lon++) {

	if (((lon-m_lonmin)>=0) && ((lon-m_lonmin)<m_nlon) &&
	    ((lat-m_latmin)>=0) && ((lat-m_latmin)<m_nlat) && exists) {
	  /* Copy from existing DEM array */
	  for (j=0;j<m_pplat;j++) {
	    /* calculate start indices of new domain to previous domain */
	    x = (lon-lonmin)*m_pplon;
	    y = j+(lat-latmin)*m_pplat;
	    u = (lon-m_lonmin)*m_pplon;
	    v = j+(lat-m_latmin)*m_pplat;
	    //printf("%i %i %i %i %i \n",j,x,y,u,v);
	    memcpy(&dem_data[y][x],&tmp_data[v][u],m_pplon * sizeof(short int));
	  }
	} else {
	  /* Read from Disk */
	  ReadDEMLonLat(lon,lat);

	  for (j=0;j<m_pplat;j++) {
	    /* calculate start indices of full domain to fit 1x1 deg cell line */
	    x = (lon-lonmin)*m_pplon;
	    y = j+(lat-latmin)*m_pplat;
	    memcpy(&dem_data[y][x],&dem_data_1deg[j][0],m_pplon * sizeof(short int));
	  }
	}
      }
    }

    /* free temp storage */
    if (tmp_data) {
      for (int j = 0; j < m_nlin; j++) {
	free(tmp_data[j]);
      }
      free(tmp_data);
    }

    m_lonmin = lonmin;
    m_lonmax = lonmax;
    m_latmin = latmin;
    m_latmax = latmax;
    m_nlon = nlon;
    m_nlat = nlat;
    m_ncol = ncol;
    m_nlin = nlin;
    
  }
    

} // end namespace OpenGC
