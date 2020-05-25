/*=============================================================================

  This is the ogcB737Clock.h header to ogcB737Clock.cpp

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-01-22
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
 * B737 style Main-Panel Clock
 */

#ifndef OGCB737Clock_H
#define OGCB737Clock_H

#include "ogcGauge.h"

extern int verbosity;

namespace OpenGC {

  class B737Clock : public Gauge {

    public:
      B737Clock ();
      virtual ~B737Clock ();
      void Render ();

  };

} // end namespace OpenGC

#endif
