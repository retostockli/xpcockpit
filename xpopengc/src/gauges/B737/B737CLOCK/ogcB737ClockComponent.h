/*=============================================================================

  This is the ogcB737ClockComponent.h header to ogcB737ClockComponent.cpp

  Created:
    Date:        2015-06-14
    Author:      Hans Jansen
    Last change: 2020-01-22
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
 * B737 style Main-Panel Clock: The Clock Window
 */

#ifndef ogcB737ClockComponent_h
#define ogcB737ClockComponent_h

#include "ogcGaugeComponent.h"

extern int verbosity;

namespace OpenGC {

  class B737ClockComponent : public GaugeComponent {

    public:
      B737ClockComponent ();
      virtual ~B737ClockComponent ();
      void Render ();

    protected:
      int m_Font;
      int m_DigiFont;
      
  };

} // end namespace OpenGC

#endif
