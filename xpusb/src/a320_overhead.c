/* This is the a320_overhead.c code which contains code for how to communicate with my Airbus 
   A320 hardware (switches, LED's, displays) connected to the OpenCockpits IOCARDS USB device.
   To be used with the QPAC Basic V2 A320 Airbus (or Peter Hager's A320 Airbus)!

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
#include "a320_overhead.h"

/* enumerations */
/* Notice that these depend on the hardware wiring! */
/* Also notice:
   - input/output numbers run *per card*, not contiguously as in SIOC
   - All PB's are Normally-Closed, except PB_rRainRplnt
   - PB_cargoTest only works intermittently
*/
enum inputs { // pushbuttons PB_, switches SW_, rotary encoders RE_
	// master card 1
	SW_adirsAdr2 = 0,																				//   0
	SW_emerGen1Line,	SW_evacCmd,			SW_fctlFac1,		SW_adirsAdr1,		SW_emerRat,
	SW_fctlElac1,		SW_fctlSec1,		SW_adirsAdr3,		SW_oxyPass,			SW_gpwsSys,		//   1 -  10
	SW_gpwsTerr,		SW_gpwsFlapMode,	SW_rcdrGndCtl,		SW_callsEmer,		SW_gpwsLdgFlap3,
	SW_gpwsGSMode,		SW_oxyCrew,			SW_emerManOn,		PB_rcdrTest,		SW_condXBldOpen,//  11 -  20
	SW_condXBldAuto,	PB_evacHornOff,		SW_emerGenTest,		SW_pc1_24,			SW_condXBldShut,
	SW_evacCaptPurs,	PB_callsAft,		PB_rcdrErase,		SW_lWiperFast,		SW_lWiperSlow,	//  21 -  30
	SW_oxyMaskManOn,	PB_callsFwd,		PB_lRainRplnt,		SW_lWiperOff,		PB_callsMech,
	SW_pc1_36,			SW_pc1_37,			SW_pc1_38,			SW_pc1_39,			SW_pc1_40,		//  31 -  40
	SW_pc1_41,			SW_pc1_42,			SW_pc1_43,			SW_pc1_44,			SW_dataSts,
	SW_pc1_46,			SW_pc1_47,			SW_dataPPos,		SW_dataWind,		SW_pc1_50,		//  41 -  50
	SW_dataTest,		SW_dataTkGs,		SW_dataHdg,			SW_pc1_54,			SW_adirsSys1,
	SW_adirsSys3,		SW_adirsSys2,		SW_adirsSysOff,		SW_pc1_59,			SW_pc1_60,		//  51 -  60
	SW_pc1_61,			SW_pc1_62,			SW_adirsIr2Nav,		SW_adirsIr3Nav,		SW_adirsIr3Att,
	SW_adirsIr1Off,		SW_adirsIr3Off,		SW_adirsIr2Off,		SW_adirsIr1Att,		SW_adirsIr1Nav,	//  61 -  70
	SW_adirsIr2Att,																					//  71
	// master card 2
	SW_fuelCTkPmp1 = 0,																				//  72
	SW_elecGalley,		SW_aIceWing,		SW_fuelModeSel,		SW_hydRatMan,		SW_elecGen1,
	SW_fuelLTkPmp2,		SW_hydEng1Pmp,		SW_fuelLTkPmp1,		SW_condRamAir,		SW_aIceEng2,	//  73 -  82
	SW_aIceEng1,		SW_elecApuGen,		SW_condPack1,		SW_elecIdg1,		PB_elecExtPwr,
	SW_elecBusTie,		SW_condEng1Bld,		SW_pc2_18,			SW_pc2_19,			SW_pc2_20,		//  83 -  92
	SW_pc2_21,			SW_pc2_22,			SW_pc2_23,			SW_pc2_24,			SW_pc2_25,
	SW_pc2_26,			SW_pc2_27,			SW_pc2_28,			SW_pc2_29,			SW_pc2_30,		//  93 - 102
	SW_pc2_31,			SW_pc2_32,			SW_pc2_33,			SW_pc2_34,			SW_pc2_35,
	SW_condAft5,		SW_pc2_37,			SW_pc2_38,			SW_condAft3,		SW_condAft7,	// 103 - 112
	SW_condAft4,		SW_condAft1,		SW_condAft2,		SW_condAft6,		SW_condFwd5,
	SW_pc2_46,			SW_pc2_47,			SW_condFwd3,		SW_condFwd7,		SW_condFwd4,	// 113 - 122
	SW_condFwd1,		SW_condFwd2,		SW_condFwd6,		SW_condCkpt5,		SW_pc2_55,
	SW_pc2_56,			SW_condCkpt3,		SW_condCkpt7,		SW_condCkpt4,		SW_condCkpt1,	// 123 - 132
	SW_condCkpt2,		SW_condCkpt6,		SW_signsNoSmOn, 	SW_pc2_64,			SW_pc2_65,
	SW_intLtIceInd,		SW_signsBeltsOn,	SW_signsNoSmOff,	SW_signsEmXtOn,		SW_signsEmXtOff,// 133 - 142
	SW_signsBeltsOff,																				// 143
	// master card 3
	PB_hydYElecPmp = 0,																				// 144
	SW_elecGen2,		SW_elecIdg2,		SW_fuelRTkPmp2,		SW_fuelRTkPmp1,		SW_condHotAir,
	SW_hydPtu,			SW_pc3_07,			SW_hydEng2Pmp,		SW_pressModeSel,	SW_elecBat1,	// 145 - 154
	SW_condPack2,		SW_condEng2Bld,		SW_elecBat2,		SW_fuelCTkPmp2,		SW_pressDitch,
	SW_condApuBld,		SW_elecAcEss,		SW_pressLdgEl6,		SW_pressLdgEl10,	SW_pressLdgEl2,	// 155 - 164
	SW_pressLdgEl0,		SW_pressLdgEl8,		SW_pressLdgEl4,		SW_pressLdgElA,		SW_pressLdgElM2,
	SW_pressLdgEl7,		SW_intLtAnnTst,		SW_intLtDomeOff,	SW_condPFlowH,		SW_condPFlowN,	// 165 - 174
	SW_intLtDomeBrt,	SW_pressManVSD,		SW_pressManVSU,		SW_condPFlowL,		SW_intLtAnnDim,
	SW_pc3_36,			SW_pc3_37,			SW_pc3_38,			SW_pc3_39,			SW_pc3_40,		// 175 - 184
	SW_pc3_41,			SW_pc3_42,			SW_pc3_43,			SW_pc3_44,			SW_pc3_45,
	SW_pc3_46,			SW_pc3_47,			SW_pc3_48,			SW_pc3_49,			SW_pc3_50,		// 185 - 194
	SW_pc3_51,			SW_pc3_52,			SW_pc3_53,			SW_pc3_54,			SW_pc3_55,
	SW_pc3_56,			SW_pc3_57,			SW_pc3_58,			SW_pc3_59,			SW_pc3_60,		// 195 - 204
	SW_pc3_61,			SW_pc3_62,			SW_pc3_63,			SW_pc3_64,			SW_pc3_65,
	SW_pc3_66,			SW_pc3_67,			SW_pc3_68,			SW_pc3_69,			SW_pc3_70,		// 205 - 214
	SW_pc3_71,																						// 215
	// master card 4
	SW_probeWHeat = 0,																				// 216
	SW_fuelXFeed,		SW_fctlElac2,		SW_ventBlower,		SW_hydBlElPmp,		PB_apuStart,
	SW_fctlSec3,		SW_fctlSec2,		SW_apuMaster,		SW_ventExtract,		SW_cargoHotAir,	// 217 - 226
	SW_cargoAftIsol,	SW_fctlFac2,		SW_ventCabFans,		SW_engManSt1,		SW_engN1Md1,
	SW_engManSt2,		SW_engN1Md2,		SW_cargoAftAg2,		SW_cargoFwAg1,		SW_rWiperOff,	// 227 - 236
	SW_rWiperSlow,		SW_cargoFwdAg2,		PB_cargoTest,		PB_rRainRplnt,		SW_rWiperFast,
	SW_cargoAftAg1,		SW_cargoHeat2,		SW_pc4_28,			SW_cargoHeat4,		SW_cargoHeat5,	// 237 - 246
	SW_pc4_31,			SW_cargoHeat3,		SW_cargoHeat7,		SW_cargoHeat6,		SW_cargoHeat1,
	SW_pc4_36,			SW_pc4_37,			SW_pc4_38,			SW_pc4_39,			SW_pc4_40,		// 247 - 256
	SW_pc4_41,			SW_pc4_42,			SW_pc4_43,			SW_pc4_44,			SW_pc4_45,
	SW_pc4_46,			SW_pc4_47,			SW_pc4_48,			SW_pc4_49,			SW_pc4_50,		// 257 - 266
	SW_pc4_51,			SW_pc4_52,			SW_pc4_53,			SW_xtLtRLdgRet,		SW_pc4_55,
	SW_pc4_56,			SW_pc4_57,			SW_xtLtLLdgRet,		SW_xtLtRLdgOn,		SW_xtLtNoseTO,	// 267 - 276
	SW_xtLtNoseOff,		SW_xtLtLLdgOn,		SW_xtLtNavLOff,		SW_xtLtBeacon,		SW_pc4_65,
	SW_xtLtRwyTOff,		SW_xtLtWing,		SW_pc4_68,			SW_xtLtStrobeOff,	SW_xtLtStrobeOn,// 277 - 286
	SW_xtLtNavL2,																					// 287
	last_input
	};
enum axes {	// Axes AX_ (from USB expander!)
	AX_int_lt=1,																					//   1 -   4
	last_axis
	};

// displays (7-segment displays DP_)
#define DP_ohBat1 0
#define DP_ohBat2 4
#define DP_ohBatSz 3

// IRS displays ??

enum outputs { // Led-lights LT_
	// master card 1
	LT_evacCmdU=11,		LT_gpwsTerrU,		LT_evacCmdL,		LT_gpwsTerrL,		LT_fctlFac1U,
	LT_gpwsFlapMdL,		LT_fctlFac1L,		LT_adirsOnBat,		LT_fctlSec1U,		LT_adirsIr2U,	//  11 -  20
	LT_fctlSec1L,		LT_gpwsGSMdL,		LT_fctlElac1U,		LT_adirsIr3U,		LT_fctlElac1L,
	LT_gpwsLdgFlap3L,	LT_emerGen1LnU,		LT_gpwsSysU,		LT_emerGen1LnL,		LT_gpwsSysL,	//  21 -  30
	LT_adirsAdr1U,		LT_adirsIr1L,		LT_adirsAdr1L,		LT_rcdrGndCtlL,		LT_adirsAdr3U,
	LT_adirsIr1U,		LT_adirsAdr3L,		LT_oxyCrewL,		LT_adirsAdr2U,		LT_pc1_40,		//  31 -  40
	LT_adirsAdr2L,		LT_oxyPassL,		LT_emerRatU,		LT_callsEmerU,		LT_pc1_45,
	LT_callsEmerL,		LT_adirsIr3L,		LT_adirsIr2L,		LT_pc1_49,			LT_pc1_50,		//  41 -  50
	LT_pc1_51,			LT_pc1_52,			LT_pc1_53,			LT_pc1_54,			LT_pc1_55,		//  51 -  55
	// master card 2
	LT_aIceWingU=11,	LT_aIceEng1U,		LT_aIceWingL,		LT_aIceEng1L,		LT_fuelModeSelU,
	LT_elecApuGenU,		LT_fuelModeSelL,	LT_elecApuGenL,		LT_hydEng1PmpU,		LT_pc2_20,		//  75 -  84
	LT_hydEng1PmpL,		LT_elecBusTieL,		LT_fuelLTkPmp2U,	LT_elecExtPwrU,		LT_fuelLTkPmp2L,
	LT_elecExtPwrL,		LT_elecGalleyU,		LT_aIceEng2U,		LT_elecGalleyL,		LT_aIceEng2L,	//  85 -  94
	LT_hydRatManU,		LT_condPack1U,		LT_hydRatManL,		LT_condPack1L,		LT_fuelLTkPmp1U,
	LT_condEng1BldU,	LT_fuelLTkPmp1L,	LT_condEng1BldL,	LT_fuelCTkPmp1U,	LT_pc2_40,		//  95 - 104
	LT_fuelCTkPmp1L,	LT_condRamAirL,		LT_elecGen1U,		LT_elecIdg1U,		LT_elecGen1L,
	LT_pc2_46,			LT_pc2_47,			LT_pc2_48,			LT_pc2_49,			LT_pc2_50,		// 105 - 114
	LT_pc2_51,			LT_pc2_52,			LT_pc2_53,			LT_pc2_54,			LT_pc2_55,		// 115 - 119
	// master card 3
	LT_elecIdg2U=11,	LT_condPack2U,		LT_pc3_13,			LT_condPack2L,		LT_fuelRTkPmp2U,
	LT_condEng2BldU,	LT_fuelRTkPmp2L,	LT_condEng2BldL,	LT_pc3_19,			LT_condApuBldU,	// 139 - 148
	LT_pc3_21,			LT_condApuBldL,		LT_hydPtuU,			LT_pc3_24,			LT_hydPtuL,
	LT_pressDitchL,		LT_elecGen2U,		LT_elecBat1U,		LT_elecGen2L,		LT_elecBat1L,	// 149 - 158
	LT_fuelRTkPmp1U,	LT_elecBat2U,		LT_fuelRTkPmp1L,	LT_elecBat2L,		LT_hydEng2PmpU,
	LT_elecAcEssU,		LT_hydEng2PmpL,		LT_elecAcEssL,		LT_hydYElecPmpU,	LT_pressMdSelU,	// 159 - 168
	LT_hydYElecPmpL,	LT_pressMdSelL,		LT_condHotAirU,		LT_fuelCTkPmp2U,	LT_condHotAirL,
	LT_fuelCTkPmp2L,	LT_pc3_47,			LT_signsEmXtL,		LT_pc3_49,			LT_pc3_50,		// 169 - 178
	LT_pc3_51,			LT_pc3_52,			LT_pc3_53,			LT_pc3_54,			LT_pc3_55,		// 179 - 183
	// master card 4
	LT_fctlElac2U=11,	LT_cargoAftIsolU,	LT_fctlElac2L,		LT_cargoAftIsolL,	LT_ventBlowerU,
	LT_fctlFac2U,		LT_ventBlowerL,		LT_fctlFac2L,		LT_fctlSec2U,		LT_pc4_20,		// 203 - 212
	LT_fctlSec2L,		LT_engManSt2L,		LT_fctlSec3U,		LT_pc4_24,			LT_fctlSec3L,
	LT_engN1Md1L,		LT_fuelXFeedU,		LT_cargoHotAirU,	LT_fuelXFeedL,		LT_cargoHotAirL,// 213 - 222
	LT_hydBElecPmpU,	LT_cargoSmFwdU,		LT_hydBElecPmpL,	LT_ventCabFansL,	LT_apuMasterU,
	LT_cargoSmAftU,		LT_apuMasterL,		LT_engN1Md2L,		LT_pc4_39,			LT_ventExtractU,// 223 - 232
	LT_probeWHeatL,		LT_ventExtractL,	LT_apuStartU,		LT_pc4_44,			LT_apuStartL,
	LT_engManSt1L,		LT_pc4_47,			LT_pc4_48,			LT_pc4_49,			LT_pc4_50,		// 233 - 242
	LT_pc4_51,			LT_pc4_52,			LT_pc4_53,			LT_pc4_54,			LT_pc4_55,		// 243 - 247
	last_output
};

// Handle the lights testing and cold&dark states for all light outputs
int ltTest = 0; int coldDark = 2;
int lightCheck (int input) {
  return ((input || ltTest) && (coldDark != 2));
}

// Here distinguish between possible A320 versions (recompilation necessary)
//#define _phager_airbus
# define _qpac_airbus
// Note that the phager stuff may be incomplete, and is untested!

void a320_overhead(void)
{

  /* local variables */

  int device = 1; // as defined by ini file!
  int card1 = 0, card2 = 1, card3 = 2, card4 = 3;
  int ret = 0, zero = lightCheck(0), one = lightCheck(1);
  int temp = 0, tmp1 = 0, tmp2 = 0, tmp3 = 0;
  int cond1 = 0, cond2 = 0, cond3 = 0, cond4 = 0, cond5 = 0, cond6 = 0, cond7 = 0;
  int land_elev_auto = 0, land_elev_min2 = 0, land_elev_0 = 0, land_elev_2 = 0;
  int land_elev_4 = 0, land_elev_6 = 0, land_elev_7 = 0, land_elev_8 = 0, land_elev_10 = 0;
  int ann_test_sw = 0, ann_dim_sw = 0, ann_brt_sw = 0;
  int strobe_on_sw = 0, strobe_off_sw = 0;
  int seat_belts_on = 0, seat_belts_off = 0, no_smoking_on = 0, no_smoking_off = 0;
  int emer_exit_on = 0, emer_exit_off = 0;
  int oxy_crew_lt = 0;

  /* Request X-Plane variables */

      // Note: these two dataref are used by all other a320 instrument modules,
      //       both in usbiocards and in opengc
      int *cold_dark = link_dataref_int ("xpserver/cold_and_dark");
      if (*cold_dark == INT_MISS) *cold_dark = 0;
      int *lights_test = link_dataref_int ("xpserver/lights_test");
      if (*lights_test == INT_MISS) *lights_test = 0;

      int *connectCenter = link_dataref_int ("AirbusFBW/SDELConnectCenter");	// 0 = batteries off
      if (*connectCenter == INT_MISS) *connectCenter = 0;
      int *connectLeft   = link_dataref_int ("AirbusFBW/SDELConnectLeft");		// 0 = avionics power off
      if (*connectLeft == INT_MISS) *connectLeft = 0;
      int *connectRight  = link_dataref_int ("AirbusFBW/SDELConnectRight");		// ditto
      if (*connectRight == INT_MISS) *connectRight = 0;
      if ((*connectCenter == 0) && (*connectLeft == 0) && (*connectRight == 0)) coldDark = 2;
	  else if (*connectCenter == 0) coldDark = 1;
	  else coldDark = 0;
      *cold_dark = coldDark; *lights_test = ltTest;

    /* generic */
//	sim/cockpit/engine/APU_N1			float	y
      float *apu_N1 = link_dataref_flt("sim/cockpit/engine/APU_N1", 0);
//	sim/cockpit2/engine/indicators/N2_percent	float[8] n
      float *eng1_N2 = link_dataref_flt_arr("sim/cockpit2/engine/indicators/N2_percent", 8, 0, 0);
      float *eng2_N2 = link_dataref_flt_arr("sim/cockpit2/engine/indicators/N2_percent", 8, 1, 0);
//	sim/operation/failures/rel_APU_press		int	y
      int *apu_press = link_dataref_int ("sim/operation/failures/rel_APU_press");
//	sim/operation/failures/rel_bleed_air_lft	int	y
      int *bleed_air_left = link_dataref_int ("sim/operation/failures/rel_bleed_air_lft");
//	sim/operation/failures/rel_bleed_air_rgt	int	y
      int *bleed_air_right = link_dataref_int ("sim/operation/failures/rel_bleed_air_rgt");
//	sim/cockpit2/switches/no_smoking		int	y
      int *no_smoking_sw = link_dataref_int ("sim/cockpit2/switches/no_smoking");
//	sim/cockpit2/switches/fasten_seat_belts		int	y
      int *seat_belts_sw = link_dataref_int ("sim/cockpit2/switches/fasten_seat_belts");

    /* Overhead */
      /* Left-Hand Column */
      /***** Adirs *****/
      // Position displays, DISPLAY/DATA and DISPLAY/SYS switches, and Keyboard not yet implemented
//	AirbusFBW/ADIRUOnBat		int	n
      int *adiru_onbat_lt = link_dataref_int ("AirbusFBW/ADIRUOnBat");
//	AirbusFBW/ADIRUSwitchArray	int[6]	y	ADIRU / ADR Switches - 0=ADIRU1, 1=ADIRU2, 2=ADIRU3
      int *adiru1_sw = link_dataref_int_arr ("AirbusFBW/ADIRUSwitchArray", 6, 0);
      int *adiru2_sw = link_dataref_int_arr ("AirbusFBW/ADIRUSwitchArray", 6, 1);
      int *adiru3_sw = link_dataref_int_arr ("AirbusFBW/ADIRUSwitchArray", 6, 2);
//	AirbusFBW/IRLights		int[6]	n	
      int *ir1_lt = link_dataref_int_arr ("AirbusFBW/IRLights", 6, 0);
      int *ir2_lt = link_dataref_int_arr ("AirbusFBW/IRLights", 6, 1);
      int *ir3_lt = link_dataref_int_arr ("AirbusFBW/IRLights", 6, 2);
//	AirbusFBW/ADRSwitchArray	int[6]	y	
      int *adr1_sw = link_dataref_int_arr ("AirbusFBW/ADRSwitchArray", 6, 0);
      int *adr2_sw = link_dataref_int_arr ("AirbusFBW/ADRSwitchArray", 6, 1);
      int *adr3_sw = link_dataref_int_arr ("AirbusFBW/ADRSwitchArray", 6, 2);
//	AirbusFBW/ADRLights		int[6]	n	
      int *adr1_lt = link_dataref_int_arr ("AirbusFBW/ADRLights", 6, 0);
      int *adr2_lt = link_dataref_int_arr ("AirbusFBW/ADRLights", 6, 1);
      int *adr3_lt = link_dataref_int_arr ("AirbusFBW/ADRLights", 6, 2);
      /***** Flt Ctrl Capt & FO *****/
//	AirbusFBW/FCCSwitchArray	int[16]	y	Elac/Sec/Fac switches - 0=ELAC1, 1=ELAC2, 2=SEC1, 3=SEC2, 4=SEC3, 5=FAC1, 6=FAC2
      int *elac1_sw = link_dataref_int_arr ("AirbusFBW/FCCSwitchArray", 16, 0);
      int *elac2_sw = link_dataref_int_arr ("AirbusFBW/FCCSwitchArray", 16, 1);
      int *sec1_sw  = link_dataref_int_arr ("AirbusFBW/FCCSwitchArray", 16, 2);
      int *sec2_sw  = link_dataref_int_arr ("AirbusFBW/FCCSwitchArray", 16, 3);
      int *sec3_sw  = link_dataref_int_arr ("AirbusFBW/FCCSwitchArray", 16, 4);
      int *fac1_sw  = link_dataref_int_arr ("AirbusFBW/FCCSwitchArray", 16, 5);
      int *fac2_sw  = link_dataref_int_arr ("AirbusFBW/FCCSwitchArray", 16, 6);
//	AirbusFBW/FCCSwitchLightsArray	int[16]	n	Elac/Sec/Fac switch lights - same numbers
      int *elac1_lt = link_dataref_int_arr ("AirbusFBW/FCCSwitchLightsArray", 16, 0);
      int *elac2_lt = link_dataref_int_arr ("AirbusFBW/FCCSwitchLightsArray", 16, 1);
      int *sec1_lt  = link_dataref_int_arr ("AirbusFBW/FCCSwitchLightsArray", 16, 2);
      int *sec2_lt  = link_dataref_int_arr ("AirbusFBW/FCCSwitchLightsArray", 16, 3);
      int *sec3_lt  = link_dataref_int_arr ("AirbusFBW/FCCSwitchLightsArray", 16, 4);
      int *fac1_lt  = link_dataref_int_arr ("AirbusFBW/FCCSwitchLightsArray", 16, 5);
      int *fac2_lt  = link_dataref_int_arr ("AirbusFBW/FCCSwitchLightsArray", 16, 6);
      /***** Evac *****/
      // This panel not yet implemented in QPAC
      /***** Emer Elec Pwr *****/
      // EmerManOn switch in ElecOHPArray (ELEC panel)
      // rest of this panel not yet implemented in QPAC
      /***** Gpws *****/
      // This panel not yet implemented in QPAC
      /***** Rcdr *****/
      // This panel not yet implemented in QPAC
      /***** Oxygen *****/
      // CREW SUPPLY switch and lights not yet implemented in QPAC
//	AirbusFBW/PaxOxySwitch		int	y	
      int *oxy_pax_sw = link_dataref_int ("AirbusFBW/PaxOxySwitch");
//	AirbusFBW/PaxOxyMask		int	n
      int *oxy_pax_lt = link_dataref_int ("AirbusFBW/PaxOxyMask");
      /***** Calls *****/
      // MECH PB not yet implemented in QPAC
//	%AirbusFBW/CheckCabin	Press any of the purser buttons
      int *calls_chk_cabin_pb = link_dataref_cmd_once ("AirbusFBW/CheckCabin");
//	AirbusFBW/EmerEvacButton	int	y
      int *calls_emer_sw = link_dataref_int ("AirbusFBW/EmerEvacButton");
//	AirbusFBW/EmerEvacLight		int	n
      int *calls_emer_lt = link_dataref_int ("AirbusFBW/EmerEvacLight");
      /***** Wiper Capt  *****/
//      int *rainRplnt; // RAIN RPLNT PB not implemented in QPAC
//      sim/cockpit2/switches/wiper_speed	int	y	enum	0=off, 1=25%speed, 2=50%speed, 3=100%speed
      int *wiper_spd_sw = link_dataref_int ("sim/cockpit2/switches/wiper_speed");
      /* Center Column */
      /***** Fire *****/
      // This panel not yet implemented in my cockpit
//      // Only ENG1/2 Fire PB & -Lts implemented yet in simulator
////	sim/cockpit2/engine/actuators/fire_extinguisher_on	int[8]	y	boolean	Fire extinguisher on - 0=Eng 1, 1 = Eng 2
//      int *fire_ext_pb = link_dataref_int_arr ("sim/cockpit2/engine/actuators/fire_extinguisher_on", 8, -1);
////      float *eng1_egt = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 0, 0);
////      float *eng2_egt = link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 1, 0);
////      int *eng1_fire = link_dataref_int("sim/operation/failures/rel_engfir0");
////      int *eng2_fire = link_dataref_int("sim/operation/failures/rel_engfir1");
//#ifdef _phager_airbus
////      int *apu_fire_test = link_dataref_cmd_hold("com/petersaircraft/airbus/APUfire_test");
//#endif
      /***** Hyd *****/
//	AirbusFBW/HydOHPArray		int[24]	y	Hydraulics Switches - 0=Eng 1 Pump, 1=Rat Man, 2=(blue) Elec Pump, 3=PTU, 4=Eng 2 Pump, 5=Yellow Elec Pump (numbers assumed)
      int *eng1pmp_sw = link_dataref_int_arr ("AirbusFBW/HydOHPArray", 24, 0);
      int *eng2pmp_sw = link_dataref_int_arr ("AirbusFBW/HydOHPArray", 24, 1);
      int *blelpmp_sw = link_dataref_int_arr ("AirbusFBW/HydOHPArray", 24, 2);
      int *ylelpmp_pb = link_dataref_int_arr ("AirbusFBW/HydOHPArray", 24, 3);
      int *ptu_sw     = link_dataref_int_arr ("AirbusFBW/HydOHPArray", 24, 4);
      int *ratman_sw  = link_dataref_int_arr ("AirbusFBW/HydOHPArray", 24, 5);
//	AirbusFBW/HydPumpOHPArray	int[8]	n	Hydraulics Switch Lights
      int *eng1pmp_lt = link_dataref_int_arr ("AirbusFBW/HydPumpOHPArray", 8, 0);
      int *eng2pmp_lt = link_dataref_int_arr ("AirbusFBW/HydPumpOHPArray", 8, 1);
      int *blelpmp_lt = link_dataref_int_arr ("AirbusFBW/HydPumpOHPArray", 8, 2);
      int *ylelpmp_lt = link_dataref_int_arr ("AirbusFBW/HydPumpOHPArray", 8, 3);
//	AirbusFBW/HydPTUOHP	int	n
      int *hyd_ptu_lt = link_dataref_int ("AirbusFBW/HydPTUOHP");
      /***** Fuel *****/
//	AirbusFBW/FuelOHPArray		int[32]	y	Fuel Pump Switches - 0=LTkPmp1, 1=LTkPmp2, 2=CTkPmp1, 3=CTkPmp2, 4=RTkPmp1, 5=RTkPmp2  6=XFeed, 7=ModeSel 
      int *ltkpmp1_sw = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 0);
      int *ltkpmp2_sw = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 1);
      int *ctkpmp1_sw = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 2);
      int *ctkpmp2_sw = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 3);
      int *rtkpmp1_sw = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 4);
      int *rtkpmp2_sw = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 5);
      int *modesel_sw = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 6);
      int *xfeed_sw   = link_dataref_int_arr ("AirbusFBW/FuelOHPArray", 32, 7);
//	AirbusFBW/FuelPumpOHPArray	int[16]	n	Fuel Pump Switch Lights
      int *ltkpmp1_lt = link_dataref_int_arr ("AirbusFBW/FuelPumpOHPArray", 16, 0);
      int *ltkpmp2_lt = link_dataref_int_arr ("AirbusFBW/FuelPumpOHPArray", 16, 1);
      int *rtkpmp1_lt = link_dataref_int_arr ("AirbusFBW/FuelPumpOHPArray", 16, 4);
      int *rtkpmp2_lt = link_dataref_int_arr ("AirbusFBW/FuelPumpOHPArray", 16, 5);
//	AirbusFBW/FuelAutoPumpOHPArray	int[16]	n	Center Fuel Pump Switch Lights
      int *ctkpmp1_lt = link_dataref_int_arr ("AirbusFBW/FuelAutoPumpOHPArray", 16, 2);
      int *ctkpmp2_lt = link_dataref_int_arr ("AirbusFBW/FuelAutoPumpOHPArray", 16, 3);
//	AirbusFBW/FuelOHPAutoSwitch	int	n	Center Pump Mode Sel Lights (switch is part of FuelOHPArray)
      int *modesel_lt = link_dataref_int ("AirbusFBW/FuelOHPAutoSwitch"); // ! 0, 2 show MAN lt, 2, 3 show FAULT lt, 1 is dark !
//	AirbusFBW/FuelXFVOHPArray	int[8]	n	Fuel XFeed Switch Lights (switch is part of FuelOHPArray)
      int *xfeed_lt = link_dataref_int_arr ("AirbusFBW/FuelXFVOHPArray", 8, 0);
      /***** Elec *****/
//	AirbusFBW/ElecOHPArray		int[32]	y	Elec Panel Switches - 0=Eng 1 Gen, 1=Eng 2 Gen, 2=APU Gen, 3=Ext Power, 4=Bus Tie,
//									      5=Batt 1 Master, 6=Batt 2 Master, 7=--, 8=--,
//									      9=Galley, 10=Emer Elec MAN ON (in Emer Elec Pwr panel)
      int *eng1gen_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 0);
      int *eng2gen_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 1);
      int *apugen_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 2);
      int *extpwr_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 3);
      if (*extpwr_sw == INT_MISS) *extpwr_sw = 0;
      int *bustie_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 4);
      int *batt1_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 5);
      int *batt2_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 6);
      int *ac_ess_altn_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 7);
      int *galley_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 9);
      int *emer_man_sw = link_dataref_int_arr ("AirbusFBW/ElecOHPArray", 32, 10);
//	AirbusFBW/AcEssAltn		int	n	AC Ess Altn Switch Lights
      int *ac_ess_altn_lt = link_dataref_int ("AirbusFBW/AcEssAltn");
//	AirbusFBW/APUGenOHPArray	int[4]	n	APU Generator Switch Lights
      int *apu_pwr_lt = link_dataref_int_arr ("AirbusFBW/APUGenOHPArray", 4, 0);
//	AirbusFBW/BatOHPArray		int[8]	n	Battery Switches Lights
      int *bat1_pwr_lt = link_dataref_int_arr ("AirbusFBW/BatOHPArray", 8, 0);
      int *bat2_pwr_lt = link_dataref_int_arr ("AirbusFBW/BatOHPArray", 8, 1); // also controls nos 2 - 7 !?
//	AirbusFBW/BatVolts		float[8] n	Batteries indicated Voltages
      float *bat1v_dp = link_dataref_flt_arr("AirbusFBW/BatVolts", 8, 0, 0);
      float *bat2v_dp = link_dataref_flt_arr("AirbusFBW/BatVolts", 8, 1, 0);
//	AirbusFBW/BusCrossTie		int	n	Bus Tie Switch Lights
      int *bus_xtie_lt = link_dataref_int ("AirbusFBW/BusCrossTie");
//	AirbusFBW/EngGenOHPArray	int[16]	n	Engine Generator Switch Lights
      int *gen1_pwr_lt = link_dataref_int_arr ("AirbusFBW/EngGenOHPArray", 16, 0);
      int *gen2_pwr_lt = link_dataref_int_arr ("AirbusFBW/EngGenOHPArray", 16, 1);
//	AirbusFBW/ExtPowOHPArray	int[4]	n	External Power Switch Lights
      int *ext_pwr_lt = link_dataref_int_arr ("AirbusFBW/ExtPowOHPArray", 4, 0);
//	AirbusFBW/GalleyPBLight		int	n	Galley Cab Switch Lights
      int *galley_lt = link_dataref_int ("AirbusFBW/GalleyPBLight");
      /***** Air Cond *****/
//	AirbusFBW/AftCabinTemp		int	y	
      int *aft_cab_temp_sw = link_dataref_int ("AirbusFBW/AftCabinTemp");
//	AirbusFBW/APUBleedInd		int	n	
      int *apu_bleed_lt = link_dataref_int ("AirbusFBW/APUBleedInd");
//	AirbusFBW/APUBleedSwitch	int	y
      int *apu_bleed_sw = link_dataref_int ("AirbusFBW/APUBleedSwitch");
//	AirbusFBW/CockpitTemp		int	y		
      int *ckpt_temp_sw = link_dataref_int ("AirbusFBW/CockpitTemp");
//	AirbusFBW/ENG1BleedInd		int	n	
      int *eng1_bleed_lt = link_dataref_int ("AirbusFBW/ENG1BleedInd");
//	AirbusFBW/ENG1BleedSwitch	int	y	
      int *eng1_bleed_sw = link_dataref_int ("AirbusFBW/ENG1BleedSwitch");
//	AirbusFBW/ENG1HPBleedInd	int	n	
      int *eng1_hp_bleed_lt = link_dataref_int ("AirbusFBW/ENG1HPBleedInd"); // not here?
//	AirbusFBW/ENG2BleedInd		int	n	
      int *eng2_bleed_lt = link_dataref_int ("AirbusFBW/ENG2BleedInd");
//	AirbusFBW/ENG2BleedSwitch	int	y	
      int *eng2_bleed_sw = link_dataref_int ("AirbusFBW/ENG2BleedSwitch");
//	AirbusFBW/ENG2HPBleedInd	int	n
      int *eng2_hp_bleed_lt = link_dataref_int ("AirbusFBW/ENG2HPBleedInd"); // not here?
//	AirbusFBW/FwdCabinTemp		int	y		
      int *fwd_cab_temp_sw = link_dataref_int ("AirbusFBW/FwdCabinTemp");
//	AirbusFBW/HotAirSwitch		int	y	
      int *cond_hot_air_sw = link_dataref_int ("AirbusFBW/HotAirSwitch");
//	AirbusFBW/HotAirSwitchIllum	int	n	
      int *cond_hot_air_lt = link_dataref_int ("AirbusFBW/HotAirSwitchIllum");
//	AirbusFBW/Pack1Switch		int	y	
      int *pack1_sw = link_dataref_int ("AirbusFBW/Pack1Switch");
//	AirbusFBW/Pack1SwitchIllum	int	n	
      int *pack1_lt = link_dataref_int ("AirbusFBW/Pack1SwitchIllum");
//	AirbusFBW/Pack2Switch		int	y	
      int *pack2_sw = link_dataref_int ("AirbusFBW/Pack2Switch");
//	AirbusFBW/Pack2SwitchIllum	int	n	
      int *pack2_lt = link_dataref_int ("AirbusFBW/Pack2SwitchIllum");
//	AirbusFBW/PackFlowSel		int	y
      int *pack_flow_sw = link_dataref_int ("AirbusFBW/PackFlowSel");
//	AirbusFBW/RamAirSwitch		int	y	
      int *ram_air_sw = link_dataref_int ("AirbusFBW/RamAirSwitch");
//	AirbusFBW/RamAirSwitchLights	int	n	
      int *ram_air_lt = link_dataref_int ("AirbusFBW/RamAirSwitchLights");
//	AirbusFBW/XBleedInd		int	n	
//      int *x_bleed_lt = link_dataref_int ("AirbusFBW/XBleedInd"); // not in OHP!
//	AirbusFBW/XBleedSwitch		int	y	
      int *x_bleed_sw = link_dataref_int ("AirbusFBW/XBleedSwitch");
      /***** Anti Ice *****/
//	AirbusFBW/ENG1AILights		int	n	Engine 1 Anti Ice Lights
      int *eng1_aice_lt = link_dataref_int ("AirbusFBW/ENG1AILights");
//	AirbusFBW/ENG1AISwitch		int	y	Engine 1 Anti Ice Switch
      int *eng1_aice_sw = link_dataref_int ("AirbusFBW/ENG1AISwitch");
//	AirbusFBW/ENG2AILights		int	n	Engine 2 Anti Ice Lights
      int *eng2_aice_lt = link_dataref_int ("AirbusFBW/ENG2AILights");
//	AirbusFBW/ENG2AISwitch		int	y	Engine 2 Anti Ice Switch
      int *eng2_aice_sw = link_dataref_int ("AirbusFBW/ENG2AISwitch");
//	AirbusFBW/WAILights		int	n	Wing Anti Ice Lights
      int *wing_aice_lt = link_dataref_int ("AirbusFBW/WAILights");
//	AirbusFBW/WAISwitch		int	y	Wing Anti Ice Switch
      int *wing_aice_sw = link_dataref_int ("AirbusFBW/WAISwitch");
      /***** Probe/Window Heat *****/
//	AirbusFBW/PRobeHeatLights	int	n	Probe Heat Lights
      int *probe_heat_lt = link_dataref_int ("AirbusFBW/PRobeHeatLights");
//	AirbusFBW/ProbeHeatSwitch	int	y	Probe Heat Switch
      int *probe_heat_sw = link_dataref_int ("AirbusFBW/ProbeHeatSwitch");
      /***** Cabin Press *****/
//	%AirbusFBW/CabVSDown	Manual Cabin Pressure Control held in 'DOWN' direction
      int *cab_vs_dn = link_dataref_cmd_hold ("AirbusFBW/CabVSDown");
//	%AirbusFBW/CabVSUp	Manual Cabin Pressure Control held in 'UP' direction
      int *cab_vs_up = link_dataref_cmd_hold ("AirbusFBW/CabVSUp");
//	AirbusFBW/CabPressMode		int	y	Pressure Mode Switch
      int *cab_press_sw = link_dataref_int ("AirbusFBW/CabPressMode");
//	AirbusFBW/CabPressModeLights	int	n	Pressure Mode Switch Lights
      int *cab_press_lt = link_dataref_int ("AirbusFBW/CabPressModeLights");
//	AirbusFBW/DitchSwitch		int	y	Ditching Switch
      int *ditch_sw = link_dataref_int ("AirbusFBW/DitchSwitch");
//	AirbusFBW/DitchSwitchLights	int	n	Ditching Switch Lights
      int *ditch_lt = link_dataref_int ("AirbusFBW/DitchSwitchLights");
//	AirbusFBW/LandElev		float	y
      float *land_elev = link_dataref_flt ("AirbusFBW/LandElev", 0);
//	AirbusFBW/ManVSSwitch		int	n	
      int *man_vs_sw = link_dataref_int ("AirbusFBW/ManVSSwitch");
      /***** Ext Lt *****/
//	AirbusFBW/OHPLightSwitches	int[16]	y	External Lights Switches - 0=Strobe, 0=Beacon, 2=Wing, 3=NavLogo, 4=RwyTo, 5=LftLdg, 6=RgtLdg, 7=Nose, 8=Dome
      int *beacon_sw  = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 0);
      int *wing_sw    = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 1);
      int *navlogo_sw = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 2);
      int *nose_sw    = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 3);
      int *lldg_sw    = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 4);
      int *rldg_sw    = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 5);
      int *rwyto_sw   = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 6);
      int *strobe_sw  = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 7);
      int *dome_sw    = link_dataref_int_arr ("AirbusFBW/OHPLightSwitches", 16, 8);
      /***** Int Lt *****/
//	AirbusFBW/AnnunBrightSwitch	int	y	Annunciators Brightness Switch
      int *annun_brt_sw = link_dataref_int ("AirbusFBW/AnnunBrightSwitch");
//	AirbusFBW/OHPBrightnessLevel	float	y	Integr. Lighting Brightness Rheostat
      // Not used in my cockpit (externally regulated)
      /***** Signs *****/
      /***** EMER EXIT  *****/
      // Emer Exit SW & LTs not yet implemented in QPAC
      /***** Apu *****/
//	AirbusFBW/APUAvail		int	n	Avail Light
      int *apu_avail_lt = link_dataref_int ("AirbusFBW/APUAvail");
//	AirbusFBW/APUMaster		int	y	Master Switch
      int *apu_master_sw = link_dataref_int ("AirbusFBW/APUMaster");
//	AirbusFBW/APUStarter		int	y	Start Switch
      int *apu_start_pb = link_dataref_int ("AirbusFBW/APUStarter");
      /* Right-Hand Column */
      /***** PushBack (my extension) *****/
      // This panel not yet implemented
      /***** Refuel (my extension) *****/
      // This panel not yet implemented
      /***** Flt Ctrl FO *****/
      // See Flt Ctrl Capt Panel
      /***** Cargo Heat *****/
      // This panel not yet implemented in QPAC
      /***** Cargo Smoke *****/
      // This panel not yet implemented in QPAC
      /***** Ventilation *****/
//	AirbusFBW/BlowerSwitch		int	y	
      int *vent_blower_sw = link_dataref_int ("AirbusFBW/BlowerSwitch");
//	AirbusFBW/BlowerSwitchLights	int	n	
      int *vent_blower_lt = link_dataref_int ("AirbusFBW/BlowerSwitchLights");
//	AirbusFBW/CabinFanSwitch	int	y	
      int *vent_fans_sw = link_dataref_int ("AirbusFBW/CabinFanSwitch");
//	AirbusFBW/CabinFanSwitchLights	int	n	
      int *vent_fans_lt = link_dataref_int ("AirbusFBW/CabinFanSwitchLights");
//	AirbusFBW/ExtractSwitch		int	y	
      int *vent_extract_sw = link_dataref_int ("AirbusFBW/ExtractSwitch");
//	AirbusFBW/ExtractSwitchLights	int	n	
      int *vent_extract_lt = link_dataref_int ("AirbusFBW/ExtractSwitchLights");
      /***** Engines *****/
//	AirbusFBW/ManStartSwitchArray	int[4]	y	
      int *man1start_sw = link_dataref_int_arr ("AirbusFBW/ManStartSwitchArray", 4, 0);
      int *man2start_sw = link_dataref_int_arr ("AirbusFBW/ManStartSwitchArray", 4, 1);
//	AirbusFBW/ManStartLightsArray	int[4]	n	
      int *man1start_lt = link_dataref_int_arr ("AirbusFBW/ManStartLightsArray", 4, 0);
      int *man2start_lt = link_dataref_int_arr ("AirbusFBW/ManStartLightsArray", 4, 1);
//	AirbusFBW/N1ModeSwitchArray	int[4]	y	
      int *n1mode1_sw = link_dataref_int_arr ("AirbusFBW/N1ModeSwitchArray", 4, 0);
      int *n1mode2_sw = link_dataref_int_arr ("AirbusFBW/N1ModeSwitchArray", 4, 1);
//	AirbusFBW/N1ModeLightsArray	int[4]	n	
      int *n1mode1_lt = link_dataref_int_arr ("AirbusFBW/N1ModeLightsArray", 4, 0);
      int *n1mode2_lt = link_dataref_int_arr ("AirbusFBW/N1ModeLightsArray", 4, 1);
      /***** Wiper FO *****/
      // see Wiper Capt panel

  // master card 1, inputs (000-071)
  // master card 2, inputs (072-143)
  // master card 3, inputs (144-215)
  // master card 4, inputs (216-287)

  /* read inputs */

  /* Left-Hand Column */
  /***** Adirs *****/
  ret = digital_input(device,card1,SW_adirsIr1Off,&tmp1,0);				// ADIRS: IR1
  ret = digital_input(device,card1,SW_adirsIr1Nav,&tmp2,0);
  ret = digital_input(device,card1,SW_adirsIr1Att,&tmp3,0);
  if (tmp1) *adiru1_sw = 0;
  else if (tmp2) *adiru1_sw = 1;
  else *adiru1_sw = 2;
  ret = digital_input(device,card1,SW_adirsIr2Off,&tmp1,0);				// ADIRS: IR1
  ret = digital_input(device,card1,SW_adirsIr2Nav,&tmp2,0);
  ret = digital_input(device,card1,SW_adirsIr2Att,&tmp3,0);
  if (tmp1) *adiru2_sw = 0;
  else if (tmp2) *adiru2_sw = 1;
  else *adiru2_sw = 2;
  ret = digital_input(device,card1,SW_adirsIr3Off,&tmp1,0);				// ADIRS: IR3
  ret = digital_input(device,card1,SW_adirsIr3Nav,&tmp2,0);
  ret = digital_input(device,card1,SW_adirsIr3Att,&tmp3,0);
  if (tmp1) *adiru3_sw = 0;
  else if (tmp2) *adiru3_sw = 1;
  else *adiru3_sw = 2;
  ret = digital_input(device,card1,SW_adirsAdr1,adr1_sw,0);				// ADIRS: ADR1
  ret = digital_input(device,card1,SW_adirsAdr2,adr2_sw,0);				// ADIRS: ADR2
  ret = digital_input(device,card1,SW_adirsAdr3,adr3_sw,0);				// ADIRS: ADR3
  /***** Flt Ctrl Capt *****/
  ret = digital_input(device,card1,SW_fctlElac1,elac1_sw,0);			// Flt Ctrl Capt: ELAC1
  ret = digital_input(device,card1,SW_fctlSec1,sec1_sw,0);				// Flt Ctrl Capt: SEC1
  ret = digital_input(device,card1,SW_fctlFac1,fac1_sw,0);				// Flt Ctrl Capt: FAC1
  /***** Evac *****/
      // This panel not yet implemented
//  ret = digital_input(device,card1,SW_evacCmd,evac_cmd_sw,0);			// EVAC: Command
//  ret = digital_input(device,card1,PB_evacHornOff,evac_horn_pb,0);	// EVAC: Horn Cutoff
//  ret = digital_input(device,card1,SW_evacCaptPurs,evac_capt_sw,0);	// EVAC: Capt & Purs
  /***** Emer Elec Pwr *****/
      // EmerManOn switch in ElecOHPArray (ELEC panel)
  ret = digital_input(device,card1,SW_emerManOn,&temp,0);				// EMER ELEC: Man On
  *emer_man_sw = (temp) ? 2 : 0;	// (2, not 1: guarded switch!)
      // rest of this panel not yet implemented
//  ret = digital_input(device,card,SW_emerGenTest,emer_test_sw,0);		// EMER ELEC: 
//  ret = digital_input(device,card,SW_emerGen1Line,emer_gen1_sw,0);	// EMER ELEC: 
  /***** Gpws *****/
      // This panel not yet implemented
//  ret = digital_input(device,card,SW_gpwsTerr,gpws_terr_sw,0);		// GPWS: Terr
//  ret = digital_input(device,card,SW_gpwsSys,gpws_sys_sw,0);			// GPWS: Sys
//  ret = digital_input(device,card,SW_gpwsGSMode,gpws_gsmd_sw,0);		// GPWS: G/S Mode
//  ret = digital_input(device,card,SW_gpwsFlapMode,gpws_flmd_sw,0);	// GPWS: Flap Mode
//  ret = digital_input(device,card,SW_gpwsLdgFlp3,gpws_ldfl_sw,0);		// GPWS: Ldg Flap 3
  /***** Rcdr *****/
      // This panel not yet implemented
//  ret = digital_input(device,card,SW_rcdrGndCtl,rcdr_gndctl_sw,0);	// RCDR: Gnd Ctl
//  ret = digital_input(device,card,SW_rcdrErase,rcdr_erase_sw,0);		// RCDR: Erase
//  ret = digital_input(device,card,SW_rcdrTest,rcdr_test_sw,0);		// RCDR: Test
  /***** Oxygen *****/
      // CREW SUPPLY switch and lights not yet implemented
  ret = digital_input(device,card1,SW_oxyMaskManOn,&temp,0);			// OXYGEN: Passengers system
  *oxy_pax_sw = (temp) ? 2 : 0;	// (2, not 1: guarded switch!)
//  ret = digital_input(device,card,SW_oxyCrew,oxy_crew_sw,0);			// OXYGEN: Crew system
  /***** Calls *****/
      // MECH PB not yet implemented
//  ret = digital_input(device,card,PB_callsMech,calls_mech_pb,0);		// CALLS: Mech
  ret = digital_input(device,card1,PB_callsFwd,&tmp1,0);				// CALLS: Forward cabin
  ret = digital_input(device,card1,PB_callsAft,&tmp2,0);				// CALLS: Aft cabin
  if ((tmp1) || (tmp2)) *calls_chk_cabin_pb = 1;
  else *calls_chk_cabin_pb = 0;
  ret = digital_input(device,card1,SW_callsEmer,calls_emer_sw,0);		// CALLS: Emer
  /***** Wiper Capt  *****/
  ret = digital_input(device,card1,SW_lWiperOff,&temp,0);				// Wiper Speed (NOTE: same as for FO!)
  if (temp) *wiper_spd_sw = 0;
  else {
    ret = digital_input(device,card1,SW_lWiperSlow,&temp,0);
    if (temp) *wiper_spd_sw = 1;
    else {
      ret = digital_input(device,card1,SW_lWiperFast,&temp,0);
      if (temp) *wiper_spd_sw = 2;
    }
  }
//  ret = digital_input(device,card1,PB_lRainRplnt,&temp,0);			// Rain Repellant (NOTE: same as for FO!)
//  if (temp) { *rainRplnt = 0; }	// ! basic pushbuttons are NC !
//  else { *rainRplnt = 1; }
  /* Center Column */
  /***** Fire *****/
  // (Fire Panel not implemented)
//#ifdef _phager_airbus
//  ret = digital_input(device,card,PB_apuft,&temp,0);
//  if (temp) { *apu_fire_test = 1; }
//  else { *apu_fire_test = 0; }
//#endif
//  ret = digital_input(device,card,PB_eng1ft,&temp,0);
//  if (temp) { *eng1_fire_test = 1; }
//  else { *eng1_fire_test = 0; }
//  ret = digital_input(device,card,PB_eng2ft,&temp,0);
//  if (temp) { *eng2_fire_test = 1; }
//  else { *eng2_fire_test = 0; }
  /***** Hyd *****/
// Hydraulics Switches - 0=Eng 1 Pump, 1=Rat Man, 2=(blue) Elec Pump, 3=PTU, 4=Eng 2 Pump, 5=Yellow Elec Pump (numbers assumed)
  ret = digital_input(device,card2,SW_hydEng1Pmp,eng1pmp_sw,0);			// HYD: Eng 1 Pump
  ret = digital_input(device,card2,SW_hydRatMan,&temp,0);				// HYD: Rat Man On
  *ratman_sw = (temp) ? 2 : 0;	// (2, not 1: guarded switch!)
  ret = digital_input(device,card4,SW_hydBlElPmp,&temp,0);				// HYD: Blue Electric Pump
  *blelpmp_sw = (temp) ? 2 : 0;	// (2, not 1: guarded switch!)
  ret = digital_input(device,card3,SW_hydPtu,ptu_sw,0);					// HYD: PTU
  ret = digital_input(device,card3,SW_hydEng2Pmp,eng2pmp_sw,0);			// HYD: Eng 2 Pump
  ret = digital_input(device,card3,PB_hydYElecPmp,ylelpmp_pb,1);		// HYD: Yellow Electric Pump (PB!)
  /***** Fuel *****/
// Fuel Pump Switches - 0=LTkPmp1, 1=LTkPmp2, 2=CTkPmp1, 3=XFeed, 4=ModeSel, 5=CTkPmp2, 6=RTkPmp1, 7=RTkPmp2 (numbers assumed)
  ret = digital_input(device,card2,SW_fuelLTkPmp1,ltkpmp1_sw,0);		// FUEL: Left Tank Pump 1
  ret = digital_input(device,card2,SW_fuelLTkPmp2,ltkpmp2_sw,0);		// FUEL: Left Tank Pump 2
  ret = digital_input(device,card2,SW_fuelCTkPmp1,ctkpmp1_sw,0);		// FUEL: Center Tank Pump 1
  ret = digital_input(device,card4,SW_fuelXFeed,xfeed_sw,0);			// FUEL: X-Feed
  ret = digital_input(device,card2,SW_fuelModeSel,modesel_sw,0);		// FUEL: Mode Sel
  ret = digital_input(device,card3,SW_fuelCTkPmp2,ctkpmp2_sw,0);		// FUEL: Center Tank Pump 2
  ret = digital_input(device,card3,SW_fuelRTkPmp1,rtkpmp1_sw,0);		// FUEL: Right Tank Pump 1
  ret = digital_input(device,card3,SW_fuelRTkPmp2,rtkpmp2_sw,0);		// FUEL: Right Tank Pump 2
  /***** Elec *****/
// Elec Panel Switches - 0=Eng 1 Gen, 1=Eng 2 Gen, 2=APU Gen, 3=Ext Power, 4=Bus Tie, 5=Batt 1 Master, 6=Batt 2 Master,
//			 7 = --, 8= --, 9=Galley, 10=Emer Elec MAN ON (in Emer Elec Pwr panel)
  ret = digital_input(device,card2,SW_elecGen1,eng1gen_sw,0);			// ELEC: Eng 1 Generator
  ret = digital_input(device,card3,SW_elecGen2,eng2gen_sw,0);			// ELEC: Eng 2 Generator
  ret = digital_input(device,card2,SW_elecApuGen,apugen_sw,0);			// ELEC: APU Generator
  ret = digital_input(device,card2,PB_elecExtPwr,extpwr_sw,1);			// ELEC: Ext Power
  ret = digital_input(device,card2,SW_elecBusTie,bustie_sw,0);			// ELEC: Bus Tie
  ret = digital_input(device,card3,SW_elecBat1,batt1_sw,0);				// ELEC: Battery 1 Master
  ret = digital_input(device,card3,SW_elecBat2,batt2_sw,0);				// ELEC: Battery 2 Master 
  ret = digital_input(device,card3,SW_elecAcEss,&temp,0);				// ELEC: AC Ess Altn
  *ac_ess_altn_sw = (temp) ? 0 : 1;
  ret = digital_input(device,card2,SW_elecGalley,galley_sw,0);			// ELEC: Galley
  /***** Air Cond *****/
  ret = digital_input(device,card2,SW_condCkpt1,&cond1,0);				// COND: Cockpit Temperature
  ret = digital_input(device,card2,SW_condCkpt2,&cond2,0);
  ret = digital_input(device,card2,SW_condCkpt3,&cond3,0);
  ret = digital_input(device,card2,SW_condCkpt4,&cond4,0);
  ret = digital_input(device,card2,SW_condCkpt5,&cond5,0);
  ret = digital_input(device,card2,SW_condCkpt6,&cond6,0);
  ret = digital_input(device,card2,SW_condCkpt7,&cond7,0);
  if (cond1) *ckpt_temp_sw = 0;
  else if (cond2) *ckpt_temp_sw = 1;
  else if (cond3) *ckpt_temp_sw = 2;
  else if (cond4) *ckpt_temp_sw = 3;
  else if (cond5) *ckpt_temp_sw = 4;
  else if (cond6) *ckpt_temp_sw = 5;
  else if (cond7) *ckpt_temp_sw = 6;
  ret = digital_input(device,card2,SW_condFwd1,&cond1,0);				// COND: Forward Cabin Temp
  ret = digital_input(device,card2,SW_condFwd2,&cond2,0);
  ret = digital_input(device,card2,SW_condFwd3,&cond3,0);
  ret = digital_input(device,card2,SW_condFwd4,&cond4,0);
  ret = digital_input(device,card2,SW_condFwd5,&cond5,0);
  ret = digital_input(device,card2,SW_condFwd6,&cond6,0);
  ret = digital_input(device,card2,SW_condFwd7,&cond7,0);
  if (cond1) *fwd_cab_temp_sw = 0;
  else if (cond2) *fwd_cab_temp_sw = 1;
  else if (cond3) *fwd_cab_temp_sw = 2;
  else if (cond4) *fwd_cab_temp_sw = 3;
  else if (cond5) *fwd_cab_temp_sw = 4;
  else if (cond6) *fwd_cab_temp_sw = 5;
  else if (cond7) *fwd_cab_temp_sw = 6;
  ret = digital_input(device,card2,SW_condAft1,&cond1,0);				// COND: Aft Cabin Temp
  ret = digital_input(device,card2,SW_condAft2,&cond2,0);
  ret = digital_input(device,card2,SW_condAft3,&cond3,0);
  ret = digital_input(device,card2,SW_condAft4,&cond4,0);
  ret = digital_input(device,card2,SW_condAft5,&cond5,0);
  ret = digital_input(device,card2,SW_condAft6,&cond6,0);
  ret = digital_input(device,card2,SW_condAft7,&cond7,0);
  if (cond1) *aft_cab_temp_sw = 0;
  else if (cond2) *aft_cab_temp_sw = 1;
  else if (cond3) *aft_cab_temp_sw = 2;
  else if (cond4) *aft_cab_temp_sw = 3;
  else if (cond5) *aft_cab_temp_sw = 4;
  else if (cond6) *aft_cab_temp_sw = 5;
  else if (cond7) *aft_cab_temp_sw = 6;
  ret = digital_input(device,card3,SW_condPFlowH,&cond1,0);				// COND: Pack Flow
  ret = digital_input(device,card3,SW_condPFlowN,&cond2,0);
  ret = digital_input(device,card3,SW_condPFlowL,&cond3,0);
  if (cond1) *pack_flow_sw = 2;
  else if (cond2) *pack_flow_sw = 1;
  else *pack_flow_sw = 0;
  ret = digital_input(device,card1,SW_condXBldOpen,&cond1,0);			// COND: Cross Bleed
  ret = digital_input(device,card1,SW_condXBldAuto,&cond2,0);
  ret = digital_input(device,card1,SW_condXBldShut,&cond3,0);
  if (cond1) *x_bleed_sw = 2;
  else if (cond2) *x_bleed_sw = 1;
  else *x_bleed_sw = 0;
  ret = digital_input(device,card2,SW_condRamAir,ram_air_sw,0);			// COND: Ram Air
  ret = digital_input(device,card3,SW_condApuBld,apu_bleed_sw,0);		// COND: Apu Bleed
  ret = digital_input(device,card2,SW_condEng1Bld,eng1_bleed_sw,0);		// COND: Eng 1 Bleed
  ret = digital_input(device,card3,SW_condEng2Bld,eng2_bleed_sw,0);		// COND: Eng 2 Bleed
  ret = digital_input(device,card2,SW_condPack1,pack1_sw,0);			// COND: Pack 1
  ret = digital_input(device,card3,SW_condPack2,pack2_sw,0);			// COND: Pack 2
  ret = digital_input(device,card3,SW_condHotAir,cond_hot_air_sw,0);	// COND: Hot Air
  /***** Anti Ice *****/
  ret = digital_input(device,card2,SW_aIceWing,wing_aice_sw,0);			// ANTI ICE: Wing
  ret = digital_input(device,card2,SW_aIceEng1,eng1_aice_sw,0);			// ANTI ICE: Engine 1
  ret = digital_input(device,card2,SW_aIceEng2,eng2_aice_sw,0);			// ANTI ICE: Engine 2
  /***** Probe/Window Heat *****/
  ret = digital_input(device,card4,SW_probeWHeat,probe_heat_sw,0);		// PROBE/WINDOW: Heat
  /***** Cabin Press *****/
  ret = digital_input(device,card3,SW_pressManVSU,cab_vs_up,0);			// CABIN PRESS: V/S Up
  ret = digital_input(device,card3,SW_pressManVSD,cab_vs_dn,0);			// CABIN PRESS: V/S Down
  ret = digital_input(device,card3,SW_pressModeSel,cab_press_sw,0);		// CABIN PRESS: Ditching
  ret = digital_input(device,card3,SW_pressLdgElA,&land_elev_auto,0);	// CABIN PRESS: Landing Elevation Selector
  ret = digital_input(device,card3,SW_pressLdgElM2,&land_elev_min2,0);
  ret = digital_input(device,card3,SW_pressLdgEl0,&land_elev_0,0);
  ret = digital_input(device,card3,SW_pressLdgEl2,&land_elev_2,0);
  ret = digital_input(device,card3,SW_pressLdgEl4,&land_elev_4,0);
  ret = digital_input(device,card3,SW_pressLdgEl6,&land_elev_6,0);
  ret = digital_input(device,card3,SW_pressLdgEl7,&land_elev_7,0);
  ret = digital_input(device,card3,SW_pressLdgEl8,&land_elev_8,0);
  ret = digital_input(device,card3,SW_pressLdgEl10,&land_elev_10,0);
  if (land_elev_auto) *land_elev = 0.0;
  else if (land_elev_min2) *land_elev = 0.125;
  else if (land_elev_0) *land_elev = 0.25;
  else if (land_elev_2) *land_elev = 0.375;
  else if (land_elev_4) *land_elev = 0.5;
  else if (land_elev_6) *land_elev = 0.625;
  else if (land_elev_7) *land_elev = 0.75;
  else if (land_elev_8) *land_elev = 0.875;
  else *land_elev = 1.0;
  ret = digital_input(device,card3,SW_pressDitch,ditch_sw,0);			// CABIN PRESS: Ditching
  /***** Ext Lt *****/
  ret = digital_input(device,card4,SW_xtLtStrobeOn,&strobe_on_sw,0);	// EXT LT: Strobe
  ret = digital_input(device,card4,SW_xtLtStrobeOff,&strobe_off_sw,0);
  if (strobe_on_sw) *strobe_sw = 2;			// on
  else if (strobe_off_sw) *strobe_sw = 0;	// off
  else *strobe_sw = 1;						// (auto)
  ret = digital_input(device,card4,SW_xtLtBeacon,beacon_sw,0);			// EXT LT: Beacon
  ret = digital_input(device,card4,SW_xtLtWing,wing_sw,0);				// EXT LT: Wing
  ret = digital_input(device,card4,SW_xtLtNavL2,&tmp1,0);				// EXT LT: Nav & Logo
  ret = digital_input(device,card4,SW_xtLtNavLOff,&tmp2,0);
  if (tmp1) *navlogo_sw = 2;				// Sys 2
  else if (tmp2) *navlogo_sw = 0;			// Off
  else *navlogo_sw = 1;						// (Sys 1)
  ret = digital_input(device,card4,SW_xtLtRwyTOff,rwyto_sw,0);			// EXT LT: Rwy Turnoff
  ret = digital_input(device,card4,SW_xtLtLLdgOn,&tmp1,0);				// EXT LT: Left Landing Light
  ret = digital_input(device,card4,SW_xtLtLLdgRet,&tmp2,0);
  if (tmp1) *lldg_sw = 2;				// on
  else if (tmp2) *lldg_sw = 0;			// retracted
  else *lldg_sw = 1;					// (off)
  ret = digital_input(device,card4,SW_xtLtRLdgOn,&tmp1,0);				// EXT LT: Right Landing Light
  ret = digital_input(device,card4,SW_xtLtRLdgRet,&tmp2,0);
  if (tmp1) *rldg_sw = 2;				// on
  else if (tmp2) *rldg_sw = 0;			// retracted
  else *rldg_sw = 1;					// (off)
  ret = digital_input(device,card4,SW_xtLtNoseTO,&tmp1,0);				// EXT LT: Nose (Taxi) Light
  ret = digital_input(device,card4,SW_xtLtNoseOff,&tmp2,0);
  if (tmp1) *nose_sw = 2;				// take-off
  else if (tmp2) *nose_sw = 0;				// off
  else *nose_sw = 1;					// (taxi)
  /***** Int Lt *****/
  ret = digital_input(device,card3,SW_intLtDomeBrt,&tmp1,0);			// INT LT: Dome Light
  ret = digital_input(device,card3,SW_intLtDomeOff,&tmp2,0);
  if (tmp1) *dome_sw = 2;				// on
  else if (tmp2) *dome_sw = 0;			// off
  else *dome_sw = 1;					// (auto)
  ret = digital_input(device,card3,SW_intLtAnnTst,&ann_test_sw,0);		// INT LT: Annunciator Lights
  ret = digital_input(device,card3,SW_intLtAnnDim,&ann_dim_sw,0);
  if (ann_test_sw) {					// Test
    *annun_brt_sw = 2; //ltTest = 1;
  }
  else if (ann_dim_sw) {				// Dim (fake Cold&Dark)
    *annun_brt_sw = 0; // coldDark = 1;
  }
  else *annun_brt_sw = 1; //ltTest = 0;		// (bright)
  if (*annun_brt_sw == 2) ltTest = 1; else ltTest = 0;
  /***** Signs *****/
  ret = digital_input(device,card2,SW_signsBeltsOn,&seat_belts_on,0);		// SIGNS: Seat Belts
  ret = digital_input(device,card2,SW_signsBeltsOff,&seat_belts_off,0);
  if (seat_belts_on) *seat_belts_sw = 2;		// on
  else if (seat_belts_off) *seat_belts_sw = 0;	// off
  else *seat_belts_sw = 1;						// (auto)
  ret = digital_input(device,card2,SW_signsNoSmOn,&no_smoking_on,0);		// SIGNS: No Smoking
  ret = digital_input(device,card2,SW_signsNoSmOff,&no_smoking_off,0);
  if (no_smoking_on) *no_smoking_sw = 2;		// on
  else if (no_smoking_off) *no_smoking_sw = 0;	// off
  else *no_smoking_sw = 1;						// (auto)
  /***** EMER EXIT  *****/
//  ret = digital_input(device,card2,SW_signsEmXtOn,&emer_exit_on,0);		// SIGNS: Emer Exit (not implemented in sim)
//  ret = digital_input(device,card2,SW_signsEmXtOff,&emer_exit_off,0);
//  if (emer_exit_on) *emer_exit_sw = 2;
//  else if (emer_exit_off) *emer_exit_sw = 0;
//  else *emer_exit_sw = 1; // (auto)
  /***** Apu *****/
  ret = digital_input(device,card4,SW_apuMaster,apu_master_sw,0);			// APU: Master
  ret = digital_input(device,card4,PB_apuStart,apu_start_pb,1);				// APU: Start
  /* Right-Hand Column */
  /***** Flt Ctrl FO *****/
  ret = digital_input(device,card4,SW_fctlElac2,elac2_sw,0);				// Flt Ctrl FO: ELAC2
  ret = digital_input(device,card4,SW_fctlSec2,sec2_sw,0);					// Flt Ctrl FO: SEC2
  ret = digital_input(device,card4,SW_fctlSec3,sec3_sw,0);					// Flt Ctrl FO: SEC3
  ret = digital_input(device,card4,SW_fctlFac2,fac2_sw,0);					// Flt Ctrl FO: FAC2
  /***** Cargo Heat *****/
// t.b.s.
  /***** Cargo Smoke *****/
// t.b.s.
  /***** Ventilation *****/
  ret = digital_input(device,card4,SW_ventBlower,vent_blower_sw,0);			// VENT: Blower
  ret = digital_input(device,card4,SW_ventExtract,vent_extract_sw,0);		// VENT: Extract
  ret = digital_input(device,card4,SW_ventCabFans,vent_fans_sw,0);			// VENT: Cab Fans
 /***** Engines *****/
  ret = digital_input(device,card4,SW_engManSt1,man1start_sw,0);			// ENGINES: Man Start Eng 1
  ret = digital_input(device,card4,SW_engManSt2,man2start_sw,0);			// ENGINES: Man Start Eng 2
  ret = digital_input(device,card4,SW_engN1Md1,n1mode1_sw,0);				// ENGINES: N1 Mode Eng 1
  ret = digital_input(device,card4,SW_engN1Md2,n1mode2_sw,0);				// ENGINES: N1 Mode Eng 2
  /***** Wiper FO *****/
//  ret = digital_input(device,card4,SW_rWiperOff,&temp,0);					// Wiper Speed (NOTE: same as for Captain!)
//  if (temp) then *wiper_spd_sw = 0;
//  else {
//    ret = digital_input(device,card4,SW_rWiperSlow,&temp,0);
//    if (temp) then *wiper_spd_sw = 1;
//    else {
//      ret = digital_input(device,card4,SW_rWiperFast,&temp,0);
//      if (temp) then *wiper_spd_sw = 2;
//    }
//  }
//  ret = digital_input(device,card4,PB_rRainRplnt,&temp,0);				// Rain Repellant (NOTE: same as for Captain!)
//  if (temp) { *rainRplnt = 0; }	// ! basic pushbuttons are NC !
//  else { *rainRplnt = 1; }

  /* write displays */
  // master card 1, displays board #1 (00-15)

  /* codes for display II card
     10 = Blank the digit			--> 0xf7 (also 0x0a)
     11 = Put the "-" sign			--> 0xf8
     12 = Put a Special 6			--> 0xf9
     13 = Put a "t"					--> 0xfa
     14 = Put a "d"					--> 0xfb
     15 = Put a "_" (Underscore)	--> 0xfc (does not work; rather, has something to do with dimming...)
  */

#define LT_ohBatDp1 50
#define LT_ohBatDp2 52

  if (coldDark == 2) {	// cold&dark: blank all display digits and decimal points
	temp = 0; // value for the decimal point position
	tmp1 = 0xf7;
	ret = mastercard_display(device, card1, DP_ohBat1,   1, &tmp1, 0);
	ret = mastercard_display(device, card1, DP_ohBat1+1, 1, &tmp1, 0);
	ret = mastercard_display(device, card1, DP_ohBat1+2, 1, &tmp1, 0);
	ret = mastercard_display(device, card1, DP_ohBat2,   1, &tmp1, 0);
	ret = mastercard_display(device, card1, DP_ohBat2+1, 1, &tmp1, 0);
	ret = mastercard_display(device, card1, DP_ohBat2+2, 1, &tmp1, 0);
  } else {
	temp = 1; // value for the decimal point position
	if (ltTest == 1) {
	  tmp1 = 888;
	  tmp2 = 888;
	} else {
	  tmp1 = (int) ((*bat1v_dp * 10) + 0.3);
	  tmp2 = (int) ((*bat2v_dp * 10) + 0.3);
	}
	ret = mastercard_display(device, card1, DP_ohBat1, DP_ohBatSz, &tmp1, 0); // Batt 1 value
	ret = mastercard_display(device, card1, DP_ohBat2, DP_ohBatSz, &tmp2, 0); // Batt 2 value
  }
	ret = digital_output(device, card1, LT_ohBatDp1, &temp); // Batt 1 decimal point
	ret = digital_output(device, card1, LT_ohBatDp2, &temp); // Batt 1 decimal point
//printf ("Cold&Dark = %i, LightsTest = %i, decPt = %i\n", coldDark, ltTest, temp); // Note: does not show on displays!!??!!

// ********************************************************************************
//  // Code to produce "Std" for Barometer display (future... how to specify?)
//  tmp2 = 0xfb; ret = mastercard_display(device, card1, DP_ohBat2,   1, &tmp2, 0);
//  tmp2 = 0xfa; ret = mastercard_display(device, card1, DP_ohBat2+1, 1, &tmp2, 0);
//  tmp2 = 0x05; ret = mastercard_display(device, card1, DP_ohBat2+2, 1, &tmp2, 0);
// Note: this belongs in a320_pedestal_mip.c!
// ********************************************************************************

  // master card 1, displays board #2 (16-31) (...)
  // IRS displays??

  /* ... */

  /* write outputs */
  // master card 1, outputs (11-55)
  // master card 2, outputs (..-..)
  // master card 3, outputs (..-..)
  // master card 4, outputs (..-..)

  /* Left-Hand Column */
  /***** Adirs *****/
  temp = lightCheck(*adiru_onbat_lt);														// ADIRS: On Bat
  ret = digital_output(device,card1,LT_adirsOnBat,&temp);
  tmp1 = lightCheck(*ir1_lt >> 1); tmp2 = lightCheck(*ir1_lt % 2);							// ADIRS: IR1
  ret = digital_output(device,card1,LT_adirsIr1U,&tmp1);
  ret = digital_output(device,card1,LT_adirsIr1L,&tmp2);
  tmp1 = lightCheck(*ir2_lt >> 1); tmp2 = lightCheck(*ir2_lt % 2);							// ADIRS: IR3
  ret = digital_output(device,card1,LT_adirsIr2U,&tmp1);
  ret = digital_output(device,card1,LT_adirsIr2L,&tmp2);
  tmp1 = lightCheck(*ir3_lt >> 1); tmp2 = lightCheck(*ir3_lt % 2);							// ADIRS: IR2
  ret = digital_output(device,card1,LT_adirsIr3U,&tmp1);
  ret = digital_output(device,card1,LT_adirsIr3L,&tmp2);
  tmp1 = lightCheck(*adr1_lt >> 1); tmp2 = lightCheck(1 - (*adr1_lt % 2));					// ADIRS: ADR1
  ret = digital_output(device,card1,LT_adirsAdr1U,&tmp1);
  ret = digital_output(device,card1,LT_adirsAdr1L,&tmp2);
  tmp1 = lightCheck(*adr2_lt >> 1); tmp2 = lightCheck(1 - (*adr2_lt % 2));					// ADIRS: ADR3
  ret = digital_output(device,card1,LT_adirsAdr2U,&tmp1);
  ret = digital_output(device,card1,LT_adirsAdr2L,&tmp2);
  tmp1 = lightCheck(*adr3_lt >> 1); tmp2 = lightCheck(1 - (*adr3_lt % 2));					// ADIRS: ADR2
  ret = digital_output(device,card1,LT_adirsAdr3U,&tmp1);
  ret = digital_output(device,card1,LT_adirsAdr3L,&tmp2);
  /***** Flt Ctrl Capt & FO *****/
  tmp1 = lightCheck(*elac1_lt >> 1); tmp2 = lightCheck(1 - (*elac1_lt % 2));				// Flt Control Capt: ELAC1
  ret = digital_output(device,card1,LT_fctlElac1U,&tmp1);
  ret = digital_output(device,card1,LT_fctlElac1L,&tmp2);
  tmp1 = lightCheck(*sec1_lt >> 1); tmp2 = lightCheck(1 - (*sec1_lt % 2));					// Flt Control Capt: SEC1
  ret = digital_output(device,card1,LT_fctlSec1U,&tmp1);
  ret = digital_output(device,card1,LT_fctlSec1L,&tmp2);
  tmp1 = lightCheck(*fac1_lt >> 1); tmp2 = lightCheck(1 - (*fac1_lt % 2));					// Flt Control Capt: FAC1
  ret = digital_output(device,card1,LT_fctlFac1U,&tmp1);
  ret = digital_output(device,card1,LT_fctlFac1L,&tmp2);
  /***** Evac *****/
//  temp = lightCheck(*evac_cmd_lt);														// EVAC: Command
//  ret = digital_output(device,card1,LT_evacCmdL,&temp);
//  ret = digital_output(device,card1,LT_evacCmdU,&zero);
  /***** Emer Elec Pwr *****/
//  temp = lightCheck(*emer_ln1u_lt);														// EMER ELEC: Gen 1 Line
//  ret = digital_output(device,card1,LT_emerGen1LnU,&temp);
//  temp = lightCheck(*emer_ln1l_lt); 
//  ret = digital_output(device,card1,LT_emerGen1LnL,&temp);
//  temp = lightCheck(*emer_rat_lt);														// EMER ELEC: Rat & Emer Gen
//  ret = digital_output(device,card1,LT_emerRatU,&temp);
  /***** Gpws *****/
//  temp = lightCheck(*gpws_terru_lt);														// GPWS: Terr
//  ret = digital_output(device,card1,LT_gpwsTerrU,&temp);
//  temp = lightCheck(*gpws_terrl_lt);
//  ret = digital_output(device,card1,LT_gpwsTerrL,&temp);
//  temp = lightCheck(*gpws_sysu_lt);														// GPWS: Sys
//  ret = digital_output(device,card1,LT_gpwsSysU,&temp);
//  temp = lightCheck(*gpws_sysl_lt); 
//  ret = digital_output(device,card1,LT_gpwsSysL,&temp);
//  temp = lightCheck(*gpws_gsmode_lt);														// GPWS: G/S Mode
//  ret = digital_output(device,card1,LT_gpwsGSMdL,&temp);
//  temp = lightCheck(*gpws_flapmode_lt);													// GPWS: Flap Mode
//  ret = digital_output(device,card1,LT_gpwsFlapMdL,&temp);
//  temp = lightCheck(*gpws_ldgflp3_lt);													// GPWS: Ldg Flap 3
//  ret = digital_output(device,card1,LT_gpwsLdgFlap3L,&temp);
  /***** Rcdr *****/
//  temp = lightCheck(*rcdr_gnd_lt);														// RCDR: Gnd Ctrl
//  ret = digital_output(device,card1,LT_rcdrGndCtlL,&temp);
  /***** Oxygen *****/
  temp = lightCheck(*oxy_pax_lt);															// OXYGEN: Passenger
  ret = digital_output(device,card1,LT_oxyPassL,&temp);
  temp = lightCheck(oxy_crew_lt);															// OXYGEN: Crew
  ret = digital_output(device,card1,LT_oxyCrewL,&temp);
  /***** Calls *****/
  temp = lightCheck(*calls_emer_lt);														// CALLS: Emer
  ret = digital_output(device,card1,LT_callsEmerL,&temp);
//  ret = digital_output(device,card1,LT_callsEmerU,&zero);
  /***** Wiper Capt *****/
  /* Center Column */
  /***** Fire *****/
  // This panel not yet implemented
  /***** Hyd *****/
  tmp1 = lightCheck(*eng1pmp_lt >> 1); tmp2 = lightCheck(1 - (*eng1pmp_lt % 2));			// HYD: Eng 1 (Green) Pump
  ret = digital_output(device,card2,LT_hydEng1PmpU,&tmp1);
  ret = digital_output(device,card2,LT_hydEng1PmpL,&tmp2);
  tmp1 = lightCheck(*eng2pmp_lt >> 1); tmp2 = lightCheck(1 - (*eng2pmp_lt % 2));			// HYD: Eng 2 (Yellow) Pump
  ret = digital_output(device,card3,LT_hydEng2PmpU,&tmp1);
  ret = digital_output(device,card3,LT_hydEng2PmpL,&tmp2);
  tmp1 = lightCheck(*blelpmp_lt >> 1); tmp2 = lightCheck(1 - (*blelpmp_lt % 2));			// HYD: Blue Elec Pump
  ret = digital_output(device,card4,LT_hydBElecPmpU,&tmp1);
  ret = digital_output(device,card4,LT_hydBElecPmpL,&tmp2);
  tmp1 = lightCheck(*ylelpmp_lt >> 1); tmp2 = lightCheck(*ylelpmp_lt % 2);					// HYD: Yellow Elec Pump
  ret = digital_output(device,card3,LT_hydYElecPmpU,&tmp1);
  ret = digital_output(device,card3,LT_hydYElecPmpL,&tmp2);
  tmp1 = lightCheck(*hyd_ptu_lt >> 1); tmp2 = lightCheck(1 - (*hyd_ptu_lt % 2));			// HYD: PTU
  ret = digital_output(device,card3,LT_hydPtuU,&tmp1);
  ret = digital_output(device,card3,LT_hydPtuL,&tmp2);
  /***** Fuel *****/
  tmp1 = lightCheck(*modesel_lt >> 1); tmp2 = lightCheck(1 - (*modesel_lt % 2));			// FUEL: Mode Sel
  ret = digital_output(device,card2,LT_fuelModeSelU,&tmp1);
  ret = digital_output(device,card2,LT_fuelModeSelL,&tmp2);
  tmp1 = lightCheck(*ltkpmp1_lt >> 1); tmp2 = lightCheck(1 - (*ltkpmp1_lt % 2));			// FUEL: Left Tank Pump 1
  ret = digital_output(device,card2,LT_fuelLTkPmp1U,&tmp1);
  ret = digital_output(device,card2,LT_fuelLTkPmp1L,&tmp2);
  tmp1 = lightCheck(*ltkpmp2_lt >> 1); tmp2 = lightCheck(1 - (*ltkpmp2_lt % 2));			// FUEL: Left Tank Pump 2
  ret = digital_output(device,card2,LT_fuelLTkPmp2U,&tmp1);
  ret = digital_output(device,card2,LT_fuelLTkPmp2L,&tmp2);
  tmp1 = lightCheck(*rtkpmp1_lt >> 1); tmp2 = lightCheck(1 - (*rtkpmp1_lt % 2));			// FUEL: Right Tank Pump 1
  ret = digital_output(device,card3,LT_fuelRTkPmp1U,&tmp1);
  ret = digital_output(device,card3,LT_fuelRTkPmp1L,&tmp2);
  tmp1 = lightCheck(*rtkpmp2_lt >> 1); tmp2 = lightCheck(1 - (*rtkpmp2_lt % 2));			// FUEL: Right Tank Pump 2
  ret = digital_output(device,card3,LT_fuelRTkPmp2U,&tmp1);
  ret = digital_output(device,card3,LT_fuelRTkPmp2L,&tmp2);
  tmp1 = lightCheck(*ctkpmp1_lt >> 1); tmp2 = lightCheck(1 - (*ctkpmp1_lt % 2));			// FUEL: Center Tank Pump 1
  ret = digital_output(device,card2,LT_fuelCTkPmp1U,&tmp1);
  ret = digital_output(device,card2,LT_fuelCTkPmp1L,&tmp2);
  tmp1 = lightCheck(*ctkpmp2_lt >> 1); tmp2 = lightCheck(1 - (*ctkpmp2_lt % 2));			// FUEL: Center Tank Pump 2
  ret = digital_output(device,card3,LT_fuelCTkPmp2U,&tmp1);
  ret = digital_output(device,card3,LT_fuelCTkPmp2L,&tmp2);
  tmp1 = lightCheck(*xfeed_lt >> 1); tmp2 = lightCheck(*xfeed_lt % 2);						// FUEL: X-Feed
  ret = digital_output(device,card4,LT_fuelXFeedU,&tmp1);
  ret = digital_output(device,card4,LT_fuelXFeedL,&tmp2);
  /***** Elec *****/
  tmp1 = lightCheck(*ac_ess_altn_lt >> 1); tmp2 = lightCheck(*ac_ess_altn_lt % 2);			// ELEC: AC Ess Altn
  ret = digital_output(device,card3,LT_elecAcEssU,&tmp1);
  ret = digital_output(device,card3,LT_elecAcEssL,&tmp2);
  tmp1 = lightCheck(*apu_pwr_lt >> 1); tmp2 = lightCheck(1 - (*apu_pwr_lt % 2));			// ELEC: APU Gen
  ret = digital_output(device,card2,LT_elecApuGenU,&tmp1);
  ret = digital_output(device,card2,LT_elecApuGenL,&tmp2);
  tmp1 = lightCheck(*bat1_pwr_lt >> 1); tmp2 = lightCheck(1 - (*bat1_pwr_lt % 2));			// ELEC: Batt 1
  ret = digital_output(device,card3,LT_elecBat1U,&tmp1);
  ret = digital_output(device,card3,LT_elecBat1L,&tmp2);
  tmp1 = lightCheck(*bat2_pwr_lt >> 1); tmp2 = lightCheck(1 - (*bat2_pwr_lt % 2));			// ELEC: Batt 2
  ret = digital_output(device,card3,LT_elecBat2U,&tmp1);
  ret = digital_output(device,card3,LT_elecBat2L,&tmp2);
  tmp1 = lightCheck(*bus_xtie_lt >> 1); tmp2 = lightCheck(1 - (*bus_xtie_lt % 2));			// ELEC: Bus Tie
//  ret = digital_output(device,card2,LT_elecBusTieU,&tmp1);
  ret = digital_output(device,card2,LT_elecBusTieL,&tmp2);
  tmp1 = lightCheck(*gen1_pwr_lt >> 1); tmp2 = lightCheck(1 - (*gen1_pwr_lt % 2));			// ELEC: Eng 1 Generator
  ret = digital_output(device,card2,LT_elecGen1U,&tmp1);
  ret = digital_output(device,card2,LT_elecGen1L,&tmp2);
  tmp1 = lightCheck(*gen2_pwr_lt >> 1); tmp2 = lightCheck(1 - (*gen2_pwr_lt % 2));			// ELEC: Eng 2 Generator
  ret = digital_output(device,card3,LT_elecGen2U,&tmp1);
  ret = digital_output(device,card3,LT_elecGen2L,&tmp2);
  tmp1 = lightCheck(*ext_pwr_lt >> 1); tmp2 = lightCheck(*ext_pwr_lt % 2);					// ELEC: Ext Power
  ret = digital_output(device,card2,LT_elecExtPwrU,&tmp1);
  ret = digital_output(device,card2,LT_elecExtPwrL,&tmp2);
  tmp1 = lightCheck(*galley_lt >> 1); tmp2 = lightCheck(1 - (*galley_lt % 2));				// ELEC: Galley
  ret = digital_output(device,card2,LT_elecGalleyU,&tmp1);
  ret = digital_output(device,card2,LT_elecGalleyL,&tmp2);
//  tmp1 = lightCheck(*_lt[1] >> 1); tmp2 = lightCheck(1 - (*_lt[1] % 2));					// ELEC: IDG1
  ret = digital_output(device,card2,LT_elecIdg1U,&zero);
//  ret = digital_output(device,card2,LT_elecIdg1L,&zero);
//  tmp1 = lightCheck(*_lt[1] >> 1); tmp2 = lightCheck(1 - (*_lt[1] % 2));					// ELEC: IDG2
  ret = digital_output(device,card3,LT_elecIdg2U,&zero);
//  ret = digital_output(device,card3,LT_elecIdg2L,&zero);
  /***** Air Cond *****/
  tmp1 = lightCheck(0); tmp2 = lightCheck(*apu_bleed_sw);									// AIR COND: Apu Bleed
//  if ((*apu_press == 6) && (*apu_N1 >= 95)) tmp1 = lightCheck(1);
  ret = digital_output(device,card3,LT_condApuBldU,&tmp1);
  ret = digital_output(device,card3,LT_condApuBldL,&tmp2);
  tmp1 = lightCheck(0); tmp2 = lightCheck(1 - *eng1_bleed_sw);								// AIR COND: Eng 1 Bleed
  if ((*bleed_air_left == 6) && (*eng1_N2 >= 95)) tmp1 = lightCheck(1);
  ret = digital_output(device,card2,LT_condEng1BldU,&tmp1);
  ret = digital_output(device,card2,LT_condEng1BldL,&tmp2);
  tmp1 = lightCheck(0); tmp2 = lightCheck(1 - *eng2_bleed_sw);								// AIR COND: Eng 2 Bleed
  if ((*bleed_air_right == 6) && (*eng2_N2 >= 95)) tmp1 = lightCheck(1);
  ret = digital_output(device,card3,LT_condEng2BldU,&tmp1);
  ret = digital_output(device,card3,LT_condEng2BldL,&tmp2);
  tmp1 = lightCheck(*cond_hot_air_lt >> 1); tmp2 = lightCheck(1 - (*cond_hot_air_lt % 2));	// AIR COND: Hot Air
  ret = digital_output(device,card3,LT_condHotAirU,&tmp1);
  ret = digital_output(device,card3,LT_condHotAirL,&tmp2);
  tmp1 = lightCheck(*pack1_lt >> 1); tmp2 = lightCheck(1 - (*pack1_lt % 2));				// AIR COND: Pack 1
  ret = digital_output(device,card2,LT_condPack1U,&tmp1);
  ret = digital_output(device,card2,LT_condPack1L,&tmp2);
  tmp1 = lightCheck(*pack2_lt >> 1); tmp2 = lightCheck(1 - (*pack2_lt % 2));				// AIR COND: Pack 2
  ret = digital_output(device,card3,LT_condPack2U,&tmp1);
  ret = digital_output(device,card3,LT_condPack2L,&tmp2);
  temp = lightCheck(*ram_air_lt);															// AIR COND: Ram Air
  ret = digital_output(device,card2,LT_condRamAirL,&temp);
  /***** Anti Ice *****/
  tmp1 = lightCheck(*eng1_aice_lt >> 1); tmp2 = lightCheck(*eng1_aice_lt % 2);				// ANTI ICE: Eng 1
  ret = digital_output(device,card2,LT_aIceEng1U,&tmp1);
  ret = digital_output(device,card2,LT_aIceEng1L,&tmp2);
  tmp1 = lightCheck(*eng2_aice_lt >> 1); tmp2 = lightCheck(*eng2_aice_lt % 2);				// ANTI ICE: Eng 2
  ret = digital_output(device,card2,LT_aIceEng2U,&tmp1);
  ret = digital_output(device,card2,LT_aIceEng2L,&tmp2);
  tmp1 = lightCheck(*wing_aice_lt >> 1); tmp2 = lightCheck(*wing_aice_lt % 2);				// ANTI ICE: Wing
  ret = digital_output(device,card2,LT_aIceWingU,&tmp1);
  ret = digital_output(device,card2,LT_aIceWingL,&tmp2);
  /***** Probe/Window Heat *****/
  temp = lightCheck(*probe_heat_lt);														// PROBE/WINDOW: Heat
  ret = digital_output(device,card4,LT_probeWHeatL,&temp);
  /***** Cabin Press *****/
  tmp1 = lightCheck(*cab_press_lt >> 1); tmp2 = lightCheck(1 - (*cab_press_lt % 2));		// CAB PRESS: Mode Sel
  ret = digital_output(device,card3,LT_pressMdSelU,&tmp1);
  ret = digital_output(device,card3,LT_pressMdSelL,&tmp2);
  temp = lightCheck(*ditch_lt);																// CAB PRESS: Ditching
  ret = digital_output(device,card3,LT_pressDitchL,&temp);
  /***** Ext Lt *****/
  /***** Int Lt *****/
  /***** Signs *****/
  /***** EMER EXIT *****/
  ret = digital_output(device,card3,LT_signsEmXtL,&zero);
  /***** Apu *****/
//  tmp1 = lightCheck(*apu_master_lt >> 1); tmp2 = lightCheck(1 - (*apu_master_lt % 2));	// APU: Master
//  ret = digital_output(device,card4,LT_apuMasterU,&tmp1);
//  ret = digital_output(device,card4,LT_apuMasterL,&tmp2);
  temp = lightCheck(*apu_master_sw);														// APU: Master SW (!)
  ret = digital_output(device,card4,LT_apuMasterL,&temp);
//  tmp1 = lightCheck(*apu_start_lt >> 1); tmp2 = lightCheck(*apu_start_lt % 2);			// APU: Start
//  ret = digital_output(device,card4,LT_apuStartU,&tmp1);
//  ret = digital_output(device,card4,LT_apuStartL,&tmp2);
  temp = lightCheck(*apu_avail_lt);															// APU: Avail LT
  ret = digital_output(device,card4,LT_apuStartU,&temp);
  temp = lightCheck(*apu_start_pb);															// APU: Start PB (!)
  ret = digital_output(device,card4,LT_apuStartL,&temp);
  /* Right-Hand Column */
  /***** Flt Ctrl FO *****/
  tmp1 = lightCheck(*elac2_lt >> 1); tmp2 = lightCheck(1 - (*elac2_lt % 2));				// Flt Control FO: ELAC2
  ret = digital_output(device,card4,LT_fctlElac2U,&tmp1);
  ret = digital_output(device,card4,LT_fctlElac2L,&tmp2);
  tmp1 = lightCheck(*sec2_lt >> 1); tmp2 = lightCheck(1 - (*sec2_lt % 2));					// Flt Control FO: SEC2
  ret = digital_output(device,card4,LT_fctlSec2U,&tmp1);
  ret = digital_output(device,card4,LT_fctlSec2L,&tmp2);
  tmp1 = lightCheck(*sec3_lt >> 1); tmp2 = lightCheck(1 - (*sec3_lt % 2));					// Flt Control FO: SEC3
  ret = digital_output(device,card4,LT_fctlSec3U,&tmp1);
  ret = digital_output(device,card4,LT_fctlSec3L,&tmp2);
  tmp1 = lightCheck(*fac2_lt >> 1); tmp2 = lightCheck(1 - (*fac2_lt % 2));					// Flt Control FO: FAC2
  ret = digital_output(device,card4,LT_fctlFac2U,&tmp1);
  ret = digital_output(device,card4,LT_fctlFac2L,&tmp2);
  /***** Cargo Heat *****/
  ret = digital_output(device,card4,LT_cargoHotAirU,&zero);									// Cargo Heat: Hot Air
  ret = digital_output(device,card4,LT_cargoHotAirL,&zero);
  ret = digital_output(device,card4,LT_cargoAftIsolU,&zero);								// Cargo Heat: Aft Isol Valve
  ret = digital_output(device,card4,LT_cargoAftIsolL,&zero);
  /***** Cargo Smoke *****/
  ret = digital_output(device,card4,LT_cargoSmFwdU,&zero);									// Cargo Smoke: Forward Smoke
  ret = digital_output(device,card4,LT_cargoSmAftU,&zero);									// Cargo Smoke: Aft Smoke
//  ret = digital_output(device,card,LT_cargoSDischAg1,&zero);								// Cargo Smoke: Discharged Agent 1
//  ret = digital_output(device,card,LT_cargoSDischAg2,&zero);								// Cargo Smoke: Discharged Agent 2
  /***** Ventilation *****/
  tmp1 = lightCheck(*vent_blower_lt >> 1); tmp2 = lightCheck(*vent_blower_lt % 2);			// Ventilation: Blower
  ret = digital_output(device,card4,LT_ventBlowerU,&tmp1);
  ret = digital_output(device,card4,LT_ventBlowerL,&tmp2);
  tmp1 = lightCheck(*vent_extract_lt >> 1); tmp2 = lightCheck(*vent_extract_lt % 2);		// Ventilation: Extract
  ret = digital_output(device,card4,LT_ventExtractU,&tmp1);
  ret = digital_output(device,card4,LT_ventExtractL,&tmp2);
  tmp1 = lightCheck(*vent_fans_lt >> 1); tmp2 = lightCheck(1 - (*vent_fans_lt % 2));		// Ventilation: Cab Fans
//  ret = digital_output(device,card4,LT_ventCabFansU,&tmp1);
  ret = digital_output(device,card4,LT_ventCabFansL,&tmp2);
  /***** Engines *****/
  tmp1 = lightCheck(*man1start_lt >> 1); tmp2 = lightCheck(*man1start_lt % 2);				// Engines: Manual Start Eng 1
//  ret = digital_output(device,card4,LT_engManSt1U,&tmp1);
  ret = digital_output(device,card4,LT_engManSt1L,&tmp2);
  tmp1 = lightCheck(*man2start_lt >> 1); tmp2 = lightCheck(*man2start_lt % 2);				// Engines: Manual Start Eng 2
//  ret = digital_output(device,card4,LT_engManSt2U,&tmp1);
  ret = digital_output(device,card4,LT_engManSt2L,&tmp2);
  tmp1 = lightCheck(*n1mode1_lt >> 1); tmp2 = lightCheck(*n1mode1_lt % 2);					// Engines: N1 Mode Eng 1
//  ret = digital_output(device,card4,LT_engN1Md1U,&tmp1);
  ret = digital_output(device,card4,LT_engN1Md1L,&tmp2);
  tmp1 = lightCheck(*n1mode2_lt >> 1); tmp2 = lightCheck(*n1mode2_lt % 2);					// Engines: N1 Mode Eng 1
//  ret = digital_output(device,card4,LT_engN1Md2U,&tmp1);
  ret = digital_output(device,card4,LT_engN1Md2L,&tmp2);
  /***** Wiper FO *****/
  // See Wiper Capt panel

}
