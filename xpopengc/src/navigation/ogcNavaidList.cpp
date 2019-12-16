/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavaidList.cpp,v $

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
#include "ogcNavaidList.h"

namespace OpenGC
{

  using namespace std;

  NavaidList
  ::NavaidList()
  {

  }

  NavaidList
  ::~NavaidList()
  {

  }

  bool
  NavaidList
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
      printf("Opened the navaid database file ok\n");

    // Throw out the first three lines, which is a copyright notice
    getline(inputFile, lineData);
    getline(inputFile, lineData);
    getline(inputFile, lineData);

    // Ok, file is open, process it
    while (inputFile.eof()!=1) {

      // Extract a line
      getline(inputFile, lineData);
    
      // First we want to see what type of navaid we're looking at
      int navaidType;
      sscanf(lineData.c_str(), "%d", &navaidType);
    
      // A type of 99 means we're at the end of the file, we'll finish loading by going back
      // to the eof() at the start of the while loop
      if(navaidType == 99)
	continue;
    
      // Potential data that can be loaded
      double lat, lon, elev, freq, typeSpecific, unknown;
      string id;
    
      // Temp navaid storage
      GeographicObject* pNavaid;
    
      // Input stream for parsing
      std::istringstream inputStream(lineData);
      inputStream >> navaidType >> lat >> lon >> elev >> freq >> typeSpecific >> unknown >> id;
      
      pNavaid = new GeographicObject();
      pNavaid->SetNavaidType(navaidType);
      pNavaid->SetDegreeLat(lat);
      pNavaid->SetDegreeLon(lon);
      pNavaid->SetAltitudeMeters(elev/3.28084);
      pNavaid->SetFrequency(freq);
      pNavaid->SetIdentification(id);
      this->push_back(pNavaid);

      //    printf("%f %f %s \n",lat,lon,id.c_str());

    }

    // Everything worked ok
    return true;
  }

} // end namespace OpenGC
