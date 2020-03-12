/* This is the iocard_test.c code which contains a sample set-up for how to communicate with the 
   iocard mastercard

   What it does:
   1. it reads the toggle switch state from input connection 0 (connect a toggle switch to input 0)
   2. it displays the state (off or on) in output 11 (connect a LED to output 11)
   3. it displays the state (0 or 1) on display 0 (connect a display II card to the mastercard)
   4. the switch turns on and off the landing lights of the plane in X-Plane if X-Plane is connected.

   Copyright (C) 2009 - 2014  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "libiocards.h"
#include "serverdata.h"
#include "iocard_test.h"

void iocard_test(void)
{

  int ret = 0;
  int card = 0;
  int device = 0;

  int intensitycommand;
  int intensity;
  int value;

  count++;
  /*
  if ((count < 0) || (count > 2)) {
    count = 0;
    }*/
  
  // test integer array data
  int *digitalinput = link_dataref_int("sim/cockpit/electrical/landing_lights_on");   

  // unsigned char *acf_tailnum   = link_dataref_byte_arr("sim/aircraft/view/acf_tailnum",  40, -1);  
  // int *digitalinput = link_dataref_int("xpserver/custominput");
  int *encoder = link_dataref_int("xpserver/customencoder");
  // double *latitude = link_dataref_dbl("sim/flightmodel/position/latitude", -3);
  //float *egt0 =  link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 0, 0);
  //float *egt1 =  link_dataref_flt_arr("sim/flightmodel/engine/ENGN_EGT_c", 8, 1, 0);

  // mastercard 1, input board # 1 (0-35)
  // mastercard 1, input board # 2 (36-71)

  /*
  if (*latitude != FLT_MISS) {
    printf("Latitude: %f\n",*latitude);
  }
  if (*egt0 != FLT_MISS) {
    printf("EGT Engine 1: %f\n",*egt0);
  }
  if (*egt1 != FLT_MISS) {
    printf("EGT Engine 2: %f\n",*egt1);
  }
  */

  /*
  if (*custom != INT_MISS) {
    printf("CUSTOM VALUE: %i \n",*custom);
    *custom = *custom + 1;
  } else {
    *custom = 1;
  }
  if (acf_tailnum) {
    printf("ACF_TAILNUM: %s \n",acf_tailnum);
  }
  */

  /* read digital input #0 on mastercard 1 */
  ret = digital_input(device,card,0,digitalinput,0);
  if (ret == 1) {
    printf("Digital Input #0 has value: %i \n",*digitalinput);
  }

  // mastercard 1, output board (11-55)

  /* update output value (connect a LED to output #11) */
  ret = digital_output(device,card,11,digitalinput);

  float updn = 0.;
  ret = mastercard_encoder(device,card,1,&updn,1.0,2,3);
  if (*encoder != INT_MISS) {
    if (ret == 1) {
      *encoder = *encoder + (int) updn;
      printf("Encoder: %i \n",*encoder);
    }
  } else {
    *encoder = 0;
  }

  // mastercard 1, display II card (max 64 displays)
  
  /* update 1 displays with display value */
  if (*digitalinput == 1) {
    value = 2;
  } else {
    value = 0;
  }

  value = 1;
  intensitycommand = 15;
  intensity = 24;
  if (count == 0) ret = mastercard_display(device,card,32,1,&value,0);
  value = 2;
  if (count == 0) ret = mastercard_display(device,card,33,1,&value,0);
  //if (count == 1) ret = mastercard_display(device,card,33,1,&intensitycommand,1);
  //if (count == 2) ret = mastercard_display(device,card,33,1,&intensity,1);
 

  /*
  value = 12345;
  intensitycommand = 15;
  intensity = 20;
  if (count == 0) ret = mastercard_display(device,card,0,5,&value,0);
  if (count == 1) ret = mastercard_display(device,card,1,1,&intensitycommand,1);
  if (count == 2) ret = mastercard_display(device,card,1,1,&intensity,1);
  */
  
}
