/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcAirportList.cpp,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:11 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "ogcAirportList.h"

namespace OpenGC
{

using namespace std;

AirportList::AirportList()
{

}

AirportList::~AirportList()
{

}

bool
AirportList
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
    printf("Opened the airport database file ok\n");

  // Throw out the first three lines, which is a copyright notice
  getline(inputFile, lineData);
  getline(inputFile, lineData);
  getline(inputFile, lineData);
  
  // Let's us know if we have an airport (=1)
  int airportHeader;
  int runwayHeader;

  // Ok, file is open, process it
  while (inputFile.eof()!=1) 
{
    // We want to loop until we find an airport
    bool foundAnAirport = false;
    
    while (foundAnAirport == false)
    {
      // cout << "Searching for an airport to load\n";
      
      // Extract a line
      getline(inputFile, lineData);
    
      // Airports are coded as 1
      sscanf(lineData.c_str(), "%d", &airportHeader);
      
      if(airportHeader == 1)
        foundAnAirport = true;
      
      // Check to see if we're at the last line in the file
      if(airportHeader == 99)
        break;
    }
    
    // If we're here without finding an airport, we're done
    if (foundAnAirport == false)
      continue;
    
    // cout << "Loading an airport\n";
    
    // Potential data that can be loaded
    double width, smoothness, elev, threshold, overrun;
    double lat, lon, lat1, lon1, lat2, lon2;
    bool hasControlTower, inop;
    int surfacetype, shouldertype, centerlights, edgelights, signs, markings, approachlights, touchdownlights, endlights;
    string id, runwaynumber;
    
    // Input stream for parsing
    std::istringstream inputStream(lineData);
      
    inputStream >> airportHeader >> elev >> hasControlTower >> inop >> id;

    // Only use 4 letter ICAO airport codes
    // we found some XLF007... codes
    if (id.length() != 4)
      continue;
    
    // Temp airport storage
    GeographicObject* pAirport;
    
    pAirport = new GeographicObject();
    pAirport->SetAltitudeMeters(elev/3.28084);
    pAirport->SetIdentification(id);
    
    // START: OLD APT 810 format
    // Get the next line, which is one end of the first runway
    // This is not a great approximation of the actual airport location, but it's easy
    //    getline(inputFile, lineData);
    //    inputStream.str(lineData);
    //    inputStream >> airportHeader >> lat >> lon;
    // END: OLD APT 810 format
 
    // START: NEW APT 850 format
    // We want to loop until we find a runway
    bool foundARunway = false;
    while (foundARunway == false)
    {
      getline(inputFile, lineData);

      // Land Runways are coded as 100
      sscanf(lineData.c_str(), "%d", &runwayHeader);
      
      if(runwayHeader == 100)
        foundARunway = true;
      
      // Check to see if we're at the last line in the file
      if(runwayHeader == 99)
        break;
    

    }

    // If we're here without finding a runway, we're done
    if (foundARunway == false) {
      printf("Found Airport without a runway: %s \n",id.c_str());
      continue;
    }

    /* examine first runway of airport */
    inputStream.str(lineData);
    inputStream >> runwayHeader >> width >> surfacetype >> shouldertype >> smoothness >> 
      centerlights >> edgelights >> signs >> 
      runwaynumber >> lat1 >> lon1 >> threshold >> overrun >> markings >> approachlights >> touchdownlights >> endlights >>
      runwaynumber >> lat2 >> lon2 >> threshold >> overrun >> markings >> approachlights >> touchdownlights >> endlights;
    // END: NEW APT 850 format

    /* approximate airport coordinates with runway center of first runway */
    lat = (lat1+lat2)/2.0;
    lon = (lon1+lon2)/2.0;

    pAirport->SetDegreeLat(lat);
    pAirport->SetDegreeLon(lon);

    //    printf("%f %f %f %f %s \n",lat1,lon1,lat2,lon2,id.c_str());
    
    // Now add the airport to the list
    this->push_back(pAirport);
  }

  // Everything worked ok
  return true;
}

} // end namespace OpenGC
