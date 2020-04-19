/*=============================================================================

  This is the ogcA320StbyAttComponent.h header to ogcA320StbyAttComponent.c

  Created:
    Date:   2018-05-03
    Author: Hans Jansen
    last change: 2020-01-24
    (see ogcSkeletonGauge.c for more details)

  Copyright (C) 2018-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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
 * Airbus A320 style Standby Attitude Indicator
 */

#ifndef ogcA320StbyAttComponent_h
#define ogcA320StbyAttComponent_h

#include "ogcGaugeComponent.h"
#include "../../ogcAirbusColors.h"

extern int verbosity;

namespace OpenGC {

  class A320StbyAttComponent : public GaugeComponent {
  
    public:
      A320StbyAttComponent ();
      virtual ~A320StbyAttComponent ();
      void Render ();

    protected:
      /** Font from the font manager */
      int m_Font;

  };

} // end namespace OpenGC

#endif
