/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcKeypadButton.h,v $

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

/**
 * Simple numeric keypad button for testing mouse click interaction
 */

#ifndef ogcKeypadButton_h
#define ogcKeypadButton_h

#include "ogcGaugeComponent.h"

namespace OpenGC
{

class KeypadButton : public GaugeComponent  
{
public:

  KeypadButton();
  virtual ~KeypadButton();

  /** Overloaded render function */
  void Render();

  /** Set the "number" of the button */
  void SetButtonNumber(int num) {m_ButtonNumber = num;}

  /** Set the character displayed in the button */
  void SetDisplayCharacter(char displayChar) {m_DisplayChar = displayChar;}

  /** Called if the down mouse click applies to this object */
  void OnMouseDown(int button, double physicalX, double physicalY);

protected:

  /** The font number provided to us by the font manager */
  int m_Font;

  /** The number of the button */
  int m_ButtonNumber;

  /** The character displayed in the button */
  char m_DisplayChar;
};

} // end namespace OpenGC

#endif
