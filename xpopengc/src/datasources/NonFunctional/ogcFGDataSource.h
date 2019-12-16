/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFGDataSource.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      John Wojnaroski
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2004/10/14 19:27:58 $
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

#ifndef ogc_FGDataSource_H
#define ogc_FGDataSource_H

#include <netSocket.h>
#include "opengc_data.hxx"
#include "ogcDataSource.h"

namespace OpenGC
{

class FGDataSource : public DataSource
{
public:
  
  FGDataSource();  
  virtual ~FGDataSource();
  
  // Open and close the network connection
  bool Open();
  void Close();

  // Get data from Flightgear
  bool GetData();

  // The "money" function
  void OnIdle(); 

  //float *update_joystick();

protected:

  // Flight model data received from FlightGear
  FGData* m_FDM;

  // The socket
  netSocket m_Socket;

  // The port we're receiving on
  unsigned int m_ReceivePort;
  
  // Are we connected to Flightgear?
  bool m_ValidConnection;

  // The message received from Flightgear
  char* m_Buffer;

  // A temporary buffer used in the receive process
  char* m_TempMsg;

  // Maximum length of the buffer
  int m_BufferLength;
};

} // end namespace OpenGC

#endif // ndef ogc_FGDataSource_H
