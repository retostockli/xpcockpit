/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavDatabase.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:13 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "ogcNavDatabase.h"
#include <stdio.h>

namespace OpenGC
{

NavDatabase
::NavDatabase()
{
  m_NavaidList = 0;
  m_NavaidHash = 0;  
  m_FixList = 0;
  m_FixHash = 0;  
  m_AirportList = 0;
  m_AirportHash = 0;
}

NavDatabase
::~NavDatabase()
{
  if(m_NavaidList != 0)
    delete m_NavaidList;

  if(m_NavaidHash != 0)
    delete m_NavaidHash;
      
  if(m_FixList != 0)
    delete m_FixList;

  if(m_FixHash != 0)
    delete m_FixHash;
  
  if(m_AirportList != 0)
    delete m_AirportList;

  if(m_AirportHash != 0)
    delete m_AirportHash;
}

void NavDatabase
::InitDatabase(string pathToNav)
{
  printf("NavDatabase::InitDatabase() - Loading geographic data...\n");

  printf("NavDatabase::InitDatabase() - Loading navigation data\n");
  m_NavaidList = new NavaidList;
  //  m_NavaidList->InitializeList( pathToNav + "earth_nav.dat" );
  m_NavaidList->InitializeList( pathToNav + "/Resources/default data/earth_nav.dat" );

  printf("NavDatabase::InitDatabase() - Creating navaid hash\n");
  m_NavaidHash = new GeographicHash;
  printf("NavDatabase::InitDatabase() - Doing the hash\n");
  m_NavaidHash->InsertGeographicList(m_NavaidList);

  printf("NavDatabase::InitDatabase() - Loading fix data\n");
  m_FixList = new FixList;
  //  m_FixList->InitializeList( pathToNav + "earth_fix.dat" );
  m_FixList->InitializeList( pathToNav + "/Resources/default data/earth_fix.dat" );
  
  printf("NavDatabase::InitDatabase() - Creating fix hash\n");
  m_FixHash = new GeographicHash;
  printf("NavDatabase::InitDatabase() - Doing the hash\n");
  m_FixHash->InsertGeographicList(m_FixList);

  printf("NavDatabase::InitDatabase() - Loading airport data\n");
  m_AirportList = new AirportList;
  //  m_AirportList->InitializeList( pathToNav + "apt.dat" );
  m_AirportList->InitializeList( pathToNav + "/Resources/default scenery/default apt dat/Earth nav data/apt.dat" );

  printf("NavDatabase::InitDatabase() - Creating airport hash\n");
  m_AirportHash = new GeographicHash;
  printf("NavDatabase::InitDatabase() - Doing the hash\n");
  m_AirportHash->InsertGeographicList(m_AirportList);
  
  printf("NavDatabase::InitDatabase() - Done loading geographic data...\n");
  printf("The navaid list contains %i items\n", (int) m_NavaidList->size() );
  printf("The fix list contains %i items\n", (int) m_FixList->size() );
  printf("The airport list contains %i items\n", (int) m_AirportList->size() );
}

} // end namespace OpenGC