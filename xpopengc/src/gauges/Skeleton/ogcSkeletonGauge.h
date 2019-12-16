/*=============================================================================

  This is the ogcSkeletonGauge.h header to ogcSkeletonGauge.c

  Created:
    Date:   2015-05-05
    Author: Hans Jansen
    (see ogcSkeletonGauge.c for more details)

  Copyright (C) 2015      Hans Jansen (hansjansen@users.sourceforge.net)
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
 * Skeleton gauge to demonstrate the decentralized dataref linkage
 */

#ifndef ogcSkeletonGauge_h
#define ogcSkeletonGauge_h

#include "ogcGauge.h"

extern int verbosity;

namespace OpenGC
{
  class SkeletonGauge : public Gauge  
  {
  public:

    SkeletonGauge();
    virtual ~SkeletonGauge();

    void Render();

  protected:

  };
} // end namespace OpenGC

#endif
