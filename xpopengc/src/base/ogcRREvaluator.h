/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module: ogcRREvaluator.h

  Copyright (c) 2024 Reto Stockli

  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * ogcRREvaluator generates a rounded rectangle
 */

#ifndef ogcRREvaluator_h
#define ogcRREvaluator_h

namespace OpenGC
{

class RREvaluator  
{

public:

  RREvaluator();
  virtual ~RREvaluator();
  
  /** Sets the radius of the rounded corners in physical units */
  void SetRadius(double radius);
  
  /** Defines the "coarseness" of the rounded corners (degree spacing of GL_POINTS) */
  void SetDegreesPerPoint(double degreesPerPoint);
  
  /** Defines the center of the rounded rectangle in physical units */
  void SetOrigin(double x, double y);

  /** Defines the center of the rounded rectangle in physical units */
  void SetSize(double x, double y);

  /** Calls the GL code to evaluate the circle (generates points) */
  void Evaluate();

protected:

  /** The origin (center) of the circle */
  double m_XOrigin, m_YOrigin;

  /** The radius of the rounded corners */
  double m_Radius;

  /** How many degrees to move before generating a new point */
  double m_DegreesPerPoint;

  /** The size of the rounded rectangle */
  double m_dx, m_dy;

};

} // end namespace OpenGC

#endif
