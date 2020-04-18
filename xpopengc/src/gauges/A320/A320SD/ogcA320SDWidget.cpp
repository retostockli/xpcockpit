/*=============================================================================

  This is the ogcA320SDWidget.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style System Display Widget ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    Last change: 2020-02-02

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
#include "ogcA320SD.h"
#include "ogcA320SDWidget.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC {

  A320SDWidget::A320SDWidget () {
    if (verbosity > 1) printf ("A320SDWidget - constructing\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;

    m_Rotation = 0;

    if (verbosity > 0) printf ("A320SDWidget - constructed\n");
  }

  A320SDWidget::~A320SDWidget () {}

  void A320SDWidget::SetRotation (int rot) {
    if (verbosity > 2) printf ("A320SDWidget::SetRotation called - rot %i\n", rot);
    switch (rot) {
      case 0:
        m_Rotation = 0;
        break;
      case 1:
        m_Rotation = -90;
        break;
      case -1:
        m_Rotation = 90;
        break;
    }
  }

//===========================The Framerate Display function====================
// Note: does not seem to work correctly?
  float simTime, oldSimTime, fps, fpsCount;

  float A320SDWidget::calculateFPS (float simTime) {
    ++fpsCount;
    if (simTime - oldSimTime > 1.0) {
      oldSimTime = simTime;
      fps = fpsCount;
      fpsCount = 0;
    }
    return fps;
  }

  void A320SDWidget::Render () {

    bool coldAndDark = true;
    CircleEvaluator aCircle;

    // Call base class to setup for size and position
    GaugeComponent::Render ();

    if (verbosity > 2) {
      printf ("A320SDWidget - physical position: %f %f\n", m_PhysicalPosition.x, m_PhysicalPosition.y);
      printf ("A320SDWidget -    pixel position: %i %i\n", m_PixelPosition.x, m_PixelPosition.y);
      printf ("A320SDWidget -     physical size: %f %f\n", m_PhysicalSize.x, m_PhysicalSize.y);
      printf ("A320SDWidget -        pixel size: %i %i\n", m_PixelSize.x, m_PixelSize.y);
    }

    // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    if (!coldAndDark) {

      // All pages

      float *sim_time = link_dataref_flt ("sim/time/total_running_time_sec", 0);
      if (*sim_time != FLT_MISS) simTime = *sim_time; else simTime = 0.0;

      int sdPage = 15;
      int *sd_page = link_dataref_int ("AirbusFBW/SDPage");
      if ((*sd_page != INT_MISS) && (*sd_page >= 0)) sdPage = *sd_page;

      // All pages except SD_STATUS (#12) and SD_EWD_mapped (#13)

      float grossWeight = 0; // 5 digits
      float *gross_weight = link_dataref_flt ("sim/flightmodel/weight/m_total", 0);
      if (*gross_weight != FLT_MISS) grossWeight = *gross_weight;

      float sat = 0;      // 2 digits
      float *sa_temp = link_dataref_flt ("sim/weather/temperature_le_c", -2);
      if (*sa_temp != FLT_MISS) sat = *sa_temp;

      float tat = 0;      // 2 digits
      float *ta_temp = link_dataref_flt ("sim/weather/temperature_ambient_c", -2);
      if (*ta_temp != FLT_MISS) tat = *ta_temp;

      int utcHours = 0;   // 2 digits
      int *utc_hours = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_hours"); 
      if (*utc_hours != INT_MISS) utcHours = *utc_hours;

      int utcMinutes = 0; // 2 digits
      int *utc_minutes = link_dataref_int ("sim/cockpit2/clock_timer/zulu_time_minutes"); 
      if (*utc_minutes != INT_MISS) utcMinutes = *utc_minutes;

      // Page SD_ENG (#0)

      int pPos1 = 0, pPos2 = 0; // horizontal position for the ENG page's numeric values

      float eng1FuelUsed = 0;
//      float *eng1_fuel_used = link_dataref_flt_arr ("AirbusFBW/ENGFuelUsedArray", 4, 0, 0); // NOTE: not available in sim or plane!
//      if (*eng1_fuel_used != FLT_MISS) eng1FuelUsed = *eng1_fuel_used;

      float eng2FuelUsed = 0;
//      float *eng2_fuel_used = link_dataref_flt_arr ("AirbusFBW/ENGFuelUsedArray", 4, 1, 0); // NOTE: not available in sim or plane!
//      if (*eng2_fuel_used != FLT_MISS) eng2FuelUsed = *eng2_fuel_used;

      float eng1OilQty = 0;
      // NOTE: this is a ratio! 0.85 (i.e. 85%) corresponds with 11.9 units
      // thus, capacity 100% is 100/85*11.9 = 14
      float *eng1_oil_qty = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/oil_quantity_ratio", 8, 0, 0);
      if (*eng1_oil_qty != FLT_MISS) eng1OilQty = *eng1_oil_qty * 14;

      float eng2OilQty = 0;
      float *eng2_oil_qty = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/oil_quantity_ratio", 8, 1, 0);
      if (*eng2_oil_qty != FLT_MISS) eng2OilQty = *eng2_oil_qty * 14;

      float eng1OilPressure = 0;
      float *eng1_oil_pressure = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/oil_pressure_psi", 8, 0, 0);
      if (*eng1_oil_pressure != FLT_MISS) eng1OilPressure = *eng1_oil_pressure;

      float eng2OilPressure = 0;
      float *eng2_oil_pressure = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/oil_pressure_psi", 8, 1, 0);
      if (*eng2_oil_pressure != FLT_MISS) eng2OilPressure = *eng2_oil_pressure;

      float eng1OilTemp = 0;
      float *eng1_oil_temp = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/oil_temperature_deg_C", 8, 0, 0);
      if (*eng1_oil_temp != FLT_MISS) eng1OilTemp = *eng1_oil_temp;

      float eng2OilTemp = 0;
      float *eng2_oil_temp = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/oil_temperature_deg_C", 8, 1, 0);
      if (*eng2_oil_temp != FLT_MISS) eng2OilTemp = *eng2_oil_temp;

      int engStartMode = 0;
      int *eng_start_mode = link_dataref_int ("AirbusFBW/EWDStartMode"); 
      if (*eng_start_mode != INT_MISS) engStartMode = *eng_start_mode;

      int eng1FadecNoPower = 0;
      int *eng1_fadec_nopower = link_dataref_int_arr ("AirbusFBW/FADECStateArray", 4, 0);
      if (*eng1_fadec_nopower != INT_MISS) eng1FadecNoPower = *eng1_fadec_nopower;

      int eng2FadecNoPower = 0;
      int *eng2_fadec_nopower = link_dataref_int_arr ("AirbusFBW/FADECStateArray", 4, 1);
      if (*eng2_fadec_nopower != INT_MISS) eng2FadecNoPower = *eng2_fadec_nopower;

      float eng1BleedPress = 0;
      float *eng1_bleed_press = link_dataref_flt ("AirbusFBW/LeftBleedPress", 0);
      if (*eng1_bleed_press != FLT_MISS) eng1BleedPress = *eng1_bleed_press;

      float eng2BleedPress = 0;
      float *eng2_bleed_press = link_dataref_flt ("AirbusFBW/RightBleedPress", 0);
      if (*eng2_bleed_press != FLT_MISS) eng2BleedPress = *eng1_bleed_press;

      int eng1StartValve = 0;
      int *eng1_start_valve = link_dataref_int_arr ("AirbusFBW/StartValveArray", 4, 0);
      if (*eng1_start_valve != FLT_MISS) eng1StartValve = *eng1_start_valve;

      int eng2StartValve = 0;
      int *eng2_start_valve = link_dataref_int_arr ("AirbusFBW/StartValveArray", 4, 1);
      if (*eng2_start_valve != FLT_MISS) eng2StartValve = *eng2_start_valve;

      float eng1NacTemp = 0;
      float *eng1_nac_temp = link_dataref_flt_arr ("AirbusFBW/NacelleTempArray", 4, 0, 0);
      if (*eng1_nac_temp != FLT_MISS) eng1NacTemp = *eng1_nac_temp;

      float eng2NacTemp = 0;
      float *eng2_nac_temp = link_dataref_flt_arr ("AirbusFBW/NacelleTempArray", 4, 1, 0);
      if (*eng2_nac_temp != FLT_MISS) eng2NacTemp = *eng2_nac_temp;

      // Page SD_BLEED (#1)

      // apuBleedIndicator/Valve is handled in APU page (#6)

      int bleedIntercon = 0;
      int *bleed_intercon = link_dataref_int ("AirbusFBW/BleedIntercon");
      if (*bleed_intercon != INT_MISS) bleedIntercon = *bleed_intercon;

      int eng1BldVlv = 0; // Note: does not seem functional)
//      int *eng1_bld_vlv = link_dataref_int_arr ("AirbusFBW/Eng1BleedInd", 2, 0);
//      if (*eng1_bld_vlv != INT_MISS) eng1BldVlv = *eng1_bld_vlv;

      int eng2BldVlv = 0; // Note: does not seem functional)
//      int *eng2_bld_vlv = link_dataref_int_arr ("AirbusFBW/Eng2BleedInd", 2, 1);
//      if (*eng2_bld_vlv != INT_MISS) eng2BldVlv = *eng2_bld_vlv;

      int eng1HPBleed = 0; // Note: does not seem functional)
//      int *eng1_hp_bleed = link_dataref_int_arr ("AirbusFBW/Eng1HPBleedInd", 2, 0);
//      if (*eng1_hp_bleed != INT_MISS) eng1HPBleed = *eng1_hp_bleed;

      int eng2HPBleed = 0; // Note: does not seem functional)
//      int *eng2_hp_bleed = link_dataref_int_arr ("AirbusFBW/Eng2HPBleedInd", 2, 1);
//      if (*eng2_hp_bleed != INT_MISS) eng2HPBleed = *eng2_hp_bleed;

      int pack1FCV = 0;
      int *pack1_fcv = link_dataref_int ("AirbusFBW/Pack1FCVInd");
      if (*pack1_fcv != INT_MISS) pack1FCV = *pack1_fcv;

      int pack2FCV = 0;
      int *pack2_fcv = link_dataref_int ("AirbusFBW/Pack2FCVInd");
      if (*pack2_fcv != INT_MISS) pack2FCV = *pack2_fcv;

      float pack1FlowInd = 0.0;
      float *pack1_flow_ind = link_dataref_flt ("AirbusFBW/Pack1Flow", 0);
      if (*pack1_flow_ind != FLT_MISS) pack1FlowInd = *pack1_flow_ind;

      float pack2FlowInd = 0.0;
      float *pack2_flow_ind = link_dataref_flt ("AirbusFBW/Pack2Flow", 0);
      if (*pack2_flow_ind != FLT_MISS) pack2FlowInd = *pack2_flow_ind;

      float pack1Temp = 0.0;
      float *pack1_temp = link_dataref_flt ("AirbusFBW/Pack1Temp", 0);
      if (*pack1_temp != FLT_MISS) pack1Temp = *pack1_temp;

      float pack2Temp = 0.0;
      float *pack2_temp = link_dataref_flt ("AirbusFBW/Pack2Temp", 0);
      if (*pack2_temp != FLT_MISS) pack2Temp = *pack2_temp;

      float ramAirValve = 0.0;
      float *ram_air_valve = link_dataref_flt ("AirbusFBW/RamAirValveSD", 0);
      if (*ram_air_valve != FLT_MISS) ramAirValve = *ram_air_valve;

      int xBldVlv = 0;
      int *x_bld_vlv = link_dataref_int ("AirbusFBW/XBleedInd");
      if (*x_bld_vlv != INT_MISS) xBldVlv = *x_bld_vlv;
 
      float eng1N2 = 0, eng2N2 = 0;
      float *eng_n2 = link_dataref_flt_arr ("sim/flightmodel/engine/ENGN_N2_", 8, -1, -2);
      if (*eng_n2 != FLT_MISS) { eng1N2 = eng_n2[0]; eng2N2 = eng_n2[1]; };

      // Page SD_PRESS (#2)

      float cabinAlt = 0;
      float *cabin_alt = link_dataref_flt ("AirbusFBW/CabinAlt", 0);
      if (*cabin_alt != FLT_MISS) cabinAlt = *cabin_alt;

      float cabinVS = 0;
      float *cabin_vs = link_dataref_flt ("AirbusFBW/CabinVS", 0);
      if (*cabin_vs != FLT_MISS) cabinVS = *cabin_vs;

      float cabinDeltaP = 0;
      float *cabin_delta_p = link_dataref_flt ("AirbusFBW/CabinDeltaP", 0);
      if (*cabin_delta_p != FLT_MISS) cabinDeltaP = *cabin_delta_p;

      int hotAirValve = 0; // "INLET" on SD PRESS page; what about "EXTRACT"/"OUTLET" ?
      int *hot_air_valve = link_dataref_int ("AirbusFBW/HotAirValve");
      if (*hot_air_valve != INT_MISS) hotAirValve = *hot_air_valve;

      float outflowValve = 0;
      float *outflow_valve = link_dataref_flt ("AirbusFBW/OutflowValve", 0);
      if (*outflow_valve != FLT_MISS) outflowValve = *outflow_valve;

      int safetyValve = 0;
      int *safety_valve = link_dataref_int ("AirbusFBW/SafetyValve");
      if (*safety_valve != FLT_MISS) safetyValve = *safety_valve;

     // Page SD_ELEC (#3)

//      int ohpElecConn0 = 0;
//      int *elec_connec0 = link_dataref_int_arr ("AirbusFBW/ElecConnectors", 64, 0);
//      if (*elec_connec0 != FLT_MISS) ohpElecConn0 = *elec_connec0;

      int ohpElecGen1 = 0;
      int *ohp_elec_gen1 = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 0);
      if (*ohp_elec_gen1 != FLT_MISS) ohpElecGen1 = *ohp_elec_gen1;

      int ohpElecGen2 = 0;
      int *ohp_elec_gen2 = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 1);
      if (*ohp_elec_gen2 != FLT_MISS) ohpElecGen2 = *ohp_elec_gen2;

      int ohpElecApuGen = 0;
      int *ohp_elec_apu_gen = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 2);
      if (*ohp_elec_apu_gen != FLT_MISS) ohpElecApuGen = *ohp_elec_apu_gen;

      int ohpElecExtPwr = 0;
      int *ohp_elec_ext_pwr = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 3);
      if (*ohp_elec_ext_pwr != FLT_MISS) ohpElecExtPwr = *ohp_elec_ext_pwr;

      int ohpElecBusTie = 0;
      int *ohp_elec_bus_tie = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 4);
      if (*ohp_elec_bus_tie != FLT_MISS) ohpElecBusTie = *ohp_elec_bus_tie;

      int ohpElecBat1 = 0;
      int *ohp_elec_bat1 = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 5);
      if (*ohp_elec_bat1 != FLT_MISS) ohpElecBat1 = *ohp_elec_bat1;

      int ohpElecBat2 = 0;
      int *ohp_elec_bat2 = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 6);
      if (*ohp_elec_bat2 != FLT_MISS) ohpElecBat2 = *ohp_elec_bat2;

      int ohpElecAcEss = 0;
      int *ohp_elec_ac_ess = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 7);
      if (*ohp_elec_ac_ess != FLT_MISS) ohpElecAcEss = *ohp_elec_ac_ess;

      int ohpElecComm = 0;
      int *ohp_elec_comm = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 8);
      if (*ohp_elec_comm != FLT_MISS) ohpElecComm = *ohp_elec_comm;

      int ohpElecGalley = 0;
      int *ohp_elec_gall = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 9);
      if (*ohp_elec_gall != FLT_MISS) ohpElecGalley = *ohp_elec_gall;

      int sdApuBox = 0;
      int *sd_apu_box = link_dataref_int ("AirbusFBW/SDAPUBox");
      if (*sd_apu_box != INT_MISS) sdApuBox = *sd_apu_box;

      int sdExtPowBox = 0;
      int *sd_ext_pow_box = link_dataref_int ("AirbusFBW/SDExtPowBox");
      if (*sd_ext_pow_box != INT_MISS) sdExtPowBox = *sd_ext_pow_box;

      int sdACCrossConnect = 0;
      int *sd_ac_cross_connect = link_dataref_int ("AirbusFBW/SDACCrossConnect");
      if (*sd_ac_cross_connect != INT_MISS) sdACCrossConnect = *sd_ac_cross_connect;

      int sdELConnectLeft = 0;
      int *sd_el_connect_left = link_dataref_int ("AirbusFBW/SDELConnectLeft");
      if (*sd_el_connect_left != INT_MISS) sdELConnectLeft = *sd_el_connect_left;

      int sdELConnectRight = 0;
      int *sd_el_connect_right = link_dataref_int ("AirbusFBW/SDELConnectRight");
      if (*sd_el_connect_right != INT_MISS) sdELConnectRight = *sd_el_connect_right;

      int sdELConnectCenter = 0;
      int *sd_el_connect_center = link_dataref_int ("AirbusFBW/SDELConnectCenter");
      if (*sd_el_connect_center != INT_MISS) sdELConnectCenter = *sd_el_connect_center;

      int sdELBatterySupply = 0;
      int *sd_el_battery_supply = link_dataref_int ("AirbusFBW/SDELBatterySupply");
      if (*sd_el_battery_supply != INT_MISS) sdELBatterySupply = *sd_el_battery_supply;

      // Page SD_HYD (#4)

      int yElecMode = 0;
      int *y_elec_mode = link_dataref_int ("AirbusFBW/HydYElecMode");
      if (*y_elec_mode != INT_MISS) yElecMode = *y_elec_mode;

      int ptuMode = 0;
      int *ptu_mode = link_dataref_int ("AirbusFBW/HydPTUMode");
      if (*ptu_mode != INT_MISS) ptuMode = *ptu_mode;

      int ratMode = 0;
      int *rat_mode = link_dataref_int ("AirbusFBW/HydRATMode");
      if (*rat_mode != INT_MISS) ratMode = *rat_mode;

      float hydPressGreen = 0, hydPressYellow = 0, hydPressBlue = 0;
      float *hyd_pressGreen = link_dataref_flt_arr ("AirbusFBW/HydSysPressArray", 8, 0, -1);
      if (*hyd_pressGreen != INT_MISS) hydPressGreen = *hyd_pressGreen;
      float *hyd_pressYellow = link_dataref_flt_arr ("AirbusFBW/HydSysPressArray", 8, 1, -1);
      if (*hyd_pressYellow != INT_MISS) hydPressYellow = *hyd_pressYellow;
      float *hyd_pressBlue = link_dataref_flt_arr ("AirbusFBW/HydSysPressArray", 8, 2, -1);
      if (*hyd_pressBlue != INT_MISS) hydPressBlue = *hyd_pressBlue;

      int hydPumpGreen = 0, hydPumpYellow = 0, hydPumpBlue = 0;
      int *hyd_pumpGreen = link_dataref_int_arr ("AirbusFBW/HydPumpArray", 8, 0);
      if (*hyd_pumpGreen != INT_MISS) hydPumpGreen = *hyd_pumpGreen;
      int *hyd_pumpYellow = link_dataref_int_arr ("AirbusFBW/HydPumpArray", 8, 1);
      if (*hyd_pumpYellow != INT_MISS) hydPumpYellow = *hyd_pumpYellow;
      int *hyd_pumpBlue = link_dataref_int_arr ("AirbusFBW/HydPumpArray", 8, 2);
      if (*hyd_pumpBlue != INT_MISS) hydPumpBlue = *hyd_pumpBlue;

      float hydQtyGreen = 0, hydQtyYellow = 0, hydQtyBlue = 0;
      float *hyd_qtyGreen = link_dataref_flt_arr ("AirbusFBW/HydSysQtyArray", 8, 0, -1);
      if (*hyd_qtyGreen != INT_MISS) hydQtyGreen = *hyd_qtyGreen;
      float *hyd_qtyYellow = link_dataref_flt_arr ("AirbusFBW/HydSysQtyArray", 8, 1, -1);
      if (*hyd_qtyYellow != INT_MISS) hydQtyYellow = *hyd_qtyYellow;
      float *hyd_qtyBlue = link_dataref_flt_arr ("AirbusFBW/HydSysQtyArray", 8, 2, -1);
      if (*hyd_qtyBlue != INT_MISS) hydQtyBlue = *hyd_qtyBlue;

      int eng1FireExt = 0, eng2FireExt = 0;
      int *eng1_fire_ext = link_dataref_int_arr ("sim/cockpit2/engine/actuators/fire_extinguisher_on", 8, 0);
      if (*eng1_fire_ext != INT_MISS) eng1FireExt = *eng1_fire_ext;
      int *eng2_fire_ext = link_dataref_int_arr ("sim/cockpit2/engine/actuators/fire_extinguisher_on", 8, 1);
      if (*eng2_fire_ext != INT_MISS) eng2FireExt = *eng2_fire_ext;

      // Page SD_FUEL (#5)

      int fuelLeftPump1 = 0, fuelLeftPump2 = 0, fuelCenterPump1 = 0, fuelCenterPump2 = 0, fuelRightPump1 = 0, fuelRightPump2 = 0;
      int *fuel_pump = link_dataref_int_arr ("AirbusFBW/FuelPumpOHPArray", 16, -1);
      if (*fuel_pump != INT_MISS) { fuelLeftPump1 = fuel_pump[0]; fuelLeftPump2 = fuel_pump[1];
                                    fuelCenterPump1 = fuel_pump[2]; fuelCenterPump2 = fuel_pump[3];
                                    fuelRightPump1 = fuel_pump[4]; fuelRightPump2 = fuel_pump[5]; }

//      AirbusFBW/FuelAutoPumpSDArray int[16] 4 pos (image SD_FUEL_Pump_Auto) // Note: does not work as specified in PlaneMaker!
//      int fuelCenterPump1 = 0, fuelCenterPump2 = 0;
//      int *fuel_ct_pump = link_dataref_int_arr ("AirbusFBW/FuelAutoPumpSDArray", 16, -1);
//      if (*fuel_ct_pump != INT_MISS) { fuelCenterPump1 = fuel_ct_pump[0]; fuelCenterPump2 = fuel_ct_pump[1]; }

      int fuelXFV = 0;
      int *fuel_xfv = link_dataref_int_arr ("AirbusFBW/FuelXFVSDArray", 8, 0);
      if (*fuel_xfv != INT_MISS) fuelXFV = *fuel_xfv;

      int fuelTVLeft1 = 0, fuelTVLeft2 = 0,  fuelTVRight1 = 0, fuelTVRight2 = 0;
      int *fuel_transfer_valves = link_dataref_int_arr ("AirbusFBW/FuelTVSDArray", 8, -1);
      if (*fuel_transfer_valves != INT_MISS) { fuelTVLeft1 = fuel_transfer_valves[0]; fuelTVLeft2 = fuel_transfer_valves[1];
                                               fuelTVRight1 = fuel_transfer_valves[2]; fuelTVRight2 = fuel_transfer_valves[3]; }

      int eng1LPValve = 0, eng2LPValve = 0;
      int *fuel_lp_valves = link_dataref_int_arr ("AirbusFBW/ENGFuelLPValveArray", 4, -1);
      if (*fuel_lp_valves != INT_MISS) { eng1LPValve = fuel_lp_valves[0]; eng2LPValve = fuel_lp_valves[1]; }

      float fuelQuant1 = 0.0, fuelQuant2 = 0.0, fuelQuant3 = 0.0, fuelQuant4 = 0.0, fuelQuant5 = 0.0, fuelQuant = 0.0;
      float *fuel_quant = link_dataref_flt_arr ("sim/cockpit2/fuel/fuel_quantity", 9, -1, 0);
      if (*fuel_quant != FLT_MISS) { fuelQuant1 = fuel_quant[3]; fuelQuant2 = fuel_quant[1]; fuelQuant3 = fuel_quant[0];
                                     fuelQuant4 = fuel_quant[2]; fuelQuant5 = fuel_quant[4];
                                     fuelQuant = fuelQuant1 + fuelQuant2 + fuelQuant3 + fuelQuant4 + fuelQuant5; }

      float fuelFlow1 = 0.0, fuelFlow2 = 0.0, fuelFlow = 0.0;
      float *fuel_flow = link_dataref_flt_arr ("sim/cockpit2/engine/indicators/fuel_flow_kg_sec", 8, -1, -4);
      if (*fuel_flow != FLT_MISS) { fuelFlow1 = fuel_flow[0]; fuelFlow2 = fuel_flow[1]; fuelFlow = fuelFlow1 + fuelFlow2; }

      // Page SD_APU (#6)

      int apuMaster = 0;
      int *apu_master = link_dataref_int ("AirbusFBW/APUMaster");
      if (*apu_master != INT_MISS) apuMaster = *apu_master;

      int apuAvail = 0;
      int *apu_avail = link_dataref_int ("AirbusFBW/APUAvail");
      if (*apu_avail != INT_MISS) apuAvail = *apu_avail;

      int apuBldValve = 0;
      int *apu_bld_vlv = link_dataref_int ("AirbusFBW/APUBleedInd");
      if (*apu_bld_vlv != INT_MISS) apuBldValve = *apu_bld_vlv;

      float apuEgtLimit = 0;
      float *apu_egt_limit = link_dataref_flt ("AirbusFBW/APUEGTLimit", 0);
      if (*apu_egt_limit != FLT_MISS) apuEgtLimit = *apu_egt_limit;

      float apuEgt = 0;
      float *apu_egt = link_dataref_flt ("AirbusFBW/APUEGT", 0);
      if (*apu_egt != FLT_MISS) apuEgt = *apu_egt;

      float apuN = 0;
      float *apu_n = link_dataref_flt ("AirbusFBW/APUN", 0);
      if (*apu_n != FLT_MISS) apuN = *apu_n;

      // Page SD_COND (#7)

      float ckptTrimValve = 0, forwTrimValve = 0, aftTrimValve = 0;
      float *ckpt_trim_valve = link_dataref_flt ("AirbusFBW/CockpitTrim", -2);
      if (*ckpt_trim_valve != INT_MISS) ckptTrimValve = *ckpt_trim_valve;
      float *forw_trim_valve = link_dataref_flt ("AirbusFBW/Zone1Trim", -2);
      if (*forw_trim_valve != INT_MISS) forwTrimValve = *forw_trim_valve;
      float *aft_trim_valve = link_dataref_flt ("AirbusFBW/Zone2Trim", -2);
      if (*aft_trim_valve != INT_MISS) aftTrimValve = *aft_trim_valve;

      int cockpitTemp = 0, fwdCabinTemp = 0, aftCabinTemp = 0;
      int *cockpit_temp = link_dataref_int ("AirbusFBW/CockpitTemp");
      if (*cockpit_temp != INT_MISS) cockpitTemp = *cockpit_temp;
      int *fwd_cabin_temp = link_dataref_int ("AirbusFBW/FwdCabinTemp");
      if (*fwd_cabin_temp != INT_MISS) fwdCabinTemp = *fwd_cabin_temp;
      int *aft_cabin_temp = link_dataref_int ("AirbusFBW/AftCabinTemp");
      if (*aft_cabin_temp != INT_MISS) aftCabinTemp = *aft_cabin_temp;

      // Page SD_DOOR (#8)

      float bulkDoor = 0;
      float *bulk_door = link_dataref_flt ("AirbusFBW/BulkDoor", -1);
      if (*bulk_door != INT_MISS) bulkDoor = *bulk_door;

      float cargoDoor1 = 0, cargoDoor2 = 0;
      float *cargo_door1 = link_dataref_flt_arr ("AirbusFBW/CargoDoorArray", 4, 0, -1);
      if (*cargo_door1 != INT_MISS) cargoDoor1 = *cargo_door1;
      float *cargo_door2 = link_dataref_flt_arr ("AirbusFBW/CargoDoorArray", 4, 1, -1);
      if (*cargo_door2 != INT_MISS) cargoDoor2 = *cargo_door2;

      float paxDoor1 = 0, paxDoor2 = 0;
      float *pax_door1 = link_dataref_flt_arr ("AirbusFBW/PaxDoorArray", 20, 0, -1);
      if (*pax_door1 != INT_MISS) paxDoor1 = *pax_door1;
      float *pax_door2 = link_dataref_flt_arr ("AirbusFBW/PaxDoorArray", 20, 1, -1);
      if (*pax_door2 != INT_MISS) paxDoor2 = *pax_door2;

      // Page SD_WHEEL (#9)

      int spoiler1L = 0, spoiler1R = 0, spoiler2L = 0, spoiler2R = 0, spoiler3L = 0, spoiler3R = 0, 
          spoiler4L = 0, spoiler4R = 0, spoiler5L = 0, spoiler5R = 0;
      int *spoiler_1l = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 0);
      if (*spoiler_1l != INT_MISS) spoiler1L = *spoiler_1l;
      int *spoiler_1r = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 1);
      if (*spoiler_1r != INT_MISS) spoiler1R = *spoiler_1r;
      int *spoiler_2l = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 2);
      if (*spoiler_2l != INT_MISS) spoiler2L = *spoiler_2l;
      int *spoiler_2r = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 3);
      if (*spoiler_2r != INT_MISS) spoiler2R = *spoiler_2r;
      int *spoiler_3l = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 4);
      if (*spoiler_3l != INT_MISS) spoiler3L = *spoiler_3l;
      int *spoiler_3r = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 5);
      if (*spoiler_3r != INT_MISS) spoiler3R = *spoiler_3r;
      int *spoiler_4l = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 6);
      if (*spoiler_4l != INT_MISS) spoiler4L = *spoiler_4l;
      int *spoiler_4r = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 7);
      if (*spoiler_4r != INT_MISS) spoiler4R = *spoiler_4r;
      int *spoiler_5l = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 8);
      if (*spoiler_5l != INT_MISS) spoiler5L = *spoiler_5l;
      int *spoiler_5r = link_dataref_int_arr ("AirbusFBW/SDSpoilerArray", 16, 9);
      if (*spoiler_5r != INT_MISS) spoiler5R = *spoiler_5r;

      int brkRelL = 0, brkRelR = 0;
      int *brk_rel_l = link_dataref_int ("AirbusFBW/LeftBrakeRelease");
      if (*brk_rel_l != INT_MISS) brkRelL = *brk_rel_l;
      int *brk_rel_r = link_dataref_int ("AirbusFBW/RightBrakeRelease");
      if (*brk_rel_r != INT_MISS) brkRelR = *brk_rel_r;

      float leftMlgDoor = 0.0, rightMlgDoor = 0.0, noseGearDoor = 0.0;
      float *left_mlg_door = link_dataref_flt ("AirbusFBW/LeftMLGDoor", -3);
      if (*left_mlg_door != FLT_MISS) leftMlgDoor = *left_mlg_door;
      float *right_mlg_door = link_dataref_flt ("AirbusFBW/RightMLGDoor", -3);
      if (*right_mlg_door != FLT_MISS) rightMlgDoor = *right_mlg_door;
      float *nose_gear_door = link_dataref_flt ("AirbusFBW/NoseGearDoorsClosed", -3);
      if (*nose_gear_door != FLT_MISS) noseGearDoor = *nose_gear_door;
      float noseGearDeploy = 0.0, leftMainGearDeploy = 0.0, rightMainGearDeploy = 0.0;
      float *nose_gear_deploy = link_dataref_flt_arr ("sim/flightmodel2/gear/deploy_ratio", 10, 0, -3);
      if (*nose_gear_deploy != FLT_MISS) noseGearDeploy = *nose_gear_deploy;
      float *left_main_gear_deploy = link_dataref_flt_arr ("sim/flightmodel2/gear/deploy_ratio", 10, 1, -3);
      if (*left_main_gear_deploy != FLT_MISS) leftMainGearDeploy = *left_main_gear_deploy;
      float *right_main_gear_deploy = link_dataref_flt_arr ("sim/flightmodel2/gear/deploy_ratio", 10, 2, -3);
      if (*right_main_gear_deploy != FLT_MISS) rightMainGearDeploy = *right_main_gear_deploy;

      int lgciuInop = 0, hydSysBkgnd = 0;
      int *lgciu_inop = link_dataref_int ("sim/operation/failures/rel_gear_ind");
      if (*lgciu_inop != INT_MISS) lgciuInop = *lgciu_inop;
      int *hyd_sys_bkgnd = link_dataref_int ("sim/operation/failures/rel_gear_act");
      if (*hyd_sys_bkgnd != INT_MISS) hydSysBkgnd = *hyd_sys_bkgnd;

      int altnBrake = 0, nwsAvail = 0;
      int *altn_brake = link_dataref_int ("AirbusFBW/AltnBrake");
      if (*altn_brake != INT_MISS) altnBrake = *altn_brake;
      int *nws_avail = link_dataref_int ("AirbusFBW/NWSAvail");
      if (*nws_avail != INT_MISS) nwsAvail = *nws_avail;

      // Page SD_FCTL (#10)

      int elac1 = 0, elac2 = 0, sec1 = 0, sec2 = 0, sec3 = 0, fac1 = 0, fac2 = 0;
      int* fcc = link_dataref_int_arr ("AirbusFBW/FCCAvailArray", 16, -1);
      if (*fcc != INT_MISS) {
        elac1 = fcc[0]; elac2 = fcc[1];
        sec1 = fcc[2]; sec2 = fcc[3]; sec3 = fcc[4];
        fac1 = fcc[5]; fac2 = fcc[6];
      }

      int elevAvail = 0;
      int *elev_avail = link_dataref_int ("AirbusFBW/FCCElevAvail");
      if (*elev_avail != INT_MISS) elevAvail = *elev_avail;

      int lAilAvail = 0, rAilAvail = 0, lElevAvail = 0, rElevAvail = 0;
      int *l_ail_av = link_dataref_int_arr ("AirbusFBW/LAilAvailArray", 3, 0);
      if (*l_ail_av != INT_MISS) lAilAvail = *l_ail_av;
      int *r_ail_av = link_dataref_int_arr ("AirbusFBW/RAilAvailArray", 3, 0);
      if (*r_ail_av != INT_MISS) rAilAvail = *r_ail_av;
      int *l_elev_av = link_dataref_int_arr ("AirbusFBW/LElevAvailArray", 2, 0);
      if (*l_elev_av != INT_MISS) lElevAvail = *l_elev_av;
      int *r_elev_av = link_dataref_int_arr ("AirbusFBW/RElevAvailArray", 2, 0);
      if (*r_elev_av != INT_MISS) rElevAvail = *r_elev_av;

      float rtlPos = 0.0, ytrPos = 0.0;
      float *rtl_pos = link_dataref_flt ("AirbusFBW/RTLPosition", 0);
      if (*rtl_pos != FLT_MISS) rtlPos = *rtl_pos;
      float *ytr_pos = link_dataref_flt ("AirbusFBW/YawTrimPosition", 0);
      if (*ytr_pos != FLT_MISS) ytrPos = *ytr_pos;

      float elevTrim = 0.0, lElevDef = 0.0, rElevDef = 0.0;
      float *elev_trim = link_dataref_flt ("sim/flightmodel/controls/elv_trim", 0);
      if (*elev_trim != FLT_MISS) elevTrim = *elev_trim;
      float *l_elev_def = link_dataref_flt ("sim/flightmodel/controls/hstab1_elv1def", 0);
      if (*l_elev_def != FLT_MISS) lElevDef = *l_elev_def;
      float *r_elev_def = link_dataref_flt ("sim/flightmodel/controls/hstab2_elv1def", 0);
      if (*r_elev_def != FLT_MISS) rElevDef = *r_elev_def;
 
      float rudDef = 0.0, lAilDef = 0.0, rAilDef = 0.0;
      float *rud_def = link_dataref_flt ("sim/flightmodel/controls/vstab1_rud1def", 0);
      if (*rud_def != FLT_MISS) rudDef = *rud_def;
      float *l_ail_def = link_dataref_flt ("sim/flightmodel/controls/wing4l_ail1def", 0);
      if (*l_ail_def != FLT_MISS) lAilDef = *l_ail_def;
      float *r_ail_def = link_dataref_flt ("sim/flightmodel/controls/wing4r_ail1def", 0);
      if (*r_ail_def != FLT_MISS) rAilDef = *r_ail_def;

      // Page SD_CRUISE (#11)

      // all datarefs already handled for other pages

      // Page SD_STATUS (#12)

      unsigned char *SD_1a = link_dataref_byte_arr("AirbusFBW/SDline1a", 37, -1); if (strlen ((char*)SD_1a) > 37) SD_1a[37] = '\0';
      unsigned char *SD_1b = link_dataref_byte_arr("AirbusFBW/SDline1b", 37, -1); if (strlen ((char*)SD_1b) > 37) SD_1b[37] = '\0';
      unsigned char *SD_1g = link_dataref_byte_arr("AirbusFBW/SDline1g", 37, -1); if (strlen ((char*)SD_1g) > 37) SD_1g[37] = '\0';
      unsigned char *SD_1r = link_dataref_byte_arr("AirbusFBW/SDline1r", 37, -1); if (strlen ((char*)SD_1r) > 37) SD_1r[37] = '\0';
      unsigned char *SD_1w = link_dataref_byte_arr("AirbusFBW/SDline1w", 37, -1); if (strlen ((char*)SD_1w) > 37) SD_1w[37] = '\0';
      unsigned char *SD_2a = link_dataref_byte_arr("AirbusFBW/SDline2a", 37, -1); if (strlen ((char*)SD_2a) > 37) SD_2a[37] = '\0';
      unsigned char *SD_2b = link_dataref_byte_arr("AirbusFBW/SDline2b", 37, -1); if (strlen ((char*)SD_2b) > 37) SD_2b[37] = '\0';
      unsigned char *SD_2g = link_dataref_byte_arr("AirbusFBW/SDline2g", 37, -1); if (strlen ((char*)SD_2g) > 37) SD_2g[37] = '\0';
      unsigned char *SD_2r = link_dataref_byte_arr("AirbusFBW/SDline2r", 37, -1); if (strlen ((char*)SD_2r) > 37) SD_2r[37] = '\0';
      unsigned char *SD_2w = link_dataref_byte_arr("AirbusFBW/SDline2w", 37, -1); if (strlen ((char*)SD_2w) > 37) SD_2w[37] = '\0';
      unsigned char *SD_3a = link_dataref_byte_arr("AirbusFBW/SDline3a", 37, -1); if (strlen ((char*)SD_3a) > 37) SD_3a[37] = '\0';
      unsigned char *SD_3b = link_dataref_byte_arr("AirbusFBW/SDline3b", 37, -1); if (strlen ((char*)SD_3b) > 37) SD_3b[37] = '\0';
      unsigned char *SD_3g = link_dataref_byte_arr("AirbusFBW/SDline3g", 37, -1); if (strlen ((char*)SD_3g) > 37) SD_3g[37] = '\0';
      unsigned char *SD_3r = link_dataref_byte_arr("AirbusFBW/SDline3r", 37, -1); if (strlen ((char*)SD_3r) > 37) SD_3r[37] = '\0';
      unsigned char *SD_3w = link_dataref_byte_arr("AirbusFBW/SDline3w", 37, -1); if (strlen ((char*)SD_3w) > 37) SD_3w[37] = '\0';
      unsigned char *SD_4a = link_dataref_byte_arr("AirbusFBW/SDline4a", 37, -1); if (strlen ((char*)SD_4a) > 37) SD_4a[37] = '\0';
      unsigned char *SD_4b = link_dataref_byte_arr("AirbusFBW/SDline4b", 37, -1); if (strlen ((char*)SD_4b) > 37) SD_4b[37] = '\0';
      unsigned char *SD_4g = link_dataref_byte_arr("AirbusFBW/SDline4g", 37, -1); if (strlen ((char*)SD_4g) > 37) SD_4g[37] = '\0';
      unsigned char *SD_4r = link_dataref_byte_arr("AirbusFBW/SDline4r", 37, -1); if (strlen ((char*)SD_4r) > 37) SD_4r[37] = '\0';
      unsigned char *SD_4w = link_dataref_byte_arr("AirbusFBW/SDline4w", 37, -1); if (strlen ((char*)SD_4w) > 37) SD_4w[37] = '\0';
      unsigned char *SD_5a = link_dataref_byte_arr("AirbusFBW/SDline5a", 37, -1); if (strlen ((char*)SD_5a) > 37) SD_5a[37] = '\0';
      unsigned char *SD_5b = link_dataref_byte_arr("AirbusFBW/SDline5b", 37, -1); if (strlen ((char*)SD_5b) > 37) SD_5b[37] = '\0';
      unsigned char *SD_5g = link_dataref_byte_arr("AirbusFBW/SDline5g", 37, -1); if (strlen ((char*)SD_5g) > 37) SD_5g[37] = '\0';
      unsigned char *SD_5r = link_dataref_byte_arr("AirbusFBW/SDline5r", 37, -1); if (strlen ((char*)SD_5r) > 37) SD_5r[37] = '\0';
      unsigned char *SD_5w = link_dataref_byte_arr("AirbusFBW/SDline5w", 37, -1); if (strlen ((char*)SD_5w) > 37) SD_5w[37] = '\0';
      unsigned char *SD_6a = link_dataref_byte_arr("AirbusFBW/SDline6a", 37, -1); if (strlen ((char*)SD_6a) > 37) SD_6a[37] = '\0';
      unsigned char *SD_6b = link_dataref_byte_arr("AirbusFBW/SDline6b", 37, -1); if (strlen ((char*)SD_6b) > 37) SD_6b[37] = '\0';
      unsigned char *SD_6g = link_dataref_byte_arr("AirbusFBW/SDline6g", 37, -1); if (strlen ((char*)SD_6g) > 37) SD_6g[37] = '\0';
      unsigned char *SD_6r = link_dataref_byte_arr("AirbusFBW/SDline6r", 37, -1); if (strlen ((char*)SD_6r) > 37) SD_6r[37] = '\0';
      unsigned char *SD_6w = link_dataref_byte_arr("AirbusFBW/SDline6w", 37, -1); if (strlen ((char*)SD_6w) > 37) SD_6w[37] = '\0';
      unsigned char *SD_7a = link_dataref_byte_arr("AirbusFBW/SDline7a", 37, -1); if (strlen ((char*)SD_7a) > 37) SD_7a[37] = '\0';
      unsigned char *SD_7b = link_dataref_byte_arr("AirbusFBW/SDline7b", 37, -1); if (strlen ((char*)SD_7b) > 37) SD_7b[37] = '\0';
      unsigned char *SD_7g = link_dataref_byte_arr("AirbusFBW/SDline7g", 37, -1); if (strlen ((char*)SD_7g) > 37) SD_7g[37] = '\0';
      unsigned char *SD_7r = link_dataref_byte_arr("AirbusFBW/SDline7r", 37, -1); if (strlen ((char*)SD_7r) > 37) SD_7r[37] = '\0';
      unsigned char *SD_7w = link_dataref_byte_arr("AirbusFBW/SDline7w", 37, -1); if (strlen ((char*)SD_7w) > 37) SD_7w[37] = '\0';
      unsigned char *SD_8a = link_dataref_byte_arr("AirbusFBW/SDline8a", 37, -1); if (strlen ((char*)SD_8a) > 37) SD_8a[37] = '\0';
      unsigned char *SD_8b = link_dataref_byte_arr("AirbusFBW/SDline8b", 37, -1); if (strlen ((char*)SD_8b) > 37) SD_8b[37] = '\0';
      unsigned char *SD_8g = link_dataref_byte_arr("AirbusFBW/SDline8g", 37, -1); if (strlen ((char*)SD_8g) > 37) SD_8g[37] = '\0';
      unsigned char *SD_8r = link_dataref_byte_arr("AirbusFBW/SDline8r", 37, -1); if (strlen ((char*)SD_8r) > 37) SD_8r[37] = '\0';
      unsigned char *SD_8w = link_dataref_byte_arr("AirbusFBW/SDline8w", 37, -1); if (strlen ((char*)SD_8w) > 37) SD_8w[37] = '\0';
      unsigned char *SD_9a = link_dataref_byte_arr("AirbusFBW/SDline9a", 37, -1); if (strlen ((char*)SD_9a) > 37) SD_9a[37] = '\0';
      unsigned char *SD_9b = link_dataref_byte_arr("AirbusFBW/SDline9b", 37, -1); if (strlen ((char*)SD_9b) > 37) SD_9b[37] = '\0';
      unsigned char *SD_9g = link_dataref_byte_arr("AirbusFBW/SDline9g", 37, -1); if (strlen ((char*)SD_9g) > 37) SD_9g[37] = '\0';
      unsigned char *SD_9r = link_dataref_byte_arr("AirbusFBW/SDline9r", 37, -1); if (strlen ((char*)SD_9r) > 37) SD_9r[37] = '\0';
      unsigned char *SD_9w = link_dataref_byte_arr("AirbusFBW/SDline9w", 37, -1); if (strlen ((char*)SD_9w) > 37) SD_9w[37] = '\0';
      unsigned char *SD_10a = link_dataref_byte_arr("AirbusFBW/SDline10a", 37, -1); if (strlen ((char*)SD_10a) > 37) SD_10a[37] = '\0';
      unsigned char *SD_10b = link_dataref_byte_arr("AirbusFBW/SDline10b", 37, -1); if (strlen ((char*)SD_10b) > 37) SD_10b[37] = '\0';
      unsigned char *SD_10g = link_dataref_byte_arr("AirbusFBW/SDline10g", 37, -1); if (strlen ((char*)SD_10g) > 37) SD_10g[37] = '\0';
      unsigned char *SD_10r = link_dataref_byte_arr("AirbusFBW/SDline10r", 37, -1); if (strlen ((char*)SD_10r) > 37) SD_10r[37] = '\0';
      unsigned char *SD_10w = link_dataref_byte_arr("AirbusFBW/SDline10w", 37, -1); if (strlen ((char*)SD_10w) > 37) SD_10w[37] = '\0';    
      unsigned char *SD_11a = link_dataref_byte_arr("AirbusFBW/SDline11a", 37, -1); if (strlen ((char*)SD_11a) > 37) SD_11a[37] = '\0';
      unsigned char *SD_11b = link_dataref_byte_arr("AirbusFBW/SDline11b", 37, -1); if (strlen ((char*)SD_11b) > 37) SD_11b[37] = '\0';
      unsigned char *SD_11g = link_dataref_byte_arr("AirbusFBW/SDline11g", 37, -1); if (strlen ((char*)SD_11g) > 37) SD_11g[37] = '\0';
      unsigned char *SD_11r = link_dataref_byte_arr("AirbusFBW/SDline11r", 37, -1); if (strlen ((char*)SD_11r) > 37) SD_11r[37] = '\0';
      unsigned char *SD_11w = link_dataref_byte_arr("AirbusFBW/SDline11w", 37, -1); if (strlen ((char*)SD_11w) > 37) SD_11w[37] = '\0';
      unsigned char *SD_12a = link_dataref_byte_arr("AirbusFBW/SDline12a", 37, -1); if (strlen ((char*)SD_12a) > 37) SD_12a[37] = '\0';
      unsigned char *SD_12b = link_dataref_byte_arr("AirbusFBW/SDline12b", 37, -1); if (strlen ((char*)SD_12b) > 37) SD_12b[37] = '\0';
      unsigned char *SD_12g = link_dataref_byte_arr("AirbusFBW/SDline12g", 37, -1); if (strlen ((char*)SD_12g) > 37) SD_12g[37] = '\0';
      unsigned char *SD_12r = link_dataref_byte_arr("AirbusFBW/SDline12r", 37, -1); if (strlen ((char*)SD_12r) > 37) SD_12r[37] = '\0';
      unsigned char *SD_12w = link_dataref_byte_arr("AirbusFBW/SDline12w", 37, -1); if (strlen ((char*)SD_12w) > 37) SD_12w[37] = '\0';
      unsigned char *SD_13a = link_dataref_byte_arr("AirbusFBW/SDline13a", 37, -1); if (strlen ((char*)SD_13a) > 37) SD_13a[37] = '\0';
      unsigned char *SD_13b = link_dataref_byte_arr("AirbusFBW/SDline13b", 37, -1); if (strlen ((char*)SD_13b) > 37) SD_13b[37] = '\0';
      unsigned char *SD_13g = link_dataref_byte_arr("AirbusFBW/SDline13g", 37, -1); if (strlen ((char*)SD_13g) > 37) SD_13g[37] = '\0';
      unsigned char *SD_13r = link_dataref_byte_arr("AirbusFBW/SDline13r", 37, -1); if (strlen ((char*)SD_13r) > 37) SD_13r[37] = '\0';
      unsigned char *SD_13w = link_dataref_byte_arr("AirbusFBW/SDline13w", 37, -1); if (strlen ((char*)SD_13w) > 37) SD_13w[37] = '\0';
      unsigned char *SD_14a = link_dataref_byte_arr("AirbusFBW/SDline14a", 37, -1); if (strlen ((char*)SD_14a) > 37) SD_14a[37] = '\0';
      unsigned char *SD_14b = link_dataref_byte_arr("AirbusFBW/SDline14b", 37, -1); if (strlen ((char*)SD_14b) > 37) SD_14b[37] = '\0';
      unsigned char *SD_14g = link_dataref_byte_arr("AirbusFBW/SDline14g", 37, -1); if (strlen ((char*)SD_14g) > 37) SD_14g[37] = '\0';
      unsigned char *SD_14r = link_dataref_byte_arr("AirbusFBW/SDline14r", 37, -1); if (strlen ((char*)SD_14r) > 37) SD_14r[37] = '\0';
      unsigned char *SD_14w = link_dataref_byte_arr("AirbusFBW/SDline14w", 37, -1); if (strlen ((char*)SD_14w) > 37) SD_14w[37] = '\0';
      unsigned char *SD_15a = link_dataref_byte_arr("AirbusFBW/SDline15a", 37, -1); if (strlen ((char*)SD_15a) > 37) SD_15a[37] = '\0';
      unsigned char *SD_15b = link_dataref_byte_arr("AirbusFBW/SDline15b", 37, -1); if (strlen ((char*)SD_15b) > 37) SD_15b[37] = '\0';
      unsigned char *SD_15g = link_dataref_byte_arr("AirbusFBW/SDline15g", 37, -1); if (strlen ((char*)SD_15g) > 37) SD_15g[37] = '\0';
      unsigned char *SD_15r = link_dataref_byte_arr("AirbusFBW/SDline15r", 37, -1); if (strlen ((char*)SD_15r) > 37) SD_15r[37] = '\0';
      unsigned char *SD_15w = link_dataref_byte_arr("AirbusFBW/SDline15w", 37, -1); if (strlen ((char*)SD_15w) > 37) SD_15w[37] = '\0';
      unsigned char *SD_16a = link_dataref_byte_arr("AirbusFBW/SDline16a", 37, -1); if (strlen ((char*)SD_16a) > 37) SD_16a[37] = '\0';
      unsigned char *SD_16b = link_dataref_byte_arr("AirbusFBW/SDline16b", 37, -1); if (strlen ((char*)SD_16b) > 37) SD_16b[37] = '\0';
      unsigned char *SD_16g = link_dataref_byte_arr("AirbusFBW/SDline16g", 37, -1); if (strlen ((char*)SD_16g) > 37) SD_16g[37] = '\0';
      unsigned char *SD_16r = link_dataref_byte_arr("AirbusFBW/SDline16r", 37, -1); if (strlen ((char*)SD_16r) > 37) SD_16r[37] = '\0';
      unsigned char *SD_16w = link_dataref_byte_arr("AirbusFBW/SDline16w", 37, -1); if (strlen ((char*)SD_16w) > 37) SD_16w[37] = '\0';
      unsigned char *SD_17a = link_dataref_byte_arr("AirbusFBW/SDline17a", 37, -1); if (strlen ((char*)SD_17a) > 37) SD_17a[37] = '\0';
      unsigned char *SD_17b = link_dataref_byte_arr("AirbusFBW/SDline17b", 37, -1); if (strlen ((char*)SD_17b) > 37) SD_17b[37] = '\0';
      unsigned char *SD_17g = link_dataref_byte_arr("AirbusFBW/SDline17g", 37, -1); if (strlen ((char*)SD_17g) > 37) SD_17g[37] = '\0';
      unsigned char *SD_17r = link_dataref_byte_arr("AirbusFBW/SDline17r", 37, -1); if (strlen ((char*)SD_17r) > 37) SD_17r[37] = '\0';
      unsigned char *SD_17w = link_dataref_byte_arr("AirbusFBW/SDline17w", 37, -1); if (strlen ((char*)SD_17w) > 37) SD_17w[37] = '\0';
      unsigned char *SD_18a = link_dataref_byte_arr("AirbusFBW/SDline18a", 37, -1); if (strlen ((char*)SD_18a) > 37) SD_18a[37] = '\0';
      unsigned char *SD_18b = link_dataref_byte_arr("AirbusFBW/SDline18b", 37, -1); if (strlen ((char*)SD_18b) > 37) SD_18b[37] = '\0';
      unsigned char *SD_18g = link_dataref_byte_arr("AirbusFBW/SDline18g", 37, -1); if (strlen ((char*)SD_18g) > 37) SD_18g[37] = '\0';
      unsigned char *SD_18r = link_dataref_byte_arr("AirbusFBW/SDline18r", 37, -1); if (strlen ((char*)SD_18r) > 37) SD_18r[37] = '\0';
      unsigned char *SD_18w = link_dataref_byte_arr("AirbusFBW/SDline18w", 37, -1); if (strlen ((char*)SD_18w) > 37) SD_18w[37] = '\0';
  
      glPushMatrix ();

//=======================The actual rendering stuff============================

//sdPage = 5; // testing only!

      int rot = 0;
      char buffer[32];

      if (sdPage <= 12) {
        // The "permanent" background lines and data

        glLineWidth (3);
        glColor3ub (COLOR_WHITE);
        glBegin (GL_LINES);
          glVertex2d ( 10, 70);
          glVertex2d (450, 70);
          glVertex2d (150, 65);
          glVertex2d (150, 15);
          glVertex2d (310, 65);
          glVertex2d (310, 15);
        glEnd ();

        // Fixed texts

        glColor3ub (COLOR_WHITE);
        m_pFontManager->SetSize (m_Font, 14, 14);
        m_pFontManager->Print ( 30,  44, "TAT", m_Font);
        m_pFontManager->Print ( 30,  20, "SAT", m_Font);
        m_pFontManager->Print (320,  44, "GW",  m_Font);
        glColor3ub (COLOR_CYAN);
        m_pFontManager->Print (106,  44, ";",  m_Font);
        m_pFontManager->Print (106,  20, ";",  m_Font);
        m_pFontManager->Print (420,  44, "KG",  m_Font);
        glColor3ub (COLOR_GREEN);
        m_pFontManager->SetSize (m_Font, 12, 12);
        m_pFontManager->Print (220,  20, "H",   m_Font);

        // Values
        m_pFontManager->SetSize (m_Font, 14, 14);
        sprintf (buffer, "%6.0f", grossWeight);
        m_pFontManager->Print (345,  44, buffer, m_Font); // Total weight
        sprintf (buffer, "%2.0f", sat);
        m_pFontManager->Print ( 75,  44, buffer, m_Font); // Static Air Temperature
        sprintf (buffer, "%2.0f", tat);
        m_pFontManager->Print ( 75,  20, buffer, m_Font); // Total Air Temperature
        sprintf (buffer, "%02i", utcHours);
        m_pFontManager->Print (193,  20, buffer, m_Font); // UTC hours
        sprintf (buffer, "%02i", utcMinutes);
        m_pFontManager->Print (235,  20, buffer, m_Font); // UTC Minutes
      }

      glColor3ub (COLOR_WHITE);
      switch (sdPage) {
        case 0: // ENG Page
#include "sdeng.c"
          break;
        case 1: // BLEED Page
#include "sdbleed.c"
          break;
        case 2: // PRESS Page
#include "sdpress.c"
          break;
        case 3: // ELEC Page
#include "sdelec.c"
          break;
        case 4: // HYD Page
#include "sdhyd.c"
          break;
        case 5: // FUEL Page
#include "sdfuel.c"
          break;
        case 6: // APU Page
#include "sdapu.c"
          break;
        case 7: // COND Page
#include "sdcond.c"
          break;
        case 8: // DOOR Page
#include "sddoor.c"
          break;
        case 9: // WHEEL Page
#include "sdwheel.c"
          break;
        case 10: // FCTL Page
#include "sdfctl.c"
          break;
        case 11: // CRUISE Page
#include "sdcruise.c"
          break;
        case 12: // STS Page
#include "sdsts.c"
          break;
        case 13:
//          SD_EWD_mapped ();
          break;
        default:
          // nothing to be done here
        break;
      }

    glPopMatrix ();

    // Display the framerate
//    glColor3ub (COLOR_WHITE);
//    m_pFontManager->SetSize (m_Font, 12, 12);
//    sprintf (buffer, "%3.0f", A320SDWidget::calculateFPS (simTime));
//    m_pFontManager->Print (210, 2, buffer, m_Font);

        } // end if (! coldAndDark)

  } // end Render()

}
