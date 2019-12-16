/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavRadioDataContainer.h,v $

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

#ifndef ogcNavRadioDataContainer_h
#define ogcNavRadioDataContainer_h

#include "ogcDataContainer.h"

namespace OpenGC
{

class NavRadioDataContainer : public DataContainer
{

  /** Frequency that the nav radio is tuned to */
  DataReadWriteMacro(Frequency, double);

  /** Omni bearing select */
  DataReadWriteMaco(OBS, double);

  /** Radial being received, if VOR */
  DataReadMacro(Radial, double);

public:
  
  void InitializeData();

  NavRadioDataContainer();
  virtual ~NavRadioDataContainer();

protected:

};

} // end namespace OpenGC
#endif
