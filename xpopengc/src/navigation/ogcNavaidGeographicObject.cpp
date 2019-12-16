/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavaidGeographicObject.cpp,v $

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

#include "ogcNavaidGeographicObject.h"

namespace OpenGC
{

NavaidGeographicObject
::NavaidGeographicObject()
{
  m_Frequency = 0.0;
  m_NavaidType = 0;
}

NavaidGeographicObject
::~NavaidGeographicObject()
{

}

} // end namespace OpenGC
