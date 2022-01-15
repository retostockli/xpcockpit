/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2001-2021 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):
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

#if !defined(BOEING737PFDSASPEEDINDICATOR_H)
#define BOEING737PFDSASPEEDINDICATOR_H

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737PFDSASpeedIndicator : public GaugeComponent  
{
public:
	B737PFDSASpeedIndicator();
	virtual ~B737PFDSASpeedIndicator();

	void Render();

protected:
	int m_Font;
};

}
#endif
