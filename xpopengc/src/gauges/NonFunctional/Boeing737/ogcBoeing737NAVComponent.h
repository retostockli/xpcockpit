/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737NAVComponent.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Jurgen Roeland
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:32 $
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

#if !defined(AFX_Boeing737NAVComponent_H__0A8CF720_020E_48FC_A33E_ED557761AC7D__INCLUDED_)
#define AFX_Boeing737NAVComponent_H__0A8CF720_020E_48FC_A33E_ED557761AC7D__INCLUDED_

#include "ogcGaugeComponent.h"
#include "geo_pos.h"
#include "gpos.h"
#include "latlongc.h"

namespace OpenGC
{

class Boeing737NAVComponent : public GaugeComponent  
{
public:
	void DrawNavigationRadioData();
	void DrawNAVPoints(float rotated);
	void DrawFixedParts();
	void DrawRangeCircles();
	Boeing737NAVComponent();
	virtual ~Boeing737NAVComponent();

	void Render();
protected:
	int m_Font;

	float outerR_Miles;
	float outerR_pixels;
	float Range_Step_pixels;
	
	char buf[10];
	float xcircle, ycircle, radians;

	RijksDriehoeksHayfordConverter* convertor;
	GeoPos ReferencePositionAirplaneNow;
	WorldPos ReferencePositionUser;
	GeoPos CornerLeftUnder;
	GeoPos CornerRightTop;
	WorldPos CornerLeftUnderUser;
	WorldPos CornerRightTopUser;
	double scaleNAVxEast;
	double scaleNAVxWest;
	double scaleNAVyNorth;
	double scaleNAVySouth;
	WorldPos cycleUser;
};

}
#endif // !defined(AFX_ogcBoeing737NAVComponent_H__0A8CF720_020E_48FC_A33E_ED557761AC7D__INCLUDED_)
