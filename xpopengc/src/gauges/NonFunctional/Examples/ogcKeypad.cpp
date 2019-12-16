/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcKeypad.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:09 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <stdio.h>

#include "ogcDataSource.h"
#include "ogcKeypadButton.h"
#include "ogcKeypadDisplay.h"
#include "ogcKeypad.h"

namespace OpenGC
{

Keypad::Keypad()
{
  printf("Keypad constructed\n");

  m_PhysicalSize.x = 60;
  m_PhysicalSize.y = 90;

  // We want to draw an outline
  this->SetGaugeOutline(true);

  KeypadButton* pKey0 = new KeypadButton();
  pKey0->SetParentRenderObject(this);
  pKey0->SetPosition(0,0);
  pKey0->SetDisplayCharacter('0');
  pKey0->SetButtonNumber(0);
  this->AddGaugeComponent(pKey0);

  KeypadButton* pKey1 = new KeypadButton();
  pKey1->SetParentRenderObject(this);
  pKey1->SetPosition(0,20);
  pKey1->SetDisplayCharacter('1');
  pKey1->SetButtonNumber(1);
  this->AddGaugeComponent(pKey1);

  KeypadButton* pKey2 = new KeypadButton();
  pKey2->SetParentRenderObject(this);
  pKey2->SetPosition(20,20);
  pKey2->SetDisplayCharacter('2');
  pKey2->SetButtonNumber(2);
  this->AddGaugeComponent(pKey2);

  KeypadButton* pKey3 = new KeypadButton();
  pKey3->SetParentRenderObject(this);
  pKey3->SetPosition(40,20);
  pKey3->SetDisplayCharacter('3');
  pKey3->SetButtonNumber(3);
  this->AddGaugeComponent(pKey3);

  KeypadButton* pKey4 = new KeypadButton();
  pKey4->SetParentRenderObject(this);
  pKey4->SetPosition(0,40);
  pKey4->SetDisplayCharacter('4');
  pKey4->SetButtonNumber(4);
  this->AddGaugeComponent(pKey4);

  KeypadButton* pKey5 = new KeypadButton();
  pKey5->SetParentRenderObject(this);
  pKey5->SetPosition(20,40);
  pKey5->SetDisplayCharacter('5');
  pKey5->SetButtonNumber(5);
  this->AddGaugeComponent(pKey5);

  KeypadButton* pKey6 = new KeypadButton();
  pKey6->SetParentRenderObject(this);
  pKey6->SetPosition(40,40);
  pKey6->SetDisplayCharacter('6');
  pKey6->SetButtonNumber(6);
  this->AddGaugeComponent(pKey6);

  KeypadButton* pKey7 = new KeypadButton();
  pKey7->SetParentRenderObject(this);
  pKey7->SetPosition(0,60);
  pKey7->SetDisplayCharacter('7');
  pKey7->SetButtonNumber(7);
  this->AddGaugeComponent(pKey7);

  KeypadButton* pKey8 = new KeypadButton();
  pKey8->SetParentRenderObject(this);
  pKey8->SetPosition(20,60);
  pKey8->SetDisplayCharacter('8');
  pKey8->SetButtonNumber(8);
  this->AddGaugeComponent(pKey8);

  KeypadButton* pKey9 = new KeypadButton();
  pKey9->SetParentRenderObject(this);
  pKey9->SetPosition(40,60);
  pKey9->SetDisplayCharacter('9');
  pKey9->SetButtonNumber(9);
  this->AddGaugeComponent(pKey9);

  KeypadDisplay* pDisp = new KeypadDisplay();
  pDisp->SetParentRenderObject(this);
  pDisp->SetPosition(0,80);
  this->AddGaugeComponent(pDisp);
}

Keypad::~Keypad()
{
  // Destruction handled by base class
}

void Keypad::Render()
{
	Gauge::Render();

  // Drawing options
  glLineWidth(2.0);
  glColor3ub(0, 0, 0);

  // Draw some black lines to divide up the grid
  glBegin(GL_LINES);
  glVertex2f(20.0, 0.0);
  glVertex2f(20.0, 80.0);

  glVertex2f(40.0, 0.0);
  glVertex2f(40.0, 80.0);

  glVertex2f(0.0, 20.0);
  glVertex2f(60.0, 20.0);

  glVertex2f(0.0, 40.0);
  glVertex2f(60.0, 40.0);

  glVertex2f(0.0, 60.0);
  glVertex2f(60.0, 60.0);
  glEnd();
}

} // end namespace OpenGC
