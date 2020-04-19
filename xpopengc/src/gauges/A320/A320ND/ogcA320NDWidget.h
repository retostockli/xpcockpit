/*=============================================================================

  This is the ogcA320NDWidget.h header to ogcA320NDWidget.cpp

  Created:
    Date:   2011-11-14
    Author: Hans Jansen
    last change: 2020-02-01
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
 * Airbus A320 style Navigation Display: the Widget
 */

#ifndef A320NDWidget_H
#define A320NDWidget_H

#include "ogcGaugeComponent.h"
#include "../ogcAirbusColors.h"

extern int verbosity;
     
namespace OpenGC {

  class A320NDWidget : public GaugeComponent {

    public:
      A320NDWidget ();
      virtual ~A320NDWidget ();
      void Render ();

    protected:
      int m_Font;

  };

  struct wptstruct {
    char name[40];
    float lon;
    float lat;
  };

} // end namespace OpenGC

#endif
