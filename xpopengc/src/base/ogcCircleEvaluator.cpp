/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcCircleEvaluator.cpp,v $

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

#include <stdio.h>
#include <math.h>
#include "ogcGLHeaders.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

CircleEvaluator
::CircleEvaluator()
{
  m_XOrigin = 0;
  m_YOrigin = 0;

  m_StartArcDegrees = 0;
  m_EndArcDegrees = 0;

  m_DegreesPerPoint = 10;

  m_Radius = 1;

  m_Nper100 = 0;
  m_Ratio = 0.5;
}

CircleEvaluator
::~CircleEvaluator()
{

}

void
CircleEvaluator
::SetOrigin(double x, double y)
{
  m_XOrigin = x;
  m_YOrigin = y;
}

void
CircleEvaluator
::SetArcStartEnd(double startArc, double endArc)
{
  m_StartArcDegrees = startArc;
  m_EndArcDegrees = endArc;
}

void
CircleEvaluator
::SetDegreesPerPoint(double degreesPerPoint)
{
  m_DegreesPerPoint = degreesPerPoint;
}

void
CircleEvaluator
::SetDashed(int nper100, float ratio)
{
  m_Nper100 = nper100;
  m_Ratio = ratio;
}

void
CircleEvaluator
::Evaluate()
{
  double x;
  double y;
  double pi = 3.14159265;

  // We parametrically evaluate the circle
  // x = sin(t)
  // y = cos(t)
  // t goes from 0 to 2pi
  // 0 degrees = 0rad, 90 degrees = pi/2rad, etc.
  
  double startRad = m_StartArcDegrees / 180 * pi;
  double endRad = m_EndArcDegrees / 180 * pi;
  double radPerPoint = m_DegreesPerPoint / 180 * pi;
  if (startRad > endRad)
    endRad += 2*pi;
  double currentRad = startRad;

  if (m_Nper100 > 0) {
  //  if (0) {
    glEnd(); /* stop glRendering since we start from scratch here */
    /* dashed circle */
    double perimeter = 2.0 * m_Radius * pi;
    double dashPerCircle = perimeter / 100.0 * (float) m_Nper100;
    double radPerDash = 2.0 * pi / dashPerCircle;
    //printf("%f %f %f \n",perimeter,dashPerCircle,radPerDash);
    do {
      double dashEndRad = currentRad + radPerDash * m_Ratio;
      glBegin(GL_LINE_STRIP);
      do {
	x = m_Radius*sin(currentRad) + m_XOrigin;
	y = m_Radius*cos(currentRad) + m_YOrigin;
	glVertex2d(x,y);
	currentRad += radPerPoint;
      } while (currentRad < dashEndRad);
      x = m_Radius*sin(dashEndRad) + m_XOrigin;
      y = m_Radius*cos(dashEndRad) + m_YOrigin;
      glVertex2d(x,y);
      glEnd();
      currentRad += radPerDash * (1.-m_Ratio); /* jump to next dash start */
    } while (currentRad < endRad);
    glBegin(GL_LINE_STRIP); /* restart glRendering for compatibility with calling circle routine */
  } else {
    /* Regular Circle with full line */
    do
      {
	x = m_Radius*sin(currentRad) + m_XOrigin;
	y = m_Radius*cos(currentRad) + m_YOrigin;
	glVertex2d(x,y);
	currentRad += radPerPoint;
      } while(currentRad < endRad);
    x = m_Radius*sin(endRad) + m_XOrigin;
    y = m_Radius*cos(endRad) + m_YOrigin;
    glVertex2d(x,y);
  }
}

void
CircleEvaluator
::SetRadius(double radius)
{
  if (radius > 0)
    m_Radius = radius;
  else
    m_Radius = 1;
}

  /* Function to draw a dashed line since in OpenGL ES the gl_line_stipple function is not available */
  void drawDashedLine(float x0, float y0, float x1, float y1, float nper100, float ratio) {

    float length = sqrt(pow(x1-x0,2) + pow(y1-y0,2));
    int n = (int) (length/100. * (float) nper100);
    for (int i=0;i<n;i++) {
      glBegin(GL_LINES);
      glVertex2f(x0+(x1-x0)*(float)i/(float)n,y0+(y1-y0)*(float)i/(float)n);
      glVertex2f(x0+(x1-x0)*((float)i+ratio)/(float)n,y0+(y1-y0)*((float)i+ratio)/(float)n);
      glEnd();
    }
  }

  /* Function to calculate the intersection of a circle with radius R centered at 0,0
     with a line defined by two points x1,y1 and x2,y2 */
  /* The result is either 0, 1 or 2 intersections */
  int calcLineCircleIntersect( float x1, float y1, float x2, float y2, float R,
			       float *ix1, float *iy1, float *ix2, float *iy2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float a = dx*dx + dy*dy;
    float b = 2 * (x1*dx + y1*dy);
    float c = x1*x1 + y1*y1 - R*R;
    float D = b*b - 4*a*c;

    if (D < 0) {
        // No intersection
        return 0;
    }

    float sqrtD = sqrt(D);
    float t1 = (-b + sqrtD) / (2*a);
    float t2 = (-b - sqrtD) / (2*a);

    *ix1 = x1 + t1*dx;
    *iy1 = y1 + t1*dy;

    *ix2 = x1 + t2*dx;
    *iy2 = y1 + t2*dy;

    return (D > 0) ? 2 : 1;  // 2 intersections or 1 intersection (tangent)
  }

} // end namespace OpenGC
