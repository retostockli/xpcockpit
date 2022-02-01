/* This is the b737_mip.c code which connects to the SISMO MCP V4

   Copyright (C) 2021 Reto Stockli

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
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libsismo.h"
#include "serverdata.h"
#include "b737_mip.h"

void b737_mip(void)
{

  /* In the SISMO MIP all switches are inverted (0=on; 1=off) */
  
  int ret;
  int temp;
  int card = 0;
  int one = 1;
  int zero = 0;
  int lights_test = 1;
 
  if ((acf_type == 2) || (acf_type == 3)) {
    float *ap_prst_warn_capt = link_dataref_flt("laminar/B738/annunciator/ap_warn1",0);
    float *ap_prst_disc_capt = link_dataref_flt("laminar/B738/annunciator/ap_disconnect1",0);
    float *ap_prst_warn_fo = link_dataref_flt("laminar/B738/annunciator/ap_warn2",0);
    float *ap_prst_disc_fo = link_dataref_flt("laminar/B738/annunciator/ap_disconnect2",0);
    float *at_prst_warn_capt = link_dataref_flt("laminar/B738/annunciator/at_fms_warn1",0);
    float *at_prst_disc_capt = link_dataref_flt("laminar/B738/annunciator/at_disconnect1",0);
    float *at_prst_warn_fo = link_dataref_flt("laminar/B738/annunciator/at_fms_warn2",0);
    float *at_prst_disc_fo = link_dataref_flt("laminar/B738/annunciator/at_disconnect2",0);
    float *fmc_prst = link_dataref_flt("laminar/B738/fmc/fmc_message_warn",0);

    float *speedbrake_armed = link_dataref_flt("laminar/B738/annunciator/speedbrake_armed",0);
    float *speedbrake_extend = link_dataref_flt("laminar/B738/annunciator/speedbrake_extend",0);
    float *stab_outoftrim = link_dataref_flt("laminar/B738/annunciator/stab_out_of_trim",0);

    /* INPUTS */

    

    /* OUTPUTS */
    ret = digital_outputf(card, 0, ap_prst_warn_capt);   // yellow
    ret = digital_outputf(card, 1, ap_prst_disc_capt);   // red
    ret = digital_outputf(card, 2, at_prst_warn_capt);   // yellow
    ret = digital_outputf(card, 3, at_prst_disc_capt);   // red
    ret = digital_outputf(card, 4, fmc_prst);  // yellow
    ret = digital_outputf(card, 32, ap_prst_warn_fo);   // yellow
    ret = digital_outputf(card, 33, ap_prst_disc_fo);   // red
    ret = digital_outputf(card, 34, at_prst_warn_fo);  // yellow
    ret = digital_outputf(card, 35, at_prst_disc_fo);  // red
    ret = digital_outputf(card, 36, fmc_prst); // yellow

    ret = digital_outputf(card, 5, speedbrake_armed);
    ret = digital_output(card, 6, &lights_test);   // speed brake do not arm
    ret = digital_outputf(card, 7, stab_outoftrim);  


    
    ret = digital_outputf(card, 37, speedbrake_extend);
    

    
    /* SERVOS */

  }
    
}
