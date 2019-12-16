/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavTestComponent.h,v $

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

#ifndef ogcNavTestComponent_h
#define ogcNavTestComponent_h

#include "ogcGauge.h"

namespace OpenGC
{

class NavTestComponent : public GaugeComponent
{
public:

  NavTestComponent();
  virtual ~NavTestComponent();

  void Render();

  /** Message handler override */
  void OnOpenGCMessage(Message message, void* data);

protected:

  /** The height (and width) of the component in nautical miles */
  double m_SizeNM;
  
  /** The font number provided to us by the font manager */
  int m_Font;
};

} // end namespace OpenGC

#endif
