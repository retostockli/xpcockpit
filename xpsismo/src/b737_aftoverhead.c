/* This is the b737_aftoverhead.c code which connects to the SISMO AFT OVERHEAD PANEL

   Copyright (C) 2023 Reto Stockli

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
#include "b737_aftoverhead.h"

void b737_aftoverhead(void)
{
  int card = 4; /* SISMO card according to ini file */

  int zero = 0;
  int ret;
  int temp;
  float fvalue;
  int display;
  int i;
  int outvalue;


  temp = 1;

  int *invalue = link_dataref_int("xpserver/test");
  
  outvalue = 1;
  ret = digital_output(card,0,&outvalue);
  outvalue = 1;
  ret = digital_output(card,1,&outvalue);

  i=0;
  ret = analog_input(card,i,&fvalue,0.0,10.0);
  if (ret == 1) {
    /* ret is 1 only if analog input has changed */
    printf("Analog Input %i changed to: %f \n",i,fvalue);
  }

  i=0;
  ret = digital_input(card, i, invalue, 0);
  if (ret == 1) {
    /* ret is 1 only if input has changed */
    printf("Digital Input %i changed to: %i \n",i,*invalue);
  }

  display = 88888;
  
  /* set 7 segment displays 0-5 to the 5 digit value of the encoder with a decimal point at digit 2 */
  ret = display_output(card, 0, 5, &display, 0, 15);

}
