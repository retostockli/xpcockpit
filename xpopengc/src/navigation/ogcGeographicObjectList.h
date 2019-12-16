/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGeographicObjectList.h,v $

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

#ifndef ogcGeographicObjectList_h
#define ogcGeographicObjectList_h

#include <list>
#include <string>
#include "ogcGeographicObject.h"

namespace OpenGC
{

class GeographicObjectList : public std::list<GeographicObject*>
{

public:

  GeographicObjectList();
  virtual ~GeographicObjectList();
  
  /** Initialize the object list - this consists of loading the data and computing positions
   * in coordinates other than lat / lon. The data loading stage should be overridden when
   * implementing a derived class
   */
  bool InitializeList(string filename);
  
  /** Read a file that contains true if successful, returns true if successful, else false */
  virtual bool LoadData(string fileName) { return false; };
  
  /** Computes additional coordinate positions for the objects in the list. This should be
   * called by LoadData in derived classes
   */
  void ComputeAdditionalCoordinates();
};

} // end namespace OpenGC

#endif
