/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2021 by:
  Reto Stockli
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================*/

/**
 * Boeing 737 style ISFD speed tape
 */

#ifndef ogcB737ISFDSpeedTape_h
#define ogcB737ISFDSpeedTape_h

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737ISFDSpeedTape : public GaugeComponent  
{
public:

  B737ISFDSpeedTape();
  virtual ~B737ISFDSpeedTape();

  /** Overloaded render function */
  void Render();

protected:

  /** The font number provided to us by the font manager */
  int m_Font;

  double indent_x; // defines region of speed tape

};

} // end namespace OpenGC

#endif
