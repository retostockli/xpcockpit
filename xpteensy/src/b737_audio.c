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
  teensy[te].pinmode[16] = PINMODE_INPUT;   /* Hand Mic PTT */
  teensy[te].pinmode[21] = PINMODE_OUTPUT; /* Headset Mic Enable */
  teensy[te].pinmode[22] = PINMODE_OUTPUT; /* Hand Mic Enable */
  teensy[te].pinmode[23] = PINMODE_OUTPUT; /* Mask Mic Enable */
  teensy[te].pinmode[24] = PINMODE_OUTPUT; /* Mic to Line IN Enable */
 
  pga2311[te][2].spi = 0; // SPI Bus number
  pga2311[te][2].cs = 17; // Chip Select Pin
  pga2311[te][3].spi = 0; // SPI Bus number
  pga2311[te][3].cs = 20; // Chip Select Pin

  /* Jump Seat ACP Mixer */
  teensy[te].pinmode[26] = PINMODE_INPUT;   /* Hand Mic PTT */
  teensy[te].pinmode[29] = PINMODE_OUTPUT; /* Headset Mic Enable */
  teensy[te].pinmode[30] = PINMODE_OUTPUT; /* Hand Mic Enable */
  teensy[te].pinmode[31] = PINMODE_OUTPUT; /* Mask Mic Enable */
  teensy[te].pinmode[32] = PINMODE_OUTPUT; /* Mic to Line IN Enable */
 
  pga2311[te][4].spi = 0; // SPI Bus number
  pga2311[te][4].cs = 27; // Chip Select Pin
  pga2311[te][5].spi = 0; // SPI Bus number
  pga2311[te][5].cs = 28; // Chip Select Pin

  /* For testing */
  teensy[te].pinmode[9] = PINMODE_INPUT;
  teensy[te].pinmode[10] = PINMODE_INPUT;

  teensy[te].pinmode[38] = PINMODE_ANALOGINPUTMEAN;
  teensy[te].pinmode[39] = PINMODE_ANALOGINPUTMEAN;
  teensy[te].pinmode[40] = PINMODE_ANALOGINPUTMEAN;

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

  //int one = 1;
  float zero = 0.0;
  
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

  
  float *volume_vhf = link_dataref_flt("xpserver/volume_vhf",-3);
  float *volume_headset = link_dataref_flt("xpserver/volume_headset",-3);
  float *volume_speaker = link_dataref_flt("xpserver/volume_speaker",-3);

  int *switch_boommask = link_dataref_int("xpserver/switch_boommask");
  int *switch_ptt = link_dataref_int("xpserver/switch_ptt");

  int switch_headsetmic = 0;
  int switch_handmic = 0;
  int switch_maskmic = 0;
  
  /* read analog input (A14) */
  ret = analog_input(te,38,volume_headset,0.0,100.0);
  if (ret == 1) {
    printf("HEADSET Volume changed to: %f \n",*volume_headset);
  }

  /* read analog input (A15) */
  ret = analog_input(te,39,volume_speaker,0.0,100.0);
  if (ret == 1) {
    printf("SPEAKER Volume changed to: %f \n",*volume_speaker);
  }

  /* read analog input (A16) */
  ret = analog_input(te,40,volume_vhf,0.0,100.0);
  if (ret == 1) {
    printf("VHF Volume changed to: %f \n",*volume_vhf);
  }

  /* read headset enable switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 9, switch_boommask,0);
  if (ret == 1) {
    printf("BOOM / MASK SWITCH changed to: %i \n",*switch_boommask);
  }
  
  /* read ptt switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 10,switch_ptt,0);
  if (ret == 1) {
    printf("PTT SWITCH changed to: %i \n",*switch_ptt);
  }
  
  /* read Hand Mic switch */
  ret = digital_input(te, TEENSY_TYPE, 0, 0,&switch_handmic,0);
  //ret = digital_input(te, TEENSY_TYPE, 0, 16,&switch_handmic,0);
  //ret = digital_input(te, TEENSY_TYPE, 0, 26,&switch_handmic,0);
  if (ret == 1) {
    printf("HAND MIC SWITCH changed to: %i \n",switch_handmic);
  }
  
  //ret = digital_output(te, MCP23017_TYPE, 0, 0, switch_boommask);

  if (switch_handmic == 1) {
    switch_headsetmic = 0;
    switch_maskmic = 0;
  } else {
    if (*switch_boommask == 1) {
      switch_headsetmic = 1;
      switch_maskmic = 0;
    } else {
      switch_headsetmic = 0;
      switch_maskmic = 1;
    }
  }
  
  /* Enable Captain Headset Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 3, &switch_headsetmic);

  /* Enable First Officer Headset Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 21, &switch_headsetmic);

  /* Enable Jump Seat Headset Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 29, &switch_headsetmic);

  /* Enable Captain Hand Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 4, &switch_handmic);

  /* Enable First Officer Hand Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 22, &switch_handmic);

  /* Enable Jump Seat Hand Mic (INOP) */
  ret = digital_output(te, TEENSY_TYPE, 0, 30, &switch_handmic);

  /* Enable Captain Mask Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 5, &switch_maskmic);

  /* Enable First Officer Mask Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 23, &switch_maskmic);

  /* Enable Jump Seat Mask Mic */
  ret = digital_output(te, TEENSY_TYPE, 0, 31, &switch_maskmic);

  /* PTT Switch: Enable Captain Mic Output to PC */
  ret = digital_output(te, TEENSY_TYPE, 0, 6, switch_ptt);

  int switch_temp = 0;
  if (*switch_ptt == 0) switch_temp = 1;
  
  /* PTT Switch: Enable First Officer Mic Output to PC */
  ret = digital_output(te, TEENSY_TYPE, 0, 24, &switch_temp);
  //ret = digital_output(te, TEENSY_TYPE, 0, 32, &switch_temp);

  /* Change volume of right VHF channel */
  ret = volume_output(te, PGA2311_TYPE, 0, 0, volume_vhf, 0.0, 100.0);
  ret = volume_output(te, PGA2311_TYPE, 0, 1, &zero, 0.0, 100.0);

  ret = volume_output(te, PGA2311_TYPE, 2, 0, volume_vhf, 0.0, 100.0);
  ret = volume_output(te, PGA2311_TYPE, 2, 1, &zero, 0.0, 100.0);
  
  ret = volume_output(te, PGA2311_TYPE, 4, 0, volume_vhf, 0.0, 100.0);
  ret = volume_output(te, PGA2311_TYPE, 4, 1, &zero, 0.0, 100.0);

  /* Change volume of Headset */
  ret = volume_output(te, PGA2311_TYPE, 1, 1, volume_headset, 0.0, 100.0);

  ret = volume_output(te, PGA2311_TYPE, 3, 1, volume_headset, 0.0, 100.0);
  
  ret = volume_output(te, PGA2311_TYPE, 5, 1, volume_headset, 0.0, 100.0);

  /* Change volume of Speaker */
  ret = volume_output(te, PGA2311_TYPE, 1, 0, volume_speaker, 0.0, 100.0);

  ret = volume_output(te, PGA2311_TYPE, 3, 0, volume_speaker, 0.0, 100.0);
  
  ret = volume_output(te, PGA2311_TYPE, 5, 0, volume_speaker, 0.0, 100.0);

 
}
