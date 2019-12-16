/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcObject.h,v $

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
 * The base class for any object in the OpenGC world that can send
 * and receive messages
 */

#ifndef ogcObject_h
#define ogcObject_h

#include <list>
#include "ogcMessages.h"

namespace OpenGC
{

class Object
{
public:
 
  Object();
  virtual ~Object();

  /** Called when a message is triggered somewhere */
  void OnOpenGCMessage(Message message, void *data);

  /** Send a message to all OpenGC objects */
  void DispatchOpenGCMessage(Message message, void *data);

private:
  
  /** List of all objects in the OpenGC world, used for global message passing */
  static std::list<Object*>* m_StaticObjectList;
  
  /** Iterator type for accessing the list of OpenGC objects */
  typedef std::list<Object*>::iterator ObjectIteratorType;
};

} // end namespace OpenGC

#endif
