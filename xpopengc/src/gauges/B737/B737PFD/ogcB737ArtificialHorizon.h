/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737ArtificialHorizon.h,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/11/24 $
  Version:   $Revision: $
  Author:    $Author: stockli $
  
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
 * Boeing 737 type artificial horizon
 */

#ifndef ogcB737ArtificialHorizon_h
#define ogcB737ArtificialHorizon_h

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737ArtificialHorizon : public GaugeComponent  
{
public:

  B737ArtificialHorizon();
  virtual ~B737ArtificialHorizon();

  /** Overloaded render function */
  void Render();

protected:

  /** The font number provided to us by the font manager */
  int m_Font;
};

} // end namespace OpenGC

#endif
