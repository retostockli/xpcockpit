/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavDatabase.h,v $

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

/**
 * ogcNavDatabase is the container that stores all other nav information
 * used by OpenGC.
 */

#ifndef ogcNavDatabase_h
#define ogcNavDatabase_h

#include <string>
#include "ogcNavaidList.h"
#include "ogcAirportList.h"
#include "ogcFixList.h"
#include "ogcGeographicHash.h"
#include "ogcTerrainData.h"
#include "ogcShorelineData.h"

namespace OpenGC
{

class NavDatabase  
{

public:

  NavDatabase();
  virtual ~NavDatabase();
  
  /** Load the nav data */
  bool InitDatabase(string pathToNav, string pathToDEM, string pathToGSHHG, int customdata);
  
  /** Get the navaid list */
  NavaidList* GetNavaidList() {return m_NavaidList;}
  
  /** Get the navaid hash */
  GeographicHash* GetNavaidHash() {return m_NavaidHash;}
  
  /** Get the fix list */
  FixList* GetFixList() {return m_FixList;}
  
  /** Get the fix hash */
  GeographicHash* GetFixHash() {return m_FixHash;}
  
  /** Get the airport list */
  AirportList* GetAirportList() {return m_AirportList;}
  
  /** Get the airport hash */
  GeographicHash* GetAirportHash() {return m_AirportHash;}

  /** Get the Terrain Data Object **/
  TerrainData* GetTerrainData() {return m_TerrainData;}
  
  /** Get the Shoreline Data Object **/
  ShorelineData* GetShorelineData() {return m_ShorelineData;}
  
protected:

  /** A list of all nav objects */
  NavaidList* m_NavaidList;
  
  /** Nav objects hashed by lat/lon */
  GeographicHash* m_NavaidHash;

  /** A list of all fix objects */
  FixList* m_FixList;
  
  /** Fix objects hashed by lat/lon */
  GeographicHash* m_FixHash;
  
  /** A list of all airports */
  AirportList* m_AirportList;

  /** Airport objects hashed by lat/lon */
  GeographicHash* m_AirportHash;

  /** Terrain Database **/
  TerrainData* m_TerrainData;
  
  /** Shoreline Database **/
  ShorelineData* m_ShorelineData;
  
};

} // end namespace OpenGC

#endif
