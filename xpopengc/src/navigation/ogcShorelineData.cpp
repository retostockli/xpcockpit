/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcShorelineData.cpp,v $

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
#include <byteswap.h>
#include "ogcShorelineData.h"

namespace OpenGC
{

  using namespace std;

  ShorelineData
  ::ShorelineData()
  {
    num_shorelines = 0;
    num_shorelinepoints = NULL;
    shoreline_lon = NULL;
    shoreline_lat = NULL;
  }

  ShorelineData
  ::~ShorelineData()
  {
    int i;
    if (num_shorelinepoints) free(num_shorelinepoints);
    if (shoreline_centerlon) free(shoreline_centerlon);
    if (shoreline_centerlat) free(shoreline_centerlat);
    if (shoreline_lon) {
      for (i=0;i<num_shorelines;i++) {
	free(shoreline_lon[i]);
      }
      free(shoreline_lon);
    }
    if (shoreline_lat) {
      for (i=0;i<num_shorelines;i++) {
	free(shoreline_lat[i]);
      }
      free(shoreline_lat);
    }
      
  }

  bool
  ShorelineData
  ::CheckFiles()
  {

    bool success = true;
    char filename[100];
    ifstream ifile;

    sprintf(filename,"%s/%s",m_PathToGSHHG.c_str(),"gshhs_h.b");
    ifile.open(filename);
    if(ifile) {
      printf("GSHHG Shoreline File exists: %s \n",filename);
      ifile.close();
    } else {
      printf("GSHHG Shoreline File doesn't exist: %s \n",filename);
      success = false;
    }

    if (!success) printf("GSHHG Shoreline Dataset incomplete. Please Check!\n");
    
    return success;
  }

  bool
  ShorelineData
  ::ReadShoreline()
  {
     
    char filename[100];
    ifstream ifile;
    int nshorelines;
    int level;
    int i,f;
    struct GSHHG_HEADER header;
    struct GSHHG_POINT points;
    
    sprintf(filename,"%s/%s",m_PathToGSHHG.c_str(),"gshhs_h.b");

    /* Read file twice: first for getting number of polygons, second read the polygons */
    for (f=0;f<2;f++) {
      nshorelines = 0;
      ifile.open (filename, ios::in | ios::binary);
      while (ifile.peek() != EOF) {
	ifile.read ((char*)&header, sizeof(header));
	header.id = swap_int32(header.id);
	header.n = swap_int32(header.n);
	header.flag = swap_int32(header.flag);
	header.west = swap_int32(header.west);
	header.east = swap_int32(header.east);
	header.south = swap_int32(header.south);
	header.north = swap_int32(header.north);
	header.area = swap_int32(header.area);
	header.area_full = swap_int32(header.area_full);
	header.container = swap_int32(header.container);
	header.ancestor = swap_int32(header.ancestor);
	level = header.flag & 255;
	if ((f==1) && (level == 2)) {
	  /* allocate polygon points for lakes */
	  num_shorelinepoints[nshorelines] = header.n;
	  shoreline_centerlon[nshorelines] = 0.0;
	  shoreline_centerlat[nshorelines] = 0.0;
	  shoreline_lon[nshorelines] = (float*)malloc(header.n * sizeof(float));
	  shoreline_lat[nshorelines] = (float*)malloc(header.n * sizeof(float));
	}
      
	for (i=0;i<header.n;i++) {
	  ifile.read ((char*)&points, sizeof(points));
	  points.x = swap_int32(points.x);
	  points.y = swap_int32(points.y);
	  //printf("%i %i %i \n",i,points.x,points.y);
	  if ((f==1) && (level == 2)) {
	    /* read polygon points for lakes */
	    if (points.x > 180000000) {
	      /* move all 180 - 360 degree lons to -180 to 0 degree lon */
	      shoreline_lon[nshorelines][i] = (float) (points.x - 360000000) / 1.e6;
	    } else {
	      shoreline_lon[nshorelines][i] = (float) points.x / 1.e6;
	    }
	    shoreline_lat[nshorelines][i] = (float) points.y / 1.e6;
	    shoreline_centerlon[nshorelines] += shoreline_lon[nshorelines][i];
	    shoreline_centerlat[nshorelines] += shoreline_lat[nshorelines][i];
	    //printf("%f %f \n",shoreline_lon[nshorelines][i],shoreline_lat[nshorelines][i]);
	  }
	}
	if ((f==1) && (level == 2)) {
	  shoreline_centerlon[nshorelines] /= (float) header.n;
	  shoreline_centerlat[nshorelines] /= (float) header.n;
	  //printf("%i %f %f \n",nshorelines,shoreline_centerlon[nshorelines],shoreline_centerlat[nshorelines]);
	}
       
	if (level == 2) {
	  nshorelines++;
	}
      }
      ifile.close();
      if (f==0) {
	num_shorelines = nshorelines;
	printf("Total number of Lakes: %i \n",num_shorelines);
	num_shorelinepoints = (int*)malloc(num_shorelines * sizeof(int));
	shoreline_centerlon = (float*)malloc(num_shorelines * sizeof(float));
	shoreline_centerlat = (float*)malloc(num_shorelines * sizeof(float));
	shoreline_lon = (float**)malloc(num_shorelines * sizeof(float*));
	shoreline_lat = (float**)malloc(num_shorelines * sizeof(float*));
      }
    }

    if (num_shorelines > 0) {
      return true;
    } else {
      printf("No Lakes found! \n");
      return false;
    }
  }
    

} // end namespace OpenGC
