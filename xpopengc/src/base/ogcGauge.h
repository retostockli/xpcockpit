/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcGauge.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:53 $
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
 * The base class for all gauges in the ogc object model.
 * Gauge's define a client rendering space and an associated
 * orthographic projection. Use of a client rendering space, in
 * the form of a glRenderWindow(), allows for greatly simplified
 * clipping, since objects drawn outside of the render window
 * are not displayed. The client window calculations are for the
 * most part used to manage the placement of GaugeComponents (i.e.
 * the intent is to use GaugeComponents for drawing, rather than
 * drawing within a Gauge, in order to promote reusability of
 * code.
 *
 * Gauge's contain one or more GaugeComponents, which also define
 * client rendering space and a viewport transform.
 */

#ifndef ogcGauge_h
#define ogcGauge_h

#include <list>
#include "ogcOrderedPair.h"
#include "ogcFontManager.h"
#include "ogcRenderObject.h"
#include "ogcGaugeComponent.h"

namespace OpenGC
{

class Gauge: public RenderObject
{
public:

  /** Iterator type for accessing the list of components */
  typedef std::list<GaugeComponent*>::iterator ComponentIteratorType;
 
  Gauge();
  virtual ~Gauge();

  /** Overloaded render method */
  void Render();

  /** Add a gauge component */
  void AddGaugeComponent(GaugeComponent* pComponent);

  /** Overloaded method for setting the monitor calibration */
  void SetUnitsPerPixel(double unitsPerPixel);

  /** Set the x and y scale of the gauge (and member components) */
  void SetScale(double xScale, double yScale);

  /** Provide Argument to Gauge **/
  void SetArg(int arg);

  int GetArg() { return m_Arg; }
 
  /** Recalculates placement of the gauge in the window */
  void RecalcWindowPlacement();

  /** Resets the gauge coordinate system before and after rendering components */
  void ResetGaugeCoordinateSystem();

  /** Return true if the click is inside the gauge
      If true, tests gauge components prior to returning */
  bool ClickTest(int button, int state, int x, int y);

  /** Determine whether or not to draw the gauge outline */
  void SetGaugeOutline(bool outline){m_DrawGaugeOutline = outline;};

  /** Called when a message is triggered somewhere
    * To provide specific functionality in OnMessage, add
    * code in a derived gauge and then call the base class
    * OnMessage to dispatch the message to member GaugeComponents
    */
  void OnMessage(Message message, void * data);

protected:

  /** Draw the gauge outline */
  void DrawGaugeOutline();
 
  /** All of the guage components */
  std::list<GaugeComponent*> m_GaugeComponentList;

  /** The number of gauge components in this gauge */
  int m_NumGaugeComponents;

  /** Whether or not to draw a blue line around the gauge */
  bool m_DrawGaugeOutline;
};

} // end namespace OpenGC

#endif
