/* This is the test.c code which can be used to test basic I/O of the SISMO Master and Daughter
   cards

   Copyright (C) 2021 Reto Stockli

   Additions for analog axes treatment by Hans Jansen 2011
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
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libsismo.h"
#include "serverdata.h"
#include "test.h"

void test(void)
{

  int ret;
  int card = 0;
  int i;
  int one = 1;
  int zero = 0;
  int display = 0;

  /* link integer data like a switch in the cockpit */
  //int *value = link_dataref_int("sim/cockpit/electrical/landing_lights_on");

  /* link floating point dataref with precision 10e-3 to local variable. This means
     that we only transfer the variable if changed by 0.001 or more */
  //float *fvalue = link_dataref_flt("sim/flightmodel/controls/parkbrake",-3);

  /* link NAV1 Frequency to encoder value */
  int *encodervalue = link_dataref_int("sim/cockpit/radios/nav1_freq_h");

  if (*encodervalue == INT_MISS) *encodervalue = 0;
  
  /* read second digital input (#1) */
  i=5;
  //ret = digital_input(card, i, value, 0);
  if (ret == 1) {
    /* ret is 1 only if input has changed */
    //    printf("Digital Input %i changed to: %i \n",i,*value);
  }
 
  /* read first analog input (#0) */
  //  for (i=0;i<5;i++) {
  i=0;
  // ret = analog_input(card,i,fvalue,0.0,10.0);
  if (ret == 1) {
    /* ret is 1 only if analog input has changed */
    //printf("Analog Input %i changed to: %f \n",i,*fvalue);
  }
    //  }
   

  /* read encoder at inputs 0 and 1 */

  ret = encoder_input(card, 0, 1, encodervalue, 1, 1);
  if (ret == 1) {
    /* ret is 1 only if encoder has been turned */
    printf("Encoder changed to: %i \n",*encodervalue);
  }

  //ret = servo_output(card,0,encodervalue,0,100);
  
  /* set LED connected to second output (#1) to value of above input */
  //for (i=0;i<64;i++) {
  //  ret = digital_output(card, i, &one);
  //ret = digital_output(card, i, &zero);
  //}


  /*
  for (i=0;i<sismo[card].noutputs;i++) {
    if ((i == *encodervalue) || i == (*encodervalue+1) || i == (*encodervalue+2) || i == (*encodervalue+3)) {
      //    if ((i == *encodervalue)) {
      ret = digital_output(card, i, &one);
    } else {
      ret = digital_output(card, i, &zero);
    }
  }
  */

  //display = 88888;
  
  /* set 7 segment displays 0-5 to the 5 digit value of the encoder with a decimal point at digit 2 */
  //ret = display_output(card, 0, 5, &display, 0, 0);

}
