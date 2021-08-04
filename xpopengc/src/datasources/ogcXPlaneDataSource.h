/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcXPlaneDataSource.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2004/10/14 19:28:01 $
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

#ifndef ogcXPlaneDataSource_h
#define ogcXPlaneDataSource_h

#include <string>

namespace OpenGC
{

using namespace std;

class XPlaneDataSource : public DataSource
{
public:
  XPlaneDataSource();
  virtual ~XPlaneDataSource();

  // transfer TCP/IP address and port from namelist
  void define_server(int port, string ip_address, int radardata);

  // The "money" function
  void OnIdle();
 
protected:
  
};

} // end namespace OpenGC

#endif
