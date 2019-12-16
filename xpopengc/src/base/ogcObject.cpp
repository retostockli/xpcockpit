/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcObject.cpp,v $

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

#include <stdio.h>
#include "ogcObject.h"

namespace OpenGC
{

Object
::Object()
{
  if(m_StaticObjectList == 0)
    m_StaticObjectList = new std::list<Object*>;
    
  // Add this object to the static list of all OpenGC objects
  m_StaticObjectList->push_back(this);
}

Object
::~Object()
{
  // Remove this object from the list of objects that exist in the world
  ObjectIteratorType it;
  
  for (it = m_StaticObjectList->begin(); it != m_StaticObjectList->end(); ++it)
  {
    if ( (*it) == this )
    {
      m_StaticObjectList->remove(*it);
      break;
    }
  }
}

void
Object
::DispatchOpenGCMessage(Message message, void *data)
{
  // Dispatch the message to all OpenGC objects
  ObjectIteratorType it;
  
  for (it = m_StaticObjectList->begin(); it != m_StaticObjectList->end(); ++it)
  {
    (*it)->OnOpenGCMessage(message, data);
  }
}

// Called when a message is triggered somewhere
void
Object
::OnOpenGCMessage(Message message, void *data)
{
  // The default version of this doesn't do anything
  // Overload to provide specific functionality
}

} // end namespace OpenGC
