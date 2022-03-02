/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737FMC.cpp,v $

  Copyright (C) 2015 by:
    Original author:
      Reto Stockli
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2017/04/30 $
    Version:   $Revision: $
    Author:    $Author: stockli $
  
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
 The Boeing 737 Flight Management Computer interfacing to the x737 FMC of Javier Cortes
 or the ZIBO MOD FMC. The Native X-Plane FMC will not (yet) work.
*/

#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "ogcGLHeaders.h"
#include "ogcGaugeComponent.h"
#include "B737/B737FMC/ogcB737FMC.h"
#include "B737/B737FMC/ogcB737FMCLine.h"
#include "B737/B737FMC/ogcB737FMCKey.h"

namespace OpenGC
{

B737FMC::B737FMC(int arg)
{

  printf("B737PFD constructed %i\n",arg);
	
  // We want to draw an outline
  this->SetGaugeOutline(false);

  /* Captain or copilot */
  bool is_captain = (arg == 0) || (arg == 2);

  /* Draw with or without keys */
  bool with_keys = (arg <= 1);

  //  m_Font = m_pFontManager->LoadDefaultFont();
  m_Font = m_pFontManager->LoadFont((char*) "CockpitMCDU.ttf");

  // Specify our physical size and position
  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;
  
  if (with_keys) {
    m_PhysicalSize.x = 110;
    m_PhysicalSize.y = 165;
  } else {
    m_PhysicalSize.x = 82;
    m_PhysicalSize.y = 76;
  }
  
  
  /* line spacing */
  float delta = (76.0-2.0)/14.0;
  float linex0;
  float liney0;
  if (with_keys) {
    linex0 = 15.0;
    liney0 = 4.0;
  } else {
    linex0 = 2.0;
    liney0 = 0.0;
  }
  float keyx0 = 12.0;
  float mkeyx0 = 4.0;

  
  B737FMCLine* line_1 = new B737FMCLine();
  line_1->SetParentRenderObject(this);
  line_1->SetPosition(linex0, m_PhysicalSize.y - liney0 - delta);
  line_1->setCurrentLine(1);
  line_1->setLineDataRef1((char*) "FJCC/UFMC/LINE_1");
  if (is_captain) {
    line_1->setLineDataRef2((char*) "laminar/B738/fmc1/Line00_L");
  } else {
    line_1->setLineDataRef2((char*) "laminar/B738/fmc2/Line00_L");
  }
  this->AddGaugeComponent(line_1);
  //  if (0) {
  
  B737FMCLine* line_1_small = new B737FMCLine();
  line_1_small->SetParentRenderObject(this);
  line_1_small->SetPosition(linex0, m_PhysicalSize.y - liney0 - delta);
  line_1_small->setCurrentLine(1);
  line_1_small->setLineSizeY(0.6);
  if (is_captain) {
    line_1_small->setLineDataRef2((char*) "laminar/B738/fmc1/Line00_S");
  } else {
    line_1_small->setLineDataRef2((char*) "laminar/B738/fmc2/Line00_S");
  }
  this->AddGaugeComponent(line_1_small);
  
  B737FMCLine* line_1_magenta = new B737FMCLine();
  line_1_magenta->SetParentRenderObject(this);
  line_1_magenta->SetPosition(linex0, m_PhysicalSize.y - liney0 - delta);
  line_1_magenta->setCurrentLine(1);
  line_1_magenta->setLineColor(255,0,255);
  if (is_captain) {
    line_1_magenta->setLineDataRef2((char*) "laminar/B738/fmc1/Line00_M");
  } else {
    line_1_magenta->setLineDataRef2((char*) "laminar/B738/fmc2/Line00_M");
  }
  this->AddGaugeComponent(line_1_magenta);
  
  B737FMCLine* line_2 = new B737FMCLine();
  line_2->SetParentRenderObject(this);
  line_2->SetPosition(linex0, m_PhysicalSize.y - liney0 - 2*delta);
  line_2->setCurrentLine(2);
  line_2->setLineSizeY(0.6);
  line_2->setLineDataRef1((char*) "FJCC/UFMC/LINE_2");
  if (is_captain) {
    line_2->setLineDataRef2((char*) "laminar/B738/fmc1/Line01_X");
  } else {
    line_2->setLineDataRef2((char*) "laminar/B738/fmc2/Line01_X");
  }
  this->AddGaugeComponent(line_2);

  B737FMCLine* line_3 = new B737FMCLine();
  line_3->SetParentRenderObject(this);
  line_3->SetPosition(linex0, m_PhysicalSize.y - liney0 - 3*delta);
  line_3->setCurrentLine(3);
  line_3->setLineDataRef1((char*) "FJCC/UFMC/LINE_3");
  if (is_captain) {
    line_3->setLineDataRef2((char*) "laminar/B738/fmc1/Line01_L");
  } else {
    line_3->setLineDataRef2((char*) "laminar/B738/fmc2/Line01_L");
  }
  this->AddGaugeComponent(line_3);

  B737FMCLine* line_3_small = new B737FMCLine();
  line_3_small->SetParentRenderObject(this);
  line_3_small->SetPosition(linex0, m_PhysicalSize.y - liney0 - 3*delta);
  line_3_small->setCurrentLine(3);
  line_3_small->setLineSizeY(0.6);
  if (is_captain) {
    line_3_small->setLineDataRef2((char*) "laminar/B738/fmc1/Line01_S");
  } else {
    line_3_small->setLineDataRef2((char*) "laminar/B738/fmc2/Line01_S");
  }
  this->AddGaugeComponent(line_3_small);

  B737FMCLine* line_3_magenta = new B737FMCLine();
  line_3_magenta->SetParentRenderObject(this);
  line_3_magenta->SetPosition(linex0, m_PhysicalSize.y - liney0 - 3*delta);
  line_3_magenta->setCurrentLine(3);
  line_3_magenta->setLineColor(255,0,255);
  if (is_captain) {
    line_3_magenta->setLineDataRef2((char*) "laminar/B738/fmc1/Line01_M");
  } else {
    line_3_magenta->setLineDataRef2((char*) "laminar/B738/fmc2/Line01_M");
  }
  this->AddGaugeComponent(line_3_magenta);

  B737FMCLine* line_4 = new B737FMCLine();
  line_4->SetParentRenderObject(this);
  line_4->SetPosition(linex0, m_PhysicalSize.y - liney0 - 4*delta);
  line_4->setCurrentLine(4);
  line_4->setLineSizeY(0.6);
  line_4->setLineDataRef1((char*) "FJCC/UFMC/LINE_4");
  if (is_captain) {
    line_4->setLineDataRef2((char*) "laminar/B738/fmc1/Line02_X");
  } else {
    line_4->setLineDataRef2((char*) "laminar/B738/fmc2/Line02_X");
  }
  this->AddGaugeComponent(line_4);

  B737FMCLine* line_5 = new B737FMCLine();
  line_5->SetParentRenderObject(this);
  line_5->SetPosition(linex0, m_PhysicalSize.y - liney0 - 5*delta);
  line_5->setCurrentLine(5);
  line_5->setLineDataRef1((char*) "FJCC/UFMC/LINE_5");
  if (is_captain) {
    line_5->setLineDataRef2((char*) "laminar/B738/fmc1/Line02_L");
  } else {
    line_5->setLineDataRef2((char*) "laminar/B738/fmc2/Line02_L");
  }
  this->AddGaugeComponent(line_5);

  B737FMCLine* line_5_small = new B737FMCLine();
  line_5_small->SetParentRenderObject(this);
  line_5_small->SetPosition(linex0, m_PhysicalSize.y - liney0 - 5*delta);
  line_5_small->setCurrentLine(5);
  line_5_small->setLineSizeY(0.6);
  if (is_captain) {
    line_5_small->setLineDataRef2((char*) "laminar/B738/fmc1/Line02_S");
  } else {
    line_5_small->setLineDataRef2((char*) "laminar/B738/fmc2/Line02_S");
  }
  this->AddGaugeComponent(line_5_small);

  B737FMCLine* line_5_magenta = new B737FMCLine();
  line_5_magenta->SetParentRenderObject(this);
  line_5_magenta->SetPosition(linex0, m_PhysicalSize.y - liney0 - 5*delta);
  line_5_magenta->setCurrentLine(5);
  line_5_magenta->setLineColor(255,0,255);
  if (is_captain) {
    line_5_magenta->setLineDataRef2((char*) "laminar/B738/fmc1/Line02_M");
  } else {
    line_5_magenta->setLineDataRef2((char*) "laminar/B738/fmc2/Line02_M");
  }
  this->AddGaugeComponent(line_5_magenta);

  B737FMCLine* line_6 = new B737FMCLine();
  line_6->SetParentRenderObject(this);
  line_6->SetPosition(linex0, m_PhysicalSize.y - liney0 - 6*delta);
  line_6->setCurrentLine(6);
  line_6->setLineSizeY(0.6);
  line_6->setLineDataRef1((char*) "FJCC/UFMC/LINE_6");
  if (is_captain) {
    line_6->setLineDataRef2((char*) "laminar/B738/fmc1/Line03_X");
  } else {
    line_6->setLineDataRef2((char*) "laminar/B738/fmc2/Line03_X");
  }
  this->AddGaugeComponent(line_6);

  B737FMCLine* line_7 = new B737FMCLine();
  line_7->SetParentRenderObject(this);
  line_7->SetPosition(linex0, m_PhysicalSize.y - liney0 - 7*delta);
  line_7->setCurrentLine(7);
  line_7->setLineDataRef1((char*) "FJCC/UFMC/LINE_7");
  if (is_captain) {
    line_7->setLineDataRef2((char*) "laminar/B738/fmc1/Line03_L");
  } else {
    line_7->setLineDataRef2((char*) "laminar/B738/fmc2/Line03_L");
  }
  this->AddGaugeComponent(line_7);

  B737FMCLine* line_7_small = new B737FMCLine();
  line_7_small->SetParentRenderObject(this);
  line_7_small->SetPosition(linex0, m_PhysicalSize.y - liney0 - 7*delta);
  line_7_small->setCurrentLine(7);
  line_7_small->setLineSizeY(0.6);
  if (is_captain) {
    line_7_small->setLineDataRef2((char*) "laminar/B738/fmc1/Line03_S");
  } else {
    line_7_small->setLineDataRef2((char*) "laminar/B738/fmc2/Line03_S");
  }
  this->AddGaugeComponent(line_7_small);

  B737FMCLine* line_7_magenta = new B737FMCLine();
  line_7_magenta->SetParentRenderObject(this);
  line_7_magenta->SetPosition(linex0, m_PhysicalSize.y - liney0 - 7*delta);
  line_7_magenta->setCurrentLine(7);
  line_7_magenta->setLineColor(255,0,255);
  if (is_captain) {
    line_7_magenta->setLineDataRef2((char*) "laminar/B738/fmc1/Line03_M");
  } else {
    line_7_magenta->setLineDataRef2((char*) "laminar/B738/fmc2/Line03_M");
  }
  this->AddGaugeComponent(line_7_magenta);

  B737FMCLine* line_8 = new B737FMCLine();
  line_8->SetParentRenderObject(this);
  line_8->SetPosition(linex0, m_PhysicalSize.y - liney0 - 8*delta);
  line_8->setCurrentLine(8);
  line_8->setLineSizeY(0.6);
  line_8->setLineDataRef1((char*) "FJCC/UFMC/LINE_8");
  if (is_captain) {
     line_8->setLineDataRef2((char*) "laminar/B738/fmc1/Line04_X");
  } else {
     line_8->setLineDataRef2((char*) "laminar/B738/fmc2/Line04_X");
  }
  this->AddGaugeComponent(line_8);

  B737FMCLine* line_9 = new B737FMCLine();
  line_9->SetParentRenderObject(this);
  line_9->SetPosition(linex0, m_PhysicalSize.y - liney0 - 9*delta);
  line_9->setCurrentLine(9);
  line_9->setLineDataRef1((char*) "FJCC/UFMC/LINE_9");
  if (is_captain) {
     line_9->setLineDataRef2((char*) "laminar/B738/fmc1/Line04_L");
  } else {
     line_9->setLineDataRef2((char*) "laminar/B738/fmc2/Line04_L");
  }
  this->AddGaugeComponent(line_9);

  B737FMCLine* line_9_small = new B737FMCLine();
  line_9_small->SetParentRenderObject(this);
  line_9_small->SetPosition(linex0, m_PhysicalSize.y - liney0 - 9*delta);
  line_9_small->setCurrentLine(9);
  line_9_small->setLineSizeY(0.6);
  if (is_captain) {
     line_9_small->setLineDataRef2((char*) "laminar/B738/fmc1/Line04_S");
  } else {
     line_9_small->setLineDataRef2((char*) "laminar/B738/fmc2/Line04_S");
  }
  this->AddGaugeComponent(line_9_small);

  B737FMCLine* line_9_magenta = new B737FMCLine();
  line_9_magenta->SetParentRenderObject(this);
  line_9_magenta->SetPosition(linex0, m_PhysicalSize.y - liney0 - 9*delta);
  line_9_magenta->setCurrentLine(9);
  line_9_magenta->setLineColor(255,0,255);
  if (is_captain) {
     line_9_magenta->setLineDataRef2((char*) "laminar/B738/fmc1/Line04_M");
  } else {
     line_9_magenta->setLineDataRef2((char*) "laminar/B738/fmc2/Line04_M");
  }
  this->AddGaugeComponent(line_9_magenta);

  B737FMCLine* line_10 = new B737FMCLine();
  line_10->SetParentRenderObject(this);
  line_10->SetPosition(linex0, m_PhysicalSize.y - liney0 - 10*delta);
  line_10->setCurrentLine(10);
  line_10->setLineSizeY(0.6);
  line_10->setLineDataRef1((char*) "FJCC/UFMC/LINE_10");
  if (is_captain) {
     line_10->setLineDataRef2((char*) "laminar/B738/fmc1/Line05_X");
  } else {
     line_10->setLineDataRef2((char*) "laminar/B738/fmc2/Line05_X");
  }
  this->AddGaugeComponent(line_10);

  B737FMCLine* line_11 = new B737FMCLine();
  line_11->SetParentRenderObject(this);
  line_11->SetPosition(linex0, m_PhysicalSize.y - liney0 - 11*delta);
  line_11->setCurrentLine(11);
  line_11->setLineDataRef1((char*) "FJCC/UFMC/LINE_11");
  if (is_captain) {
     line_11->setLineDataRef2((char*) "laminar/B738/fmc1/Line05_L");
  } else {
     line_11->setLineDataRef2((char*) "laminar/B738/fmc2/Line05_L");
  }
  this->AddGaugeComponent(line_11);

  B737FMCLine* line_11_small = new B737FMCLine();
  line_11_small->SetParentRenderObject(this);
  line_11_small->SetPosition(linex0, m_PhysicalSize.y - liney0 - 11*delta);
  line_11_small->setCurrentLine(11);
  line_11_small->setLineSizeY(0.6);
  if (is_captain) {
     line_11_small->setLineDataRef2((char*) "laminar/B738/fmc1/Line05_S");
  } else {
     line_11_small->setLineDataRef2((char*) "laminar/B738/fmc2/Line05_S");
  }
  this->AddGaugeComponent(line_11_small);

  B737FMCLine* line_11_magenta = new B737FMCLine();
  line_11_magenta->SetParentRenderObject(this);
  line_11_magenta->SetPosition(linex0, m_PhysicalSize.y - liney0 - 11*delta);
  line_11_magenta->setCurrentLine(11);
  line_11_magenta->setLineColor(255,0,255);
  if (is_captain) {
    line_11_magenta->setLineDataRef2((char*) "laminar/B738/fmc1/Line05_M");
  } else {
    line_11_magenta->setLineDataRef2((char*) "laminar/B738/fmc2/Line05_M");
  }
  this->AddGaugeComponent(line_11_magenta);

  B737FMCLine* line_12 = new B737FMCLine();
  line_12->SetParentRenderObject(this);
  line_12->SetPosition(linex0, m_PhysicalSize.y - liney0 - 12*delta);
  line_12->setCurrentLine(12);
  line_12->setLineSizeY(0.6);
  line_12->setLineDataRef1((char*) "FJCC/UFMC/LINE_12");
  if (is_captain) {
    line_12->setLineDataRef2((char*) "laminar/B738/fmc1/Line06_X");
  } else {
    line_12->setLineDataRef2((char*) "laminar/B738/fmc2/Line06_X");
  }
  this->AddGaugeComponent(line_12);

  B737FMCLine* line_13 = new B737FMCLine();
  line_13->SetParentRenderObject(this);
  line_13->SetPosition(linex0, m_PhysicalSize.y - liney0 - 13*delta);
  line_13->setCurrentLine(13);
  line_13->setLineDataRef1((char*) "FJCC/UFMC/LINE_13");
  line_13->setLineDataRef2((char*) "laminar/B738/fmc1/Line06_L");
  this->AddGaugeComponent(line_13);

  B737FMCLine* line_13_small = new B737FMCLine();
  line_13_small->SetParentRenderObject(this);
  line_13_small->SetPosition(linex0, m_PhysicalSize.y - liney0 - 13*delta);
  line_13_small->setCurrentLine(13);
  line_13_small->setLineSizeY(0.6);
  if (is_captain) {
    line_13_small->setLineDataRef2((char*) "laminar/B738/fmc1/Line06_S");
  } else {
    line_13_small->setLineDataRef2((char*) "laminar/B738/fmc2/Line06_S");
  }
  this->AddGaugeComponent(line_13_small);

  B737FMCLine* line_13_magenta = new B737FMCLine();
  line_13_magenta->SetParentRenderObject(this);
  line_13_magenta->SetPosition(linex0, m_PhysicalSize.y - liney0 - 13*delta);
  line_13_magenta->setCurrentLine(13);
  line_13_magenta->setLineColor(255,0,255);
  if (is_captain) {
    line_13_magenta->setLineDataRef2((char*) "laminar/B738/fmc1/Line06_M");
  } else {
    line_13_magenta->setLineDataRef2((char*) "laminar/B738/fmc2/Line06_M");
  }
  this->AddGaugeComponent(line_13_magenta);

  B737FMCLine* line_14 = new B737FMCLine();
  line_14->SetParentRenderObject(this);
  line_14->SetPosition(linex0, m_PhysicalSize.y - liney0 - 14*delta);
  line_14->setCurrentLine(14);
  line_14->setLineDataRef1((char*) "FJCC/UFMC/LINE_14");
  if (is_captain) {
    line_14->setLineDataRef2((char*) "laminar/B738/fmc1/Line_entry");
  } else {
    line_14->setLineDataRef2((char*) "laminar/B738/fmc2/Line_entry");
  }
  this->AddGaugeComponent(line_14);

  //  }

  if (with_keys) {

    // vertical key spacing
    float deltay = (151.0 - 71.0)/8.0;
    float deltax = (96.0 - 12.0)/8.0;
    float deltax2 = (96.0 - 12.0)/6.0;

    B737FMCKey* key_dot = new B737FMCKey();
    key_dot->SetParentRenderObject(this);
    key_dot->SetPosition(keyx0 + 0*deltax, 3 + 0*deltay);
    key_dot->setKeyName((char*) ".");
    key_dot->setKeyForm(3);
    key_dot->setKeyDataRef1((char*) "FJCC/UFMC/punto");
    key_dot->setKeyDataRef2((char*) "laminar/B738/button/fmc1_period");
    this->AddGaugeComponent(key_dot);
  
    B737FMCKey* key_0 = new B737FMCKey();
    key_0->SetParentRenderObject(this);
    key_0->SetPosition(keyx0 + 1*deltax, 3 + 0*deltay);
    key_0->setKeyName((char*) "0");
    key_0->setKeyForm(3);
    key_0->setKeyDataRef1((char*) "FJCC/UFMC/0");
    key_0->setKeyDataRef2((char*) "laminar/B738/button/fmc1_0");
    this->AddGaugeComponent(key_0);

    B737FMCKey* key_pm = new B737FMCKey();
    key_pm->SetParentRenderObject(this);
    key_pm->SetPosition(keyx0 + 2*deltax, 3 + 0*deltay);
    key_pm->setKeyName((char*) "-");
    key_pm->setKeyForm(3);
    key_pm->setKeyDataRef1((char*) "FJCC/UFMC/menos");
    key_pm->setKeyDataRef2((char*) "laminar/B738/button/fmc1_minus");
    this->AddGaugeComponent(key_pm);

    B737FMCKey* key_7 = new B737FMCKey();
    key_7->SetParentRenderObject(this);
    key_7->SetPosition(keyx0 + 0*deltax, 3 + 1*deltay);
    key_7->setKeyName((char*) "7");
    key_7->setKeyForm(3);
    key_7->setKeyDataRef1((char*) "FJCC/UFMC/7");
    key_7->setKeyDataRef2((char*) "laminar/B738/button/fmc1_7");
    this->AddGaugeComponent(key_7);

    B737FMCKey* key_8 = new B737FMCKey();
    key_8->SetParentRenderObject(this);
    key_8->SetPosition(keyx0 + 1*deltax, 3 + 1*deltay);
    key_8->setKeyName((char*) "8");
    key_8->setKeyForm(3);
    key_8->setKeyDataRef1((char*) "FJCC/UFMC/8");
    key_8->setKeyDataRef2((char*) "laminar/B738/button/fmc1_8");
    this->AddGaugeComponent(key_8);

    B737FMCKey* key_9 = new B737FMCKey();
    key_9->SetParentRenderObject(this);
    key_9->SetPosition(keyx0 + 2*deltax, 3 + 1*deltay);
    key_9->setKeyName((char*) "9");
    key_9->setKeyForm(3);
    key_9->setKeyDataRef1((char*) "FJCC/UFMC/9");
    key_9->setKeyDataRef2((char*) "laminar/B738/button/fmc1_9");
    this->AddGaugeComponent(key_9);

    B737FMCKey* key_4 = new B737FMCKey();
    key_4->SetParentRenderObject(this);
    key_4->SetPosition(keyx0 + 0*deltax, 3 + 2*deltay);
    key_4->setKeyName((char*) "4");
    key_4->setKeyForm(3);
    key_4->setKeyDataRef1((char*) "FJCC/UFMC/4");
    key_4->setKeyDataRef2((char*) "laminar/B738/button/fmc1_4");
    this->AddGaugeComponent(key_4);

    B737FMCKey* key_5 = new B737FMCKey();
    key_5->SetParentRenderObject(this);
    key_5->SetPosition(keyx0 + 1*deltax, 3 + 2*deltay);
    key_5->setKeyName((char*) "5");
    key_5->setKeyForm(3);
    key_5->setKeyDataRef1((char*) "FJCC/UFMC/5");
    key_5->setKeyDataRef2((char*) "laminar/B738/button/fmc1_5");
    this->AddGaugeComponent(key_5);

    B737FMCKey* key_6 = new B737FMCKey();
    key_6->SetParentRenderObject(this);
    key_6->SetPosition(keyx0 + 2*deltax, 3 + 2*deltay);
    key_6->setKeyName((char*) "6");
    key_6->setKeyForm(3);
    key_6->setKeyDataRef1((char*) "FJCC/UFMC/6");
    key_6->setKeyDataRef2((char*) "laminar/B738/button/fmc1_6");
    this->AddGaugeComponent(key_6);

    B737FMCKey* key_1 = new B737FMCKey();
    key_1->SetParentRenderObject(this);
    key_1->SetPosition(keyx0 + 0*deltax, 3 + 3*deltay);
    key_1->setKeyName((char*) "1");
    key_1->setKeyForm(3);
    key_1->setKeyDataRef1((char*) "FJCC/UFMC/1");
    key_1->setKeyDataRef2((char*) "laminar/B738/button/fmc1_1");
    this->AddGaugeComponent(key_1);

    B737FMCKey* key_2 = new B737FMCKey();
    key_2->SetParentRenderObject(this);
    key_2->SetPosition(keyx0 + 1*deltax, 3 + 3*deltay);
    key_2->setKeyName((char*) "2");
    key_2->setKeyForm(3);
    key_2->setKeyDataRef1((char*) "FJCC/UFMC/2");
    key_2->setKeyDataRef2((char*) "laminar/B738/button/fmc1_2");
    this->AddGaugeComponent(key_2);

    B737FMCKey* key_3 = new B737FMCKey();
    key_3->SetParentRenderObject(this);
    key_3->SetPosition(keyx0 + 2*deltax, 3 + 3*deltay);
    key_3->setKeyName((char*) "3");
    key_3->setKeyForm(3);
    key_3->setKeyDataRef1((char*) "FJCC/UFMC/3");
    key_3->setKeyDataRef2((char*) "laminar/B738/button/fmc1_3");
    this->AddGaugeComponent(key_3);

    B737FMCKey* key_z = new B737FMCKey();
    key_z->SetParentRenderObject(this);
    key_z->SetPosition(keyx0 + 3*deltax, 3 + 0*deltay);
    key_z->setKeyName((char*) "Z");
    key_z->setKeyForm(0);
    key_z->setKeyDataRef1((char*) "FJCC/UFMC/Z");
    key_z->setKeyDataRef2((char*) "laminar/B738/button/fmc1_Z");
    this->AddGaugeComponent(key_z);

    B737FMCKey* key_space = new B737FMCKey();
    key_space->SetParentRenderObject(this);
    key_space->SetPosition(keyx0 + 4*deltax, 3 + 0*deltay);
    key_space->setKeyName((char*) " ");
    key_space->setKeyForm(0);
    key_space->setKeyDataRef1((char*) "FJCC/UFMC/espacio");
    key_space->setKeyDataRef2((char*) "laminar/B738/button/fmc1_SP");
    this->AddGaugeComponent(key_space);

    B737FMCKey* key_del = new B737FMCKey();
    key_del->SetParentRenderObject(this);
    key_del->SetPosition(keyx0 + 5*deltax, 3 + 0*deltay);
    key_del->setKeyName((char*) "DEL");
    key_del->setKeyForm(0);
    key_del->setKeyDataRef1((char*) "FJCC/UFMC/DEL");
    key_del->setKeyDataRef2((char*) "laminar/B738/button/fmc1_del");
    this->AddGaugeComponent(key_del);

    B737FMCKey* key_slash = new B737FMCKey();
    key_slash->SetParentRenderObject(this);
    key_slash->SetPosition(keyx0 + 6*deltax, 3 + 0*deltay);
    key_slash->setKeyName((char*) "/");
    key_slash->setKeyForm(0);
    key_slash->setKeyDataRef1((char*) "FJCC/UFMC/barra");
    key_slash->setKeyDataRef2((char*) "laminar/B738/button/fmc1_slash");
    this->AddGaugeComponent(key_slash);

    B737FMCKey* key_clr = new B737FMCKey();
    key_clr->SetParentRenderObject(this);
    key_clr->SetPosition(keyx0 + 7*deltax, 3 + 0*deltay);
    key_clr->setKeyName((char*) "CLR");
    key_clr->setKeyForm(0);
    key_clr->setKeyDataRef1((char*) "FJCC/UFMC/CLR");
    key_clr->setKeyDataRef2((char*) "laminar/B738/button/fmc1_clr");
    this->AddGaugeComponent(key_clr);

    B737FMCKey* key_u = new B737FMCKey();
    key_u->SetParentRenderObject(this);
    key_u->SetPosition(keyx0 + 3*deltax, 3 + 1*deltay);
    key_u->setKeyName((char*) "U");
    key_u->setKeyForm(0);
    key_u->setKeyDataRef1((char*) "FJCC/UFMC/U");
    key_u->setKeyDataRef2((char*) "laminar/B738/button/fmc1_U");
    this->AddGaugeComponent(key_u);

    B737FMCKey* key_v = new B737FMCKey();
    key_v->SetParentRenderObject(this);
    key_v->SetPosition(keyx0 + 4*deltax, 3 + 1*deltay);
    key_v->setKeyName((char*) "V");
    key_v->setKeyForm(0);
    key_v->setKeyDataRef1((char*) "FJCC/UFMC/V");
    key_v->setKeyDataRef2((char*) "laminar/B738/button/fmc1_V");
    this->AddGaugeComponent(key_v);

    B737FMCKey* key_w = new B737FMCKey();
    key_w->SetParentRenderObject(this);
    key_w->SetPosition(keyx0 + 5*deltax, 3 + 1*deltay);
    key_w->setKeyName((char*) "W");
    key_w->setKeyForm(1);
    key_w->setKeyDataRef1((char*) "FJCC/UFMC/W");
    key_w->setKeyDataRef2((char*) "laminar/B738/button/fmc1_W");
    this->AddGaugeComponent(key_w);

    B737FMCKey* key_x = new B737FMCKey();
    key_x->SetParentRenderObject(this);
    key_x->SetPosition(keyx0 + 6*deltax, 3 + 1*deltay);
    key_x->setKeyName((char*) "X");
    key_x->setKeyForm(0);
    key_x->setKeyDataRef1((char*) "FJCC/UFMC/X");
    key_x->setKeyDataRef2((char*) "laminar/B738/button/fmc1_X");
    this->AddGaugeComponent(key_x);

    B737FMCKey* key_y = new B737FMCKey();
    key_y->SetParentRenderObject(this);
    key_y->SetPosition(keyx0 + 7*deltax, 3 + 1*deltay);
    key_y->setKeyName((char*) "Y");
    key_y->setKeyForm(0);
    key_y->setKeyDataRef1((char*) "FJCC/UFMC/Y");
    key_y->setKeyDataRef2((char*) "laminar/B738/button/fmc1_Y");
    this->AddGaugeComponent(key_y);

    B737FMCKey* key_p = new B737FMCKey();
    key_p->SetParentRenderObject(this);
    key_p->SetPosition(keyx0 + 3*deltax, 3 + 2*deltay);
    key_p->setKeyName((char*) "P");
    key_p->setKeyForm(0);
    key_p->setKeyDataRef1((char*) "FJCC/UFMC/P");
    key_p->setKeyDataRef2((char*) "laminar/B738/button/fmc1_P");
    this->AddGaugeComponent(key_p);

    B737FMCKey* key_q = new B737FMCKey();
    key_q->SetParentRenderObject(this);
    key_q->SetPosition(keyx0 + 4*deltax, 3 + 2*deltay);
    key_q->setKeyName((char*) "Q");
    key_q->setKeyForm(0);
    key_q->setKeyDataRef1((char*) "FJCC/UFMC/Q");
    key_q->setKeyDataRef2((char*) "laminar/B738/button/fmc1_Q");
    this->AddGaugeComponent(key_q);

    B737FMCKey* key_r = new B737FMCKey();
    key_r->SetParentRenderObject(this);
    key_r->SetPosition(keyx0 + 5*deltax, 3 + 2*deltay);
    key_r->setKeyName((char*) "R");
    key_r->setKeyForm(0);
    key_r->setKeyDataRef1((char*) "FJCC/UFMC/R");
    key_r->setKeyDataRef2((char*) "laminar/B738/button/fmc1_R");
    this->AddGaugeComponent(key_r);

    B737FMCKey* key_s = new B737FMCKey();
    key_s->SetParentRenderObject(this);
    key_s->SetPosition(keyx0 + 6*deltax, 3 + 2*deltay);
    key_s->setKeyName((char*) "S");
    key_s->setKeyForm(1);
    key_s->setKeyDataRef1((char*) "FJCC/UFMC/S");
    key_s->setKeyDataRef2((char*) "laminar/B738/button/fmc1_S");
    this->AddGaugeComponent(key_s);

    B737FMCKey* key_t = new B737FMCKey();
    key_t->SetParentRenderObject(this);
    key_t->SetPosition(keyx0 + 7*deltax, 3 + 2*deltay);
    key_t->setKeyName((char*) "T");
    key_t->setKeyForm(0);
    key_t->setKeyDataRef1((char*) "FJCC/UFMC/T");
    key_t->setKeyDataRef2((char*) "laminar/B738/button/fmc1_T");
    this->AddGaugeComponent(key_t);

    B737FMCKey* key_k = new B737FMCKey();
    key_k->SetParentRenderObject(this);
    key_k->SetPosition(keyx0 + 3*deltax, 3 + 3*deltay);
    key_k->setKeyName((char*) "K");
    key_k->setKeyForm(0);
    key_k->setKeyDataRef1((char*) "FJCC/UFMC/K");
    key_k->setKeyDataRef2((char*) "laminar/B738/button/fmc1_K");
    this->AddGaugeComponent(key_k);

    B737FMCKey* key_l = new B737FMCKey();
    key_l->SetParentRenderObject(this);
    key_l->SetPosition(keyx0 + 4*deltax, 3 + 3*deltay);
    key_l->setKeyName((char*) "L");
    key_l->setKeyForm(0);
    key_l->setKeyDataRef1((char*) "FJCC/UFMC/L");
    key_l->setKeyDataRef2((char*) "laminar/B738/button/fmc1_L");
    this->AddGaugeComponent(key_l);

    B737FMCKey* key_m = new B737FMCKey();
    key_m->SetParentRenderObject(this);
    key_m->SetPosition(keyx0 + 5*deltax, 3 + 3*deltay);
    key_m->setKeyName((char*) "M");
    key_m->setKeyForm(0);
    key_m->setKeyDataRef1((char*) "FJCC/UFMC/M");
    key_m->setKeyDataRef2((char*) "laminar/B738/button/fmc1_M");
    this->AddGaugeComponent(key_m);

    B737FMCKey* key_n = new B737FMCKey();
    key_n->SetParentRenderObject(this);
    key_n->SetPosition(keyx0 + 6*deltax, 3 + 3*deltay);
    key_n->setKeyName((char*) "N");
    key_n->setKeyForm(1);
    key_n->setKeyDataRef1((char*) "FJCC/UFMC/N");
    key_n->setKeyDataRef2((char*) "laminar/B738/button/fmc1_N");
    this->AddGaugeComponent(key_n);

    B737FMCKey* key_o = new B737FMCKey();
    key_o->SetParentRenderObject(this);
    key_o->SetPosition(keyx0 + 7*deltax, 3 + 3*deltay);
    key_o->setKeyName((char*) "O");
    key_o->setKeyForm(0);
    key_o->setKeyDataRef1((char*) "FJCC/UFMC/O");
    key_o->setKeyDataRef2((char*) "laminar/B738/button/fmc1_O");
    this->AddGaugeComponent(key_o);

    B737FMCKey* key_f = new B737FMCKey();
    key_f->SetParentRenderObject(this);
    key_f->SetPosition(keyx0 + 3*deltax, 3 + 4*deltay);
    key_f->setKeyName((char*) "F");
    key_f->setKeyForm(0);
    key_f->setKeyDataRef1((char*) "FJCC/UFMC/F");
    key_f->setKeyDataRef2((char*) "laminar/B738/button/fmc1_F");
    this->AddGaugeComponent(key_f);

    B737FMCKey* key_g = new B737FMCKey();
    key_g->SetParentRenderObject(this);
    key_g->SetPosition(keyx0 + 4*deltax, 3 + 4*deltay);
    key_g->setKeyName((char*) "G");
    key_g->setKeyForm(0);
    key_g->setKeyDataRef1((char*) "FJCC/UFMC/G");
    key_g->setKeyDataRef2((char*) "laminar/B738/button/fmc1_G");
    this->AddGaugeComponent(key_g);

    B737FMCKey* key_h = new B737FMCKey();
    key_h->SetParentRenderObject(this);
    key_h->SetPosition(keyx0 + 5*deltax, 3 + 4*deltay);
    key_h->setKeyName((char*) "H");
    key_h->setKeyForm(0);
    key_h->setKeyDataRef1((char*) "FJCC/UFMC/H");
    key_h->setKeyDataRef2((char*) "laminar/B738/button/fmc1_H");
    this->AddGaugeComponent(key_h);

    B737FMCKey* key_i = new B737FMCKey();
    key_i->SetParentRenderObject(this);
    key_i->SetPosition(keyx0 + 6*deltax, 3 + 4*deltay);
    key_i->setKeyName((char*) "I");
    key_i->setKeyForm(0);
    key_i->setKeyDataRef1((char*) "FJCC/UFMC/I");
    key_i->setKeyDataRef2((char*) "laminar/B738/button/fmc1_I");
    this->AddGaugeComponent(key_i);

    B737FMCKey* key_j = new B737FMCKey();
    key_j->SetParentRenderObject(this);
    key_j->SetPosition(keyx0 + 7*deltax, 3 + 4*deltay);
    key_j->setKeyName((char*) "J");
    key_j->setKeyForm(0);
    key_j->setKeyDataRef1((char*) "FJCC/UFMC/J");
    key_j->setKeyDataRef2((char*) "laminar/B738/button/fmc1_J");
    this->AddGaugeComponent(key_j);

    B737FMCKey* key_a = new B737FMCKey();
    key_a->SetParentRenderObject(this);
    key_a->SetPosition(keyx0 + 3*deltax, 3 + 5*deltay);
    key_a->setKeyName((char*) "A");
    key_a->setKeyForm(0);
    key_a->setKeyDataRef1((char*) "FJCC/UFMC/A");
    key_a->setKeyDataRef2((char*) "laminar/B738/button/fmc1_A");
    this->AddGaugeComponent(key_a);

    B737FMCKey* key_b = new B737FMCKey();
    key_b->SetParentRenderObject(this);
    key_b->SetPosition(keyx0 + 4*deltax, 3 + 5*deltay);
    key_b->setKeyName((char*) "B");
    key_b->setKeyForm(0);
    key_b->setKeyDataRef1((char*) "FJCC/UFMC/B");
    key_b->setKeyDataRef2((char*) "laminar/B738/button/fmc1_B");
    this->AddGaugeComponent(key_b);

    B737FMCKey* key_c = new B737FMCKey();
    key_c->SetParentRenderObject(this);
    key_c->SetPosition(keyx0 + 5*deltax, 3 + 5*deltay);
    key_c->setKeyName((char*) "C");
    key_c->setKeyForm(0);
    key_c->setKeyDataRef1((char*) "FJCC/UFMC/C");
    key_c->setKeyDataRef2((char*) "laminar/B738/button/fmc1_C");
    this->AddGaugeComponent(key_c);

    B737FMCKey* key_d = new B737FMCKey();
    key_d->SetParentRenderObject(this);
    key_d->SetPosition(keyx0 + 6*deltax, 3 + 5*deltay);
    key_d->setKeyName((char*) "D");
    key_d->setKeyForm(0);
    key_d->setKeyDataRef1((char*) "FJCC/UFMC/D");
    key_d->setKeyDataRef2((char*) "laminar/B738/button/fmc1_D");
    this->AddGaugeComponent(key_d);

    B737FMCKey* key_e = new B737FMCKey();
    key_e->SetParentRenderObject(this);
    key_e->SetPosition(keyx0 + 7*deltax, 3 + 5*deltay);
    key_e->setKeyName((char*) "E");
    key_e->setKeyForm(1);
    key_e->setKeyDataRef1((char*) "FJCC/UFMC/E");
    key_e->setKeyDataRef2((char*) "laminar/B738/button/fmc1_E");
    this->AddGaugeComponent(key_e);

    B737FMCKey* key_prev = new B737FMCKey();
    key_prev->SetParentRenderObject(this);
    key_prev->SetPosition(keyx0 + 0*deltax2, 3 + 4*deltay);
    key_prev->setKeyName((char*) "PREV PAGE");
    key_prev->setKeyForm(2);
    key_prev->setKeySizeX(deltax2);
    key_prev->setKeyDataRef1((char*) "FJCC/UFMC/PREVPAGE");
    key_prev->setKeyDataRef2((char*) "laminar/B738/button/fmc1_prev_page");
    this->AddGaugeComponent(key_prev);

    B737FMCKey* key_next = new B737FMCKey();
    key_next->SetParentRenderObject(this);
    key_next->SetPosition(keyx0 + 1*deltax2, 3 + 4*deltay);
    key_next->setKeyName((char*) "NEXT PAGE");
    key_next->setKeyForm(2);
    key_next->setKeySizeX(deltax2);
    key_next->setKeyDataRef1((char*) "FJCC/UFMC/NEXTPAGE");
    key_next->setKeyDataRef2((char*) "laminar/B738/button/fmc1_next_page");
    this->AddGaugeComponent(key_next);

    B737FMCKey* key_n1 = new B737FMCKey();
    key_n1->SetParentRenderObject(this);
    key_n1->SetPosition(keyx0 + 0*deltax2, 3 + 5*deltay);
    key_n1->setKeyName((char*) "N1 LIMIT");
    key_n1->setKeyForm(2);
    key_n1->setKeySizeX(deltax2);
    key_n1->setKeyDataRef2((char*) "laminar/B738/button/fmc1_n1_lim");
    this->AddGaugeComponent(key_n1);

    B737FMCKey* key_fix = new B737FMCKey();
    key_fix->SetParentRenderObject(this);
    key_fix->SetPosition(keyx0 + 1*deltax2, 3 + 5*deltay);
    key_fix->setKeyName((char*) "FIX");
    key_fix->setKeyForm(2);
    key_fix->setKeySizeX(deltax2);
    key_fix->setKeyDataRef1((char*) "FJCC/UFMC/FIX");
    key_fix->setKeyDataRef2((char*) "laminar/B738/button/fmc1_fix");
    this->AddGaugeComponent(key_fix);

    B737FMCKey* key_menu = new B737FMCKey();
    key_menu->SetParentRenderObject(this);
    key_menu->SetPosition(keyx0 + 0*deltax2, 3 + 6*deltay);
    key_menu->setKeyName((char*) "MENU");
    key_menu->setKeyForm(2);
    key_menu->setKeySizeX(deltax2);
    key_menu->setKeyDataRef1((char*) "FJCC/UFMC/MENU");
    key_menu->setKeyDataRef2((char*) "laminar/B738/button/fmc1_menu");
    this->AddGaugeComponent(key_menu);

    B737FMCKey* key_legs = new B737FMCKey();
    key_legs->SetParentRenderObject(this);
    key_legs->SetPosition(keyx0 + 1*deltax2, 3 + 6*deltay);
    key_legs->setKeyName((char*) "LEGS");
    key_legs->setKeyForm(2);
    key_legs->setKeySizeX(deltax2);
    key_legs->setKeyDataRef1((char*) "FJCC/UFMC/LEGS");
    key_legs->setKeyDataRef2((char*) "laminar/B738/button/fmc1_legs");
    this->AddGaugeComponent(key_legs);

    B737FMCKey* key_deparr = new B737FMCKey();
    key_deparr->SetParentRenderObject(this);
    key_deparr->SetPosition(keyx0 + 2*deltax2, 3 + 6*deltay);
    key_deparr->setKeyName((char*) "DEP ARR");
    key_deparr->setKeyForm(2);
    key_deparr->setKeySizeX(deltax2);
    key_deparr->setKeyDataRef1((char*) "FJCC/UFMC/DEPARR");
    key_deparr->setKeyDataRef2((char*) "laminar/B738/button/fmc1_dep_app");
    this->AddGaugeComponent(key_deparr);

    B737FMCKey* key_hold = new B737FMCKey();
    key_hold->SetParentRenderObject(this);
    key_hold->SetPosition(keyx0 + 3*deltax2, 3 + 6*deltay);
    key_hold->setKeyName((char*) "HOLD");
    key_hold->setKeyForm(2);
    key_hold->setKeySizeX(deltax2);
    key_hold->setKeyDataRef1((char*) "FJCC/UFMC/HOLD");
    key_hold->setKeyDataRef2((char*) "laminar/B738/button/fmc1_hold");
    this->AddGaugeComponent(key_hold);

    B737FMCKey* key_prog = new B737FMCKey();
    key_prog->SetParentRenderObject(this);
    key_prog->SetPosition(keyx0 + 4*deltax2, 3 + 6*deltay);
    key_prog->setKeyName((char*) "PROG");
    key_prog->setKeyForm(2);
    key_prog->setKeySizeX(deltax2);
    key_prog->setKeyDataRef1((char*) "FJCC/UFMC/PROG");
    key_prog->setKeyDataRef2((char*) "laminar/B738/button/fmc1_prog");
    this->AddGaugeComponent(key_prog);

    B737FMCKey* key_exec = new B737FMCKey();
    key_exec->SetParentRenderObject(this);
    key_exec->SetPosition(keyx0 + 5*deltax2, 3 + 6*deltay);
    key_exec->setKeyName((char*) "EXEC");
    key_exec->setKeyForm(2);
    key_exec->setKeySizeX(deltax2);
    key_exec->setKeyDataRef1((char*) "FJCC/UFMC/EXEC");
    key_exec->setKeyDataRef2((char*) "laminar/B738/button/fmc1_exec");
    this->AddGaugeComponent(key_exec);

    B737FMCKey* key_initref = new B737FMCKey();
    key_initref->SetParentRenderObject(this);
    key_initref->SetPosition(keyx0 + 0*deltax2, 3 + 7*deltay);
    key_initref->setKeyName((char*) "INIT REF");
    key_initref->setKeyForm(2);
    key_initref->setKeySizeX(deltax2);
    key_initref->setKeyDataRef1((char*) "FJCC/UFMC/INITREF");
    key_initref->setKeyDataRef2((char*) "laminar/B738/button/fmc1_init_ref");
    this->AddGaugeComponent(key_initref);

    B737FMCKey* key_rte = new B737FMCKey();
    key_rte->SetParentRenderObject(this);
    key_rte->SetPosition(keyx0 + 1*deltax2, 3 + 7*deltay);
    key_rte->setKeyName((char*) "RTE");
    key_rte->setKeyForm(2);
    key_rte->setKeySizeX(deltax2);
    key_rte->setKeyDataRef1((char*) "FJCC/UFMC/RTE");
    key_rte->setKeyDataRef2((char*) "laminar/B738/button/fmc1_rte");
    this->AddGaugeComponent(key_rte);

    B737FMCKey* key_clb = new B737FMCKey();
    key_clb->SetParentRenderObject(this);
    key_clb->SetPosition(keyx0 + 2*deltax2, 3 + 7*deltay);
    key_clb->setKeyName((char*) "CLB");
    key_clb->setKeyForm(2);
    key_clb->setKeySizeX(deltax2);
    key_clb->setKeyDataRef1((char*) "FJCC/UFMC/CLB_x737");
    key_clb->setKeyDataRef2((char*) "laminar/B738/button/fmc1_clb");
    this->AddGaugeComponent(key_clb);

    B737FMCKey* key_crz = new B737FMCKey();
    key_crz->SetParentRenderObject(this);
    key_crz->SetPosition(keyx0 + 3*deltax2, 3 + 7*deltay);
    key_crz->setKeyName((char*) "CRZ");
    key_crz->setKeyForm(2);
    key_crz->setKeySizeX(deltax2);
    key_crz->setKeyDataRef1((char*) "FJCC/UFMC/CRZ_x737");
    key_crz->setKeyDataRef2((char*) "laminar/B738/button/fmc1_crz");
    this->AddGaugeComponent(key_crz);

    B737FMCKey* key_des = new B737FMCKey();
    key_des->SetParentRenderObject(this);
    key_des->SetPosition(keyx0 + 4*deltax2, 3 + 7*deltay);
    key_des->setKeyName((char*) "DES");
    key_des->setKeyForm(2);
    key_des->setKeySizeX(deltax2);
    key_des->setKeyDataRef1((char*) "FJCC/UFMC/DES");
    key_des->setKeyDataRef2((char*) "laminar/B738/button/fmc1_des");
    this->AddGaugeComponent(key_des);

    B737FMCKey* key_lk1 = new B737FMCKey();
    key_lk1->SetParentRenderObject(this);
    key_lk1->SetPosition(mkeyx0, m_PhysicalSize.y - liney0 - 3*delta);
    key_lk1->setKeyName((char*) "--");
    key_lk1->setKeyForm(4);
    key_lk1->setKeySizeX(8);
    key_lk1->setKeySizeY(5);
    key_lk1->setKeyDataRef1((char*) "FJCC/UFMC/LK1");
    key_lk1->setKeyDataRef2((char*) "laminar/B738/button/fmc1_1L");
    this->AddGaugeComponent(key_lk1);

    B737FMCKey* key_lk2 = new B737FMCKey();
    key_lk2->SetParentRenderObject(this);
    key_lk2->SetPosition(mkeyx0, m_PhysicalSize.y - liney0 - 5*delta);
    key_lk2->setKeyName((char*) "--");
    key_lk2->setKeyForm(4);
    key_lk2->setKeySizeX(8);
    key_lk2->setKeySizeY(5);
    key_lk2->setKeyDataRef1((char*) "FJCC/UFMC/LK2");
    key_lk2->setKeyDataRef2((char*) "laminar/B738/button/fmc1_2L");
    this->AddGaugeComponent(key_lk2);

    B737FMCKey* key_lk3 = new B737FMCKey();
    key_lk3->SetParentRenderObject(this);
    key_lk3->SetPosition(mkeyx0, m_PhysicalSize.y - liney0 - 7*delta);
    key_lk3->setKeyName((char*) "--");
    key_lk3->setKeyForm(4);
    key_lk3->setKeySizeX(8);
    key_lk3->setKeySizeY(5);
    key_lk3->setKeyDataRef1((char*) "FJCC/UFMC/LK3");
    key_lk3->setKeyDataRef2((char*) "laminar/B738/button/fmc1_3L");
    this->AddGaugeComponent(key_lk3);

    B737FMCKey* key_lk4 = new B737FMCKey();
    key_lk4->SetParentRenderObject(this);
    key_lk4->SetPosition(mkeyx0, m_PhysicalSize.y - liney0 - 9*delta);
    key_lk4->setKeyName((char*) "--");
    key_lk4->setKeyForm(4);
    key_lk4->setKeySizeX(8);
    key_lk4->setKeySizeY(5);
    key_lk4->setKeyDataRef1((char*) "FJCC/UFMC/LK4");
    key_lk4->setKeyDataRef2((char*) "laminar/B738/button/fmc1_4L");
    this->AddGaugeComponent(key_lk4);

    B737FMCKey* key_lk5 = new B737FMCKey();
    key_lk5->SetParentRenderObject(this);
    key_lk5->SetPosition(mkeyx0, m_PhysicalSize.y - liney0 - 11*delta);
    key_lk5->setKeyName((char*) "--");
    key_lk5->setKeyForm(4);
    key_lk5->setKeySizeX(8);
    key_lk5->setKeySizeY(5);
    key_lk5->setKeyDataRef1((char*) "FJCC/UFMC/LK5");
    key_lk5->setKeyDataRef2((char*) "laminar/B738/button/fmc1_5L");
    this->AddGaugeComponent(key_lk5);

    B737FMCKey* key_lk6 = new B737FMCKey();
    key_lk6->SetParentRenderObject(this);
    key_lk6->SetPosition(mkeyx0, m_PhysicalSize.y - liney0 - 13*delta);
    key_lk6->setKeyName((char*) "--");
    key_lk6->setKeyForm(4);
    key_lk6->setKeySizeX(8);
    key_lk6->setKeySizeY(5);
    key_lk6->setKeyDataRef1((char*) "FJCC/UFMC/LK6");
    key_lk6->setKeyDataRef2((char*) "laminar/B738/button/fmc1_6L");
    this->AddGaugeComponent(key_lk6);

    B737FMCKey* key_rk1 = new B737FMCKey();
    key_rk1->SetParentRenderObject(this);
    key_rk1->SetPosition(m_PhysicalSize.x-mkeyx0-8, m_PhysicalSize.y - liney0 - 3*delta);
    key_rk1->setKeyName((char*) "--");
    key_rk1->setKeyForm(4);
    key_rk1->setKeySizeX(8);
    key_rk1->setKeySizeY(5);
    key_rk1->setKeyDataRef1((char*) "FJCC/UFMC/RK1");
    key_rk1->setKeyDataRef2((char*) "laminar/B738/button/fmc1_1R");
    this->AddGaugeComponent(key_rk1);

    B737FMCKey* key_rk2 = new B737FMCKey();
    key_rk2->SetParentRenderObject(this);
    key_rk2->SetPosition(m_PhysicalSize.x-mkeyx0-8, m_PhysicalSize.y - liney0 - 5*delta);
    key_rk2->setKeyName((char*) "--");
    key_rk2->setKeyForm(4);
    key_rk2->setKeySizeX(8);
    key_rk2->setKeySizeY(5);
    key_rk2->setKeyDataRef1((char*) "FJCC/UFMC/RK2");
    key_rk2->setKeyDataRef2((char*) "laminar/B738/button/fmc1_2R");
    this->AddGaugeComponent(key_rk2);

    B737FMCKey* key_rk3 = new B737FMCKey();
    key_rk3->SetParentRenderObject(this);
    key_rk3->SetPosition(m_PhysicalSize.x-mkeyx0-8, m_PhysicalSize.y - liney0 - 7*delta);
    key_rk3->setKeyName((char*) "--");
    key_rk3->setKeyForm(4);
    key_rk3->setKeySizeX(8);
    key_rk3->setKeySizeY(5);
    key_rk3->setKeyDataRef1((char*) "FJCC/UFMC/RK3");
    key_rk3->setKeyDataRef2((char*) "laminar/B738/button/fmc1_3R");
    this->AddGaugeComponent(key_rk3);

    B737FMCKey* key_rk4 = new B737FMCKey();
    key_rk4->SetParentRenderObject(this);
    key_rk4->SetPosition(m_PhysicalSize.x-mkeyx0-8, m_PhysicalSize.y - liney0 - 9*delta);
    key_rk4->setKeyName((char*) "--");
    key_rk4->setKeyForm(4);
    key_rk4->setKeySizeX(8);
    key_rk4->setKeySizeY(5);
    key_rk4->setKeyDataRef1((char*) "FJCC/UFMC/RK4");
    key_rk4->setKeyDataRef2((char*) "laminar/B738/button/fmc1_4R");
    this->AddGaugeComponent(key_rk4);

    B737FMCKey* key_rk5 = new B737FMCKey();
    key_rk5->SetParentRenderObject(this);
    key_rk5->SetPosition(m_PhysicalSize.x-mkeyx0-8, m_PhysicalSize.y - liney0 - 11*delta);
    key_rk5->setKeyName((char*) "--");
    key_rk5->setKeyForm(4);
    key_rk5->setKeySizeX(8);
    key_rk5->setKeySizeY(5);
    key_rk5->setKeyDataRef1((char*) "FJCC/UFMC/RK5");
    key_rk5->setKeyDataRef2((char*) "laminar/B738/button/fmc1_5R");
    this->AddGaugeComponent(key_rk5);

    B737FMCKey* key_rk6 = new B737FMCKey();
    key_rk6->SetParentRenderObject(this);
    key_rk6->SetPosition(m_PhysicalSize.x-mkeyx0-8, m_PhysicalSize.y - liney0 - 13*delta);
    key_rk6->setKeyName((char*) "--");
    key_rk6->setKeyForm(4);
    key_rk6->setKeySizeX(8);
    key_rk6->setKeySizeY(5);
    key_rk6->setKeyDataRef1((char*) "FJCC/UFMC/RK6");
    key_rk6->setKeyDataRef2((char*) "laminar/B738/button/fmc1_6R");
    this->AddGaugeComponent(key_rk6);

  }
  
}

B737FMC::~B737FMC()
{
  // Destruction handled by base class
}

void B737FMC::Render()
{
  
  bool is_captain = (this->GetArg() == 0) || (this->GetArg() == 2);
  bool with_keys = (this->GetArg() <= 1);
  
  int acf_type = m_pDataSource->GetAcfType();
  
  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
  if ((*avionics_on == 1) && (acf_type >= 1)) {

    // after drawing the background, draw components
    Gauge::Render();

    if (with_keys) {      

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      // grey FMC plate
      /*
	glColor3ub(COLOR_GRAY40);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_POLYGON);
	glVertex2f( 0, 0);
	glVertex2f( m_PhysicalSize.x, 0);
	glVertex2f( m_PhysicalSize.y, m_PhysicalSize.y);
	glVertex2f( 0, m_PhysicalSize.y);
	glEnd();
      */

      int message = 0;
      int exec = 0;
      if ((acf_type == 1) || (acf_type == 2) || (acf_type == 3)) {
	if (acf_type == 1) {
	  float *fmessage = link_dataref_flt("FJCC/UFMC/Offset_on",0);
	  float *fexec = link_dataref_flt("FJCC/UFMC/Exec_Light_on",0);
	  if (*fmessage == 1.0) message = 1;
	  if (*fexec == 1.0) exec = 1;
	} else {
	  int *imessage = link_dataref_int("laminar/B738/fmc/fmc_message");
	  int *iexec = link_dataref_int("laminar/B738/indicators/fmc_exec_lights");
	  if (*imessage == 1) message = 1;
	  if (*iexec == 1) exec = 1;
	}
	
	if (message == 1) {
	  glColor3ub(COLOR_ORANGE);
	  char buffer[2];
	  float fontSize = 0.035 * m_PhysicalSize.x;
	  m_pFontManager->SetSize( m_Font, fontSize, fontSize );
	  snprintf( buffer, sizeof(buffer), "M");
	  m_pFontManager->Print( m_PhysicalSize.x-10, m_PhysicalSize.y/2-40, buffer, m_Font);
	  snprintf( buffer, sizeof(buffer), "S");
	  m_pFontManager->Print( m_PhysicalSize.x-10, m_PhysicalSize.y/2-45, buffer, m_Font);
	  snprintf( buffer, sizeof(buffer), "G");
	  m_pFontManager->Print( m_PhysicalSize.x-10, m_PhysicalSize.y/2-50, buffer, m_Font);
	
	}

	if (exec == 1) {
	  float x0 = m_PhysicalSize.x / 2 + 29;
	  float x1 = m_PhysicalSize.x / 2 + 40;
	  float y0 = m_PhysicalSize.y / 2 - 6;
	  float y1 = m_PhysicalSize.y / 2 - 8;
	
	  glColor3ub(COLOR_YELLOW);
	  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	  glBegin(GL_POLYGON);
	  glVertex2f( x0, y0);
	  glVertex2f( x1, y0);
	  glVertex2f( x1, y1);
	  glVertex2f( x0, y1);
	  glEnd();

	}
      
      }
   
      // white rectangle of actual FMC display
      glColor3ub(COLOR_WHITE);
      //    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      //    glBegin(GL_POLYGON);
      glBegin(GL_LINE_LOOP);
      glVertex2f( 13, m_PhysicalSize.y - 1 );
      glVertex2f( 97, m_PhysicalSize.y - 1 );
      //glVertex2f( 98, 4 );
      //glVertex2f( 10, 4 );
      glVertex2f( 97, m_PhysicalSize.y - 79 );
      glVertex2f( 13, m_PhysicalSize.y - 79 );
      glEnd();

 
      glPopMatrix();

    }

  }
}

} // end namespace OpenGC

