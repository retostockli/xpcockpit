/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcDummyGauge.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:05 $
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
 * Dummy gauge used for initializing static ogcRenderObject members
 */

#ifndef ogcDummyGauge_h
#define ogcDummyGauge_h

#include "ogcGauge.h"

namespace OpenGC
{

class DummyGauge : public Gauge  
{
public:

  DummyGauge();
  virtual ~DummyGauge();

  void Render();

};

} // end namespace OpenGC

#endif
