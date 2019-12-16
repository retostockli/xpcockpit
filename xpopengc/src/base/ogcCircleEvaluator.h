/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcCircleEvaluator.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:52 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * ogcCircleEvaluator generates a series of points which lie on the
 * perimeter of the circle. The circle is evaluated as an arc,
 * given a start and end point in degrees. Degrees are defined as 0=360 being
 * up, 90 degrees right, etc.
 */

#ifndef ogcCircleEvaluator_h
#define ogcCircleEvaluator_h

namespace OpenGC
{

class CircleEvaluator  
{

public:

  CircleEvaluator();
  virtual ~CircleEvaluator();
  
  /** Sets the radius of the circle in physical units */
  void SetRadius(double radius);
  
  /** Defines the "coarseness" of the circle (degree spacing of GL_POINTS) */
  void SetDegreesPerPoint(double degreesPerPoint);
  
  /** Defines the start and end of the arc in degrees */
  void SetArcStartEnd(double startArc, double endArc);

  /** Defines the center of the circle in physical units */
  void SetOrigin(double x, double y);

  /** Calls the GL code to evaluate the circle (generates points) */
  void Evaluate();

protected:

  /** The origin (center) of the circle */
  double m_XOrigin, m_YOrigin;

  /** The radius of the arc/circle */
  double m_Radius;

  /** The start and end of the arc in degrees */
  double m_StartArcDegrees, m_EndArcDegrees;

  /** How many degrees to move before generating a new point */
  double m_DegreesPerPoint;

};

} // end namespace OpenGC

#endif
