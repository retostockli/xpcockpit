/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGaugeComponent.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:54 $
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
 * GaugeComponents are the most "primitive" building block in the
 * OpenGC design scheme. Gauges are composed of a number of
 * GaugeComponents, each of which is capable of positioning and
 * drawing itself.
 *
 * By breaking a gauge into multiple GaugeComponents, recycling
 * of code is encouraged between different gauge designs.
 */

#ifndef ogcGaugeComponent_h
#define ogcGaugeComponent_h

#include "ogcFontManager.h"
#include "ogcDataSource.h"
#include "ogcOrderedPair.h"
#include "ogcRenderObject.h"

namespace OpenGC
{

class GaugeComponent: public RenderObject
{
public:
  GaugeComponent();
  virtual ~GaugeComponent();

  /** handle positioning and clipping */
  void setPosition ();
  void setClip ();
  void unsetClip ();

  /** Render the gauge component */
  virtual void Render();

  /** Return true if the click is inside the gauge component */
  bool ClickTest(int button, int state, int x, int y);
};

} // end namespace OpenGC

#endif

