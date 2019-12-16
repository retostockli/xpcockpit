/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGeographicObjectList.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:12 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stdio.h>
#include <math.h>
#include "ogcGeographicObjectList.h"

namespace OpenGC
{

using namespace std;

GeographicObjectList
::GeographicObjectList()
{

}

GeographicObjectList
::~GeographicObjectList()
{
  // Delete all of the objects in this list
  GeographicObjectList::iterator it;
  
  for (it = this->begin(); it != this->end(); ++it)
  {
    if( (*it) != 0)
    {
      delete (*it);
      (*it) = 0;
    }
  }
  
}

bool
GeographicObjectList
::InitializeList(string filename)
{
  // Try to load the data
  bool result = this->LoadData(filename);
  
  // If we did, do the fancy coordinate math
  if(result)
  {
    this->ComputeAdditionalCoordinates();
    return true;
  }
  else
    return false;
}

void
GeographicObjectList
::ComputeAdditionalCoordinates()
{
  /*
  In general this function would compute all additional coordinate systems
  used for mapping, but for now it does only Mercator, as defined below
  
  L = latitude in radians (positive north)
  Lo = longitude in radians (positive east)
  E = easting (meters)
  N = northing (meters)

  For the sphere 

  E = r Lo
  N = r ln [ tan (pi/4 + L/2) ]

  where 

  r = radius of the sphere (meters)
  ln() is the natural logarithm
  */
  
  // lat/lon of geo object in _radians_
  double lat, lon;
  
  // whether or not the coordinate is in the southern hemisphere
  bool isInSouthernHemisphere;
  
  // mercator coordinates
  double northing, easting;
  
  // approximate radius of the earth in meters
  double r = 6371000.0;
  
  GeographicObjectList::iterator it;
  
  for (it = this->begin(); it != this->end(); ++it)
  {
    isInSouthernHemisphere = false;
    
    lat = (*it)->GetDegreeLat() * 3.1415927 / 180.0;
    lon = (*it)->GetDegreeLon() * 3.1415927 / 180.0;
    
    if(lat < 0)
    {
      isInSouthernHemisphere = true;
      lat = lat * -1.0;
    }
    
    easting = r * lon;
    northing = r * log(tan(3.1415927 / 4.0 + lat / 2.0));
    
    if(isInSouthernHemisphere)
      northing = northing * -1.0;
      
    (*it)->SetMercatorMeters(northing, easting);
  }
}

} // end namespace OpenGC
