/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavaidGeographicObject.h,v $

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

#if !defined(ogcNavaidGeographicObject_h)
#define ogcNavaidGeographicObject_h

#include "ogcGeographicObject.h"

namespace OpenGC
{

class NavaidGeographicObject : public GeographicObject
{
public:

	NavaidGeographicObject();
	virtual ~NavaidGeographicObject();
  
  /** Accessors for frequency */
  void SetFrequency(float freq) {m_Frequency = freq;}
  float GetFrequency() {return m_Frequency;}
  
  /** Accessors for navaid type */
  void SetNDBType() {m_NavaidType = 0;}
  void SetVORType() {m_NavaidType = 1;}
  void SetDMEType() {m_NavaidType = 2;}
  
  unsigned int GetType() {return m_NavaidType;};

protected:

  /** Frequency of the navaid */
  float m_Frequency;
  
  /** Type of navaid */
  unsigned int m_NavaidType;

};

} // end namespace OpenGC

#endif
