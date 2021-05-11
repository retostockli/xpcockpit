/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGeographicObject.cpp,v $

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

#include <math.h>
#include "ogcGeographicObject.h"

namespace OpenGC
{

GeographicObject
::GeographicObject()
{
  m_AltitudeMeters=0.0;
  m_DegreeLat=0.0;
  m_DegreeLon=0.0;

  m_Frequency = 0.0;
  m_NavaidType = 0;
}

GeographicObject
::~GeographicObject()
{

}


  void lonlat2azeq(double *lon, double *lat, double *x_meters, double *y_meters, double *lon0, double *lat0)
  {
    /* Transforms longitude/latitude to Azimuthal Equidistant Projection in meters distance
       north/east to the center longitude/latitude.
       This geographic projection is best used for navigation displays since it conserves
       both distance and direction 
       Formula from wikipedia site "Azimuthal equidistant projection" */

    /* for now use spherical earth, may switch to WGS84 ellipsoid */
    double a = 6378137.0; /* major axis (m) */
    //    double b = 6356752.314245; /* semi-major axis (m) */
    double dtor = 0.0174533; /* radians per degree */
    // double radeg = 57.2958;  /* degree per radians */

    double rho = acos(sin(*lat0 * dtor) * sin(*lat * dtor) + 
		      cos(*lat0 * dtor) * cos(*lat * dtor) * cos((*lon - *lon0) * dtor));

    double theta = atan2(cos(*lat * dtor) * sin((*lon - *lon0) * dtor),
			 cos(*lat0 * dtor) * sin(*lat * dtor) - 
			 sin(*lat0 * dtor) * cos(*lat * dtor) * cos((*lon - *lon0) * dtor));

    *x_meters = a * rho * sin(theta);
    *y_meters = a * -rho * cos(theta);

  }
  
  void lonlat2gnomonic(double *lon, double *lat, double *x_meters, double *y_meters, double *lon0, double *lat0)
  {
    /* Transforms longitude/latitude to Gnomonic Projection in meters distance
       north/east to the center longitude/latitude.
       This geographic projection is best used for navigation displays since it conserves
       both distance and direction 
       Formula from http://mathworld.wolfram.com/GnomonicProjection.html */

    /* for now use spherical earth, may switch to WGS84 ellipsoid */
    double a = 6378137.0; /* major axis (m) */
    //    double b = 6356752.314245; /* semi-major axis (m) */
    double dtor = 0.0174533; /* radians per degree */
    // double radeg = 57.2958;  /* degree per radians */

    // double cos_c = sin(*lat0 * dtor) * sin(*lat * dtor) + 
    //		    cos(*lat0 * dtor) * cos(*lat * dtor) * cos((*lon - *lon0) * dtor);

    double cos_c = 1.0; // ZIBO MOD SIMPLIFICATION
    
    *x_meters = a * cos(*lat * dtor) * sin((*lon - *lon0) * dtor) / cos_c;

    *y_meters = -a * (cos(*lat0 * dtor) * sin(*lat * dtor) - sin(*lat0 * dtor) * cos(*lat * dtor) *
		 cos((*lon - *lon0) * dtor)) / cos_c;
     
  }

  double heading_from_a_to_b(double *lon1, double *lat1, double *lon2, double *lat2)
  {
    /* Calculates heading in degrees wrt N from point 1 to point 2 on the globe */
    
    double dtor = 0.0174533; /* radians per degree */
    
    double angle = modulus(atan2(sin(dtor * (*lon2 - *lon1)) * cos(dtor * *lat2),
				 cos(dtor * *lat1) * sin(dtor * *lat2) -
				 sin(dtor * *lat1)*cos(dtor * *lat2)*cos(dtor * (*lon2 - *lon1))),
			   2.0 * M_PI) * 180.0 / M_PI;
    return angle;
  }
  
  double modulus(double y, double x) {
    return (y - x * floor(y / x));
  }
  
  double distance_from_a_to_b(double *lon1, double *lat1, double *lon2, double *lat2)
  {
    /* calculates distance between two points on the globe in NM */
    
    double dtor = 0.0174533; /* radians per degree */
    // why 3443.9f? Wikipedia says that the average radius of the earth is 3440.07NM
    
    double distance = acos( (cos(dtor * *lat1) * cos(dtor * *lon1) * cos(dtor * *lat2) * cos(dtor * *lon2)) +
			    (cos(dtor * *lat1) * sin(dtor * *lon1) * cos(dtor * *lat2) * sin(dtor * *lon2)) +
			    (sin(dtor * *lat1) * sin(dtor * *lat2)) ) * 3443.9;
    
    return distance;
  }

  void latlon_at_dist_heading(double *lon, double *lat, double *dist, double *track, double *lon2, double *lat2)
  {

    double dtor = 0.0174533; /* radians per degree */
    double radeg = 57.2958;  /* degree per radians */
    double earthRadius = 6378.1; /* earth radius in km */
    
    *lat2 = radeg * asin( sin(*lat * dtor) * cos(*dist * 1.852 / earthRadius) +
			  cos(*lat * dtor) * sin(*dist * 1.852 / earthRadius) * cos(*track * dtor));
    *lon2 = radeg * (*lon * dtor + atan2(sin(*track * dtor) * sin(*dist * 1.852 / earthRadius) * cos(*lat * dtor),
					 cos(*dist * 1.852 / earthRadius) - sin(*lat * dtor) * sin(*lat2 * dtor)));
    
  }
 
} // end namespace OpenGC
