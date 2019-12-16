/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcBoeing737NAV.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Jurgen Roeland
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:43 $
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


#if !defined(AFX_ogcBoeing737NAV_H)
#define AFX_ogcBoeing737NAV_H

#include "ogcGauge.h"

class ogcBoeing737NAV : public ogcGauge  
{
public:
	ogcBoeing737NAV();
	virtual ~ogcBoeing737NAV();

	void Render();
};

#endif // !defined(AFX_ogcBoeing737NAV_H)
