/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737VerticalSpeedDigitalComponent.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Jurgen Roeland (Jurgen.Roeland@AISYstems.be)
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:36 $
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

#if !defined(AFX_BOEING737VERTICALSPEEDDIGITALCOMPONENT_H__0A8CF720_020E_48FC_A33E_ED557761AC7D__INCLUDED_)
#define AFX_BOEING737VERTICALSPEEDDIGITALCOMPONENT_H__0A8CF720_020E_48FC_A33E_ED557761AC7D__INCLUDED_

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class Boeing737VerticalSpeedDigitalComponent : public GaugeComponent  
{
public:
	Boeing737VerticalSpeedDigitalComponent();
	virtual ~Boeing737VerticalSpeedDigitalComponent();

	void Render();
protected:
	int m_Font;
};

}
#endif // !defined(AFX_OGCBOEING737VERTICALSPEEDDIGITALCOMPONENT_H__0A8CF720_020E_48FC_A33E_ED557761AC7D__INCLUDED_)
