/* customdata.c is the header file to customdata.c

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

#define MAXCUSTOMALLOC 100

typedef struct {
  char datarefname[100];  /* name of custom dataref */
  int type;               /* x-plane variable type: 
			     1: int
			     2: float
			     4: double (2+4=6 can concur!)
			     8: float array
			     16: int array
			     32: byte/char data
			     128: command */
  int nelements;          /* number of elements. Only applicable to types 8, 16 and 32 */
  void *data;             /* pointer to current data values */
} customdata_struct;

extern customdata_struct *customdata; /* pointer to dynamically allocatable custom dataref structure */

/* function prototypes */
int allocate_customdata(int type, int nelements, char datarefname[]);

int GetDataiCB(void *refcon);
void SetDataiCB(void *refcon, int outValue);
float GetDatafCB(void *refcon);
void SetDatafCB(void *refcon, float outValue);
double GetDatadCB(void *refcon);
void SetDatadCB(void *refcon, double outValue);
int GetDataviCB(void *refcon, int *outValues, int inOffset, int inMax);
void SetDataviCB(void *refcon, int *inValues, int inOffset, int inMax);
int GetDatavfCB(void *refcon, float *outValues, int inOffset, int inMax);
void SetDatavfCB(void *refcon, float *inValues, int inOffset, int inMax);
int GetDatabCB(void *refcon, void *outValues, int inOffset, int inMax);
void SetDatabCB(void *refcon, void *inValues, int inOffset, int inMax);
