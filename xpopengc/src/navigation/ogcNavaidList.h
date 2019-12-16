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

#ifndef ogcNavaidList_h
#define ogcNavaidList_h

#include <list>
#include <string>
#include "ogcGeographicObjectList.h"

namespace OpenGC
{

class NavaidList : public GeographicObjectList
{

public:

  NavaidList();
  virtual ~NavaidList();
  
  /** Read a file contain navaids, returns true if successful, else false */
  bool LoadData(string fileName);
};

} // end namespace OpenGC

#endif
