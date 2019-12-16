/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcRenderObject.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:27:54 $
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
 * RenderObjects are the base class for any class which draws in
 * RenderWindow and defines a client rendering space. Currently
 * this includes Gauges and GaugeComponents.
 */

#ifndef ogcRenderObject_h
#define ogcRenderObject_h

#include "ogcObject.h"
#include "ogcOrderedPair.h"
#include "ogcFontManager.h"
#include "ogcNavDatabase.h"
#include "ogcGLHeaders.h"
#include "ogcMessages.h"

namespace OpenGC
{

// Forward declaration of data source
class DataSource;

class RenderObject : public Object
{
public:
 
  RenderObject();
  virtual ~RenderObject();

  /** Set the data source */
  void SetDataSource(DataSource* pDataSource) { m_pDataSource = pDataSource; }

  /** Set the font manager */
  void SetFontManager(FontManager* pFontManager) { m_pFontManager = pFontManager; }
  
  /** Set the nav database */
  void SetNavDatabase(NavDatabase* pNavDatabase) { m_pNavDatabase = pNavDatabase; }

  /** Set the monitor calibration */
  void SetUnitsPerPixel(double unitsPerPixel);

  /** All child classes must have a Render() method */
  virtual void Render() = 0;

  /** Set the scale of the gauge */
  void SetScale(double xScale, double yScale);

  /** Set the size of the gauge */
  void SetSize(double xSize, double ySize);

  /** Set the position of the gauge in the render window in physical units */
  void SetPosition(double xPos, double yPos);

  /** Provide and retrieve an optional argument to the gauge **/

  void SetArg(int arg);

  int GetArg() { return m_Arg; }


  /** Get the physical position */
  OrderedPair<double> GetPhysicalPosition() { return m_PhysicalPosition; }

  /** Set the parent render object (in order to cascade transformations) */
  void SetParentRenderObject(RenderObject* pObject) { m_pParentRenderObject = pObject; }

  /** Called by framework when a mouse click occurs (x/y in pixel coordinates) */
  void HandleMouseButton(int button, int state, int x, int y);

  /** Returns true if the click applies to this object (x/y in pixel coordiantes) */
  virtual bool ClickTest(int button, int state, int x, int y) = 0;

  /** Called when a mouse "down" event occurs (x/y in physical coordinates) */
  virtual void OnMouseDown(int button, double physicalX, double physicalY);

  /** Called when a mouse "up" event occurs (x/y in physical coordinates) */
  virtual void OnMouseUp(int button, double physicalX, double physicalY);

  /** Returns the fontmanager */
  FontManager* GetFontManager() { return m_pFontManager;}

protected:

  /** Pointer to data source */
  static DataSource* m_pDataSource;

  /** Pointer to font manager */
  static FontManager* m_pFontManager;
  
  /** Pointer to nav database */
  static NavDatabase* m_pNavDatabase;

  /** Set by the render window to describe pixel-realspace conversions */
  double m_UnitsPerPixel;

  /** Parent object, for cascading position information */
  RenderObject* m_pParentRenderObject;

  /** 1.0=normal scale, 0.5=half, 2=double, etc. */
  OrderedPair<double> m_Scale;

  /** Position in mm in the render window */
  OrderedPair<double> m_PhysicalPosition;

  /** Placement in render window in pixel units */
  OrderedPair<unsigned int> m_PixelPosition;

  /** Size in mm, defined by derived classes, NOT initialized */
  OrderedPair<double> m_PhysicalSize;

  /** Size in render window in pixel units */
  OrderedPair<unsigned int> m_PixelSize;

  /** Optional Argument to the Gauge **/
  int m_Arg;
  
  /** Rotation, in degrees left */
  int m_Rotation;
};

} // end namespace OpenGC

#endif
