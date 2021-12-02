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
  }

  ShorelineData
  ::~ShorelineData()
  {
  }

  bool
  ShorelineData
  ::CheckFiles()
  {

    bool success = true;
    char filename[100];
    ifstream ifile;

    sprintf(filename,"%s/%s",m_PathToGSHHG.c_str(),"gshhs_c.b");
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
    int npoints;
    int i;
    struct GSHHG_HEADER header;
    struct GSHHG_POINT points;
    
    sprintf(filename,"%s/%s",m_PathToGSHHG.c_str(),"gshhs_c.b");
    ifile.open (filename, ios::in | ios::binary);
    while (ifile.peek() != EOF) {
      num_shorelines++;
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
      printf("%i %i %i \n",header.id,header.n,header.area);
      for (i=0;i<header.n;i++) {
	ifile.read ((char*)&points, sizeof(points));
	points.x = swap_int32(points.x);
	points.y = swap_int32(points.y);
	//printf("%i %i %i \n",i,points.x,points.y);
      }
    }
    ifile.close();

    printf("Total number of Shorelines: %i \n",num_shorelines);
    
    return false;
  }
    

} // end namespace OpenGC
    int id;       /* Unique polygon id number, starting at 0 */
    int n;        /* Number of points in this polygon */
    int flag;     /* = level + version << 8 + greenwich << 16 + source << 24 + river << 25 */
    /* flag contains 5 items, as follows:
     * low byte:    level = flag & 255: Values: 1 land, 2 lake, 3 island_in_lake, 4 pond_in_island_in_lake
     * 2nd byte:    version = (flag >> 8) & 255: Values: Should be 12 for GSHHG release 12 (i.e., version 2.2)
     * 3rd byte:    greenwich = (flag >> 16) & 1: Values: Greenwich is 1 if Greenwich is crossed
     * 4th byte:    source = (flag >> 24) & 1: Values: 0 = CIA WDBII, 1 = WVS
     * 4th byte:    river = (flag >> 25) & 1: Values: 0 = not set, 1 = river-lake and level = 2
     */
    int west, east, south, north;   /* min/max extent in micro-degrees */
    int area;       /* Area of polygon in 1/10 km^2 */
    int area_full;  /* Area of original full-resolution polygon in 1/10 km^2*/
    int container;  /* Id of container polygon that encloses this polygon (1 if none) */
    int ancestor; 
