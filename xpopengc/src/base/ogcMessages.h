/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcMessages.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:54 $
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
 * Defines messages which can be triggered by mouse clicks, key presses, etc.
 */

#ifndef ogcMessages_h
#define ogcMessages_h

namespace OpenGC
{

enum Message
{
  /* Messages used by the virtual keypad in the test gauge*/
  MSG_VIRTUAL_KEYPAD_0,
  MSG_VIRTUAL_KEYPAD_1,
  MSG_VIRTUAL_KEYPAD_2,
  MSG_VIRTUAL_KEYPAD_3,
  MSG_VIRTUAL_KEYPAD_4,
  MSG_VIRTUAL_KEYPAD_5,
  MSG_VIRTUAL_KEYPAD_6,
  MSG_VIRTUAL_KEYPAD_7,
  MSG_VIRTUAL_KEYPAD_8,
  MSG_VIRTUAL_KEYPAD_9,

  /* Altimeter pressure increment/decrement by 1/100 inch */
  MSG_ALTIMETER_PRESSURE_INCREMENT,
  MSG_ALTIMETER_PRESSURE_DECREMENT,

  /* Nav zoom in and out */
  MSG_NAV_ZOOM_DECREASE,
  MSG_NAV_ZOOM_INCREASE
};

} // end namespace OpenGC

#endif
