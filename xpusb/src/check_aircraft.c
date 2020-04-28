/* This is the check_aircraft.c code which determines aircraft based on tail number
   Feel free to add your own aircraft. Aircraft type will be available through the
   common.h shared variable acf_type for all xpusb codes

   Copyright (C) 2018 Reto Stockli

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
#include <sys/types.h>

#include "common.h"
#include "iniparser.h"
#include "libiocards.h"
#include "handleserver.h"
#include "serverdata.h"

#include "check_aircraft.h"

void check_aircraft(void) {

  /* determine ACF by tail number */
  unsigned char *tailnum = link_dataref_byte_arr("sim/aircraft/view/acf_tailnum",  40, -1);

  if (tailnum) {
    //    printf("ACF_TAILNUM: %s \n",tailnum);

    /* Add your own number > 3 for any of your favorite aircraft */
    
    if (strcmp((const char*) tailnum,"ZB738")==0) {
      /* ZIBO MOD of B737-800 */
      acf_type = 3;
    } else if (strcmp((const char*) tailnum,"NN816N")==0) {
      /* Laminar B738-800 */
      acf_type = 2;
    } else if (strcmp((const char*) tailnum,"OY-GRL")==0) {
      /* Air Greenland Livery of x737 */
      acf_type = 1;
    } else if (strcmp((const char*) tailnum,"D-ATUC")==0) {
      /* Standard x737 */
      acf_type = 1;
    } else {
      /* Any other X-Plane Aircraft */
      acf_type = 0;
    }
    
  }
  
}
