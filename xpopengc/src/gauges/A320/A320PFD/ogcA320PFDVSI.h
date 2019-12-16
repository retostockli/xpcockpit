/*=============================================================================

  This is the ogcA320PFDVSI.h header to ogcA320PFDVSI.cpp

  Created:
    Date:   2011-12-12
    Author: Hans Jansen
    (see ogcSkeletonGauge.cpp for more details)

  Copyright (C) 2011-2016 Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>. 

=============================================================================*/

/**
 * Airbus A320 style Primary Flight Display: the Vertical Speed Indicator
 */

#ifndef ogcA320PFDVSI_h
#define ogcA320PFDVSI_h

#include "ogcGaugeComponent.h"
#include "../ogcAirbusColors.h"

extern int verbosity;

namespace OpenGC
{
  class A320PFDVSI : public GaugeComponent  
  {
  public:

    A320PFDVSI();
    virtual ~A320PFDVSI();

    /** Overloaded render function */
    void Render();

  protected:

    /** Routine to convert an altitude to needle position */
    double VSpeedToNeedle(double vspd);

    /** The font number provided to us by the font manager */
    int m_Font;

    /** Centered position of needle in physical coordinates */
    double m_NeedleCenter;

    /** Max positive or negative deflection of needle in physical coordinates */
    double m_MaxNeedleDeflection;
  };
} // end namespace OpenGC

#endif
