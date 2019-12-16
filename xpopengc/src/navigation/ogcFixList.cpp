/*=========================================================================

  NOTE: temporary file while waiting for Reto's version!

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcFixList.cpp,v $

  Last modification:
  Date:      $Date: 2004/10/14 19:28:12 $
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
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "ogcFixList.h"

namespace OpenGC
{

  using namespace std;

  FixList
  ::FixList()
  {

  }

  FixList
  ::~FixList()
  {

  }

  bool
  FixList
  ::LoadData(string fileName)
  {
    // Buffer used to read from file
    string lineData;
  
    // Open the input file for reading
    ifstream inputFile( fileName.c_str() );

    // Test to see if the file is open
    if( inputFile.is_open() == 0 )
      return false;
    else
      printf("Opened the fixes database file ok\n");

    // Throw out the first three lines, which is a copyright notice
    getline(inputFile, lineData);
    getline(inputFile, lineData);
    getline(inputFile, lineData);

    // Ok, file is open, process it
    while (inputFile.eof()!=1) {

      // Extract a line
      getline(inputFile, lineData);
    
      // Potential data that can be loaded
      double lat, lon;
      string id;
    
      // Temp navaid storage
      GeographicObject* pFix;
    
      // Input stream for parsing
      std::istringstream inputStream(lineData);
      inputStream >>  lat >> lon >> id;
      
      pFix = new GeographicObject();
      pFix->SetDegreeLat(lat);
      pFix->SetDegreeLon(lon);
      pFix->SetIdentification(id);
      this->push_back(pFix);

    }

    // Everything worked ok
    return true;
  }

} // end namespace OpenGC
