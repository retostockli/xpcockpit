/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Original Developer's web site at http://www.opengc.org
  
  Now hosted by xpcockpit project on github.com
   
  Copyright (c) 2021 Reto Stockli

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ogcShorelineData_h
#define ogcShorelineData_h

#include <list>
#include <string>
#include <stdint.h>
#include "ogcGeographicObjectList.h"

namespace OpenGC
{

class ShorelineData : public GeographicObjectList
{

public:

  ShorelineData();
  virtual ~ShorelineData();

  void SetPathToGSHHG(string pathToGSHHG) { m_PathToGSHHG = pathToGSHHG;}
  
  /* Check if GSHHG Files are there, returns true if successful, else false */
  bool CheckFiles();

  /* Read Global Shoreline and Lake Data */
  bool ReadShoreline();

  int num_shorelines;
  int *num_shorelinepoints;
  float *shoreline_centerlon;
  float *shoreline_centerlat;
  float **shoreline_lon;
  float **shoreline_lat;
  
protected:
  
  string m_PathToGSHHG;

  struct GSHHG_HEADER {  /* Global Self-consistent Hierarchical High-resolution Shorelines */
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
    int ancestor;   /* Id of ancestor polygon in the full resolution set that was the source of this polygon (-1 if none) */
  };
    
  /* Following each header structure is n structures of coordinates: */
  struct GSHHG_POINT {	/* Each lon, lat pair is stored in micro-degrees in 4-byte signed integer format */
    int32_t x;
    int32_t y;
  };

  //! Byte swap int
  int32_t swap_int32( int32_t val )
  {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | ((val >> 16) & 0xFFFF);
  }
  
};

} // end namespace OpenGC

#endif
