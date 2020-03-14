/*=============================================================================

  This is the ogcA320NDWidget.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Navigation Display Widget ===

  Created:
    Date:   2011-11-14
    Author: Hans Jansen
    last change: 2020-02-01
    (see ogcSkeletonGauge.cpp for more details)

  Copyright (C) 2011-2020 Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>. 

===============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================*/

#include <math.h>
#include "ogcGaugeComponent.h"
#include "ogcA320ND.h"
#include "ogcA320NDWidget.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320NDWidget::A320NDWidget () {
    if (verbosity > 1) printf ("A320NDWidget - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    if (verbosity > 0) printf ("A320NDWidget - constructed\n");
  }

  A320NDWidget::~A320NDWidget () {}

  // The variables for the subpages
  bool avionicsOn;
  int ndPage,        ndRange;
  int headingValid,  mapAvail,      oppsideMsg;
  int utcHours,      utcMinutes,    utcSeconds;
  int headingDev;
  int efis1SelPilot, efis2SelPilot;
  int ilsVDefVis,    ilsHDefVis;

  //  int *nav_shows_wxr;
  //  int *nav_shows_acf;
  int *nav_shows_apt;
  int *nav_shows_cst;
  int *nav_shows_fix;
  int *nav_shows_vor;
  int *nav_shows_ndb;

  float hdgMag,   hdgTrue,  gndSpd,   airSpd,   wndDirT, wndDirM, wndSpd, chronoCapt;
  float brgNav1,  brgNav2,  brgAdf1,  brgAdf2;
  float distNav1, distNav2, distAdf1, distAdf2;
  float crsNav1,  hdefNav1, vdefNav1, freqNav1;
  float crsIls,   hdefIls,  vdefIls,  freqIls;
  float tcasHdg,  tcasAlt,  tcasDist;
  unsigned char   *toWptId;
  float           wptDist, wptCrs;
  int             wptHrs, wptMins;

  int    fpNoWp, fpToWp;
  
  double acLat,   acLon;
  
  unsigned char *adf1_name, *adf2_name, *ils_name, *nav1_name, *nav2_name;
  unsigned char *fp_dept_name, *fp_dest_name, *fp_alt_name, *fp_wp_id;
  
  char buff[36]; int rotation, tmpHdg;
  float ndRanges[6][4] = { {2.5,   5, 7.5,  10},
                           {  5,  10,  15,  20},
                           { 10,  20,  30,  40},
                           { 20,  40,  60,  80},
                           { 40,  80, 120, 160},
                           { 80, 160, 240, 320} };
  float ndDist, ndBrg;
  int alpha = 0;
  int gps_prim_valid = 0; // assuming 0 = not visible, 1 = "GPS primary available", 2 = "GPS primary lost"

  double dtor = 0.0174533;   // degree to radian
  // double rtod = 57.2958;  //radian to degree
  // double mtok = 1.852;    // nautical mile to kilometer
  // double ktom = 0.539957; // kilometer to nautical mile

  void A320NDWidget::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    // define map center position
    int mapSize = m_PhysicalSize.x * 0.5;  // half of total component size (just for name simplification <- square gauge!)
    int mapCenterX = mapSize * 1.0, mapCenterY = mapSize *1.0; // defines map center (now resp. 250 and 250)
    int mapCircle  = mapSize * 0.64; // radius of the outer range circle (now 160)
    int mapCircle1 = mapSize * 0.32; // radius of the inner range circle (now  80)
    int mapRange;
    float symbolSize = mapSize * 0.04; // size of the symbols in the display

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    int *avionics_on = link_dataref_int ("sim/cockpit/electrical/avionics_on");
    if (*avionics_on != INT_MISS) avionicsOn = *avionics_on; else avionicsOn = 0;

    if ((!coldAndDark) && (avionicsOn)) {
      int *nd_page = link_dataref_int ("AirbusFBW/NDmodeCapt");
      if ((*nd_page != INT_MISS) && (*nd_page >= 0)) ndPage = *nd_page;
      int *nd_range = link_dataref_int ("AirbusFBW/NDrangeCapt");
      if ((*nd_range != INT_MISS) && (*nd_range >= 0)) ndRange = *nd_range;

      int *hdg_valid  = link_dataref_int ("AirbusFBW/CaptHDGValid");
      if (*hdg_valid != INT_MISS) headingValid = *hdg_valid; else headingValid = 0;
      int *map_avail = link_dataref_int ("AirbusFBW/CaptMAPAvail");
      if (*map_avail != INT_MISS) mapAvail = *map_avail; else mapAvail = 0;
      int *oppside_msg = link_dataref_int ("AirbusFBW/OppSideMessage");
      if (*oppside_msg != INT_MISS) oppsideMsg = *oppside_msg; else oppsideMsg = 0;

      // Aircraft position
      double *aircraft_lat = link_dataref_dbl ("sim/flightmodel/position/latitude",-5);
      if (*aircraft_lat != INT_MISS) acLat = *aircraft_lat; else acLat = 0;
      double *aircraft_lon = link_dataref_dbl ("sim/flightmodel/position/longitude",-5);
      if (*aircraft_lon != INT_MISS) acLon = *aircraft_lon; else acLon = 0;
      // Aircraft heading
      float *heading_mag = link_dataref_flt ("sim/flightmodel/position/magpsi",-1);
      if (*heading_mag != FLT_MISS) hdgMag = *heading_mag; else hdgMag = 0;
      float *heading_true = link_dataref_flt ("sim/flightmodel/position/psi",0);
      if (*heading_true != FLT_MISS) hdgTrue = *heading_true; else hdgTrue = 0;
      float *heading_mag_ap = link_dataref_flt ("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot",0);
//?      float *heading_mag_ap = link_dataref_flt ("AirbusFBW/APHDG_Capt",0);
      float *magnetic_variation = link_dataref_flt ("sim/flightmodel/position/magnetic_variation",-1);
      // heading deviation
      float *hdg_dev = link_dataref_flt ("AirbusFBW/Drift_Capt",0);
      if (*hdg_dev != FLT_MISS) headingDev = *hdg_dev; else headingDev = 0;

      float *ground_speed = link_dataref_flt ("sim/flightmodel/position/groundspeed",-1);
      if (*ground_speed != FLT_MISS) gndSpd = (*ground_speed * 1.943844); else gndSpd = 0; // (m/s -> kts)
      float *air_speed = link_dataref_flt ("sim/flightmodel/position/true_airspeed",-1);
      if (*air_speed != FLT_MISS) airSpd = (*air_speed * 1.943844); else airSpd = 0;
      float *wind_direction_true = link_dataref_flt ("sim/weather/wind_direction_degt",-1);
      if (*wind_direction_true != FLT_MISS) wndDirT = *wind_direction_true; else wndDirT = 0;
      float *wind_direction_mag = link_dataref_flt ("sim/cockpit2/gauges/indicators/wind_heading_deg_mag",-1);
      if (*wind_direction_mag != FLT_MISS) wndDirM = *wind_direction_mag; else wndDirM = 0;
      float *wind_speed = link_dataref_flt ("sim/cockpit2/gauges/indicators/wind_speed_kts",-1);
      if (*wind_speed != FLT_MISS) wndSpd = *wind_speed; else wndSpd = 0;
      int *efis1_sel_pilot = link_dataref_int ("sim/cockpit2/EFIS/EFIS_1_selection_pilot"); // 0=ADF, 1=off, 2=VOR
      if (*efis1_sel_pilot != FLT_MISS) efis1SelPilot = *efis1_sel_pilot; else efis1SelPilot = 0;
//      int *efis1_sel_copilot = link_dataref_int ("sim/cockpit2/EFIS/EFIS_1_selection_copilot");
//      if (*efis1_sel_copilot != FLT_MISS) efis1SelCopilot = *efis1_sel_pilot; else efis1SelCopilot = 0;
      int *efis2_sel_pilot = link_dataref_int ("sim/cockpit2/EFIS/EFIS_2_selection_pilot"); // 0=ADF, 1=off, 2=VOR
      if (*efis2_sel_pilot != FLT_MISS) efis2SelPilot = *efis2_sel_pilot; else efis2SelPilot = 0;
//      int *efis2_sel_copilot = link_dataref_int ("sim/cockpit2/EFIS/EFIS_2_selection_copilot");
//      if (*efis2_sel_copilot != FLT_MISS) efis2SelCopilot = *efis2_sel_pilot; else efis2SelCopilot = 0;

      nav_shows_cst = link_dataref_int ("AirbusFBW/NDShowCSTRCapt");
      nav_shows_fix = link_dataref_int ("AirbusFBW/NDShowWPTCapt");
      nav_shows_apt = link_dataref_int ("AirbusFBW/NDShowARPTCapt");
      nav_shows_vor = link_dataref_int ("AirbusFBW/NDShowVORDCapt");
      nav_shows_ndb = link_dataref_int ("AirbusFBW/NDShowNDBCapt");
//      nav_shows_acf = link_dataref_int ("sim/cockpit2/EFIS/EFIS_tcas_on");
//      nav_shows_wxr = link_dataref_int ("sim/cockpit2/EFIS/EFIS_weather_on");

      nav1_name = link_dataref_byte_arr ("sim/cockpit2/radios/indicators/nav1_nav_id",500,-1);
      nav2_name = link_dataref_byte_arr ("sim/cockpit2/radios/indicators/nav2_nav_id",500,-1);
      adf1_name = link_dataref_byte_arr ("sim/cockpit2/radios/indicators/adf1_nav_id",500,-1);
      adf2_name = link_dataref_byte_arr ("sim/cockpit2/radios/indicators/adf2_nav_id",500,-1);
      int *nav1_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/nav1_has_dme");
      int *nav2_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/nav2_has_dme");
      int *adf1_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/adf1_has_dme");
      int *adf2_has_dme = link_dataref_int ("sim/cockpit2/radios/indicators/adf2_has_dme");
      float *nav1_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/nav1_dme_distance_nm",-1);
      if (*nav1_dme_distance_nm != FLT_MISS) distNav1 = *nav1_dme_distance_nm; else distNav1 = 0;
      float *nav2_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/nav2_dme_distance_nm",-1);
      if (*nav2_dme_distance_nm != FLT_MISS) distNav2 = *nav2_dme_distance_nm; else distNav2 = 0;
      float *adf1_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/adf1_dme_distance_nm",-1);
      if (*adf1_dme_distance_nm != FLT_MISS) distAdf1 = *adf1_dme_distance_nm; else distAdf1 = 0;
      float *adf2_dme_distance_nm = link_dataref_flt ("sim/cockpit2/radios/indicators/adf2_dme_distance_nm",-1);
      if (*adf2_dme_distance_nm != FLT_MISS) distAdf2 = *adf2_dme_distance_nm; else distAdf2 = 0;
      float *nav1_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/nav1_bearing_deg_mag",-1);
      if (*nav1_bearing != FLT_MISS) brgNav1 = *nav1_bearing; else brgNav1 = 0;
      float *nav2_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/nav2_bearing_deg_mag",-1);
      if (*nav2_bearing != FLT_MISS) brgNav2 = *nav2_bearing; else brgNav2 = 0;
      float *adf1_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/adf1_bearing_deg_mag",-1);
      if (*adf1_bearing != FLT_MISS) brgAdf1 = *adf1_bearing; else brgAdf1 = 0;
      float *adf2_bearing = link_dataref_flt ("sim/cockpit2/radios/indicators/adf2_bearing_deg_mag",-1);
      if (*adf2_bearing != FLT_MISS) brgAdf2 = *adf2_bearing; else brgAdf2 = 0;
      float *nav1_course = link_dataref_flt ("sim/cockpit2/radios/actuators/nav1_course_deg_mag_pilot",-1);
      if (*nav1_course != FLT_MISS) crsNav1 = *nav1_course; else crsNav1 = 0;
      float *nav1_hdef = link_dataref_flt ("sim/cockpit2/radios/indicators/nav1_hdef_dots_pilot",-1);
      if (*nav1_hdef != FLT_MISS) hdefNav1 = *nav1_hdef; else hdefNav1 = 0;
      float *nav1_vdef = link_dataref_flt ("sim/cockpit2/radios/indicators/nav1_vdef_dots_pilot",-1);
      if (*nav1_vdef != FLT_MISS) vdefNav1 = *nav1_vdef; else vdefNav1 = 0;
      int *nav1_frequency = link_dataref_int ("sim/cockpit2/radios/actuators/nav1_frequency_hz");
      if (*nav1_frequency != INT_MISS) freqNav1 = *nav1_frequency / 100.; else freqNav1 = 0.0;
      float *chrono_nd_capt = link_dataref_flt ("AirbusFBW/ChronoTimeND1",-1);
      if (*chrono_nd_capt != FLT_MISS) chronoCapt = *chrono_nd_capt; else chronoCapt = 0;

      ils_name = link_dataref_byte_arr ("AirbusFBW/ILS1",8,-1);
      int *ils_vdef_shown = link_dataref_int ("AirbusFBW/GSonCapt");
      if (*ils_vdef_shown != INT_MISS) ilsVDefVis = *ils_vdef_shown; else ilsVDefVis = 0;
      int *ils_hdef_shown = link_dataref_int ("AirbusFBW/ILSonCapt");
      if (*ils_hdef_shown != INT_MISS) ilsHDefVis = *ils_hdef_shown; else ilsHDefVis = 0;
      int *ils_frequency = link_dataref_int ("qpac_airbus/pfdoutputs/general/ils_frequency");
      if (*ils_frequency != INT_MISS) freqIls = *ils_frequency / 100.; else freqIls = 0.0;
      float *ils_course = link_dataref_flt ("AirbusFBW/ILSCrs",-1);
      if (*ils_course != FLT_MISS) crsIls = *ils_course; else crsIls = 0;
//      float *ils_hdef = link_dataref_flt ("sim/cockpit2/radios/indicators/ils_hdef_dots_pilot",-1);
      float *ils_hdef = link_dataref_flt ("sim/cockpit2/radios/indicators/hsi_hdef_dots_pilot",-1);
      if (*ils_hdef != FLT_MISS) hdefIls = *ils_hdef; else hdefIls = 0;
//      float *ils_vdef = link_dataref_flt ("sim/cockpit2/radios/indicators/ils_vdef_dots_pilot",-1);
      float *ils_vdef = link_dataref_flt ("sim/cockpit2/radios/indicators/hsi_vdef_dots_pilot",-1);
      if (*ils_vdef != FLT_MISS) vdefIls = *ils_vdef; else vdefIls = 0;

      /* TCAS */
      float *tcas_heading =  link_dataref_flt_arr ("sim/cockpit2/tcas/indicators/relative_bearing_degs",20,-1,0);
      if (*tcas_heading != FLT_MISS) tcasHdg = *tcas_heading; else tcasHdg = 0;
      float *tcas_distance = link_dataref_flt_arr ("sim/cockpit2/tcas/indicators/relative_distance_mtrs",20,-1,1);
      if (*tcas_distance != FLT_MISS) tcasDist = *tcas_distance; else tcasDist = 0;
      float *tcas_altitude = link_dataref_flt_arr ("sim/cockpit2/tcas/indicators/relative_altitude_mtrs",20,-1,1);
      if (*tcas_altitude != FLT_MISS) tcasAlt = *tcas_altitude; else tcasAlt = 0;
      //   double *x1 = link_dataref_dbl ("sim/multiplayer/position/plane1_x",1);
      //   double *y1 = link_dataref_dbl ("sim/multiplayer/position/plane1_y",1);
      //   double *z1 = link_dataref_dbl ("sim/multiplayer/position/plane1_z",1);

      toWptId = link_dataref_byte_arr ("AirbusFBW/WPT_ID",8,-1);
      float *wpt_dist = link_dataref_flt ("AirbusFBW/WPT_Dist",-1);
      if (*wpt_dist != FLT_MISS) wptDist = *wpt_dist; else wptDist = 0;
      float *wpt_crs = link_dataref_flt ("AirbusFBW/WPT_Crs",-1);
      if (*wpt_crs != FLT_MISS) wptCrs = *wpt_crs; else wptCrs = 0;
      int *wpt_hours = link_dataref_int ("AirbusFBW/WPT_Hours");
      if (*wpt_hours != FLT_MISS) wptHrs = *wpt_hours; else wptHrs = 0;
      int *wpt_minutes = link_dataref_int ("AirbusFBW/WPT_Minutes");
      if (*wpt_minutes != FLT_MISS) wptMins = *wpt_minutes; else wptMins = 0;

/*
      // Flightplan data
      fp_dept_name = link_dataref_byte_arr ("qpac_airbus/flightplan/departure_icao",5,-1);
      fp_dest_name = link_dataref_byte_arr ("qpac_airbus/flightplan/destination_icao",5,-1);
      fp_alt_name = link_dataref_byte_arr ("qpac_airbus/flightplan/alternate_icao",5,-1);
      int *fp_act_init = link_dataref_int ("qpac_airbus/flightplan/active_init");
      int *fp_alt_init = link_dataref_int ("qpac_airbus/flightplan/alternate_init");
      int *fp_no_wp = link_dataref_int ("qpac_airbus/flightplan/no_wp");
      if (*fp_no_wp != FLT_MISS) fpNoWp = *fp_no_wp; else fpNoWp = 0;
      int *fp_wp_type = link_dataref_int ("qpac_airbus/flightplan/wptype");
      fp_wp_id = link_dataref_byte_arr ("qpac_airbus/flightplan/wpid",10,-1);
      int *fp_to_wp = link_dataref_int ("qpac_airbus/flightplan/current_to_waypoint");
      if (*fp_to_wp != FLT_MISS) fpToWp = *fp_to_wp; else fpToWp = 0;
      int *fp_ins_wp_idx = link_dataref_int ("qpac_airbus/flightplan/insert_wp_index");
      int *fp_edit_wp_idx = link_dataref_int ("qpac_airbus/flightplan/edit_wp_index");
      int *fp_del_wp_idx = link_dataref_int ("qpac_airbus/flightplan/delete_wp_index");
      int *fp_query_wp_idx = link_dataref_int ("qpac_airbus/flightplan/query_wp_index");
      int *fp_dest_idx = link_dataref_int ("qpac_airbus/flightplan/dest_index");
      int *fp_flags = link_dataref_int ("qpac_airbus/flightplan/flags");
      float *fp_alt = link_dataref_flt ("qpac_airbus/flightplan/altitude",-1);
      float *fp_lat = link_dataref_flt ("qpac_airbus/flightplan/latitude",-1);
      float *fp_lon = link_dataref_flt ("qpac_airbus/flightplan/longitude",-1);
      int *fp_pass_cd = link_dataref_int ("qpac_airbus/flightplan/passcode");
      float *fp_spd_lim = link_dataref_flt ("qpac_airbus/flightplan/speedlimit",-1);
*/

      if (verbosity > 2) {
        printf ("A320NDWidget - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
        printf ("A320NDWidget -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
        printf ("A320NDWidget -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
        printf ("A320NDWidget -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
      }

//=======================The actual rendering stuff============================

      mapRange = ndRanges[ndRange][3];

      glPushMatrix ();

      // All pages

      // Texts occurring on multiple pages
      m_pFontManager->SetSize (m_Font, 11, 11);
      glColor3ub (COLOR_WHITE);
      m_pFontManager->Print (  5, 480, "GS", m_Font);
      m_pFontManager->Print ( 65, 480, "TAS", m_Font);
      m_pFontManager->SetSize (m_Font, 13, 13);
      m_pFontManager->Print ( 36, 460, "/", m_Font);
      // The GS value
      m_pFontManager->SetSize (m_Font, 13, 13);
      glColor3ub (COLOR_GREEN);
      sprintf (buff, "%3d", (int) gndSpd); m_pFontManager->Print ( 28, 480, buff, m_Font);
      sprintf (buff, "%3d", (int) airSpd); m_pFontManager->Print (100, 480, buff, m_Font);
      //The Wind Values and Arrow (draw only if wind > 2kts!)
      if (wndSpd > 2) {
        sprintf (buff, "%03d", (int) wndDirT); m_pFontManager->Print (  5, 460, buff, m_Font);
        sprintf (buff,  "%2d", (int) wndSpd);  m_pFontManager->Print ( 50, 460, buff, m_Font);
        // -- push matrix, shift, rotate, draw arrow, pop matrix
        glPushMatrix ();
        glTranslatef (30, 430, 0);
        if (ndPage != 4) glRotatef (180.0 - (wndDirM - hdgMag), 0, 0, 1);
        else glRotatef (180.0 - wndDirM, 0, 0, 1);
        glLineWidth (2);
        glColor3ub (COLOR_WHITE);
        glBegin (GL_LINES);
        glVertex2d (  0, -15); glVertex2d (  0 , 15);
        glEnd ();
        glBegin (GL_LINE_STRIP);
        glVertex2d ( -5,  5); glVertex2d (  0,  15); glVertex2d (  5,  5);
        glEnd ();
        glPopMatrix ();
      } else {
        m_pFontManager->Print (  5, 460, "---", m_Font);
        m_pFontManager->Print ( 50, 460, "--",  m_Font);
      } // end of (wndSpd > 2)
      // End of texts on multiple pages

      // Draw the "fixed" data in the corners
      if (ndPage != 4) { // not on PLAN page
        // The NavAids
        if (efis1SelPilot == 2) {
          // VOR 1
          glLineWidth (2);
          glColor3ub (COLOR_WHITE);
          glBegin (GL_LINE_STRIP);
          glVertex2d ( 12,  66); glVertex2d ( 12 , 60); 
          glVertex2d (  6,  49); glVertex2d ( 18,  49);
          glVertex2d ( 12,  60);
          glEnd ();
          glBegin (GL_LINES);
          glVertex2d ( 12,  49); glVertex2d ( 12,  34);
          glEnd ();
          m_pFontManager->SetSize (m_Font, 14, 14);
          glColor3ub (COLOR_WHITE);
          m_pFontManager->Print ( 22,  52, "VOR1", m_Font);
          sprintf (buff, "%s", nav1_name);
          m_pFontManager->Print ( 22,  33, buff, m_Font);
          glColor3ub (COLOR_GREEN);
          if (*nav1_has_dme == 1) {
            sprintf (buff, "%0.1f", distNav1);
            if (strlen (buff) < 4) sprintf (buff, " %0.1f", distNav1);
            m_pFontManager->Print ( 14,   9, buff, m_Font);
            m_pFontManager->SetSize (m_Font, 11, 11);
            glColor3ub (COLOR_CYAN);
            m_pFontManager->Print ( 61,   9, "NM", m_Font);
          }
        } else if (efis1SelPilot == 0) {
          // ADF 1
          glLineWidth (2);
          glColor3ub (COLOR_GREEN);
          glBegin (GL_LINES);
          glVertex2d ( 12,  66); glVertex2d ( 12,  34);
          glEnd ();
          glBegin (GL_LINE_STRIP);
          glVertex2d (  6,  49); glVertex2d ( 12,  60); glVertex2d ( 18,  49);
          glEnd ();
          m_pFontManager->SetSize (m_Font, 14, 14);
          glColor3ub (COLOR_GREEN);
          m_pFontManager->Print ( 22,  52, "ADF1", m_Font);
          sprintf (buff, "%s", adf1_name);
          m_pFontManager->Print ( 22,  33, buff, m_Font);
          if (*adf1_has_dme == 1) {
            sprintf (buff, "%0.1f", distAdf1);
            if (strlen (buff) < 4) sprintf (buff, " %0.1f", distAdf1);
            m_pFontManager->Print ( 14,   9, buff, m_Font);
            m_pFontManager->SetSize (m_Font, 11, 11);
            glColor3ub (COLOR_CYAN);
            m_pFontManager->Print ( 61,   9, "NM", m_Font);
          }
        }
        if (efis2SelPilot == 2) {
          // VOR 2
          glLineWidth (2);
          glColor3ub (COLOR_WHITE);
          glBegin (GL_LINE_STRIP);
          glVertex2d (488,  66); glVertex2d (488 , 60);
          glVertex2d (481,  49); glVertex2d (484,  49);
          glVertex2d (484,  34);
          glEnd ();
          glBegin (GL_LINE_STRIP);
          glVertex2d (488,  60); glVertex2d (495,  49);
          glVertex2d (492,  49); glVertex2d (492,  34);
          glEnd ();
          m_pFontManager->SetSize (m_Font, 14, 14);
          glColor3ub (COLOR_WHITE);
          m_pFontManager->Print (432,  52, "VOR2", m_Font);
          sprintf (buff, "%s", nav2_name);
          m_pFontManager->Print (432,  33, buff, m_Font);
          glColor3ub (COLOR_GREEN);
          if (*nav2_has_dme == 1) {
            sprintf (buff, "%0.1f", distNav2);
            if (strlen (buff) < 4) sprintf (buff, " %0.1f", distNav2);
            m_pFontManager->Print (426,   9, buff, m_Font);
            m_pFontManager->SetSize (m_Font, 11, 11);
            glColor3ub (COLOR_CYAN);
            m_pFontManager->Print (473,   9, "NM", m_Font);
          }
        } else if (efis2SelPilot == 0) {
          // ADF 2
          glLineWidth (2);
          glColor3ub (COLOR_GREEN);
          glBegin (GL_LINE_STRIP);
          glVertex2d (488,  66); glVertex2d (488,  60);
          glVertex2d (481,  49); glVertex2d (484,  49);
          glVertex2d (484,  34);
          glEnd ();
          glBegin (GL_LINE_STRIP);
          glVertex2d (488,  60); glVertex2d (495,  49);
          glVertex2d (492,  49); glVertex2d (492,  34);
          glEnd ();
          m_pFontManager->SetSize (m_Font, 14, 14);
          glColor3ub (COLOR_GREEN);
          m_pFontManager->Print (432,  52, "ADF2", m_Font);
          sprintf (buff, "%s", adf2_name);
          m_pFontManager->Print (432,  33, buff, m_Font);
          if (*adf2_has_dme == 1) {
            sprintf (buff, "%0.1f", distAdf2);
            if (strlen (buff) < 4) sprintf (buff, " %0.1f", distAdf2);
            m_pFontManager->Print ( 14,   9, buff, m_Font);
            m_pFontManager->SetSize (m_Font, 11, 11);
            glColor3ub (COLOR_CYAN);
            m_pFontManager->Print ( 61,   9, "NM", m_Font);
          }
        }
      } // end of (ndPage != 4)
      // end of "fixed" data

      // Draw the map range circles
      if (ndPage != 3) { // not on ARC page: has its own extensive rendering
        // is the heading valid?
        if ((headingValid != 1) && (ndPage != 4)) glColor3ub (COLOR_RED);
        else glColor3ub (COLOR_WHITE);
        // The outer range circle
        glLineWidth (3);
        aCircle.SetOrigin (mapCenterX, mapCenterY);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetRadius (mapCircle);
        aCircle.SetArcStartEnd (0, 360); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        // The inner range circle
        glLineWidth (2);
        aCircle.SetOrigin (250, 250);
        aCircle.SetRadius ( 80);
        aCircle.SetDegreesPerPoint (5);
        aCircle.SetArcStartEnd (  0,  10); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd ( 20,  30); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd ( 40,  50); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd ( 60,  70); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd ( 80,  90); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (100, 110); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (120, 130); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (140, 150); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (160, 170); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (180, 190); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (200, 210); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (220, 230); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (240, 250); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (260, 270); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (280, 290); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (300, 310); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (320, 330); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
        aCircle.SetArcStartEnd (340, 350); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();

        if ((headingValid != 1) && (ndPage != 4)) { // not on ARC page && not on PLAN page
          aCircle.SetRadius (  5); 
          aCircle.SetArcStartEnd (  0, 360); glBegin (GL_LINE_STRIP); aCircle.Evaluate (); glEnd ();
          m_pFontManager->SetSize (m_Font, 24, 24);
          m_pFontManager->Print (220, 370, "HDG", m_Font);

        } else { // i.e. (headingValid == 1, not on ARC page)

          // The Range Indicators
          if (headingValid == 1) {
            m_pFontManager->SetSize (m_Font, 10, 10);
            glColor3ub (COLOR_CYAN);
            if (ndRanges[ndRange][3] == 10) {
              sprintf (buff, "%3.1f", ndRanges[ndRange][0]);
              m_pFontManager->Print (200, 200, buff, m_Font);
              sprintf (buff, "%2.0f", ndRanges[ndRange][1]);
              m_pFontManager->Print (140, 140, buff, m_Font);
            } else {
              sprintf (buff, "%3.0f", ndRanges[ndRange][0]);
              m_pFontManager->Print (200, 200, buff, m_Font);
              sprintf (buff, "%3.0f", ndRanges[ndRange][1]);
              m_pFontManager->Print (140, 140, buff, m_Font);
            }
          }

          if (ndPage != 4) { // not on ARC page && not on PLAN page

            // The Compass Triangles
            glPushMatrix ();
              glTranslated (250,250,0);
              glPushMatrix ();
                glColor3ub (COLOR_WHITE);
                glLineWidth (2);
                for (rotation = 0; rotation < 315; rotation += 45) {
                  glRotated (45, 0, 0, 1);
                  glBegin (GL_TRIANGLES);
                    glVertex2d (  0, 165); glVertex2d ( -5, 177); glVertex2d (  5, 177);
                  glEnd ();
                }
              glPopMatrix ();
            glPopMatrix ();

            // The heading ticks and numbers on the circle
            glPushMatrix ();
            glLineWidth (3);
            glColor3ub (COLOR_WHITE);
            glTranslated (250, 250, 0);
            glRotated (hdgMag, 0, 0, 1);

            for (alpha = 355; alpha >= 0; alpha -= 5)  {
              glRotated (5.0, 0, 0, 1);
              if (alpha % 10 == 0) {
                // draw long tick
                glBegin (GL_LINES); glVertex2d (  0, 165); glVertex2d (  0, 175); glEnd ();
                int hdg = alpha / 10;
                sprintf (buff, "%d", hdg);
                if (hdg %3 == 0) {
                  m_pFontManager->SetSize (m_Font, 18, 18);
                  if (hdg < 10) m_pFontManager->Print ( -7, 180, buff, m_Font);
                  else          m_pFontManager->Print (-15, 180, buff, m_Font);
                }
              } else {
                // draw short tick
                glBegin (GL_LINES); glVertex2d (  0, 165); glVertex2d (  0, 170); glEnd ();
              }
            }
            glPopMatrix ();
            
          // The NavAids arrows
          if (efis1SelPilot == 2) {
            // The VOR1 Arrow
            glPushMatrix ();
              glLineWidth (2);
              glColor3ub (COLOR_WHITE);
              glTranslated (250, 250, 0);
              glRotatef (hdgMag - brgNav1, 0, 0, 1); // VOR1 Direction
              glBegin (GL_LINE_STRIP);
                glVertex2d (  0,  165); glVertex2d (  0,  150);
                glVertex2d (-10,  130); glVertex2d ( 10,  130);
                glVertex2d (  0,  150);
              glEnd ();
              glBegin (GL_LINES);
                glVertex2d (  0,  130); glVertex2d (  0,   80);
              glEnd ();
              glBegin (GL_LINE_STRIP);
                glVertex2d (  0,  -80); glVertex2d (  0, -130);
                glVertex2d (-10, -150); glVertex2d ( 10, -150);
                glVertex2d (  0, -130);
              glEnd ();
              glBegin (GL_LINES);
                glVertex2d (  0, -150); glVertex2d (  0, -165);
              glEnd ();
            glPopMatrix ();
          } else if (efis1SelPilot == 0) {
            // ADF 1
              glPushMatrix ();
                glLineWidth (2);
                glColor3ub (COLOR_GREEN);
                glTranslated (250, 250, 0);
                glRotatef (hdgMag - brgAdf1, 0, 0, 1); // ADF1 Direction
                glBegin (GL_LINES);
                  glVertex2d (  0,  165); glVertex2d (  0,   80);
                  glVertex2d (  0,  -80); glVertex2d (  0, -165);
                glEnd ();
                glBegin (GL_LINE_STRIP);
                  glVertex2d (-10,  130); glVertex2d (  0,  150); glVertex2d ( 10,  130);
                glEnd ();
                glBegin (GL_LINE_STRIP);
                  glVertex2d (-10, -150); glVertex2d (  0, -130); glVertex2d ( 10, -150);
                glEnd ();
              glPopMatrix ();
            }
            if (efis2SelPilot == 2) {
              // VOR 2
              glPushMatrix ();
                glLineWidth (2);
                glColor3ub (COLOR_WHITE);
                glTranslated (250, 250, 0);
                glRotatef (hdgMag - brgNav2, 0, 0, 1); // VOR2 Direction
                glBegin (GL_LINE_STRIP);
                  glVertex2d (  0,  165); glVertex2d (  0,  120);
                  glVertex2d (-10,  100); glVertex2d ( -5,  100);
                  glVertex2d ( -5,   80);
                glEnd ();
                glBegin (GL_LINE_STRIP);
                  glVertex2d (  0,  120); glVertex2d ( 10,  100);
                  glVertex2d (  5,  100); glVertex2d (  5,   80);
                glEnd ();
                glBegin (GL_LINE_STRIP);
                  glVertex2d ( -5,  -80); glVertex2d ( -5, -110);
                  glVertex2d (  0, -100); glVertex2d (  5, -110);
                  glVertex2d (  5,  -80);
                glEnd ();
                glBegin (GL_LINES);
                  glVertex2d (  0, -100); glVertex2d (  0, -165);
                glEnd ();
              glPopMatrix ();
            } else if (efis2SelPilot == 0) {
              // ADF 2
              glPushMatrix ();
                glLineWidth (2);
                glColor3ub (COLOR_GREEN);
                glTranslated (250, 250, 0);
                glRotatef (hdgMag - brgAdf2, 0, 0, 1); // ADF2 Direction
                glBegin (GL_LINE_STRIP);
                  glVertex2d (  0,  165); glVertex2d (  0,  120);
                  glVertex2d (-10,  100); glVertex2d ( -5,  100);
                  glVertex2d ( -5,   80);
                glEnd ();
                glBegin (GL_LINE_STRIP);
                  glVertex2d (  0,  120); glVertex2d ( 10,  100);
                  glVertex2d (  5,  100); glVertex2d (  5,   80);
                glEnd ();
                glBegin (GL_LINE_STRIP);
                  glVertex2d ( -5,  -80); glVertex2d ( -5, -110);
                  glVertex2d (  0, -100); glVertex2d (  5, -110);
                  glVertex2d (  5,  -80);
                glEnd ();
                glBegin (GL_LINES);
                  glVertex2d (  0, -100); glVertex2d (  0, -165);
                glEnd ();
              glPopMatrix ();
            }
          } // end of (ndPage != 4)
        } // end of (headingValid == 1)
      } // end of (ndPage != 3)
      // End of map range circles

      // The TO-WPT info
      // N.B. Also use "NDInhibit_Capt" dataref? (No, doesn't seem to do anything...)
      if ((ndPage == 2) || (ndPage == 3) || (ndPage == 4)) {
        glColor3ub (COLOR_WHITE);
        m_pFontManager->SetSize (m_Font, 14, 14);
        m_pFontManager->Print (390, 480, "PPOS", m_Font);
        /* Note: following does not work (yet?) in sim...
        sprintf (buff, "%s", toWptId);
        m_pFontManager->Print (390, 480, buff, m_Font);
        glColor3ub (COLOR_CYAN);
        m_pFontManager->Print (483, 480, "\260", m_Font); // the "degree" sign
        m_pFontManager->Print (472, 458, "NM", m_Font);
        glColor3ub (COLOR_GREEN);
        sprintf (buff, "%03.0f", wptCrs);
        m_pFontManager->Print (450, 480, buff, m_Font);
        sprintf (buff, "%3.1f", wptDist);
        m_pFontManager->Print (425, 458, buff, m_Font);
        sprintf (buff, "%02i", *wptHrs);
        m_pFontManager->Print (440, 436, buff, m_Font);
        m_pFontManager->Print (460, 436, ":", m_Font);
        sprintf (buff, "%02i", *wptMins);
        m_pFontManager->Print (470, 436, buff, m_Font);
        */
      } // end of ((ndPage == 2) || (ndPage == 3) || (ndPage == 4))

      // The Chronometer background
      if (chronoCapt != 0.0) {
        glColor3ub (COLOR_GRAY25);
        glBegin (GL_POLYGON);
          glVertex2d (  5,  70);
          glVertex2d (  5,  95);
          glVertex2d ( 80,  95);
          glVertex2d ( 80,  70);
        glEnd ();
        // The chrono value itself
        glColor3ub (COLOR_CYAN);
        m_pFontManager->SetSize (m_Font, 14, 14);
        sprintf (buff, "%2i\'%02i\"", (int) chronoCapt / 60, (int) chronoCapt % 60);
        m_pFontManager->Print ( 10,  75, buff, m_Font);
      }

      // The GPS warning message
      if (gps_prim_valid != 0) {
        // The text box
        glColor3ub (COLOR_WHITE);
        glLineWidth (2);
        glBegin (GL_LINE_LOOP);
          glVertex2d (100,  20);
          glVertex2d (100,  45);
          glVertex2d (400,  45);
          glVertex2d (400,  20);
        glEnd ();
        // The text itself
        if (gps_prim_valid == 2) glColor3ub (COLOR_AMBER);
        m_pFontManager->SetSize (m_Font, 14, 14);
        if (gps_prim_valid == 1) m_pFontManager->Print (190,  25, "GPS PRIMARY", m_Font);
        else if (gps_prim_valid == 2) m_pFontManager->Print (162,  25, "GPS PRIMARY LOST", m_Font);
      }

      glPopMatrix ();

      // The Oppside message (on all pages?)
      if (oppsideMsg == -1) {
        glColor3ub (COLOR_AMBER);
        m_pFontManager->SetSize (m_Font, 14, 14);
        m_pFontManager->Print (140,   3, "SET OPPSIDE RNG/MODE", m_Font);
      }

// Note: next part still to be implemented!
if (0 == 1) {
      // Mapping the environment <= BEGIN =============

      // The input coordinates are in lon/lat, so we have to rotate against true heading
      // despite the NAV display is showing mag heading
      if (*heading_true != FLT_MISS) {

      // Shift center and rotate about heading
      glMatrixMode (GL_MODELVIEW);
      glPushMatrix ();
//        glTranslatef (m_PhysicalSize.x*acf_x, m_PhysicalSize.y*acf_y, 0.0);
        if (ndPage == 3) glTranslatef (m_PhysicalSize.x*0.5, m_PhysicalSize.y*0.25, 0.0); // ARC page: (250, 100)
        else glTranslatef (m_PhysicalSize.x*0.5, m_PhysicalSize.y*0.5, 0.0); // other pages: (250, 250)
        if (ndPage != 4) glRotatef (*heading_true, 0, 0, 1); // NOT on PLAN page!

        // valid coordinates?
        if ((*aircraft_lon >= -180.0) && (*aircraft_lon <= 180.0) && (*aircraft_lat >= -90.0) && (*aircraft_lat <= 90.0)) {

          // Set up circle for small symbols
          CircleEvaluator aCircle;
          aCircle.SetArcStartEnd (0,360);
          aCircle.SetDegreesPerPoint (10);

          double northing;
          double easting;
          double northing2;
          double easting2;
          double lon;
          double lat;
          float xPos;
          float yPos;
          float xPosR;
          float yPosR;
          float xPos2;
          float yPos2;
          float sideL;
          float sideR;

          // calculate approximate lon/lat range to search for given selected map range
          int nlat = ceil (ndRanges[ndRange][3] * 1852.0 / (2.0 * 3.141592 * 6378137.0) * 360.0);
          int nlon;
          if (fabs (*aircraft_lat) < 89.5) {
            nlon = ceil (ndRanges[ndRange][3] * 1852.0 / (2.0 * 3.141592 * 6378137.0) * 360.0 / 
                     cos (*aircraft_lat * 0.0174533));
          } else nlon = 180; // north/south pole: span full longitude range

          double aircraft_lon2;
          double aircraft_lat2;

          for (int la=-nlat; la <=nlat; la++) {
            for (int lo=-nlon; lo <=nlon; lo++) {

              aircraft_lon2 = *aircraft_lon + (double) lo;
              aircraft_lat2 = *aircraft_lat + (double) la;

              // only display Fixes with a map range <= 40 nm
              if ((ndRange < 3) && (*nav_shows_fix == 1)) {

                //----------Fixes-----------

                // FixList::iterator fixIt;    
                // FixList* pFixList = m_pNavDatabase->GetFixList ();

                GeographicObjectList::iterator fixIt;
                GeographicHash* pFixHash = m_pNavDatabase->GetFixHash ();
                std::list<OpenGC::GeographicObject*>* pFixList = pFixHash->GetListAtLatLon (aircraft_lat2,aircraft_lon2);

                for (fixIt = pFixList->begin (); fixIt != pFixList->end (); ++fixIt) {

                  lon = (*fixIt)->GetDegreeLon ();
                  lat = (*fixIt)->GetDegreeLat ();

                  // convert to azimuthal equidistant coordinates with acf in center
                  lonlat2gnomonic (&lon, &lat, &easting, &northing, aircraft_lon, aircraft_lat);

                  // Compute physical position relative to acf center on screen
                  yPos = -northing / 1852.0 / mapRange * mapCircle; 
                  xPos = easting / 1852.0  / mapRange * mapCircle;

                  // Only draw the nav if it's visible within the rendering area
                  if ( sqrt (xPos*xPos + yPos*yPos) < mapRange) {

                    // calculate physical position on heading rotated nav map
                    float rotateRad = -1.0 * *heading_true * dtor;

                    xPosR = cos (rotateRad) * xPos + sin (rotateRad) * yPos;
                    yPosR = -sin (rotateRad) * xPos + cos (rotateRad) * yPos;

                    sideL = (xPosR + 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                            (yPosR - 0.75 * mapRange) * (-0.15 * mapRange + 0.75 * mapRange);
                    sideR = (xPosR - 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                            (yPosR - 0.75 * mapRange) * ( 0.15 * mapRange - 0.75 * mapRange);

                    // Only draw the nave if it is in the upper section of the screen
                    if ((sideL <= 0.0) && (sideR >= 0.0)) {

                      // We're going to be clever and "derotate" each label
                      // by the heading, so that everything appears upright despite
                      // the global rotation. This makes all of the labels appear at
                      // 0,0 in the local coordinate system
                      glPushMatrix ();
                        glTranslatef (xPos, yPos, 0.0);
                        glRotatef (-1.0* *heading_true, 0, 0, 1);

                        /* small triangle: light blue */
                        float ss2 = symbolSize * 0.5;
                        glColor3ub (0, 255, 255);
                        glLineWidth (1.5);
                        glBegin (GL_LINE_LOOP);
                          glVertex2f (-ss2, -1.0*ss2);
                          glVertex2f (ss2, -1.0*ss2);
                          glVertex2f (0.0, 1.0*ss2);
                        glEnd ();

                        m_pFontManager->SetSize (m_Font, 8.5, 8.5); // <= (m_Font, 0.65*fontSize, 0.65*fontSize);
                        m_pFontManager->Print (1.25*ss2,-1.75*ss2, (*fixIt)->GetIdentification ().c_str(), m_Font);
                      glPopMatrix ();

                    }
                  }
                }
              }

              // only display Fixes with a map range <= 40 nm
              if (ndRange < 3) {

                //----------Navaids-----------

                // NavaidList::iterator navIt;
                // NavaidList* pNavList = m_pNavDatabase->GetNavaidList ();

                GeographicObjectList::iterator navIt;
                GeographicHash* pNavHash = m_pNavDatabase->GetNavaidHash ();
                std::list<OpenGC::GeographicObject*>* pNavList = pNavHash->GetListAtLatLon (aircraft_lat2,aircraft_lon2);

                for (navIt = pNavList->begin (); navIt != pNavList->end (); ++navIt) {

                  lon = (*navIt)->GetDegreeLon ();
                  lat = (*navIt)->GetDegreeLat ();

                  // convert to azimuthal equidistant coordinates with acf in center
                  lonlat2gnomonic (&lon, &lat, &easting, &northing, aircraft_lon, aircraft_lat);

                  // Compute physical position relative to acf center on screen
                  yPos = -northing / 1852.0 / mapRange * mapCircle; 
                  xPos = easting / 1852.0  / mapRange * mapCircle;

                  // Only draw the nav if it's visible within the rendering area
                  if ( sqrt (xPos*xPos + yPos*yPos) < mapRange) {

                    // calculate physical position on heading rotated nav map
                    float rotateRad = -1.0 * *heading_true * dtor;

                    xPosR = cos (rotateRad) * xPos + sin (rotateRad) * yPos;
                    yPosR = -sin (rotateRad) * xPos + cos (rotateRad) * yPos;

                    sideL = (xPosR + 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                            (yPosR - 0.75 * mapRange) * (-0.15 * mapRange + 0.75 * mapRange);
                    sideR = (xPosR - 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                            (yPosR - 0.75 * mapRange) * ( 0.15 * mapRange - 0.75 * mapRange);

                    // Only draw the nav if it is in the upper section of the screen
                    if ((sideL <= 0.0) && (sideR >= 0.0)) {

                      int type = (*navIt)->GetNavaidType();

                      if (((type == 2) && (*nav_shows_ndb == 1)) || 
                          ((*nav_shows_vor == 1) && ((type == 3) || (type == 12) || (type == 13)))) {

                        // We're going to be clever and "derotate" each label
                        // by the heading, so that everything appears upright despite
                        // the global rotation. This makes all of the labels appear at
                        // 0,0 in the local coordinate system
                        glPushMatrix ();

                          if (ndPage != 4) { // NOT on PLAN page!
                            glTranslatef (xPos, yPos, 0.0);
                            glRotatef (-1.0* *heading_true, 0, 0, 1);
                          }
      
                          if (type == 2) {
                            /* NDB: two circles violet */
                            glColor3ub (52, 72, 139);
                            glLineWidth (2.0);
                            aCircle.SetRadius (symbolSize);
                            aCircle.SetOrigin (0.0, 0.0);
                            glBegin (GL_LINE_LOOP);
                              aCircle.Evaluate();
                            glEnd ();

                            aCircle.SetRadius (0.5*symbolSize);
                            aCircle.SetOrigin (0.0,0.0);
                            glBegin (GL_LINE_LOOP);
                              aCircle.Evaluate();
                            glEnd ();
                          } else if (type == 3) {
                            // VOR: six point polygon light blue
                            float ss3 = symbolSize * 0.65;
                            glColor3ub (0, 255, 82);
                            glLineWidth (2.0);
                            glBegin (GL_LINE_LOOP);
                              glVertex2f (-0.5*ss3, -0.866*ss3);
                              glVertex2f (0.5*ss3, -0.866*ss3);
                              glVertex2f (ss3, 0.0);
                              glVertex2f (0.5*ss3, 0.866*ss3);
                              glVertex2f (-0.5*ss3, 0.866*ss3);
                              glVertex2f (-ss3, 0.0);
                            glEnd ();
                          } else {
                            // DME or DME part of VOR/VORTAC: add three radial boxes to VOR symbol
                            float ss4 = symbolSize * 0.65;
                            // glColor3ub (130, 168, 217);
                            glColor3ub (0, 255, 82);
                            glLineWidth (2.0);
                            glBegin (GL_LINE_LOOP);
                              glVertex2f (-0.5*ss4, -0.866*ss4);
                              glVertex2f (0.5*ss4, -0.866*ss4);
                              glVertex2f (ss4, 0.0);
                              glVertex2f (0.5*ss4, 0.866*ss4);
                              glVertex2f (-0.5*ss4, 0.866*ss4);
                              glVertex2f (-ss4, 0.0);
                            glEnd ();
                            glBegin (GL_LINE_STRIP);
                              glVertex2f (-0.5*ss4, -0.866*ss4);
                              glVertex2f (-0.5*ss4, -1.732*ss4);
                              glVertex2f (0.5*ss4, -1.732*ss4);
                              glVertex2f (0.5*ss4, -0.866*ss4);
                            glEnd ();
                            glBegin (GL_LINE_STRIP);
                              glVertex2f (ss4, 0.0);
                              glVertex2f (1.75*ss4, 0.433*ss4);
                              glVertex2f (1.25*ss4, 1.366*ss4);
                              glVertex2f (0.5*ss4, 0.866*ss4);
                            glEnd ();
                            glBegin (GL_LINE_STRIP);
                              glVertex2f (-0.5*ss4, 0.866*ss4);
                              glVertex2f (-1.25*ss4, 1.366*ss4);
                              glVertex2f (-1.75*ss4, 0.433*ss4);
                              glVertex2f (-ss4, 0.0);
                            glEnd ();
                          }

                          m_pFontManager->SetSize (m_Font, 8.5, 8.5); // <= (m_Font, 0.65*fontSize, 0.65*fontSize);
                          m_pFontManager->Print (symbolSize * 1.25,-symbolSize * 1.75, (*navIt)->GetIdentification ().c_str(), m_Font);
                        glPopMatrix ();
                      }
                    }
                  }
                }
              }

              //----------Airports-----------
              if (*nav_shows_apt == 1) {
//printf ("physSize.y %f, map_y_max %f, acf_y %f, mapRange %f\n", m_PhysicalSize.y, map_y_max, acf_y, mapRange);
//printf ("mapRange = m_PhysicalSize.y * (map_y_max - acf_y) -> %f * (%f - %f) -> %f * %f -> %f\n", m_PhysicalSize.y, map_y_max, acf_y, m_PhysicalSize.y, map_y_max - acf_y, mapRange);
                GeographicObjectList::iterator aptIt;
                GeographicHash* pAptHash = m_pNavDatabase->GetAirportHash ();
                std::list<OpenGC::GeographicObject*>* pAptList = pAptHash->GetListAtLatLon (aircraft_lat2,aircraft_lon2);
//printf ("phSizeX %f, acfX %f, phSizeY %f, acfY %f mapRange %f\n", m_PhysicalSize.x, acf_x, m_PhysicalSize.y, acf_y, mapRange);
                // AirportList::iterator aptIt;
                // AirportList* pAptList = m_pNavDatabase->GetAirportList ();
                for (aptIt = pAptList->begin (); aptIt != pAptList->end (); ++aptIt) {
                  lon = (*aptIt)->GetDegreeLon ();
                  lat = (*aptIt)->GetDegreeLat ();

                  // convert to azimuthal equidistant coordinates with acf in center
                  lonlat2gnomonic (&lon, &lat, &easting, &northing, aircraft_lon, aircraft_lat);

                  // Compute physical position relative to acf center on screen
                  yPos = -northing / 1852.0 / mapRange * mapCircle; 
                  xPos = easting / 1852.0  / mapRange * mapCircle;
                  // Only draw the airport if it's visible within the rendering area
//                  if ( sqrt (xPos*xPos + yPos*yPos) < mapRange) {

                    // calculate physical position on heading rotated nav map
                    float rotateRad = -1.0 * *heading_true * dtor;

                    xPosR = cos (rotateRad) * xPos + sin (rotateRad) * yPos;
                    yPosR = -sin (rotateRad) * xPos + cos (rotateRad) * yPos;

                    sideL = (xPosR + 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                            (yPosR - 0.75 * mapRange) * (-0.15 * mapRange + 0.75 * mapRange);
                    sideR = (xPosR - 0.95 * mapRange) * (-0.25 * mapRange - 0.75 * mapRange) -
                            (yPosR - 0.75 * mapRange) * ( 0.15 * mapRange - 0.75 * mapRange);

                    // Only draw the nav if it is in the upper section of the screen
//                    if ((sideL <= 0.0) && (sideR >= 0.0)) {

                      // printf ("APT: %s %f %f \n",(*aptIt)->GetIdentification ().c_str(),lon,lat);

                      // We're going to be clever and "derotate" each label
                      // by the heading, so that everything appears upright despite
                      // the global rotation. This makes all of the labels appear at
                      // 0,0 in the local coordinate system
                      glPushMatrix ();

                        if (ndPage != 4) { // NOT on PLAN page!
                          glTranslatef (xPos, yPos, 0.0);
                          glRotatef (-1.0* *heading_true, 0, 0, 1);
                        }

                        // APT: green circle
                        glColor3ub (0, 255, 255);
                        glLineWidth (2.0);
                        aCircle.SetRadius (symbolSize);
                        aCircle.SetOrigin (0.0, 0.0);
                        glBegin (GL_LINE_LOOP);
                          aCircle.Evaluate();
                        glEnd ();

                        m_pFontManager->SetSize (m_Font, 8.5, 8.5); // <= (m_Font, 0.65*fontSize, 0.65*fontSize);
                        m_pFontManager->Print (symbolSize * 1.25,-symbolSize * 1.75, (*aptIt)->GetIdentification ().c_str(), m_Font);
                      glPopMatrix ();

//                    }
//                  } end if ( sqrt (xPos*xPos ...
                }
              }
            } // loop through longitude range around acf
          } // loop through latitude range around acf

/*
         //----------UFMC-----------
         //      printf ("%i %i \n",*fmc_nidx,nwpt);
          // Plot UFMC waypoints
          if ((*fmc_ok > 0.5) && (*fmc_nidx > 0)) {
            if (*fmc_nidx != nwpt) {
              nwpt = *fmc_nidx;
              if (wpt != NULL) delete [] wpt;
              wpt = new wptstruct[nwpt];
              for (int i=0;i<nwpt;i++) {
                memset (wpt[i].name,0,sizeof (wpt[i].name));
                wpt[i].lon = FLT_MISS;
                wpt[i].lat = FLT_MISS;
              }
              *fmc_idx = 0;
              *fmc_lon = -1000.0;
              *fmc_lat = -1000.0;
            }
            if ((*fmc_idx >= 0) && (*fmc_idx < nwpt)) {
              if ((*fmc_lon != -1000.0) && (*fmc_lat != -1000.0)) {
                printf ("Fetching Waypoint %i of %i from UFMC: %f %f %s \n",(*fmc_idx)+1,*fmc_nidx,*fmc_lat,*fmc_lon,fmc_name);
                if ((*fmc_lon != 0.0) && (*fmc_lat != 0.0)) {
                  memcpy(&wpt[*fmc_idx].name,fmc_name,sizeof (wpt[*fmc_idx].name));
                  wpt[*fmc_idx].lon = *fmc_lon;
                  wpt[*fmc_idx].lat = *fmc_lat;
                  *fmc_lon = -1000.0;
                  *fmc_lat = -1000.0;
                }
                (*fmc_idx)++;
              }
            }

            // for (int i = 0; i < (nwpt); i++) printf ("%i %f %f \n", i, wpt[i].lon, wpt[i].lat);
            for (int i = 0; i <= nwpt; i++) {
              // convert to azimuthal equidistant coordinates with acf in center
              if ((wpt[max (i-1,0)].lon != FLT_MISS) && (wpt[max (i-1,0)].lat != FLT_MISS) &&
                  (wpt[i].lon != FLT_MISS) && (wpt[i].lat != FLT_MISS)) {
                // convert to azimuthal equidistant coordinates with acf in center
                if (i == 0) {
                  lon = *aircraft_lon;
                  lat = *aircraft_lat;
                  northing = 0.0;
                  easting = 0.0;
                } else {
                  lon = (double) wpt[i-1].lon;
                  lat = (double) wpt[i-1].lat;
                  lonlat2gnomonic (&lon, &lat, &easting, &northing, aircraft_lon, aircraft_lat);
                }
                if (i == nwpt) {
                  northing2 = northing;
                  easting2 = easting;
                } else {
                  lon = (double) wpt[i].lon;
                  lat = (double) wpt[i].lat;
                  lonlat2gnomonic (&lon, &lat, &easting2, &northing2, aircraft_lon, aircraft_lat);
                }

                // Compute physical position relative to acf center on screen
                yPos  = -northing  / 1852.0 / mapRange * mapCircle; 
                xPos  =  easting   / 1852.0 / mapRange * mapCircle;
                yPos2 = -northing2 / 1852.0 / mapRange * mapCircle; 
                xPos2 =  easting2  / 1852.0 / mapRange * mapCircle;
                //  Only draw the waypoint if it's visible within the rendering area
                //  if (( sqrt (xPos*xPos + yPos*yPos) < 10.0*mapRange) &&
                //      ( sqrt (xPos2*xPos2 + yPos2*yPos2) < 10.0*mapRange)){
                // calculate physical position on heading rotated nav map
                //  float rotateRad = -1.0 * *heading_true * dtor;

                glPushMatrix ();
                  glTranslatef (xPos, yPos, 0.0);
                  glColor3ub (255, 0, 200);
                  glBegin (GL_LINES);
                    glVertex2f (0.0,0.0);
                    glVertex2f (xPos2-xPos,yPos2-yPos);
                  glEnd ();
                  glRotatef (-1.0* *heading_true, 0, 0, 1);
                  glColor3ub (255, 255, 255);

/*
                  float ss2 = 0.50*ss;
                  glColor3ub (0, 255, 255);
                  glLineWidth (1.5);
                  glBegin (GL_LINE_LOOP);
                    glVertex2f (-ss2, -1.0*ss2);
                    glVertex2f (ss2, -1.0*ss2);
                    glVertex2f (0.0, 1.0*ss2);
                  glEnd ();
*/
/* UFMC
                  float ss5 = ss;
                  glLineWidth (3.0);
                  glBegin (GL_LINE_LOOP);
                    glVertex2f (0.25*ss5, 0.25*ss5);
                    glVertex2f (1.0*ss5,  0.0);
                    glVertex2f (0.25*ss5, -0.25*ss5);
                    glVertex2f (0.0, -1.0*ss5);
                    glVertex2f (-0.25*ss5, -0.25*ss5);
                    glVertex2f (-1.0*ss5, 0.0);
                    glVertex2f (-0.25*ss5, 0.25*ss5);
                    glVertex2f ( 0.0, 1.0*ss5);
                  glEnd ();

                  m_pFontManager->SetSize (m_Font, 8.5, 8.5); // <= (m_Font, 0.65*fontSize, 0.65*fontSize);
                  if (i > 0)  m_pFontManager->Print (4,-4, wpt[i-1].name, m_Font);

                glPopMatrix ();
              }
            }
          } else {
            if (wpt != NULL) {
              delete [] wpt;
              wpt = NULL;
              nwpt = 0;
              printf ("UFMC waypoints cleared\n");
            }
          } // end if ((*fmc_ok ... (UFMC plotting)
*/

          //----------Flight Plan-----------
                    /* T.b.s.... */

          //----------TCAS-----------
          if (ndPage != 4) { // NOT on PLAN page!
            // plot TCAS of AI aircraft
            for (int i = 0; i < 20; i++) {
//printf ("%i %f %f %f \n",i,*(tcas_distance+i),*(tcas_heading+i),*(tcas_altitude+i));
              if ((*(tcas_distance+i) > 0.0) && (*(tcas_heading+i) != FLT_MISS) &&
                  (*(tcas_altitude+i) != FLT_MISS)) {

                float distancemeters = *(tcas_distance+i);
                float altitudemeters = *(tcas_altitude+i);
                float rotateRad = (*(tcas_heading+i) + *heading_true) * dtor;

                xPos = distancemeters * sin (rotateRad) / 1852.0 / mapRange * mapCircle; 
                yPos = distancemeters * cos (rotateRad) / 1852.0 / mapRange * mapCircle;
//printf ("ai-pln: %d, dist: %f, rel hdg: %f, rot: %f, xPos: %f, yPos: %f\n",i,distancemeters,*(tcas_heading+i), rotateRad, xPos, yPos);

                glPushMatrix ();

                glTranslatef (xPos, yPos, 0.0);
                glRotatef (-1.0* *heading_true, 0, 0, 1);
                glLineWidth (2.0);
                float ss2 = symbolSize * 0.65;
                if (fabs (altitudemeters)*3.28084 < 300.) {
                  // Resolution Advisory (red filled square)
                  glColor3ub (255, 0, 0);
                  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
                  glBegin (GL_POLYGON);
                  glVertex2f (-ss2, -ss2);
                  glVertex2f (ss2, -ss2);
                  glVertex2f (ss2, ss2);
                  glVertex2f (-ss2, ss2);
                  glEnd ();
                } else if (fabs (altitudemeters)*3.28084 < 900.) {
                // traffic advisory (yellow filled circle)
                glColor3ub (255, 255, 0);
                CircleEvaluator tCircle;
                tCircle.SetDegreesPerPoint (20);    
                tCircle.SetArcStartEnd (0,360);
                tCircle.SetRadius (ss2);
                tCircle.SetOrigin (0,0);
                glBegin (GL_POLYGON);
                tCircle.Evaluate();
                glEnd ();
                } else {
                  // other: white diamond
                  glColor3ub (255, 255, 255);
                  glPolygonMode (GL_FRONT,GL_LINE);
                  glBegin (GL_POLYGON);
                  glVertex2f (0, -ss2);
                  glVertex2f (ss2, 0);
                  glVertex2f (0, ss2);
                  glVertex2f (-ss2, 0);
                  glEnd ();
                  glPolygonMode (GL_FRONT,GL_FILL);
                }

                if (fabs (altitudemeters*3.28084/100) < 100) {
                  if (altitudemeters > 0) {
                    m_pFontManager->SetSize (m_Font, 8, 8); // <= ( m_Font, 0.6*fontSize, 0.6*fontSize );
                    sprintf (buff, "+%i", (int) (fabs (altitudemeters)*3.28084/100));
//                    m_pFontManager->Print (-0.9*fontSize,ss2+0.05*fontSize, &buff[0], m_Font);
                    m_pFontManager->Print (-0.9*13,ss2+0.05*13, &buff[0], m_Font);
                  } else {
                    m_pFontManager->SetSize (m_Font, 8, 8); // <= ( m_Font, 0.6*fontSize, 0.6*fontSize );
                    sprintf (buff, "-%i", (int) (fabs (altitudemeters)*3.28084/100));
//                    m_pFontManager->Print (-0.9*fontSize,-ss2-0.65*fontSize, &buff[0], m_Font);
                    m_pFontManager->Print (-0.9*13,-ss2-0.65*13, &buff[0], m_Font);
                  }
                }
                glPopMatrix ();
              }
            } // end for (int i = ... (TCAS AI aircraft)

          // plot TCAS of Multiplayer aircraft
          // t.b.s.

          } // end if (ndPage != 4) ...

        } // end of "valid coordinates?"

        /* end of down-shifted coordinate system */
      glPopMatrix ();

      // end of environment mapping
} // end if (0 == 1) ...
      
      } // end if (! coldAndDark)

    } // end Render()

  }
}

