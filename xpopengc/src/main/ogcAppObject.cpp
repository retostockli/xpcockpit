/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2001-2021 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Michael DeFeyter
  John Wojnaroski
  Reto Stockli
  
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

//--------GL Stuff---------
#include "ogcGLHeaders.h"

//---------Base Stuff----------
#include <FL/Fl.H>
extern "C" {
#include "dictionary.h"
#include "iniparser.h"
#include "wxrdata.h"
}
#include "ogcAppObject.h"
#include "ogcRenderObject.h"

//--------Data Sources---------
// Not all data sources work on all platforms,
// but in the worst case all will at least default
// to a basic data source
#include "ogcXPlaneDataSource.h"

//-----------Gauges------------
#include "WXR/ogcWXR.h"
#include "Dummy/ogcDummyGauge.h"
#include "BasicClock/ogcBasicClock.h"
#include "A320/A320PFD/ogcA320PFD.h"
#include "A320/A320ND/ogcA320ND.h"
#include "A320/A320EWD/ogcA320EWD.h"
#include "A320/A320SD/ogcA320SD.h"
#include "A320/A320MCDU/ogcA320MCDU.h"
#include "A320/A320Misc/A320Clock/ogcA320Clock.h"
#include "A320/A320Misc/A320BrkTripleInd/ogcA320BrkTripleInd.h"
#include "A320/A320Stby/A320StbyAlt/ogcA320StbyAlt.h"
#include "A320/A320Stby/A320StbyASI/ogcA320StbyASI.h"
#include "A320/A320Stby/A320StbyAtt/ogcA320StbyAtt.h"
#include "A320/A320Stby/A320StbyISIS/ogcA320StbyISIS.h"
#include "A320/A320Stby/A320StbyRMI/ogcA320StbyRMI.h"
#include "B737/B737PFD/ogcB737PFD.h"
#include "B737/B737PFDSA/ogcB737PFDSA.h"
#include "B737/B737EICAS/ogcB737EICAS.h"
#include "B737/B737NAV/ogcB737NAV.h"
#include "B737/B737FMC/ogcB737FMC.h"
#include "B737/B737MIP/ogcB737MIP.h"
#include "B737/B737CLOCK/ogcB737Clock.h"
#include "B737/B737ISFD/ogcB737ISFD.h"
#include "B737/B737RMI/ogcB737RMI.h"

//----------NonFunctional--------
/*
  #include "ogcB737AnalogFlaps.h"
  #include "ogcB737VerticalSpeedDigital.h"
  #include "ogcKeypad.h"
  #include "ogcNavTestGauge.h"
  #include "ogcNavTestGauge.h"
*/

extern int verbosity;

extern "C" char clientname[100];

namespace OpenGC
{

  AppObject::AppObject()
  {
    /*if (verbosity > 0)*/ printf("AppObject - constructing\n");
  
    // Make sure all the pointers are nulled out
    m_pFontManager = 0;
    m_pRenderWindow = 0;
    m_pDataSource = 0;
    m_pNavDatabase = 0;
    verbosity = 0;

    m_InitState = 0;
    
    // Run a frame-rate test when loading OpenGC?
    m_FrameTest = false;

    /*if (verbosity > 1)*/ printf("AppObject - constructed\n");
  }

  AppObject::~AppObject()
  {
    // The way OpenGC is laid out, the destructor for the App isn't actually called,
    // but if it did, we'd want to do a little cleanup
    this->Cleanup();
  }
  
  int AppObject::Go(char* programPath, char* iniFileName)
  {
    char iniFile[255];
    char cwd[200];
    char *pch;
    
    /* check if we are in the source code directory or in the binary installation path */
    if (strncmp("/",programPath,1)==0) {
      /* we have full path starting code */
      printf("Starting with full program path \n");
      /* remove program name from path */
#ifdef WIN
      pch = strrchr(programPath,'\\');
#else
      pch = strrchr(programPath,'/');
#endif
      *pch = '\0';
      printf("%s\n",programPath);
#ifdef WIN
      pch = strstr (programPath,"\\bin");
#else
      pch = strstr (programPath,"/bin");
#endif
      if (pch == NULL) {
	snprintf (iniFile, sizeof(iniFile), "%s/../../inidata/%s.ini",programPath,iniFileName);
	snprintf (m_FontPath, sizeof(m_FontPath), "%s/../../fonts/",programPath);
      } else {
	snprintf (iniFile, sizeof(iniFile), "%s/../share/xpopengc/%s.ini",programPath,iniFileName);
	snprintf (m_FontPath, sizeof(m_FontPath), "%s/../share/xpopengc/",programPath);
      }
    } else {
      /* assume we start from source or bin dir */
      printf("Starting from bin/ or src/ directory \n");
      if (getcwd(cwd, sizeof(cwd)) == NULL) return -2;
#ifdef WIN
      pch = strstr (cwd,"\\bin");
#else
      pch = strstr (cwd,"/bin");
#endif
      if (pch == NULL) {
	snprintf (iniFile, sizeof(iniFile), "../../inidata/%s.ini", iniFileName);
	snprintf (m_FontPath, sizeof(m_FontPath), "../../fonts/");
      } else {
	snprintf (iniFile, sizeof(iniFile), "../share/xpopengc/%s.ini", iniFileName);
	snprintf (m_FontPath, sizeof(m_FontPath), "../share/xpopengc/");
      }
    }

    // Process the initialization file, if this fails, abort!
    if( !this->DoFileInitialization(iniFile) )
      {
	return 0;
      }

    printf ("AppObject - Initialization complete. Starting main loop.\n");
    if (verbosity > 0) printf ("====================\n");

    m_InitState = 1;
  
    // First, check the frame rate if the user wants to
    if(m_FrameTest)
      this->CheckFrameRate();

    // Now it's time to enter the event loop
    Fl::run();

    // This return value doesn't mean anything
    return 1;
  }

  void
  AppObject
  ::IdleFunction()
  {
    
    // Every time we loop we grab some new data and re-render the window
    m_pDataSource->OnIdle();
    if ((numreceived > 0) || (m_InitState <= 1) || (wxr_newdata == 1)) {
      //printf("%i %i \n",numreceived,m_InitState);
      m_pRenderWindow->redraw();
      Fl::flush();
    }
    if (m_InitState == 1) m_InitState++;
  }

  bool AppObject::IntermediateInitialization()
  {
    // Initialize the static members of RenderObject
    DummyGauge dummyRenderObject;
    dummyRenderObject.SetDataSource(m_pDataSource);
    dummyRenderObject.SetFontManager(m_pFontManager);
    dummyRenderObject.SetNavDatabase(m_pNavDatabase);
  
    if (verbosity > 1) printf("AppObject - Finished creating the dummy render object\n");

    return true;
  }

  void AppObject::Cleanup()
  {  
    if (verbosity > 0) cout << "AppObject - Cleaning up\n";
  
    if(m_pDataSource != 0)
      {
	if (verbosity > 1) cout << "AppObject - Deleting Data Source\n";
	delete m_pDataSource;
	m_pDataSource = 0;
      }
  
    if(m_pRenderWindow != 0)
      {
	if (verbosity > 1) cout << "AppObject - Deleting render window\n";
	delete m_pRenderWindow;
	m_pRenderWindow = 0;
      }
  
    if(m_pFontManager != 0)
      {
	if (verbosity > 1) cout << "AppObject - Deleting font manager\n";
	delete m_pFontManager;
	m_pFontManager = 0;
      }
  
    if (m_pDataSource != 0)
      {
	if (verbosity > 1) cout << "AppObject - Deleting data source\n";
	delete m_pDataSource;
	m_pDataSource = 0;
      }
  
    if (m_pNavDatabase != 0)
      {
	if (verbosity > 1) cout << "AppObject - Deleting nav database\n";
	delete m_pNavDatabase;
	m_pNavDatabase = 0;
      }
  
    if (verbosity > 1) cout << "AppObject - Finished memory cleanup\n";
  }

  bool AppObject::DoFileInitialization(char* iniFile)
  {
    char temp_string[50];
    dictionary *ini;
    int default_verbosity = 0;
    char default_server_ip[] = "127.0.0.1";
    int default_server_port = 8091;
    char default_data_source[] = "X-Plane";
    char default_xplane_path[] = "";
    char default_client_name[] = "xpopengc";
    int default_customdata = 0; // do not read from X-Plane's "Custom Data" directory by default
    int default_radardata = 0; // do not read from X-Plane's UDP radar data by default
    char default_dem_path[] = "";
    char default_gshhg_path[] = "";

    printf("AppObject - Starting initialization with %s\n", iniFile);

    // parse the *ogc.ini file
    ini = iniparser_load(iniFile);
    if (ini == NULL) {
      printf("AppObject - Error opening initialization file %s\n", iniFile);
      return false;
    } else {
      // verbosity: 0: no debug, 1: some debug, 2: lots of debug
      verbosity = iniparser_getint(ini,"General:Verbosity", default_verbosity); 

      /* client name for the server */
      strncpy(clientname,iniparser_getstring(ini,"General:Name", default_client_name),sizeof(clientname));

      // path to X-Plane installation
      strcpy(m_XPlanePath,iniparser_getstring(ini,"General:XPlanePath",default_xplane_path));

      // whether to use X-Plane's Custom Data path
      m_customdata = iniparser_getint(ini,"General:CustomData", default_customdata);

      // whether to use X-Plane's UDP-based WXR radar data
      // 0: do not receive WXR data
      // 1: turn on control pad under network and put IP address of this computer here
      // 2: use regular UDP data stream of x-plane
      m_radardata = iniparser_getint(ini,"General:RadarData", default_radardata);

      // path to GLOBE DEM Files for Terrain Rendering in NAV Display. Will also be needed for VSI
      strcpy(m_DEMPath,iniparser_getstring(ini,"General:DEMPath",default_dem_path));

      // path to GSHHG Files for shoreline/lake Rendering in NAV Display.
      strcpy(m_GSHHGPath,iniparser_getstring(ini,"General:GSHHGPath",default_gshhg_path));

      // Initialize the nav database
      if (strcmp(m_XPlanePath,"")) {
	if (verbosity > 0) printf("AppObject - Initializing the navigation database in %s\n", m_XPlanePath);
	m_pNavDatabase = new NavDatabase;
	if (!m_pNavDatabase->InitDatabase(m_XPlanePath,m_DEMPath,m_GSHHGPath,m_customdata)) return false;
      } else {
	if (verbosity > 0) printf("AppObject - Not Loading navigation database since X-Plane path is empty.\n");
      }
    
      // Set up font manager
      m_pFontManager = new FontManager();

      // Specify smoothed fonts
      m_pFontManager->SetSmooth(true);
      //m_pFontManager->SetSmooth(false);
    
      if (verbosity > 0) printf("AppObject - Font path %s\n", m_FontPath);
      m_pFontManager->SetFontPath(m_FontPath);
    
      // Set up the network data
      strcpy(m_ip_address,iniparser_getstring(ini,"network:ServerIP", default_server_ip));
      m_port = iniparser_getint(ini,"network:ServerPort", default_server_port);

      // Set up the data source
      if (verbosity > 2) printf("AppObject - Setting up the Data Source\n");
      strcpy(temp_string,iniparser_getstring(ini,"network:DataSource", default_data_source));
      // Currently only the X-Plane Data Source is supported
      if( strcmp(temp_string, "X-Plane")==0 )
	m_pDataSource = new XPlaneDataSource();

      // Now initialize the data source
      m_pDataSource->define_server(m_port,m_ip_address,m_radardata);

      // Setup the render window
      int initX = iniparser_getint(ini,"Window:xpos", 0);
      int initY= iniparser_getint(ini,"Window:ypos", 0);
      int width = iniparser_getint(ini,"Window:width", 1000);
      int height = iniparser_getint(ini,"Window:height", 750);
      int decoration = iniparser_getint(ini, "Window:undecorated", 0);
      int frameratetest = iniparser_getint(ini,"Window:frametest", 0);
      if (verbosity > 1) printf("AppObject - ready to create render window\n");

      // Create the new render window
      m_pRenderWindow = new FLTKRenderWindow(initX, initY, width, height, "The Open Glass Cockpit Project");
      m_pRenderWindow->resizable(m_pRenderWindow);
      m_pRenderWindow->mode(FL_RGB | FL_DOUBLE | FL_MULTISAMPLE);
      //m_pRenderWindow->mode(FL_RGB | FL_MULTISAMPLE);
      //m_pRenderWindow->mode(FL_RGB | FL_DOUBLE);

      
      m_pRenderWindow->border(!decoration);
      //==================================================================================================//
      //  Temporary solution to produce a borderless (undecorated) window                               ==//
      //==================================================================================================//
      m_pRenderWindow->border(!decoration);         // <-- TODO: make this into an ini parameter!  ==//
      //==================================================================================================//
      
      // Are we doing a frame rate test?
      m_FrameTest = (bool)frameratetest;

      if (verbosity > 0) printf("AppObject - ready to show render window\n");

      // We need to go ahead and show the window so that an OpenGL device context exists
      // once we start loading fonts
      m_pRenderWindow->show();

      // Now we can do the intermediate init stuff
      this->IntermediateInitialization();

      // Force the render window to update to obtain a valid device context
      m_pRenderWindow->redraw();
      Fl::flush();

      // Ready with the window; now set up the gauge(s)
      int gaugeDefined = 0;
      int i = 1;
      char gaugeKey[50];
      char *gaugeName;
      float xpos, ypos, xscale, yscale;
      int gaugeArg;
    
      while (0==0) {
	snprintf (gaugeKey, sizeof(gaugeKey), "Gauge:%i:Name",i);
	gaugeName = iniparser_getstring (ini, gaugeKey, (char *) "");
	if (strcmp (gaugeName, "") == 0) break;
	if (verbosity > 0) printf ("Gauge %i: Name, value: %s\n", i, gaugeName);
	snprintf (gaugeKey, sizeof(gaugeKey), "Gauge:%i:xPos", i);
	xpos = iniparser_getdouble (ini, gaugeKey, 0);
	if (verbosity > 0) printf ("Gauge %i: xpos, value: %f\n", i, xpos);
	snprintf (gaugeKey, sizeof(gaugeKey), "Gauge:%i:yPos", i);
	ypos = iniparser_getdouble (ini, gaugeKey, 0);
	if (verbosity > 0) printf ("Gauge %i: ypos, value: %f\n", i, ypos);
	snprintf (gaugeKey, sizeof(gaugeKey), "Gauge:%i:xscale", i);
	xscale = iniparser_getdouble (ini, gaugeKey, 0);
	if (verbosity > 0) printf ("Gauge %i: xscale, value: %f\n", i, xscale);
	snprintf (gaugeKey, sizeof(gaugeKey), "Gauge:%i:yscale", i);
	yscale = iniparser_getdouble (ini, gaugeKey, 0);
	if (verbosity > 0) printf ("Gauge %i: yscale, value: %f\n", i, yscale);
	snprintf (gaugeKey, sizeof(gaugeKey), "Gauge:%i:arg", i);
	gaugeArg = iniparser_getint (ini, gaugeKey, 0);
	if (verbosity > 0) printf ("Gauge %i: arg, value: %i\n", i, gaugeArg);
	gaugeDefined = 1;
	i++;
	CreateGauge (gaugeName, xpos, ypos, xscale, yscale, gaugeArg);
      }
      if (! gaugeDefined) {
	printf ("No gauges defined!\n");
	iniparser_freedict(ini);
	return false;
      }
      if (verbosity > 0) printf ("%i gauges defined\n", i - 1);
      iniparser_freedict(ini);
      return true;
    }

    return true;
  }

  // Create a gauge
  void AppObject::CreateGauge(char* name, float xPos, float yPos, float xScale, float yScale, int arg)
  {
    // A pointer to reference the gauge we're creating
    Gauge* pGauge = 0;

    if (verbosity > 0) printf ("AppObject - Gauge %s, xp %f, yp %f, xs %f, ys %f, (arg %i)\n", name, xPos, yPos, xScale, yScale, arg);

    if (strcmp(name, "BasicClock")==0) pGauge = new BasicClock();
    else if (strcmp(name, "WXR")==0) pGauge = new WXR();
    else if (strcmp(name, "B737PFD")==0) pGauge = new B737PFD();
    else if (strcmp(name, "B737PFDSA")==0) pGauge = new B737PFDSA();
    else if (strcmp(name, "B737EICAS")==0) pGauge = new B737EICAS();
    else if (strcmp(name, "B737NAV")==0) pGauge = new B737NAV();
    else if (strcmp(name, "B737FMC")==0) pGauge = new B737FMC(arg);
    else if (strcmp(name, "B737MIP")==0) pGauge = new B737MIP();
    else if (strcmp(name, "B737CLOCK")==0) pGauge = new B737Clock();
    else if (strcmp(name, "B737ISFD")==0) pGauge = new B737ISFD();
    else if (strcmp(name, "B737RMI")==0) pGauge = new B737RMI();
    /*
      else if (strcmp(name, "B737AnalogFlaps")==0) pGauge = new B737AnalogFlaps();
      else if (strcmp(name, "B737VerticalSpeedDigital")==0) pGauge = new B737VerticalSpeedDigital();
      else if (strcmp(name, "Keypad")==0) pGauge = new Keypad();
      else if (strcmp(name, "NavTestGauge")==0) pGauge = new NavTestGauge();
    */
    else if (strcmp(name, "A320PFD")==0) pGauge = new A320PFD();
    else if (strcmp(name, "A320ND")==0) pGauge = new A320ND();
    else if (strcmp(name, "A320EWD")==0) pGauge = new A320EWD();
    else if (strcmp(name, "A320SD")==0) pGauge = new A320SD();
    else if (strcmp(name, "A320MCDU")==0) pGauge = new A320MCDU();
    else if (strcmp(name, "A320Clock")==0) pGauge = new A320Clock();
    else if (strcmp(name, "A320BrkTripleInd")==0) pGauge = new A320BrkTripleInd();
    else if (strcmp(name, "A320StbyAlt")==0) pGauge = new A320StbyAlt();
    else if (strcmp(name, "A320StbyASI")==0) pGauge = new A320StbyASI();
    else if (strcmp(name, "A320StbyAtt")==0) pGauge = new A320StbyAtt();
    else if (strcmp(name, "A320StbyISIS")==0) pGauge = new A320StbyISIS();
    else if (strcmp(name, "A320StbyRMI")==0) pGauge = new A320StbyRMI();
    else {
      printf("Gauge %s not defined in CreateGauge. Aborting\n",name);
      exit(-1);
    }
    pGauge->SetUnitsPerPixel(m_pRenderWindow->GetUnitsPerPixel());
    pGauge->SetPosition(xPos, yPos);
    pGauge->SetScale(xScale, yScale);
    pGauge->SetArg(arg);
    m_pRenderWindow->AddGauge(pGauge);
  }

  void AppObject::CheckFrameRate()
  {/*
     double reps = 200;
     
     m_pDataSource->ILS_Glideslope_Alive = true;
     m_pDataSource->Nav1_Valid = true;
     
     for(double i = 0; i < reps; i += 1.0)
     {
     // Pitch and bank bounce around
     m_pDataSource->Pitch = 15 * sin( i * 3.14 / 180 );
     m_pDataSource->Bank = 30 * sin( 2 * i * 3.14 / 180 );
     
     // ILS needles slew
     m_pDataSource->Nav1_Localizer_Needle =  sin( i * 3.14 / 90 );
     m_pDataSource->Nav1_Glideslope_Needle = sin( i * 3.14 / 90 );
     
     // Airspeed ramps from 0 to 250 and back down
     m_pDataSource->IAS = 250 * sin( (i / reps) * 3.14 );
     
     // Altitude ramps from 7500 to 12500 and back down
     m_pDataSource->Barometric_Alt_Feet = 7500 + 5000 * sin( (i / reps) * 3.14 );
     
     // Position slews around (Pittsburgh area)
     m_pDataSource->Longitude = -80.0 + 10 * sin( (i / reps) * 3.14 );
     m_pDataSource->Latitude = 40.27 + 10 * sin( (i / reps) * 3.14 );
     
     // Force the render window to update
     m_pRenderWindow->redraw();
     Fl::flush();
     }
     
     // Reset the data source
     m_pDataSource->InitializeData();
     
     m_pDataSource->ILS_Glideslope_Alive = false;
     m_pDataSource->Nav1_Valid = false;
     
     m_FrameTest = false;
   */
  }
  
} // end namespace OpenGC
