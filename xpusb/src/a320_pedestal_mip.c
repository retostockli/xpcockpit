/* This is the a320_pedestal_mip.c code which contains code for how to communicate with my Airbus 
   A320 hardware (switches, LED's, displays) connected to the OpenCockpits IOCARDS USB device.

   To be used with the QPAC Basic A320 Airbus V2.04.

   Copyright (C) 2014  Hans Jansen, inspired by Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libiocards.h"
#include "serverdata.h"
#include "a320_pedestal_mip.h"

/* enumerations */
/* Notice that these depend on the hardware wiring! */
/* Also notice that input/output numbers run *per card*, not contiguously as in the hardware! */
enum inputs { // pushbuttons PB_, switches SW_, rotary encoders RE_
	// master card 1 (Pedestal)
	PB_lttest = 0,													//  0
	SW_parkbrkoff = 1,	SW_parkbrkon,	PB_rdtrres,	PB_rdtrrt,		PB_rdtrlt,
	SW_spdbrkarm,	SW_spdbrknarm,	SW_eng1master,	SW_eng2master,	SW_flapsup,		//  1 - 10
	SW_flaps1,		SW_flaps2,		SW_flaps3,		SW_flapsfull,	SW_ignnorm,
	SW_igncont,		SW_igncrank,	PB_ecam_eng,	PB_ecam_cond,	PB_ecam_apu,	// 11 - 20
	PB_ecam_sts,	PB_ecam_clrl,	PB_ecam_press,	PB_ecam_tocf,	PB_ecam_door,
	PB_ecam_bleed,	PB_ecam_hyd,	PB_ecam_canc,	PB_ecam_fctl,	PB_ecam_fuel,	// 21 - 30
	PB_ecam_rcl,	PB_ecam_all,	PB_ecam_wheel,	PB_ecam_clrr,	PB_ecam_elec,
	SW_pc1_36,		SW_pc1_37,		SW_pc1_38,		SW_pc1_39,		SW_pc1_40,		// 31 - 40
	SW_pc1_41,		SW_pc1_42,		SW_pc1_43,		SW_pc1_44,		SW_pc1_45,
	SW_pc1_46,		SW_pc1_47,		SW_pc1_48,		SW_pc1_49,		SW_pc1_50,		// 41 - 50
	SW_pc1_51,		SW_pc1_52,		SW_pc1_53,		SW_pc1_54,		SW_pc1_55,
	SW_pc1_56,		SW_pc1_57,		SW_pc1_58,		SW_pc1_59,		SW_pc1_60,		// 51 - 60
	SW_pc1_61,		SW_pc1_62,		SW_pc1_63,		SW_pc1_64,		SW_pc1_65,
	SW_pc1_66,		SW_pc1_67,		SW_pc1_68,		SW_pc1_69,		SW_pc1_70,		// 61 - 70
	SW_pc1_71,																		// 71
	// master card 2 (MIP and Glareshield)
	SW_fcuencspdb=0,
	SW_fcuencaltb,		SW_fcuencvsb,		SW_fcuenchdgb,
	SW_fcuencalta,		SW_fcuenchdga,		SW_fcuencvsa,		SW_pc2_79,			SW_fcuencspda,	//  72 - 80
	SW_fcuenchdgpull,	SW_fcuencspdpush,	SW_fcuencvspush,	SW_fcuencvspull,	SW_fcuencspdpull,
	SW_fcupbmetricalt,	SW_fcuencaltpull,	SW_fcuencaltpush,	SW_fcuenchdgpush,	PB_mipabrkmed,	//  81 - 90
	PB_mipabrklo,		SW_miplgup,			SW_mipnwsteer,		SW_mippbterrnd,		SW_miplgdown,
	PB_mipabrkmax,		SW_mipbrkfan,		SW_pc2_98,			SW_fcupbathr,		SW_fcupbselalt,	//  91 - 100
	SW_fcupbap1,		SW_fcupbhdgtrk,		SW_fcupbap2,		SW_fcupbappr,		SW_fcupbexped,
	SW_fcupbspdmach,	SW_fcupbloc,		SW_efisencb,		SW_efispbwpt,		SW_efispbvord,	// 101 -110
	SW_efispbndb,		SW_efispbcstr,		SW_efisenca,		SW_efispbarpt,		SW_efisencpull,
	SW_efisencpush,		SW_efispbils,		SW_efisrng320,		SW_efisrng160,		SW_efisrng40,	// 111 - 120
	SW_efisrng80,		SW_efispbfd,		SW_efisinhghpa,		SW_efisrng10,		SW_efisrng20,
	SW_efiss1_1,		SW_efismodearc,		SW_efismodeplan,	SW_efismodenav,		SW_efismodevor,	// 121 - 130
	SW_efiss1_2,		SW_efiss2_2,		SW_efismodeils,		SW_efiss2_1,		PB_wingautoland,
	PB_wingwarning,		PB_wingcaution,		PB_wingchrono,		SW_mipgpws,			SW_mipxfr,		// 131 - 140
	PB_wingstprio,		SW_pc2_142,																	// 141 - 142
	last_input
	};
enum axes {	// Axes AX_ (from USB expander!)
	AX_spdbrk=0,																					//  1 -  4
	last_axis
	};
enum displays { // 7-segment displays DP_
	DP_rudtrim=1,		DP_rudtrim_SZ = 3,	DP_rudtrim_NEG = 1,	DP_no_NEG = 0						//  1 -  3
	};
enum outputs { // Led-lights LT_
	// master card 1 (Pedestal)
	LT_eng1fault=11,	LT_eng1fire,	LT_eng2fault,		LT_eng2fire,	LT_ecam_sts,
	LT_ecam_elec,		LT_ecam_eng,	LT_ecam_wheel,		LT_ecam_apu,	LT_ecam_hyd,			// 11 - 20
	LT_ecam_cond,		LT_ecam_fctl,	LT_ecam_bleed,		LT_ecam_fuel,	LT_ecam_press,
	LT_ecam_clr,		LT_ecam_door,	LT_pc1_28,			LT_pc1_29,		LT_pc1_30,				// 21 - 30
	LT_pc1_31,			LT_pc1_32,		LT_pc1_33,			LT_pc1_34,		LT_pc1_35,
	LT_pc1_36,			LT_pc1_37,		LT_mcduFM1,			LT_mcduIND,		LT_mcduRDY,				// 31 - 40
	LT_mcduinop,		LT_mcduFM2,		LT_mcduFAIL,		LT_mcduFM,		LT_mcduMENUupp,
	LT_mcduMENUlow,		LT_pc1_47,		LT_pc1_48,			LT_pc1_49,		LT_pc1_50,				// 41 - 50
	LT_pc1_51,			LT_pc1_52,		LT_pc1_53,			LT_pc1_54,								// 51 - 54
	// master card 2 (MIP and Glareshield)
	LT_fculoc=11,		LT_abrkmax,		LT_fcuathr,			LT_abrkmed,		LT_fcuexped,			// 75 - 80
	LT_abrklo,			LT_fcuappr,		LT_abrkterrnd,		LT_fcuap1,		LT_abrkfan,
	LT_fcuap2,			LT_lgrightlock,	LT_fcudpvs3,		LT_lgnoselock,	LT_fcualt,				// 81 - 90
	LT_lgleftlock,		LT_fcuhdg,		LT_abrkdecmax,		LT_fcuspd,		LT_abrkdecmed,
	LT_fcudpspd1,		LT_abrkdeclo,	LT_efiscstr,		LT_abrkhot,		LT_efisarpt,			// 91 - 100
	LT_lgrighttransit,	LT_efisvord,	LT_lgnosetransit,	LT_efisndb,		LT_lglefttransit,
	LT_efiswpt,			LT_lgwarnarrow,	LT_efisfd,			LT_efisils,		LT_pc2_45,				// 101 - 110
	LT_efisdpbaro3,		LT_pc2_47,		LT_pc2_48,			LT_mipgpws,		LT_mipgs,
	LT_wingautoland,	LT_wingwarning,	LT_wingcaution,	LT_wingstkprioarrow,	LT_wingstkpriocapt, // 111 - 119
	last_output
};

// Here distinguish between possible A320 versions (recompilation necessary)
//#define _phager_airbus
# define _qpac_airbus
// Note that the phager stuff may be incomplete, and is untested!

// Static variables

int efis_baro_unit = 0, fcu_alt_unit = 0;
int chronoButtonPressed = 0, ecamKeyPressed = 0;

// Handle the lights testing and cold&dark states for all light outputs
int ltTestPed = 0, coldDarkPed = 2;
int lightCheckPed (int input) {
  return ((input || ltTestPed) && (coldDarkPed != 2));
}

void a320_pedestal_mip(void)
{

  int device = 0; // as defined by ini file!
  int card1 = 0, card2 = 1;
  int ret = 0;
  int temp = 0, temp1 = 0, temp2 = 0;

  // The selector for left/right MCDU
  // Note: this is maintained in the a320_mcdu_keys.c module
  extern int mcdu_select;

  /* Request X-Plane variables */

/*
 * Set initial brightness of displays until hardware available
    AirbusFBW/DUBrightness[0]	Pilot PFD
    AirbusFBW/DUBrightness[1]	Pilot ND
    AirbusFBW/DUBrightness[6]	Copilot PFD
    AirbusFBW/DUBrightness[3]	Copilot ND
    AirbusFBW/DUBrightness[4]	EWD
    AirbusFBW/DUBrightness[5]	SD
    AirbusFBW/DUBrightness[6]	MCDU1
    AirbusFBW/DUBrightness[7]	MCDU2
    AirbusFBW/WXAlphaND1([1])	(Pilot ND?)
    AirbusFBW/WXAlphaND2([3])	(Copilot ND?)
 */
  float *pf_pfd_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 0, 0);
  float *pf_nd_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 1, 0);
  float *cp_pfd_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 2, 0);
  float *cp_nd_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 3, 0);
  float *ewd_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 4, 0);
  float *sd_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 5, 0);
  float *mcdu1_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 6, 0);
  float *mcdu2_brt = link_dataref_flt_arr("AirbusFBW/DUBrightness", 8, 7, 0);
  *pf_pfd_brt = *pf_nd_brt = *cp_pfd_brt = *cp_nd_brt = *ewd_brt = *sd_brt = *mcdu1_brt = *mcdu2_brt = 1.0;
  float *wx1_brt = link_dataref_flt("AirbusFBW/WXAlphaND1", -2);
  float *wx2_brt = link_dataref_flt("AirbusFBW/WXAlphaND2", -2);
  *wx1_brt = *wx2_brt = 0.5;

  // Note: these two datarefs are maintained by the a320_overhead.c module
  int *cold_dark_ped = link_dataref_int ("xpserver/cold_and_dark");
  if (*cold_dark_ped == INT_MISS) coldDarkPed = 0; else coldDarkPed = *cold_dark_ped;
  int *lights_test_ped = link_dataref_int ("xpserver/lights_test");
  if (*lights_test_ped == INT_MISS) ltTestPed = 0; else ltTestPed = *lights_test_ped;

  /* Rear Pedestal */
  int *sw_park_brake = link_dataref_int("AirbusFBW/ParkBrake");
  float *flap_ratio = link_dataref_flt("sim/cockpit2/controls/flap_ratio", -2);
  float *speedbrake_ratio = link_dataref_flt("sim/cockpit2/controls/speedbrake_ratio", -2);
  int *pb_rudder_trim_left = link_dataref_cmd_hold("sim/flight_controls/rudder_trim_left");
  int *pb_rudder_trim_right = link_dataref_cmd_hold("sim/flight_controls/rudder_trim_right");
#ifdef _phager_airbus
  int *pb_rudder_trim_center = link_dataref_cmd_once("sim/flight_controls/rudder_trim_center");
#endif
#ifdef _qpac_airbus
  int *pb_rudder_trim_center = link_dataref_cmd_hold("sim/flight_controls/rudder_trim_center");
#endif
  /* Front Pedestal */
  // Engines Panel
  int *sw_engine_mode = link_dataref_int("AirbusFBW/ENGModeSwitch");
#ifdef _phager_airbus
  int *sw_eng1_master = link_dataref_int("AirbusFBW/ENG1_masterPA");
  int *sw_eng2_master = link_dataref_int("AirbusFBW/ENG2_masterPA");
#endif
#ifdef _qpac_airbus
  int *sw_eng1_master = link_dataref_int("AirbusFBW/ENG1MasterSwitch");
  int *sw_eng2_master = link_dataref_int("AirbusFBW/ENG2MasterSwitch");
#endif
  float *eng1_egt = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 0, 0);
  float *eng2_egt = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 1, 0);
  int *eng1_fire = link_dataref_int("sim/operation/failures/rel_engfir0");
  int *eng2_fire = link_dataref_int("sim/operation/failures/rel_engfir1");
  // TCAS Panel
  // Audio Mgt Panel
  // Radio Mgt Panel
  // MCDU keys	 (Note: MCDU keys are handled in separate module a320_mcdu_keys.c)
  // MCDU Screen (Note: screen is handled in OpenGC module ogcA320MCDU.cpp)
  // ECAM Panel
//  int *pb_ecam_all = link_dataref_int("AirbusFBW/ALL");		// not yet implemented in QPAC
  int *pb_ecam_apu = link_dataref_int("AirbusFBW/SDAPU");
  int *pb_ecam_bleed = link_dataref_int("AirbusFBW/SDBLEED");
  int *pb_ecam_cond = link_dataref_int("AirbusFBW/SDCOND");
  int *pb_ecam_door = link_dataref_int("AirbusFBW/SDDOOR");
  int *pb_ecam_elec = link_dataref_int("AirbusFBW/SDELEC");
//  int *pb_ecam_canc = link_dataref_int("AirbusFBW/EMCANC");	// not yet implemented in QPAC
  int *pb_ecam_eng = link_dataref_int("AirbusFBW/SDENG");
  int *pb_ecam_fctl = link_dataref_int("AirbusFBW/SDFCTL");
  int *pb_ecam_fuel = link_dataref_int("AirbusFBW/SDFUEL");
  int *pb_ecam_hyd = link_dataref_int("AirbusFBW/SDHYD");
  int *pb_ecam_press = link_dataref_int("AirbusFBW/SDPRESS");
//  int *pb_ecam_rcl = link_dataref_int("AirbusFBW/RCL");		// not yet implemented in QPAC
  int *pb_ecam_sts = link_dataref_int("AirbusFBW/SDSTATUS");
  int *pb_ecam_wheel = link_dataref_int("AirbusFBW/SDWHEEL");
  int *lt_ecam_apu = link_dataref_int("AirbusFBW/SDAPU");
  int *lt_ecam_bleed = link_dataref_int("AirbusFBW/SDBLEED");
  int *lt_ecam_cond = link_dataref_int("AirbusFBW/SDCOND");
  int *lt_ecam_door = link_dataref_int("AirbusFBW/SDDOOR");
  int *lt_ecam_elec = link_dataref_int("AirbusFBW/SDELEC");
  int *lt_ecam_eng = link_dataref_int("AirbusFBW/SDENG");
  int *lt_ecam_fctl = link_dataref_int("AirbusFBW/SDFCTL");
  int *lt_ecam_fuel = link_dataref_int("AirbusFBW/SDFUEL");
  int *lt_ecam_hyd = link_dataref_int("AirbusFBW/SDHYD");
  int *lt_ecam_press = link_dataref_int("AirbusFBW/SDPRESS");
  int *lt_ecam_wheel = link_dataref_int("AirbusFBW/SDWHEEL");
  int *lt_ecam_sts = link_dataref_int("AirbusFBW/SDSTATUS");
  int *lt_ecam_clr = link_dataref_int("AirbusFBW/CLRillum");
  int *pb_ecam_tocf = link_dataref_cmd_once("AirbusFBW/TOConfigPress");
  int *pb_ecam_clr = link_dataref_cmd_once("sim/annunciator/clear_master_accept");
  /* MIP & Glareshield */
  // Landing Gear & Auto Brake Panel
  int *sw_lgear_lever = link_dataref_int("AirbusFBW/GearLever");
  int *lt_lgear_warning = link_dataref_int("sim/cockpit2/annunciators/gear_warning");
  int *sw_nw_steer = link_dataref_int("AirbusFBW/NWSnAntiSkid");
  int *abrk_level = link_dataref_int("sim/cockpit2/switches/auto_brake_level");
  float *lgear_ratio = link_dataref_flt_arr("sim/flightmodel2/gear/deploy_ratio", 10, -1, -2);
  // FCU
  int *alt100_1000 = link_dataref_int("AirbusFBW/ALT100_1000");
  // EFIS Capt.
  int *barounitcapt = link_dataref_int("AirbusFBW/BaroUnitCapt");
  // Wing Capt.
  int *pb_master_warning = link_dataref_cmd_once("sim/annunciator/clear_master_warning");
  int *pb_master_caution = link_dataref_cmd_once("sim/annunciator/clear_master_caution");
  int *pb_capt_chrono = link_dataref_cmd_once("AirbusFBW/CaptChronoButton");
#ifdef _phager_airbus
  int *lt_master_warning = link_dataref_int("com/petersaircraft/airbus/Master_W_Flash");
  int *lt_master_caution = link_dataref_int("com/petersaircraft/airbus/Master_Cau");
  int *lt_autoland = link_dataref_int("sim/cockpit2/gauges/indicators/radio_altimeter_ft_pilot");
#endif
#ifdef _qpac_airbus
  int *lt_master_warning = link_dataref_int("sim/cockpit/warnings/annunciators/master_warning");
  int *lt_master_caution = link_dataref_int("sim/cockpit/warnings/annunciators/master_caution");
#endif
  int *pb_abrk_lo = link_dataref_cmd_once("AirbusFBW/AbrkLo");
  int *pb_abrk_max = link_dataref_cmd_once("AirbusFBW/AbrkMax");
  int *pb_abrk_med = link_dataref_cmd_once("AirbusFBW/AbrkMed");
  int *abrk_lo_ind = link_dataref_int("AirbusFBW/AutoBrkLo");
  int *abrk_max_ind = link_dataref_int("AirbusFBW/AutoBrkMax");
  int *abrk_med_ind = link_dataref_int("AirbusFBW/AutoBrkMed");
  int *lg_leftgear_ind = link_dataref_int("AirbusFBW/LeftGearInd");
  int *lg_nosegear_ind = link_dataref_int("AirbusFBW/NoseGearInd");
  int *lg_rightgear_ind = link_dataref_int("AirbusFBW/RightGearInd");

  /* local variables */
  int parkbrake_on;
  int flaps_up, flaps_1, flaps_2, flaps_3, flaps_full;
  int speedbrake_armswitch;
  float speedbrake_axis;
  int ign_crank, ign_norm, ign_cont, eng_mode;
  int gear_handle_up, gear_handle_down;

  // master card 0, inputs (000-071)
  // master card 1, inputs (072-143) (...)

  /* read inputs */

  /* Parking Brake */ /* Parking Brake: 0 = off, 1 = on */
  ret = digital_input(device,card1,SW_parkbrkon,&parkbrake_on,0);
  *sw_park_brake = (parkbrake_on) ? 1 : 0;

  /* Flaps handle */ /* Flaps: 0.0 = up, 1.0 = full down */
  ret = digital_input(device,card1,SW_flapsup,&flaps_up,0);
  ret = digital_input(device,card1,SW_flaps1,&flaps_1,0);
  ret = digital_input(device,card1,SW_flaps2,&flaps_2,0);
  ret = digital_input(device,card1,SW_flaps3,&flaps_3,0);
  ret = digital_input(device,card1,SW_flapsfull,&flaps_full,0);
  if (flaps_up) { *flap_ratio = 0.0; }
  else if (flaps_1) { *flap_ratio = 0.25; }
  else if (flaps_2) { *flap_ratio = 0.5; }
  else if (flaps_3) { *flap_ratio = 0.75; }
  else if (flaps_full) { *flap_ratio = 1.0; }

  /* Speedbrake handle */ /* Speedbrake: -0.5 = armed, 0.0 = down, 1.0 = up */
  ret = digital_input(device,card1,SW_spdbrkarm,&speedbrake_armswitch,0);
  if (speedbrake_armswitch) { *speedbrake_ratio = -0.5; }
  else {
    *speedbrake_ratio = 0.0;
    ret = axis_input(device,AX_spdbrk,&speedbrake_axis,0.0,255.0);
    if (speedbrake_axis < 40) *speedbrake_ratio = 0.0;
    else if ((speedbrake_axis >= 40) && (speedbrake_axis < 100)) *speedbrake_ratio = 0.25;
    else if ((speedbrake_axis >= 100) && (speedbrake_axis < 140)) *speedbrake_ratio = 0.5;
    else if ((speedbrake_axis >= 140) && (speedbrake_axis < 180)) *speedbrake_ratio = 0.75;
    else if (speedbrake_axis >= 180) *speedbrake_ratio = 1.0;
//printf ("Speed Brake position: %3f, ratio: %3f\n", speedbrake_axis, *speedbrake_ratio);
  }

  /* Rudder trim switches */ 
  // trim left
  ret = digital_input(device,card1,PB_rdtrlt,&temp,0);
  if (temp) { *pb_rudder_trim_left = 1; }
  else { *pb_rudder_trim_left = 0; }
  // trim right
  ret = digital_input(device,card1,PB_rdtrrt,&temp,0);
  if (temp) { *pb_rudder_trim_right = 1; }
  else { *pb_rudder_trim_right = 0; }
  // trim reset
  ret = digital_input(device,card1,PB_rdtrres,&temp,0);
  if (temp) { *pb_rudder_trim_center = 1; }
  else { *pb_rudder_trim_center = 0; }

  /* Engine panel switches */
  ret = digital_input(device,card1,SW_igncrank,&ign_crank,0);
  ret = digital_input(device,card1,SW_ignnorm,&ign_norm,0);
  ret = digital_input(device,card1,SW_igncont,&ign_cont,0);
  if (ign_norm == 1) eng_mode = 1;
  else if (ign_cont == 1) eng_mode = 2;
  else if (ign_crank == 1) eng_mode = 0;
  *sw_engine_mode = eng_mode;
  ret = digital_input(device,card1,SW_eng1master,sw_eng1_master,0);
  ret = digital_input(device,card1,SW_eng2master,sw_eng2_master,0);

  /* ECAM Switching Panel */
  // selector buttons
  ret = digital_input(device,card1,PB_ecam_apu,pb_ecam_apu,1);
  ret = digital_input(device,card1,PB_ecam_bleed,pb_ecam_bleed,1);
  ret = digital_input(device,card1,PB_ecam_cond,pb_ecam_cond,1);
  ret = digital_input(device,card1,PB_ecam_door,pb_ecam_door,1);
  ret = digital_input(device,card1,PB_ecam_elec,pb_ecam_elec,1);
//  ret = digital_input(device,card1,PB_ecam_canc,pb_ecam_canc,1);				// not yet implemented in QPAC
  ret = digital_input(device,card1,PB_ecam_eng,pb_ecam_eng,1);
  ret = digital_input(device,card1,PB_ecam_fctl,pb_ecam_fctl,1);
  ret = digital_input(device,card1,PB_ecam_fuel,pb_ecam_fuel,1);
  ret = digital_input(device,card1,PB_ecam_hyd,pb_ecam_hyd,1);
  ret = digital_input(device,card1,PB_ecam_press,pb_ecam_press,1);
//  ret = digital_input(device,card1,PB_ecam_rcl,pb_ecam_rcl,1);				// not yet implemented in QPAC
  ret = digital_input(device,card1,PB_ecam_wheel,pb_ecam_wheel,1);
  ret = digital_input(device,card1,PB_ecam_sts,pb_ecam_sts,1);
//  ret = digital_input(device,card1,PB_ecam_all,pb_ecam_all,1);				// not yet implemented in QPAC
  // command buttons
  ret = digital_input(device,card1,PB_ecam_clrl,&temp,0);
  if (temp == 1) ret = digital_input(device,card1,PB_ecam_clrl,pb_ecam_clr,0);
  else ret = digital_input(device,card1,PB_ecam_clrr,pb_ecam_clr,0);
  ret = digital_input(device,card1,PB_ecam_tocf,pb_ecam_tocf,0);

  /* Landing Gear & Auto Brake Panel */
  ret = digital_input(device,card2,SW_miplgup,&gear_handle_up,0);
  ret = digital_input(device,card2,SW_miplgdown,&gear_handle_down,0);
  if (gear_handle_up == 1) {
    *sw_lgear_lever = 0;
  } else {
    if (gear_handle_down == 1) {
      *sw_lgear_lever = 1;
    }
  }
  ret = digital_input(device,card2,PB_mipabrklo,pb_abrk_lo,0);
  ret = digital_input(device,card2,PB_mipabrkmed,pb_abrk_med,0);
  ret = digital_input(device,card2,PB_mipabrkmax,pb_abrk_max,0);
  ret = digital_input(device,card2,SW_mipnwsteer,sw_nw_steer,0);

//  printf("A/Brk level: %i \n",*abrk_level);
  ret = digital_input(device,card2,PB_mipabrklo,&temp,0); // ABRK-LO
  if (temp)  *abrk_level = 2;
  ret = digital_input(device,card2,PB_mipabrkmed,&temp,0); // ABRK-MED
  if (temp)  *abrk_level = 4;
  ret = digital_input(device,card2,PB_mipabrkmax,&temp,0); // ABRK-MAX
  if (temp)  *abrk_level = 5;
//  printf("A/Brk level:  %i \n",*abrk_level);

  ret = digital_input(device,card2,SW_mipbrkfan,&temp,0); // Brake Fan On
  ret = digital_input(device,card2,SW_mippbterrnd,&temp,0); // Terr On ND

//  ret = digital_input(device,card2,SW_mipgpws,&temp,0); // GPWS (// not yet implemented in QPAC)
//  if (temp) {
//    printf ("SW GPWS selected\n");
//  }
//  ret = digital_input(device,card2,SW_mipxfr,&temp,0); // G/S (// not yet implemented in QPAC)
//  if (temp) {
//    printf ("SW GS selected\n");
//  }

  /* Captains's Wing Panel */
  ret = digital_input(device,card2,PB_wingautoland,&temp,0); // AutoLand (not in QPAC) (use for EFIS Baro inHg<->hPa)
  if ((temp == 1) && (!efis_baro_unit)) {
    if (efis_baro_unit != 1) {
      *barounitcapt = 1 - *barounitcapt;
      efis_baro_unit = 1;
    }
  } else {
    if ((temp == 0) && (efis_baro_unit)) {
      efis_baro_unit = 0;
    }
  }   

  ret = digital_input(device,card2,PB_wingwarning,pb_master_warning,0); // Master warning
  ret = digital_input(device,card2,PB_wingcaution,pb_master_caution,0); // Master caution
  ret = digital_input(device,card2,PB_wingchrono,pb_capt_chrono,0);     // Chrono

  ret = digital_input(device,card2,PB_wingstprio,&temp,0); // Stick Priority (not used in QPAC) (use for FCU Alt 100<->1000)
  if ((temp == 1) && (!fcu_alt_unit)) {
    if (fcu_alt_unit != 1) {
      *alt100_1000 = 1 - *alt100_1000;
      fcu_alt_unit = 1;
    }
  } else {
    if ((temp == 0) && (fcu_alt_unit)) {
      fcu_alt_unit = 0;
    }
  }   

  /* write displays */

  // master card 1, displays board #1 (00-15)
  // master card 1, displays board #2 (16-31) (...)

  /* Rudder trim display */
//  ret = mastercard_display(device,card1,DP_rudtrim, DP_rudtrim_SZ, rudder_trim, DP_rudtrim_NEG); // starting at display 1, 3 positions plus "-" sign, decimal point at pos 2!

  /* ... */

  /* write outputs */

  int zero = lightCheckPed(0), one = lightCheckPed(1);

  // master card 1, outputs (11-55)

  // Engine panel, the engine Fault lights
  // not used in this plane
  ret = digital_output(device,card1,LT_eng1fault,&zero);
  ret = digital_output(device,card1,LT_eng2fault,&zero);

  // Engine panel, the engine FIRE lights
  int engFire = lightCheckPed(((*eng1_fire == 6) && (*eng1_egt > 200.)) ? 1 : 0);
  ret = digital_output(device,card1,LT_eng1fire,&engFire);
  engFire = lightCheckPed(((*eng2_fire == 6) && (*eng2_egt > 200.)) ? 1 : 0);
  ret = digital_output(device,card1,LT_eng2fire,&engFire);

  // TCAS Panel
  // Audio Mgt Panel
  // Radio Mgt Panel
  // MCDU keys	 (Note: MCDU keys are handled in separate module a320_mcdu_keys.c)
  // MCDU Screen (Note: screen is handled in OpenGC module ogcA320MCDU.cpp)

  // ECAM Panel
  temp = lightCheckPed(*lt_ecam_apu);
  ret = digital_output(device,card1,LT_ecam_apu,&temp);
  temp = lightCheckPed(*lt_ecam_bleed);
  ret = digital_output(device,card1,LT_ecam_bleed,&temp);
  temp = lightCheckPed(*lt_ecam_cond);
  ret = digital_output(device,card1,LT_ecam_cond,&temp);
  temp = lightCheckPed(*lt_ecam_door);
  ret = digital_output(device,card1,LT_ecam_door,&temp);
  temp = lightCheckPed(*lt_ecam_elec);
  ret = digital_output(device,card1,LT_ecam_elec,&temp);
  temp = lightCheckPed(*lt_ecam_eng);
  ret = digital_output(device,card1,LT_ecam_eng,&temp);
  temp = lightCheckPed(*lt_ecam_fctl);
  ret = digital_output(device,card1,LT_ecam_fctl,&temp);
  temp = lightCheckPed(*lt_ecam_fuel);
  ret = digital_output(device,card1,LT_ecam_fuel,&temp);
  temp = lightCheckPed(*lt_ecam_hyd);
  ret = digital_output(device,card1,LT_ecam_hyd,&temp);
  temp = lightCheckPed(*lt_ecam_press);
  ret = digital_output(device,card1,LT_ecam_press,&temp);
  temp = lightCheckPed(*lt_ecam_wheel);
  ret = digital_output(device,card1,LT_ecam_wheel,&temp);
  temp = lightCheckPed(*lt_ecam_sts);
  ret = digital_output(device,card1,LT_ecam_sts,&temp);
  temp = lightCheckPed(*lt_ecam_clr);
  ret = digital_output(device,card1,LT_ecam_clr,&temp);

  // MCDU annunciators
  ret = digital_output(device,card1,LT_mcduRDY,&one);
  if (mcdu_select == 0) {
    ret = digital_output(device,card1,LT_mcduFM1,&one);
    ret = digital_output(device,card1,LT_mcduFM2,&zero);
  } else {
    ret = digital_output(device,card1,LT_mcduFM1,&zero);
    ret = digital_output(device,card1,LT_mcduFM2,&one);
  }

  // master card 2, outputs (75-119)

  // Landing Gear & Auto Brake Panel
  temp = lightCheckPed(*lt_lgear_warning);
  ret = digital_output(device,card2,LT_lgwarnarrow,&temp);
  switch (*abrk_lo_ind) {
    case 0:
      ret = digital_output(device,card2,LT_abrklo,&zero);
      ret = digital_output(device,card2,LT_abrkdeclo,&zero);
    break;
    case 1:
      ret = digital_output(device,card2,LT_abrklo,&one);
      ret = digital_output(device,card2,LT_abrkdeclo,&one);
    break;
    case 2:
      ret = digital_output(device,card2,LT_abrklo,&one);
      ret = digital_output(device,card2,LT_abrkdeclo,&one);
    break;
  }
  switch (*abrk_max_ind) {
    case 0:
      ret = digital_output(device,card2,LT_abrkmax,&zero);
      ret = digital_output(device,card2,LT_abrkdecmax,&zero);
    break;
    case 1:
      ret = digital_output(device,card2,LT_abrkmax,&one);
      ret = digital_output(device,card2,LT_abrkdecmax,&one);
    break;
    case 2:
      ret = digital_output(device,card2,LT_abrkmax,&one);
      ret = digital_output(device,card2,LT_abrkdecmax,&one);
    break;
  }
  switch (*abrk_med_ind) {
    case 0:
      ret = digital_output(device,card2,LT_abrkmed,&zero);
      ret = digital_output(device,card2,LT_abrkdecmed,&zero);
    break;
    case 1:
      ret = digital_output(device,card2,LT_abrkmed,&one);
      ret = digital_output(device,card2,LT_abrkdecmed,&one);
    break;
    case 2:
      ret = digital_output(device,card2,LT_abrkmed,&one);
      ret = digital_output(device,card2,LT_abrkdecmed,&one);
    break;
  }

  ret = digital_output(device,card2,LT_abrkfan,&zero);
  ret = digital_output(device,card2,LT_abrkhot,&zero);

  switch (*lg_leftgear_ind) {
    case 0:
      ret = digital_output(device,card2,LT_lglefttransit,&zero);
      ret = digital_output(device,card2,LT_lgleftlock,&zero);
    break;
    case 1:
      ret = digital_output(device,card2,LT_lglefttransit,&one);
      ret = digital_output(device,card2,LT_lgleftlock,&zero);
    break;
    case 2:
      ret = digital_output(device,card2,LT_lglefttransit,&zero);
      ret = digital_output(device,card2,LT_lgleftlock,&one);
    break;
    case 3:
      ret = digital_output(device,card2,LT_lglefttransit,&one);
      ret = digital_output(device,card2,LT_lgleftlock,&one);
    break;
  }
  switch (*lg_nosegear_ind) {
    case 0:
      ret = digital_output(device,card2,LT_lgnosetransit,&zero);
      ret = digital_output(device,card2,LT_lgnoselock,&zero);
    break;
    case 1:
      ret = digital_output(device,card2,LT_lgnosetransit,&one);
      ret = digital_output(device,card2,LT_lgnoselock,&zero);
    break;
    case 2:
      ret = digital_output(device,card2,LT_lgnosetransit,&zero);
      ret = digital_output(device,card2,LT_lgnoselock,&one);
    break;
    case 3:
      ret = digital_output(device,card2,LT_lgnosetransit,&one);
      ret = digital_output(device,card2,LT_lgnoselock,&one);
    break;
  }
  switch (*lg_rightgear_ind) {
    case 0:
      ret = digital_output(device,card2,LT_lgrighttransit,&zero);
      ret = digital_output(device,card2,LT_lgrightlock,&zero);
    break;
    case 1:
      ret = digital_output(device,card2,LT_lgrighttransit,&one);
      ret = digital_output(device,card2,LT_lgrightlock,&zero);
    break;
    case 2:
      ret = digital_output(device,card2,LT_lgrighttransit,&zero);
      ret = digital_output(device,card2,LT_lgrightlock,&one);
    break;
    case 3:
      ret = digital_output(device,card2,LT_lgrighttransit,&one);
      ret = digital_output(device,card2,LT_lgrightlock,&one);
    break;
  }

  ret = digital_output(device,card2,LT_abrkterrnd,&zero);			// (untested: not yet implemented in QPAC)
  ret = digital_output(device,card2,LT_mipgpws,&zero);				// (untested: not yet implemented in QPAC)
  ret = digital_output(device,card2,LT_mipgs,&zero);				// (untested: not yet implemented in QPAC)

  // Captain's Wing Panel
  ret = digital_output(device,card2,LT_wingautoland,&zero);
  temp = lightCheckPed(*lt_master_caution);
  ret = digital_output(device,card2,LT_wingcaution,&temp);
  temp = lightCheckPed(*lt_master_warning);
  ret = digital_output(device,card2,LT_wingwarning,&temp);
  ret = digital_output(device,card2,LT_wingstkprioarrow,&zero);
  ret = digital_output(device,card2,LT_wingstkpriocapt,&zero);

  /* ... */
//printf ("cd: %i (%i), lt: %i (%i)\n", coldDarkPed, *cold_dark_ped, ltTestPed, *lights_test_ped);

}

