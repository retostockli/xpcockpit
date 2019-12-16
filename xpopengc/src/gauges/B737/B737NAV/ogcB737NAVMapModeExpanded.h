/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MapModeExpanded.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:10 $
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
 * Test gauge component for nav stuff
 */

#ifndef ogcB737NAVMapModeExpanded_h
#define ogcB737NAVMapModeExpanded_h

#include "ogcGauge.h"
#include "ogcB737NAV.h"

namespace OpenGC
{

class B737NAVMapModeExpanded : public GaugeComponent
{
public:

  B737NAVMapModeExpanded();
  virtual ~B737NAVMapModeExpanded();

  void Render();

  void SetNAVGauge(B737NAV* NAVGauge) {m_NAVGauge = NAVGauge;}

protected:
  
  /** The font number provided to us by the font manager */
  int m_Font;

  /* Pointer to the calling Navigation Gauge */
  B737NAV* m_NAVGauge;

};

} // end namespace OpenGC

#endif
