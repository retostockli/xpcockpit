/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737FMCKey.h,v $

  Copyright (C) 2018 by:
    Original author:
      Reto Stockli
    Contributors (in alphabetical order):

  Last modification:

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

#if !defined(BOEING737FMCKey_H)
#define BOEING737FMCKey_H

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737FMCKey : public GaugeComponent  
{
public:
  B737FMCKey();
  virtual ~B737FMCKey();
  void Render();
  
  void setKeyName(char keyName[]) {strcpy(m_KeyName,keyName);}
  void setKeyForm(int keyForm) {m_KeyForm = keyForm;}  // 0: quadratic, 1: quadratic with border, 2: rectangular, 3: round, 4: R/L Display Keys
  void setKeyDataRef1(char* keyDataRef) {strcpy(m_KeyDataRef1,keyDataRef);}
  void setKeyDataRef2(char* keyDataRef) {strcpy(m_KeyDataRef2,keyDataRef);}
  void setKeySizeX(int keyPhysicalSizeX) {m_PhysicalSize.x = keyPhysicalSizeX;}
  void setKeySizeY(int keyPhysicalSizeY) {m_PhysicalSize.y = keyPhysicalSizeY;}

  /** Called if the down mouse click applies to this object */
  void OnMouseDown(int button, double physicalX, double physicalY);
  
 protected:
  int m_Font;
  char m_KeyName[10];
  int m_KeyForm;
  char m_KeyDataRef1[50];
  char m_KeyDataRef2[50];
};

} // end namespace OpenGC

#endif
