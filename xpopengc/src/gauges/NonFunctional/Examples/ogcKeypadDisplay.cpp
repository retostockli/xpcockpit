/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcKeypadDisplay.cpp,v $

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
#include "ogcKeypadDisplay.h"

namespace OpenGC
{

KeypadDisplay::KeypadDisplay()
{
  printf("KeypadDisplay constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 60;
  m_PhysicalSize.y = 10;

  m_Scale.x = 1.0;
  m_Scale.y = 1.0;

  // Intialize the queue with a bunch of 0's
  for(int i = 0; i <=9; i++)
  {
    m_Display.push_back(0);
  }
}

KeypadDisplay::~KeypadDisplay()
{

}

void KeypadDisplay::Render()
{
  // Call base class to setup viewport and projection
  GaugeComponent::Render();

  // First thing to draw is a box
  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(0.0, 0.0);
  glVertex2f(60.0, 0.0);
  glVertex2f(60.0, 10.0);
  glVertex2f(0.0, 10.0);
  glEnd();

  // Buffer for writing characters
  char buffer[2];
  
  // Keep track of the horizontal position during rendering
  float horPos = 54.0;

  // Iterator to traverse the display queue
  std::deque<int>::iterator it;

  m_pFontManager->SetSize(m_Font, 5.0, 5.0);

  // Now render all of the characters in the display
  for (it = m_Display.begin(); it != m_Display.end(); ++it)
  {
    // Store the display number as a char
    sprintf(buffer, "%i\0", *it);

    // Draw the character
    m_pFontManager->Print(horPos, 2.5, &buffer[0], m_Font);

    horPos -= 6.0;
  }
}

// Message handler
void KeypadDisplay::OnOpenGCMessage(Message message, void* data)
{
  // Send the appropriate message based on the button number
  switch(message)
  {
  case MSG_VIRTUAL_KEYPAD_0:
    {
      m_Display.pop_back();
      m_Display.push_front(0);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_1:
    {
      m_Display.pop_back();
      m_Display.push_front(1);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_2:
    {
      m_Display.pop_back();
      m_Display.push_front(2);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_3:
    {
      m_Display.pop_back();
      m_Display.push_front(3);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_4:
    {
      m_Display.pop_back();
      m_Display.push_front(4);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_5:
    {
      m_Display.pop_back();
      m_Display.push_front(5);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_6:
    {
      m_Display.pop_back();
      m_Display.push_front(6);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_7:
    {
      m_Display.pop_back();
      m_Display.push_front(7);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_8:
    {
      m_Display.pop_back();
      m_Display.push_front(8);
    }
    break;

  case MSG_VIRTUAL_KEYPAD_9:
    {
      m_Display.pop_back();
      m_Display.push_front(9);
    }
    break;
  }
}

} // end namespace OpenGC
