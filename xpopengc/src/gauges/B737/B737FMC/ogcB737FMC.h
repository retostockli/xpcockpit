/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737FMC.h,v $

  Copyright (C) 2017 by:
    Original author:
      Reto Stockli
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2017/04/30 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
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

/*
This file defines the class/structure of the UDP packet that sends
the simulation data created by FlightGear to the glass displays. It
is required to "sync" the data types contained in the packet
*/

#ifndef ogcB737FMC_h
#define ogcB737FMC_h

#include <ogcGauge.h>
#include "../ogcBoeingColors.h"

namespace OpenGC
{

class B737FMC : public Gauge  
{
public:

  B737FMC(int arg);
  virtual ~B737FMC();

  void Render();

protected:

  // The font number
  int m_Font;

};

} // end namespace OpenGC

#endif
