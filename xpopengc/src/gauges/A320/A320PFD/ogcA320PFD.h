/*=============================================================================

  This is the ogcA320PFD.h header to ogcA320PFD.cpp

  Created:
    Date:   2011-11-14
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
 * Airbus A320 style Primary Flight Display
 */

#ifndef OGCA320PFD_H
#define OGCA320PFD_H

#include "ogcGauge.h"
#include "../ogcAirbusColors.h"

extern int verbosity;

namespace OpenGC
{

  /** Central handling of the Cold&Dark state */
  extern bool ColdAndDark ();

  class A320PFD : public Gauge  
  {
    public:

      A320PFD();
      virtual ~A320PFD();

      /** Overloaded render function */
      void Render();

      /** Specify whether this gauge should be rotated */
      void SetRotation(bool rot);

  protected:

    /** Font from the font manager */
    int m_Font;

    /** Rotation parameter for the gauge */
    int m_GaugeRotation;
  };
} // end namespace OpenGC
#endif
