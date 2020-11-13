/* This is the clientdata.h header to clientdata.c

   Copyright (C) 2009 - 2014  Reto Stockli
   Additions for X-Plane commands by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#include "XPLMDataAccess.h"

/* data structure for storing client data */

/* A dynamically (run-time) expandable number of datarefs of any X-Plane internal data type
   and number of vector elements per dataref is created */

/* dataref name, type and size can be found in the x-plane dataref list:
   http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html
   or in the x737 specific datarefs, located in Benedikt Stratmann's x737 aircraft folder */

typedef struct {
  char datarefname[100];  /* name of x-plane dataref */
  int type;               /* x-plane variable type: 
			     1: int
			     2: float
			     4: double (2+4=6 can concur!)
			     8: float array
			     16: int array
			     32: byte/char data
			     128: command */
  int nelements;          /* number of elements. Only applicable to types 8, 16 and 32 */
  int index;              /* index of specific element 0..(nelements-1), or -1 for all */
  int precision;          /* maximum precision of data (power of 10, or -9999 for none). 
			     Data is only transferred between x-plane and the client
			     if data changes at least 10^precision (e.g. +/-0.1 for precision=-1)
			     from one time step to the next. This allows to minimize data 
			     transfer for variables that change rapidly, but with very small 
			     amounts (e.g. air pressure, location) */
  int status;             /* status of the data: see XPSTATUS defines above */
  void *data;             /* pointer to current data values */
  void *initval;          /* initial value */
  void *dataref;          /* Dataref pointer (X-Plane specific format) */
  int write;              /* dataref has read/write access (1) or only read access (0) in x-plane */
} clientdata_struct;

extern clientdata_struct *clientdata; /* pointer to dynamically allocatable xp data structure */

extern int numalloc; /* number of clientdata elements allocated */
extern int numlink; /* number of clientdata elements linked */
extern char clientname[100]; /* name of x-plane client application */

/* Prototypes */
int allocate_clientdata(int offset, int type, int nelements, int index, int precision, char datarefname[]);
int deallocate_clientdata(int offset);
int check_clientdata(int offset);
void clear_clientdata();
void count_clientdata();
