/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFSUIPCDataSource.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2004/10/14 19:27:59 $
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
Wraps Peter Dowson's FSUIPC interface to FS2000. Overloads
OnIdle() to read data from the FSUIPC.dll or WideFS and
store sim parameters in the local data format.

This class does nothing if not on a Win32 platform, although
it can still be instantiated through the magic of ifdefs
*/

#ifndef ogcFSUIPCDataSource_h
#define ogcFSUIPCDataSource_h

#include "ogcDataSource.h"

#ifdef _WIN32
  #include "FSUIPC_User.h"
#endif

namespace OpenGC
{

class FSUIPCDataSource : public DataSource  
{
public:
  FSUIPCDataSource();
  virtual ~FSUIPCDataSource();

  // The "money" function
  void OnIdle();

  // Simple function to wrap headings around 360 degrees
  double WrapHeading(double rawHead);

protected:
  // For transferring data from FSUIPC
  unsigned char * m_pData;

  // Are we connected to FS?
  bool m_ValidConnection;

};

} // end namespace OpenGC

#endif


