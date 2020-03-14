/*=============================================================================

  This is the ogcA320EWDSlatsFlaps.h header to ogcA320EWDSlatsFlaps.cpp

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-06
    (see ogcSkeletonGauge.cpp for more details)

  Copyright (C) 2011-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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
 * Airbus A320 style Engine/Warning Display: the Slats/Flaps Indicator
 */

#ifndef A320EWDSlatsFlaps_H
#define A320EWDSlatsFlaps_H

#include "ogcGaugeComponent.h"
#include "../ogcAirbusColors.h"

extern int verbosity;

namespace OpenGC {

  class A320EWDSlatsFlaps : public GaugeComponent {

  public:
      A320EWDSlatsFlaps ();
      virtual ~A320EWDSlatsFlaps ();
      void Render ();
      void setCurrentEngine (int engine);

  protected:
      int m_Font;
      int currentEngine;

  };

} // end namespace OpenGC

#endif
