/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcKeypadDisplay.h,v $

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
 * A simple display that responds to messages from numeric keypad buttons
 * similar to how a conventional calculator display works.
 */

#ifndef ogcKeypadDisplay_h
#define ogcKeypadDisplay_h

#include <deque>
#include "ogcGaugeComponent.h"

namespace OpenGC
{

class KeypadDisplay : public GaugeComponent  
{
public:

  KeypadDisplay();
  virtual ~KeypadDisplay();

  /** Overloaded render function */
  void Render();

  /** Message handling function that adds numbers to our display */
  void OnOpenGCMessage(Message message, void* data);

protected:

  /** The font number provided to us by the font manager */
  int m_Font;

  /** A deque that stores a bunch of numbers to render */
  std::deque<int> m_Display;
};

} // end namespace OpenGC

#endif
