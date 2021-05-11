/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGeographicObject.h,v $

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

/**
 * The base class for all objects that occupy a location on the surface of the earth
 */

#if !defined(ogcGeographicObject_h)
#define ogcGeographicObject_h

#include <stdio.h>
#include <string>

namespace OpenGC
{

using namespace std;

class GeographicObject
{
public:

  GeographicObject();
  virtual ~GeographicObject();
  
  /** Set physical parameters */
  void SetDegreeLat(double degree) {m_DegreeLat = degree;}
  void SetDegreeLon(double degree) {m_DegreeLon = degree;}
  void SetAltitudeMeters(double alt) { m_AltitudeMeters = alt; }
  
  /** Set mercator map coordinates */
  void SetMercatorMeters(double n, double e) {m_MercatorNorthingMeters = n; m_MercatorEastingMeters = e;}
  
  /** Get mercator map coordinates */
  void GetMercatorMeters(double & n, double & e) {n = m_MercatorNorthingMeters; e = m_MercatorEastingMeters;}
  
  /** Set names */
  void SetIdentification(string s) { m_Identification = s; }
  void SetFullname(string s) { m_FullName = s; }

  /** Accessors for physical parameters */
  double GetAltitudeMeters() { return m_AltitudeMeters; }
  double GetDegreeLat() { return m_DegreeLat; }
  double GetDegreeLon() { return m_DegreeLon; }
  
  /** Accessors for IDs */
  string GetIdentification() { return m_Identification; }
  string GetFullName() { return m_FullName; };
  
  /** Accessors for frequency */
  void SetFrequency(float freq) {m_Frequency = freq;}
  float GetFrequency() {return m_Frequency;}
  
  /** Accessors for navaid type */
  void SetNavaidType(unsigned int type) {m_NavaidType = type;};
  unsigned int GetNavaidType() {return m_NavaidType;};

protected:

  /** Physical parameters */
  double m_DegreeLat, m_DegreeLon, m_AltitudeMeters;
  
  /** Mercator map coordinates in meters */
  double m_MercatorNorthingMeters, m_MercatorEastingMeters;
  
  /** Brief name, for instance KPIT */
  string m_Identification;
  
  /** Full name, for instance Pittsburgh International Airport */
  string m_FullName;

  /** Frequency of the navaid */
  float m_Frequency;
  
  /** Type of navaid */
  unsigned int m_NavaidType;

};

  void lonlat2azeq(double *lon, double *lat, double *x_meters, double *y_meters, double *lon0, double *lat0);
  void lonlat2gnomonic(double *lon, double *lat, double *x_meters, double *y_meters, double *lon0, double *lat0);
  double heading_from_a_to_b(double *lon1, double *lat1, double *lon2, double *lat2);
  double modulus(double y, double x);
  double distance_from_a_to_b(double *lon1, double *lat1, double *lon2, double *lat2);
  void latlon_at_dist_heading(double *lon, double *lat, double *dist, double *track, double *lon2, double *lat2);

} // end namespace OpenGC

#endif
