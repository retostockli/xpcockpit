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

  /** Defines dashed circle */
  void SetDashed(int nper100, float ratio);

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

  /** How many dashes per 100 pixels diameter (set to 0 for no dashes) **/
  int m_Nper100;

  /** Ratio of dashed versus total space **/
  float m_Ratio;

};

  /* Function to draw a dashed line since in OpenGL ES the gl_line_stipple function is not available */
  void drawDashedLine(float x0, float y0, float x1, float y1, float nper100, float ratio);

  /* Function to calculate the intersection of a circle with radius R centered at 0,0
     with a line defined by two points x1,y1 and x2,y2 */
  int calcLineCircleIntersect( float x1, float y1, float x2, float y2, float R,
			       float *ix1, float *iy1, float *ix2, float *iy2);

} // end namespace OpenGC

#endif
