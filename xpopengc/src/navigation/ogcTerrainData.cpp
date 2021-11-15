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
#include "ogcTerrainData.h"

namespace OpenGC
{

  using namespace std;

  TerrainData
  ::TerrainData()
  {
    m_nlon = 7;
    m_nlat = 5;
   
  }

  TerrainData
  ::~TerrainData()
  {

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
	//printf("DEM File exists: %s \n",filename);
	ifile.close();
      } else {
	printf("DEM File doesn't exist: %s \n",filename);
	success = false;
      }
    }

    if (!success) printf("DEM Dataset incomplete. Please Check\n");
    
    return success;
  }

  void
  TerrainData
  ::SetCurrentLonLat(double lon, double lat)
  {
    m_curlon = (int) lon;
    m_curlat = (int) lat;

    /* HANDLE Dateline and Poles */
    m_lonmin = max(min(m_curlon - (m_nlon-1)/2,180),-180);
    m_lonmax = max(min(m_curlon + (m_nlon-1)/2,180),-180);
    m_latmin = max(min(m_curlat - (m_nlat-1)/2,90),-90);
    m_latmax = max(min(m_curlat + (m_nlat-1)/2,90),-90);
   
    printf("Current lon/lat: %i %i \n",m_curlon,m_curlat);
    printf("Lonmin/Lonmax: %i %i \n",m_lonmin,m_lonmax);
    printf("Latmin/Latmax: %i %i \n",m_latmin,m_latmax);
    
  }

} // end namespace OpenGC
