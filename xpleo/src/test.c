/* This is the test.c code which contains a sample set-up for how to communicate with the 
   BU0836X or BU0836A card from Leo Bodnar

   Copyright (C) 2025  Reto Stockli

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
#include "libleo.h"
#include "serverdata.h"
#include "test.h"

int encodervalue;

void test(void)
{

  int ret = 0;
  int device = 0;
  int axis;
  int input;
  float minval = 0.0;
  float maxval = 100.0;

  int test;
  float value;

  int *encodervalue = link_dataref_int("sim/cockpit/radios/transponder_code");
  
  /* read potentiometer from analog input #1 on BU0836X/A card, scale it to the range 0-100 */
  for (axis=0; axis<1; axis++) {
    ret = axis_input(device,axis,&value,minval,maxval);
    if ((ret == 1) && (axis == 0)) {
      printf("Analog Input %i has value: %f \n",axis,value);
    }
  }

  /* read buttons of BU0836X/A card */
  for (input=0; input<15; input++) {
    ret = digital_input(device,input,&test,0);
    if (ret == 1) {
      printf("Button %i has value %i \n",input,test);
    }
  }

  /* read encoder connected to buttons 2 and 3 */
  int acceleration = 1;
  int multiplier = 1;
  ret = encoder_input(device, 2, encodervalue, multiplier, acceleration, 2);
  if (ret == 1) {
    printf("Encoder Value: %i \n",*encodervalue);
  }
  
  
}
