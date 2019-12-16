/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcKeypadButton.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:10 $
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

#include "ogcGaugeComponent.h"
#include "ogcKeypadButton.h"

namespace OpenGC
{

KeypadButton::KeypadButton()
{
  printf("KeypadButton constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 20;
  m_PhysicalSize.y = 20;

  m_Scale.x = 1.0;
  m_Scale.y = 1.0;

  m_DisplayChar = '0';
  m_ButtonNumber = 0;
}

KeypadButton::~KeypadButton()
{

}

void KeypadButton::Render()
{
  // Call base class to setup viewport and projection
  GaugeComponent::Render();

  // Buffer for writing characters
  char buffer[2];

  sprintf(buffer, "%c\0", m_DisplayChar);

  // Draw white background
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex2d(0.0, 0.0);
  glVertex2d(0.0, 20.0);
  glVertex2d(20.0, 20.0);
  glVertex2d(20.0, 0.0);
  glVertex2f(0.0, 0.0);
  glEnd();

  // Draw the character
  glColor3ub(0.0, 0.0, 0.0);
  m_pFontManager->SetSize(m_Font, 6.0, 6.0);
  m_pFontManager->Print(7.0, 7.0, &buffer[0], m_Font);
}

// Called if the mouse click applies to this object
void KeypadButton::OnMouseDown(int button, double physicalX, double physicalY)
{
  printf("Keypad button %i\n", m_ButtonNumber);

  // Send the appropriate message based on the button number
  switch(m_ButtonNumber)
  {
  case 0:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_0, 0);
    break;
  case 1:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_1, 0);
    break;
  case 2:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_2, 0);
    break;
  case 3:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_3, 0);
    break;
  case 4:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_4, 0);
    break;
  case 5:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_5, 0);
    break;
  case 6:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_6, 0);
    break;
  case 7:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_7, 0);
    break;
  case 8:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_8, 0);
    break;
  case 9:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_9, 0);
    break;
  default:
    this->DispatchOpenGCMessage(MSG_VIRTUAL_KEYPAD_0, 0);
  }
}

} // end namespace OpenGC
