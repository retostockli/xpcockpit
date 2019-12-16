/*=========================================================================

  NOTE: temporary file while waiting for Reto's version!

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFixList.h,v $

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

#ifndef ogcFixList_h
#define ogcFixList_h

#include <list>
#include <string>
#include "ogcGeographicObjectList.h"

namespace OpenGC
{

class FixList : public GeographicObjectList
{

public:

  FixList();
  virtual ~FixList();
  
  /** Read a file containing fixes, returns true if successful, else false */
  bool LoadData(string fileName);
};

} // end namespace OpenGC

#endif
