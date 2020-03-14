/*=============================================================================

  This is the ogcA320BrkTripleIndDial.h header to ogcA320BrkTripleIndDial.cpp

  Created:
    Date:   2015-06-14
    Author: Hans Jansen
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
 * Airbus A320 style Triple Brake Indicator: the Dials
 */

#ifndef ogcA320BrkTripleIndDial_h
#define ogcA320BrkTripleIndDial_h

#include "ogcGaugeComponent.h"

extern int verbosity;

namespace OpenGC {

  class A320BrkTripleIndDial : public GaugeComponent {

    public:

      A320BrkTripleIndDial ();
      virtual ~A320BrkTripleIndDial ();

      void Render();

    protected:

      /** Font from the font manager */
      int m_Font;

  };

} // end namespace OpenGC

#endif
