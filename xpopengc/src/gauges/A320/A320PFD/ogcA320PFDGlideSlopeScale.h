/*=============================================================================

  This is the ogcA320PFDGlideSlopeScale.h header to ogcA320PFDGlideSlopeScale.cpp

  Created:
    Date:   2015-05-18
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
 * Airbus A320 style Primary Flight Display: the Glide-Slope Scale
 */

#ifndef ogcA320PFDGlideSlopeScale_h
#define ogcA320PFDGlideSlopeScale_h

#include "ogcGaugeComponent.h"
#include "../ogcAirbusColors.h"

extern int verbosity;

namespace OpenGC
{

  class A320PFDGlideSlopeScale : public GaugeComponent  
  {
  public:

    A320PFDGlideSlopeScale();
    virtual ~A320PFDGlideSlopeScale();

    /** Overloaded render function */
    void Render();

  protected:

    /** The font number provided to us by the font manager */
    int m_Font;
  };

} // end namespace OpenGC

#endif
