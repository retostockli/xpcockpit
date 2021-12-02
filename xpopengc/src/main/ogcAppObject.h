/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcAppObject.h,v $

  Copyright (C) 2001-2 by:
    Original author:
      Damion Shelton
    Contributors (in alphabetical order):
      John Wojnaroski

  Last modification:
    Date:      $Date: 2004/10/14 19:27:34 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================*/

/**
 * AppObject is the functional piece of the OpenGC example program.
 * It is responsible for creating gauges and allocating/deallocating
 * the various objects used in OpenGC.
 */

#ifndef ogcAppObject_h
#define ogcAppObject_h

#include <iostream>
#include <fstream>
#include "ogcMessages.h"
#include "ogcFLTKRenderWindow.h"
#include "ogcFontManager.h"
#include "ogcNavDatabase.h"

namespace OpenGC
{

// This is so we don't have to use std:: for all of the fstream stuff
using namespace std;

class AppObject  
{
public:

  AppObject();
  virtual ~AppObject();

  /** Call using atexit() in main() to clean up memory alloc */
  void Cleanup();

  /** Setup and run the glass cockpit - enters message loop */
  int Go(char* programPath, char* iniFileName);

  /** The idle function used to repeatedly update the display */
  void IdleFunction();

  /**
   * Send a message to the render windows(s)
   * Also responsible for handling messages meant for the app
   */
  void DispatchMessage(Message message, void* data);
  
  /** Function for analyzing maximum possible frame rate */
  void CheckFrameRate();
  
  /** Main pieces of the application */
  FLTKRenderWindow* m_pRenderWindow;
  DataSource* m_pDataSource;
  FontManager* m_pFontManager;
  NavDatabase* m_pNavDatabase;

protected:

  /** Create a layout from an initialization file */
  bool DoFileInitialization(char* iniFile);

  /**
   * Called during file initialization after data source and
   * render window are specified
   */
  bool IntermediateInitialization();

  /** Create a gauge from an input file */
  void CreateGauge(char* name, float xPos, float yPos, float xScale, float yScale, int arg);

  /** Path to fonts */
  char m_FontPath[255];

  /** Path to X-Plane Installation (contains nav data) */
  char m_XPlanePath[255];

  /** Server IP Address */
  char m_ip_address[128];

  /** Server Port number */
  int m_port;

  /** whether to read from X-Plane's Custom Data path **/
  int m_customdata;

  /** whether to read from X-Plane's UDP WXR radar data **/
  int m_radardata;

  /** Path to Globe Project DEM Files */
  char m_DEMPath[255];

  /** Path to GSHHG Shoreline Files */
  char m_GSHHGPath[255];

  /**
   * We treat the file initialization process as a finite
   * state machine, where the current state is given below
   */
  enum InitStateTable
  {
    DATASOURCE_STATE,
    RENDERWINDOW_STATE,
    NAV_PATH_STATE,
    FONT_PATH_STATE,
    GENERIC_STATE,
    SERVER_IP_STATE,
    SERVER_PORT_STATE,
  };

  /** The current intialization state: 0: prior to initialization, 1: after initialization, 2: after first loop  */
  int m_InitState;

  /** Whether or not to calculate frame rate */
  bool m_FrameTest;

};

} // end namespace OpenGC

#endif
