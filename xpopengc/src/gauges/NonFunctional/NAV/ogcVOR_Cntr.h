/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcVOR_Cntr.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:41 $
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

#ifndef ogcVOR_CNTR_H
#define ogcVOR_CNTR_H

#include <stdio.h>
#include <Base/ogcGLHeaders.h>
#include <Base/ogcGaugeComponent.h>

class ogcVOR_Cntr : public ogcGaugeComponent 
{
public:

  ogcVOR_Cntr();
  virtual ~ogcVOR_Cntr();

  // Overloaded render function
  void Render();

protected:

  // The font number provided to us by the font manager
  int m_Font;

};

#endif
