/* This is the serverdata.h header to serverdata.c

   Copyright (C) 2009 - 2014  Reto Stockli
   Additions for Command handling by Hans Jansen 2011
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

#include <time.h>

/* missing values: UCHAR has no missing value. FLT and DBL are the same */
#define INT_MISS -2000000000
#define FLT_MISS -2000000000.0
#define DBL_MISS -2000000000.0

/* x-plane variable types: */
#define XPTYPE_UNKNOWN		  0 // unknown 
#define XPTYPE_INT		  1 // int
#define XPTYPE_FLT		  2 // float
#define XPTYPE_DBL		  4 // double (2+4=6 can concur!)
#define XPTYPE_FLT_ARR		  8 // float array
#define XPTYPE_INT_ARR		 16 // int array
#define XPTYPE_BYTE_ARR		 32 // byte/char data
#define XPTYPE_CMD_ONCE		 64 // one-shot command
#define XPTYPE_CMD_HOLD		128 // on/off command

/* serverdata element status */
#define XPSTATUS_UNINITIALIZED	  0 // serverdata structure for dataref uninitialized in client
#define XPSTATUS_ALLOC		  1 // serverdata memory for dataref allocated in client
#define XPSTATUS_DEALLOC	  2 // serverdata memory for dataref deallocated in client
#define XPSTATUS_LINK	          3 // client sent dataref link request to x-plane
#define XPSTATUS_UNLINK	          4 // client sent dataref unlink request to x-plane
#define XPSTATUS_VALID	          5 // x-plane confirmed dataref validity to client

/* Protocol Version 2 data structure */

/* A dynamically (run-time) expandable number of datarefs of any X-Plane internal data type
   and number of vector elements per dataref is created */

/* dataref name, type and size can be found in the x-plane dataref list:
   http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html
   or in the x737 specific datarefs, located in Benedikt Stratmann's x737 aircraft folder */

typedef struct {
  char datarefname[100];  /* name of x-plane dataref */
  int type;               /* x-plane variable type: 
			     0: unknown 
			     1: int
			     2: float
			     4: double (2+4=6 can concur!)
			     8: float array
			     16: int array
			     32: byte/char data */
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
  void *data_old;         /* pointer to last data values */
  int nextindex;          /* stores the index of the next index that will be called */
  int nsend;              /* number of times the dataref was sent to the server */
  int nrecv;              /* number of times the dataref was received from the server */
  int received;           /* 1 if this dataref was received during this loop, 0 else */
} serverdata_struct;

extern struct timeval time_start;

extern serverdata_struct *serverdata; /* pointer to dynamically allocatable xp data structure */
extern int numalloc; /* number of serverdata elements allocated */
extern int numlink; /* number of serverdata elements linked */
extern char clientname[100]; /* name of x-plane client package */
extern int lastindex; /* stores the index of the last Dataref called */
extern int numreceived; /* stores number of datarefs that were received in this loop */

/* Prototype functions for x-plane data handling */
int initialize_dataref();
void clear_dataref();
void count_dataref();
void count_received();
void reset_received();

/* the following dataref subscription functions refer to arguments which are defined as follows

   datarefname : name of X-Plane dataref, or alternatively custom dataref name
   which is a) just local to this code here, or b) available in X-Plane for
   sharing among other plugins or with other clients of xpserver. In the case of
   b) the dateref has to start with the name of the data server plugin (e.g. xpserver/flightdata/blabla).

   precision : for floating point data, specify the minimum change step for which the updated state 
   is communicated between X-Plane and the client. The minimum change step is defined as 10^precision.
   e.g. if you want the EGT only to be sent for every 0.01 degree Celsius change, specifiy precision 
   as -2 (10^-2 = 0.01). All changes below 0.01 will not be sent. This measure reduces the amount of data
   transferred between X-Plane and the client and vice versa. Integer and byte/char data is always transferred
   when it changes by at least +/- 1.

   nelements : for arrays, specify the number of array elements. This number has to exactly correspond
   to the number of array elements of the X-Plane dataref.

   index : for arrays, if you want to transfer only a single array element, specify the array element
   as a number (0-based) from 0..nelements-1. If you want to always get the full array into a pointer
   to an array of appropriate length, then set the index to -1.
*/

/* master function, needs typecasting in user space since it returns a void pointer */
void *link_dataref(const char datarefname[], int type, int nelements, int index, int precision);

/* easier to use type specific wrapper functions, return type specific pointers */
int *link_dataref_int(const char datarefname[]);
float *link_dataref_flt(const char datarefname[], int precision);
double *link_dataref_dbl(const char datarefname[], int precision);
float *link_dataref_flt_arr(const char datarefname[], int nelements, int index, int precision);
int *link_dataref_int_arr(const char datarefname[], int nelements, int index);
unsigned char *link_dataref_byte_arr(const char datarefname[], int nelements, int index);
int *link_dataref_cmd_once(const char datarefname[]);
int *link_dataref_cmd_hold(const char datarefname[]);

/* remove a dataref from the subscription */
int unlink_dataref(const char datarefname[]);
