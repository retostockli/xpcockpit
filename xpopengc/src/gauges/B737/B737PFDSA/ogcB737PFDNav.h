/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737PFDNav.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:34 $
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

#if !defined(BOEING737PFDNAV_H)
#define BOEING737PFDNAV_H

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737PFDNav : public GaugeComponent  
{
public:
	B737PFDNav();
	virtual ~B737PFDNav();

	void Render();

	void setBottomHide(float bh) {bottomHide = bh;};

protected:
	int m_Font;

	float bottomHide;
};

}

#endif
