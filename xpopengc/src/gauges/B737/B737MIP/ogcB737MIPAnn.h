/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MIPAnn.h,v $

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

#if !defined(BOEING737MIPAnn_H)
#define BOEING737MIPAnn_H

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class B737MIPAnn : public GaugeComponent  
{
public:
  B737MIPAnn();
  virtual ~B737MIPAnn();
  void Render();
  
  void setAnnName(char keyName[]) {strcpy(m_AnnName,keyName);}
  void setAnnForm(int keyForm) {m_AnnForm = keyForm;}
  void setAnnBGColor(int R, int G, int B) {m_AnnBGColor[0] = R;m_AnnBGColor[1] = G;m_AnnBGColor[2] = B;}
  void setAnnFGColor(int R, int G, int B) {m_AnnFGColor[0] = R;m_AnnFGColor[1] = G;m_AnnFGColor[2] = B;}
  void setAnnDataRef1(char* DataRef) {strcpy(m_AnnDataRef1,DataRef);}
  void setAnnKeyDataRef1(char* DataRef) {strcpy(m_AnnKeyDataRef1,DataRef);}
  void setAnnDataRefType1(int dataRefType) {m_AnnDataRefType1 = dataRefType;}
  void setAnnKeyDataRefType1(int dataRefType) {m_AnnKeyDataRefType1 = dataRefType;}
  void setAnnDataRef2(char* DataRef) {strcpy(m_AnnDataRef2,DataRef);}
  void setAnnKeyDataRef2(char* DataRef) {strcpy(m_AnnKeyDataRef2,DataRef);}
  void setAnnDataRefType2(int dataRefType) {m_AnnDataRefType2 = dataRefType;}
  void setAnnKeyDataRefType2(int dataRefType) {m_AnnKeyDataRefType2 = dataRefType;}
  void setAnnSizeX(int PhysicalSizeX) {m_PhysicalSize.x = PhysicalSizeX;}
  void setAnnSizeY(int PhysicalSizeY) {m_PhysicalSize.y = PhysicalSizeY;}

  /** Called if the down mouse click applies to this object */
  void OnMouseDown(int button, double physicalX, double physicalY);
  
 protected:
  int m_Font;
  char m_AnnName[30];
  int m_AnnForm;
  int m_AnnBGColor[3];
  int m_AnnFGColor[3];
  char m_AnnDataRef1[100];
  char m_AnnKeyDataRef1[100];
  char m_AnnDataRef2[100];
  char m_AnnKeyDataRef2[100];

  int m_AnnDataRefType1; /* 1: int, 2: flt */
  int m_AnnKeyDataRefType1; /* 1: int, 2: flt, 3: cmd once, 4: cmd hold */
  int m_AnnDataRefType2;
  int m_AnnKeyDataRefType2;
};

} // end namespace OpenGC

#endif
