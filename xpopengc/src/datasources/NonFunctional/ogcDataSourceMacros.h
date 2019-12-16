/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcDataSourceMacros.h,v $

  Last modification:
    Date:      $Date: 2004/11/23 05:00:23 $
    Version:   $Revision: 1.2 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2004 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ogcDataSourceMacros_h
#define ogcDataSourceMacros_h

/** Macro for data members that are read-only */
#define DataReadMacro(name,type) \
  public: \
  type Get##name() \
  { \
    return m_##name; \
  } \
  void InternalSet##name(type value) \
  { \
    m_##name = value; \
  } \
  protected: \
  type m_##name;

/** Macro for data members which allow non-source write operations */
#define DataReadWriteMacro(name,type) \
  public: \
  type Get##name() \
  { \
    return m_##name; \
  } \
  void Set##name(type value) \
  { \
    m_Modified_##name = value; \
    m_##name##_Modified = true; \
  } \
  void InternalSet##name(type value) \
  { \
    m_##name = value; \
  } \
  protected: \
  type m_##name; \
  type m_Modified_##name; \
  bool m_##name##_Modified;

#endif