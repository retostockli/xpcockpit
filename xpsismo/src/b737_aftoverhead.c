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

#define COUNT_MAX 5

int col;
int counter;


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
  int i0;
  int o0;



  /*** IRS DISPLAY PANEL ***/

  i0 = 0;
  o0 = 0;
  
  temp = 1;

  int *invalue = link_dataref_int("xpserver/test");

  /* BRT Potentiometer driving 7 segment brightness */
  i=0;
  ret = analog_input(card,i,&fvalue,0.0,10.0);
  if (ret == 1) {
    /* ret is 1 only if analog input has changed */
    printf("Analog Input %i changed to: %f \n",i,fvalue);
  }

  /* ENT Key Light */
  outvalue = 1;
  ret = digital_output(card,o0+6,&outvalue);

  /* CLR Key Light */
  outvalue = 1;
  ret = digital_output(card,o0+7,&outvalue);

  /* 12 key keyboard: Matrix arrangement */
  if ((col<0) || (col>2)) col = 0;
  if ((counter<0) || (counter>COUNT_MAX)) counter = 0;

  //col = 0;
  
  //if (counter == 0) {
    for (int c=0;c<3;c++) {
      if (c==col) { outvalue = 0; } else { outvalue = 1; }
      ret = digital_output(card,o0+c,&outvalue);
    }
    //}

  //printf("%i %i \n",col,counter);
  
  //if (counter == COUNT_MAX) {
    i0=8;
    for (int r=0;r<4;r++) {
      *invalue = 0;
      ret = digital_input(card, i0+r, invalue, 0);
      if (ret == 1) {
	/* ret is 1 only if input has changed */
	printf("Column %i Row %i changed to: %i \n",col,r,*invalue);
      }
    }
    //}

  if (counter == COUNT_MAX) {
    col++;
    if (col>2) col = 0;
    counter = 0;
  }
  counter++;
    
  display = 888888;
  //display = 999999;
  
  /* set 7 segment displays 0-5 to the 5 digit value of the encoder with a decimal point at digit 2 */
  ret = display_output(card, 0, 6, &display, 0, 10);
  ret = display_output(card, 8, 6, &display, 0, 10);

}
