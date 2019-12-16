/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFDSDataSource.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2004/10/14 19:27:57 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
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
Connects to FS2004 using the FlightDeckSoftware interface.

This class does nothing if not on a Win32 platform, although
it can still be instantiated through the magic of ifdefs
*/

#ifndef ogcFDSDataSource_h
#define ogcFDSDataSource_h

#include "ogcDataSource.h"
#ifdef _WIN32
  #include "FDSConnection.h"
#endif

namespace OpenGC
{

#ifdef _WIN32

class FDSDataSource : public DataSource  
{
public:
  FDSDataSource();
  virtual ~FDSDataSource();

  // The "money" function
  void OnIdle();

  // Simple function to wrap headings around 360 degrees
  double WrapHeading(double rawHead);

protected:

  /** Connection to FDS interface */
  CFDSConnection* m_pFDSConnection;

  // Are we connected to FS?
  bool m_ValidConnection;
};

#else // Not a Win32 platform

class FDSDataSource : public DataSource  
{
public:
  FDSDataSource();
  virtual ~FDSDataSource();

  // The "money" function
  void OnIdle();

protected:

  // Are we connected to FS?
  bool m_ValidConnection;
};

#endif // Not a Win32 platform

} // end namespace OpenGC

#endif // ndef ogcFDSDataSource_h


