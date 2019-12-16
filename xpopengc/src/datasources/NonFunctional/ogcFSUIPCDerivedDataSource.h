/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFSUIPCDerivedDataSource.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:00 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2004 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * Connects to Microsoft Flight Simulator line of sims using either
 * FSUIPC or FDSconnect.

 * This class does nothing if not on a Win32 platform, although
 * it can still be instantiated
 */

#ifndef ogcFSUIPCDerivedDataSource_h
#define ogcFSUIPCDerivedDataSource_h

#include "ogcDataSource.h"
#ifdef _WIN32
  #include "FDSConnection.h"
  #include "FSUIPC_User.h"
#endif

namespace OpenGC
{

#ifdef _WIN32

class FSUIPCDerivedDataSource : public DataSource  
{
public:
  /** Initialize with connection type, 0 for FSUIPC, 1 for FDS */
  FSUIPCDerivedDataSource(bool connectionType);
  virtual ~FSUIPCDerivedDataSource();

  /** The "money" function */
  void OnIdle();

  /** Simple function to wrap headings around 360 degrees */
  double WrapHeading(double rawHead);

  /** The local read function that calls either the FDS or FSUIPC versions */
  void Read(DWORD offset, DWORD size, void *data);

  /** The local process function that calls either the FDS or FSUIPC versions */
  void Process();

protected:

  /** Type of connection, 0 for FSUIPC, 1 for FDS */
  bool m_ConnectionType;

  /** Connection to FDS interface, if we're using that rather than FSUIPC */
  CFDSConnection* m_FDSConnection;

  /** Are we connected to FS? */
  bool m_ValidConnection;
};

#else // Not a Win32 platform

class FSUIPCDerivedDataSource : public DataSource  
{
public:
  FSUIPCDerivedDataSource(bool connectionType);
  virtual ~FSUIPCDerivedDataSource();

  /** The "money" function */
  void OnIdle();

protected:

  /** Are we connected to FS? */
  bool m_ValidConnection;
};

#endif // Not a Win32 platform

} // end namespace OpenGC

#endif // ndef ogcFSUIPCDerivedDataSource_h


