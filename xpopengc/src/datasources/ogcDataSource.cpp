/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcDataSource.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:56 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "ogcDataSource.h"
#include <stdio.h>

namespace OpenGC
{

DataSource::DataSource()
{
  printf("DataSource constructed\n");

  //  m_Airframe = new AirframeDataContainer;
}

DataSource::~DataSource()
{
  //  delete m_Airframe;
}

// Initialize the data
void DataSource::InitializeData()
{
  //  m_Airframe->InitializeData();
}

void DataSource::OnIdle()
{
  // The default version does nothing
}

  void DataSource::define_server(int port, string ip_address, int radardata)
{
  printf("WRONG SOURCE");
  // The default version does nothing
}

bool DataSource::Open()
{
	// This is redefined in FGDatasource.cpp so as to delay opening the socket until all file and
	// command options have been tested for network parameters
  
  // The default version returns false, which doesn't necessarily mean anything
  return false;
}

} // end namespace OpenGC
