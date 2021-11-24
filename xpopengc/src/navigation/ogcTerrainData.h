/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavaidList.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:13 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ogcTerrainData_h
#define ogcTerrainData_h

#include <list>
#include <string>
#include "ogcGeographicObjectList.h"

namespace OpenGC
{

class TerrainData : public GeographicObjectList
{

public:

  TerrainData();
  virtual ~TerrainData();

  void SetPathToDEM(string pathToDEM) { m_PathToDEM = pathToDEM;}
  
  /* Check if DEM Files are there, returns true if successful, else false */
  bool CheckFiles();

  /* Create the color table for terrain visualization */
  void CreateColorTable();
  
  /* Set the bounds of the data to read from the current location */
  void CalcBounds(double lon, double lat, int *lonmin, int *lonmax, int *latmin, int *latmax);

  /* Get the bounds of the dem data stored for the current location */
  void GetBounds(int* lonmin, int* lonmax, int* latmin, int* latmax) {
    *lonmin = m_lonmin;
    *lonmax = m_lonmax;
    *latmin = m_latmin;
    *latmax = m_latmax;
  }

  /* Set the bounds of the dem data stored for the current location */
  void SetBounds(int lonmin, int lonmax, int latmin, int latmax) {
    m_lonmin = lonmin;
    m_lonmax = lonmax;
    m_latmin = latmin;
    m_latmax = latmax;
  }

  void GetResolution(int* pplon, int *pplat) {
    *pplon = m_pplon;
    *pplat = m_pplat;
  }

  /* Read 1x1 degree lon/lat square covering integer lon/lat */
  void ReadDEMLonLat(int lon, int lat);

  /* Read DEM covering integer lon/lat bounds */
  void ReadDEM(int lonmin, int lonmax, int latmin, int latmax);

  /* DEM is stored from S->N and W->E */
  short int **dem_data;
  short int **dem_data_1deg;

  short int dem_miss;

  unsigned char dem_colortable[256][3];
  
protected:
  string m_PathToDEM;

  int m_nlon; // number of longitudes to store. odd number: e.g. 5: will be current lon +/- 2 lon
  int m_nlat; // number of latitudes to store. odd number: e.g. 5: will be current lat +/- 2 lat
  int m_pplon; // number of pixels per longitude
  int m_pplat; // number of pixels per latitude
  int m_lonmin;
  int m_lonmax;
  int m_latmin;
  int m_latmax;
  int m_ncol;
  int m_nlin;

};

} // end namespace OpenGC

#endif
