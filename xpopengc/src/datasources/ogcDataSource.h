/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcDataSource.h,v $

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

#ifndef ogcDataSource_h
#define ogcDataSource_h

// #include "ogcAirframeDataContainer.h"
#include <string>

extern "C" {
#include "serverdata.h"
#include "udpdata.h"
}

namespace OpenGC
{

using namespace std;

class DataSource  
{
public:
  DataSource();
  virtual ~DataSource();

  // transfer TCP/IP address and port from namelist
  virtual void define_server(int port, string ip, int radardata);

  /** Initialize the data to a default value*/
  void InitializeData();
  
  /** Called by the base AppObject after all the init parameters
    * have been complete. This should open the connection to the sim
    */
  virtual bool Open();

  /** Called by the render window during idle processing
    * This function is the one and only place where OpenGC
    * should acquire data from the sim
    */
  virtual void OnIdle();

  /** Get access to airframe data */
  //  AirframeDataContainer* GetAirframe() { return m_Airframe; }

  /* Determine Aircraft type */
  virtual void SetAcfType(int acfType) { m_AcfType = acfType; }
  virtual int GetAcfType() { return m_AcfType; }
  
protected:

  /** Data that describes the airframe (alt, heading, control surfaces, etc.) */
  //  AirframeDataContainer* m_Airframe;

  int m_AcfType;
  
};

} // end namespace OpenGC

#endif

