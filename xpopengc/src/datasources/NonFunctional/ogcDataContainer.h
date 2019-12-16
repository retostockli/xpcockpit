/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcDataContainer.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:56 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2004 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ogcDataContainer_h
#define ogcDataContainer_h

#include "ogcDataSourceMacros.h"

namespace OpenGC
{

class DataContainer
{
public:

  DataContainer();
  virtual ~DataContainer();

  void InitializeData();
};

} // end namespace OpenGC
#endif
