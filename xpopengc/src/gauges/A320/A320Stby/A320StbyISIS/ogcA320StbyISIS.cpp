/*=============================================================================

  This is the ogcA320StbyISIS.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Standby Integrated Standby Instrument System Display ===

  Created:
    Date:   2011-11-14
    Author: Hans Jansen

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

===============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================*/

#include "ogcA320StbyISIS.h"

namespace OpenGC
{

A320StbyISIS::A320StbyISIS()
{
  printf("A320StbyISIS constructed\n");
}

A320StbyISIS::~A320StbyISIS()
{
  // Destruction handled by base class
}

void A320StbyISIS::Render()
{
	// Rendering the dummy class does nothing
}

} // end namespace OpenGC
