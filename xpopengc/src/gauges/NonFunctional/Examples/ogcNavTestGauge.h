/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavTestGauge.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:11 $
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
 * Test gauge for nav stuff
 */

#ifndef ogcNavTestGauge_h
#define ogcNavTestGauge_h

#include "ogcGauge.h"

namespace OpenGC
{

class NavTestGauge : public Gauge  
{
public:

  NavTestGauge();
  virtual ~NavTestGauge();

  void Render();

  /** Called if the down mouse click applies to this object */
  void OnMouseDown(int button, double physicalX, double physicalY);

protected:

  /** The font number provided to us by the font manager */
  int m_Font;
};

} // end namespace OpenGC

#endif
