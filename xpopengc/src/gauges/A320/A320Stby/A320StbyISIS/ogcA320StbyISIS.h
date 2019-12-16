/*=============================================================================

  This is the ogcA320StbyISIS.h header to ogcA320StbyISIS.cpp

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
 * Airbus A320 style Integrated Standby Instrument System Display
 */

#ifndef OGCA320StbyISIS_H
#define OGCA320StbyISIS_H

#include "ogcGauge.h"

namespace OpenGC
{

class A320StbyISIS : public Gauge  
{
public:

  A320StbyISIS();
  virtual ~A320StbyISIS();

  void Render();

};

} // end namespace OpenGC

#endif
