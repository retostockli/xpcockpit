/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737FMCLine.h,v $

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

#if !defined(BOEING737FMCLine_H)
#define BOEING737FMCLine_H

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737FMCLine : public GaugeComponent  
{
public:
  B737FMCLine();
  virtual ~B737FMCLine();
  void Render();
  
  void setCurrentLine(int line) {m_Line = line;}
  void setLineDataRef1(char* lineDataRef) {strcpy(m_LineDataRef1,lineDataRef);}
  void setLineDataRef2(char* lineDataRef) {strcpy(m_LineDataRef2,lineDataRef);}
  void setLineSizeX(float lineSizeX) {m_LineSizeX = lineSizeX;}
  void setLineSizeY(float lineSizeY) {m_LineSizeY = lineSizeY;}
  void setLineColor(int R, int G, int B) {m_LineColor[0]=R; m_LineColor[1]=G; m_LineColor[2]=B;}
  
protected:
  int m_Font;
  int m_Line;
  char m_LineDataRef1[50];
  char m_LineDataRef2[50];
  float m_LineSizeX;
  float m_LineSizeY;
  int m_LineColor[3];
  
};

} // end namespace OpenGC

#endif
