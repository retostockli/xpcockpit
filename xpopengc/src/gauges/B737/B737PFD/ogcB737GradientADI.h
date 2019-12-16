/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737GradientADI.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:07 $
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
 * Boeing 777 type artificial horizon, with modifications to show how
 * easy it is to use an alternate (gradient) color scheme.
 */

#ifndef ogcB737GradientADI_h
#define ogcB737GradientADI_h

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737GradientADI : public GaugeComponent  
{
public:

  B737GradientADI();
  virtual ~B737GradientADI();

  /** Overloaded render function */
  void Render();

protected:

  /** The font number provided to us by the font manager */
  int m_Font;
};

} // end namespace OpenGC

#endif
