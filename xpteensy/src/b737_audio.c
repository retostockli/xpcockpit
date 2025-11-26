/* This is the b737_audio.c code which controls the digital audio mixer
   in my Boeing 737 cockpit, fed by the three audio control panels, using a Teensy

   Copyright (C) 2025 Reto Stockli

   Also several cosmetic changes and changes for Linux compilation
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
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libteensy.h"
#include "serverdata.h"
#include "b737_audio.h"

void init_b737_audio(void)
{
  int te = 2;

  /* Captain ACP Mixer */
  teensy[te].pinmode[0] = PINMODE_INPUT;  /* Hand Mic PTT */
  teensy[te].pinmode[3] = PINMODE_OUTPUT; /* Headset Mic Enable */
  teensy[te].pinmode[4] = PINMODE_OUTPUT; /* Hand Mic Enable */
  teensy[te].pinmode[5] = PINMODE_OUTPUT; /* Mask Mic Enable */
  teensy[te].pinmode[6] = PINMODE_OUTPUT; /* Mic to Line IN Enable */

  pga2311[te][0].spi = 0; // SPI Bus number
  pga2311[te][0].cs = 1; // Chip Select Pin
  pga2311[te][1].spi = 0; // SPI Bus number
  pga2311[te][1].cs = 2; // Chip Select Pin

  /* First Officer ACP Mixer */
  teensy[te].pinmode[16] = PINMODE_INPUT;  /* Hand Mic PTT */
  teensy[te].pinmode[21] = PINMODE_OUTPUT; /* Headset Mic Enable */
  teensy[te].pinmode[22] = PINMODE_OUTPUT; /* Hand Mic Enable */
  teensy[te].pinmode[23] = PINMODE_OUTPUT; /* Mask Mic Enable */
  teensy[te].pinmode[24] = PINMODE_OUTPUT; /* Mic to Line IN Enable */
 
  pga2311[te][2].spi = 0; // SPI Bus number
  pga2311[te][2].cs = 17; // Chip Select Pin
  pga2311[te][3].spi = 0; // SPI Bus number
  pga2311[te][3].cs = 20; // Chip Select Pin

  /* Jump Seat ACP Mixer */
  teensy[te].pinmode[26] = PINMODE_INPUT;  /* Hand Mic PTT */
  teensy[te].pinmode[29] = PINMODE_OUTPUT; /* Headset Mic Enable */
  teensy[te].pinmode[30] = PINMODE_OUTPUT; /* Hand Mic Enable */
  teensy[te].pinmode[31] = PINMODE_OUTPUT; /* Mask Mic Enable */
  teensy[te].pinmode[32] = PINMODE_OUTPUT; /* Mic to Line IN Enable */
 
  pga2311[te][4].spi = 0; // SPI Bus number
  pga2311[te][4].cs = 27; // Chip Select Pin
  pga2311[te][5].spi = 0; // SPI Bus number
  pga2311[te][5].cs = 28; // Chip Select Pin

  /* /\****** START TESTING COMMENT OUT WHEN OPERATIONAL ******\/ */
  /* /\* For testing inputs directly on Teensy in Audio Box *\/ */
  /* teensy[te].pinmode[9] = PINMODE_INPUT; */
  /* teensy[te].pinmode[10] = PINMODE_INPUT; */

  /* teensy[te].pinmode[38] = PINMODE_ANALOGINPUTMEAN; */
  /* teensy[te].pinmode[39] = PINMODE_ANALOGINPUTMEAN; */
  /* teensy[te].pinmode[40] = PINMODE_ANALOGINPUTMEAN; */
  /* /\****** END TESTING COMMENT OUT WHEN OPERATIONAL ******\/ */

  /* For testing */
  /* mcp23017[te][0].pinmode[0] = PINMODE_OUTPUT; */
  /* mcp23017[te][0].intpin = INITVAL; // also define pin 6 of teensy as INTERRUPT above! */
  /* mcp23017[te][0].wire = 0;  // I2C Bus: 0, 1 or 2 */
  /* mcp23017[te][0].address = 0x20; // I2C address of MCP23017 device */
  
}

void b737_audio(void)
{

  int ret;
  int te = 2;
 
  int *mic_capt_yoke = link_dataref_int("xpserver/mic_capt_yoke");
  int *mic_fo_yoke = link_dataref_int("xpserver/mic_fo_yoke");
  int *mic_capt = link_dataref_int("xpserver/mic_capt");
  int *mic_fo = link_dataref_int("xpserver/mic_fo");
 
  int *acp1_micsel_vhf1 = link_dataref_int("xpserver/acp1_micsel_vhf1");
  int *acp1_micsel_vhf2 = link_dataref_int("xpserver/acp1_micsel_vhf2");
  int *acp1_mask_boom = link_dataref_int("xpserver/acp1_mask_boom");
  
  float *acp1_vol_vhf1 = link_dataref_flt("xpserver/acp1_vol_vhf1",0);
  float *acp1_vol_vhf2 = link_dataref_flt("xpserver/acp1_vol_vhf2",0);
  float *acp1_vol_pa = link_dataref_flt("xpserver/acp1_vol_pa",0);
  float *acp1_vol_spkr = link_dataref_flt("xpserver/acp1_vol_spkr",0);
  
  int *acp2_micsel_vhf1 = link_dataref_int("xpserver/acp2_micsel_vhf1");
  int *acp2_micsel_vhf2 = link_dataref_int("xpserver/acp2_micsel_vhf2");
  int *acp2_mask_boom = link_dataref_int("xpserver/acp2_mask_boom");
  
  float *acp2_vol_vhf1 = link_dataref_flt("xpserver/acp2_vol_vhf1",0);
  float *acp2_vol_vhf2 = link_dataref_flt("xpserver/acp2_vol_vhf2",0);
  float *acp2_vol_pa = link_dataref_flt("xpserver/acp2_vol_pa",0);
  float *acp2_vol_spkr = link_dataref_flt("xpserver/acp2_vol_spkr",0);

  int *acp3_mask_boom = link_dataref_int("xpserver/acp3_mask_boom");

  float *acp3_vol_vhf1 = link_dataref_flt("xpserver/acp3_vol_vhf1",0);
  float *acp3_vol_vhf2 = link_dataref_flt("xpserver/acp3_vol_vhf2",0);
  float *acp3_vol_pa = link_dataref_flt("xpserver/acp3_vol_pa",0);
  float *acp3_vol_spkr = link_dataref_flt("xpserver/acp3_vol_spkr",0);

  int *tx_com1;
  int *tx_com2;
  if ((acf_type == 2) || (acf_type == 3)) {
    tx_com1 = link_dataref_cmd_once("laminar/B738/audio/capt/mic_push1");
    tx_com2 = link_dataref_cmd_once("laminar/B738/audio/capt/mic_push2");
  } else {
    tx_com1 = link_dataref_int("xpserver/tx_com1");
    tx_com2 = link_dataref_int("xpserver/tx_com2");
  }
    
  int switch_handmic_captain = 0;
  int switch_handmic_copilot = 0;
  int switch_handmic_jumpseat = 0;
  int switch_headsetmic_captain = 0;
  int switch_headsetmic_copilot = 0;
  int switch_headsetmic_jumpseat = 0;
  int switch_maskmic_captain = 0;
  int switch_maskmic_copilot = 0;
  int switch_maskmic_jumpseat = 0;
  int switch_ptt_captain = 0;
  int switch_ptt_copilot = 0;
  int switch_ptt_jumpseat = 0;


  /* /\****** START TESTING COMMENT OUT WHEN OPERATIONAL ******\/ */
  /* /\* Potentiometers and Switches connected directly to Teensy in Audio Box */
  /*    for Testing Purpose *\/   */
  
  /* /\* read analog input (A14) *\/ */
  /* ret = analog_input(te,38,acp1_vol_pa,0.0,100.0); */
  /* if (ret == 1) { */
  /*  printf("CAPT HEADSET Volume changed to: %f \n",*acp1_vol_pa); */
  /* } */
  /* //  *acp2_vol_pa = *acp1_vol_pa; */
  /* //  *acp3_vol_pa = *acp1_vol_pa; */

  /* /\* read analog input (A15) *\/ */
  /* //ret = analog_input(te,39,acp1_vol_spkr,0.0,100.0); */
  /* //if (ret == 1) { */
  /* //  printf("CAPT SPEAKER Volume changed to: %f \n",*acp1_vol_spkr); */
  /* //} */

  /* /\* read analog input (A16) *\/ */
  /* //ret = analog_input(te,40,acp1_vol_vhf1,0.0,100.0); */
  /* //if (ret == 1) { */
  /* //  printf("CAPT VHF Volume changed to: %f \n",*acp1_vol_vhf1); */
  /* //} */

  /* /\* read headset / mask enable switch *\/ */
  /* ret = digital_input(te, TEENSY_TYPE, 0, 9, acp1_mask_boom,0); */
  /* if (ret == 1) { */
  /*  printf("CAPT BOOM / MASK SWITCH changed to: %i \n",*acp1_mask_boom); */
  /* } */
  /* //  *acp2_mask_boom = *acp1_mask_boom; */
  /* //  *acp3_mask_boom = *acp1_mask_boom; */
  
  /* /\* read ptt switch *\/ */
  /* ret = digital_input(te, TEENSY_TYPE, 0, 10,mic_capt,0); */
  /* if (ret == 1) { */
  /*  printf("CAPT PTT SWITCH changed to: %i \n",*mic_capt); */
  /* } */
  /* *mic_fo = 0; */
  /* if (*mic_capt == 0) *mic_fo = 1; */
  /* /\****** END TESTING COMMENT OUT WHEN OPERATIONAL******\/ */
  
  /* read Hand Mic switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 0,&switch_handmic_captain,0);
  if (ret == 1) {
    printf("CAPTAIN HAND MIC SWITCH changed to: %i \n",switch_handmic_captain);
  }
  ret = digital_input(te, TEENSY_TYPE, 0, 16,&switch_handmic_copilot,0);
  if (ret == 1) {
    printf("COPILOT HAND MIC SWITCH changed to: %i \n",switch_handmic_copilot);
  }
  ret = digital_input(te, TEENSY_TYPE, 0, 26,&switch_handmic_jumpseat,0);
  if (ret == 1) {
    printf("JUMPSEAT HAND MIC SWITCH changed to: %i \n",switch_handmic_jumpseat);
  }

  /* if hand mic button is pressed, headset and mask mic are off
     on each acp you can also choose whether headset (boom) or mask mic are used */

  /* CAPTAIN */
  if (switch_handmic_captain == 1) {
    switch_headsetmic_captain = 0;
    switch_maskmic_captain = 0;
  } else {
    if (*acp1_mask_boom == 0) {
      switch_headsetmic_captain = 1;
      switch_maskmic_captain = 0;
    } else {
      switch_headsetmic_captain = 0;
      switch_maskmic_captain = 1;
    }
  }

  /* FIRST OFFICER */
  if (switch_handmic_copilot == 1) {
    switch_headsetmic_copilot = 0;
    switch_maskmic_copilot = 0;
  } else {
    if (*acp2_mask_boom == 0) {
      switch_headsetmic_copilot = 1;
      switch_maskmic_copilot = 0;
    } else {
      switch_headsetmic_copilot = 0;
      switch_maskmic_copilot = 1;
    }
  }
  
  /* JUMP SEAT (has no hand mic, however) */
  if (switch_handmic_jumpseat == 1) {
    switch_headsetmic_jumpseat = 0;
    switch_maskmic_jumpseat = 0;
  } else {
    if (*acp3_mask_boom == 0) {
      switch_headsetmic_jumpseat = 1;
      switch_maskmic_jumpseat = 0;
    } else {
      switch_headsetmic_jumpseat = 0;
      switch_maskmic_jumpseat = 1;
    }
  }
  
  /* Enable Captain Headset Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 3, &switch_headsetmic_captain);
  if (ret == 1) {
    printf("CAPTAIN HEADSET MIC SWITCH changed to: %i \n",switch_headsetmic_captain);
  }
  
  /* Enable First Officer Headset Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 21, &switch_headsetmic_copilot);
  if (ret == 1) {
    printf("COPILOT HEADSET MIC SWITCH changed to: %i \n",switch_headsetmic_copilot);
  }

  /* Enable Jump Seat Headset Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 29, &switch_headsetmic_jumpseat);
  if (ret == 1) {
    printf("JUMP SEAT HEADSET MIC SWITCH changed to: %i \n",switch_headsetmic_jumpseat);
  }

  /* Enable Captain Hand Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 4, &switch_handmic_captain);
  if (ret == 1) {
    printf("CAPTAIN HAND MIC SWITCH changed to: %i \n",switch_handmic_captain);
  }

  /* Enable First Officer Hand Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 22, &switch_handmic_copilot);
  if (ret == 1) {
    printf("COPILOT HAND MIC SWITCH changed to: %i \n",switch_handmic_copilot);
  }

  /* Enable Jump Seat Hand Mic (INOP) */
  ret = digital_output(te, TEENSY_TYPE, 0, 30, &switch_handmic_jumpseat);
  if (ret == 1) {
    printf("JUMPSEAT HAND MIC SWITCH changed to: %i \n",switch_handmic_jumpseat);
  }

  /* Enable Captain Mask Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 5, &switch_maskmic_captain);
  if (ret == 1) {
    printf("CAPTAIN MASK MIC SWITCH changed to: %i \n",switch_maskmic_captain);
  }

  /* Enable First Officer Mask Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 23, &switch_maskmic_copilot);
  if (ret == 1) {
    printf("COPILOT MASK MIC SWITCH changed to: %i \n",switch_maskmic_copilot);
  }

  /* Enable Jump Seat Mask Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 31, &switch_maskmic_jumpseat);
  if (ret == 1) {
    printf("JUMPSEAT MASK MIC SWITCH changed to: %i \n",switch_maskmic_jumpseat);
  }

  if ((*mic_capt == 1) || (*mic_capt_yoke == 1)) switch_ptt_captain = 1;
  if ((*mic_fo == 1) || (*mic_fo_yoke == 1)) switch_ptt_copilot = 1;
  
  /* PTT Switch: Enable Captain Mic Output to PC */
  ret = digital_output(te, TEENSY_TYPE, 0, 6, &switch_ptt_captain);
  if (ret == 1) {
    printf("CAPTAIN MIC OUTPUT TO PC changed to: %i \n",switch_ptt_captain);
    if (switch_ptt_captain == 1) {
      if (*acp1_micsel_vhf1 == 1) {
	*tx_com1 = 1;
      }
      if (*acp1_micsel_vhf2 == 1) {
	*tx_com2 = 1;
      }
    }
  }
  
  /* PTT Switch: Enable First Officer Mic Output to PC */
  ret = digital_output(te, TEENSY_TYPE, 0, 24, &switch_ptt_copilot);
  if (ret == 1) {
    printf("COPILOT MIC OUTPUT TO PC changed to: %i \n",switch_ptt_copilot);
    if (switch_ptt_copilot == 1) {
      if (*acp2_micsel_vhf1 == 1) {
	*tx_com1 = 1;
      }
      if (*acp2_micsel_vhf2 == 1) {
	*tx_com2 = 1;
      }
    }
  }
  
  /* PTT Switch: Enable Jumpseat Mic Output to PC (jump seat has no mic button, however) */
  ret = digital_output(te, TEENSY_TYPE, 0, 32, &switch_ptt_jumpseat);
  if (ret == 1) {
    printf("JUMPSEAT MIC OUTPUT TO PC changed to: %i \n",switch_ptt_jumpseat);
  }

  /* Change volume of Captain VHF1 channel */
  ret = volume_output(te, PGA2311_TYPE, 0, 0, acp1_vol_vhf1, 0.0, 100.0);

  /* Change volume of Captain VHF2 channel */
  ret = volume_output(te, PGA2311_TYPE, 0, 1, acp1_vol_vhf2, 0.0, 100.0);

  /* Change volume of First Officer VHF1 channel */
  ret = volume_output(te, PGA2311_TYPE, 2, 0, acp2_vol_vhf1, 0.0, 100.0);

  /* Change volume of First Officer VHF2 channel */
  ret = volume_output(te, PGA2311_TYPE, 2, 1, acp2_vol_vhf2, 0.0, 100.0);
  
  /* Change volume of Jump Seat VHF1 channel */
  ret = volume_output(te, PGA2311_TYPE, 4, 0, acp3_vol_vhf1, 0.0, 100.0);

  /* Change volume of Jump Seat VHF2 channel */
  ret = volume_output(te, PGA2311_TYPE, 4, 1, acp3_vol_vhf2, 0.0, 100.0);

  /* Change volume of Captain Headset */
  ret = volume_output(te, PGA2311_TYPE, 1, 1, acp1_vol_pa, 0.0, 100.0);

  /* Change volume of First Officer Headset */
  ret = volume_output(te, PGA2311_TYPE, 3, 1, acp2_vol_pa, 0.0, 100.0);
  
  /* Change volume of Jump Seat Headset */
  ret = volume_output(te, PGA2311_TYPE, 5, 1, acp3_vol_pa, 0.0, 100.0);

  /* Change volume of Captain Speaker */
  ret = volume_output(te, PGA2311_TYPE, 1, 0, acp1_vol_spkr, 0.0, 100.0);

  /* Change volume of First Officer Speaker */
  ret = volume_output(te, PGA2311_TYPE, 3, 0, acp2_vol_spkr, 0.0, 100.0);
  
  /* Change volume of Jump Seat Speaker */
  ret = volume_output(te, PGA2311_TYPE, 5, 0, acp3_vol_spkr, 0.0, 100.0);

 
}
