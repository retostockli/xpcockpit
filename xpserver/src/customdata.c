/* customdata.c creates custom X-Plane datarefs

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

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"

#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"

#include "customdata.h"

/* allocation of global variables defined in customdata.h */
customdata_struct *customdata; /* pointer to dynamically allocatable custom dataref structure */

int allocate_customdata(int type, int nelements, char datarefname[]) {

  int offset;
  int j;
  int found = 0;
  int datai;
  float dataf;
  unsigned char datab;
  void *dataref;  

  // Allocate Memory for custom dataref

  /* disable array element selection for scalar data */
  if ((type == XPTYPE_INT) || (type == XPTYPE_FLT) || (type == XPTYPE_DBL) || 
      (type == XPTYPE_CMD_ONCE) || (type == XPTYPE_CMD_HOLD)) {
    nelements = 1;
  }
    
  /* check for zero or negative number of elements elements */
  if (nelements < 1) {
    if (verbose > 0) fprintf(logfileptr,"Number of elements < 1 for custom dataref %s \n",datarefname);
    return -1;
  }
           
  /* check for zero length string in datarefname */
  if (!strcmp(datarefname,"")) {
    if (verbose > 0) fprintf(logfileptr,"Custom dataref has no name. Not created\n");
    return -1;
  }

  if (customdata != NULL) {
    /* search for dataref */
    offset = 0;
    while ((found == 0) && (offset<MAXCUSTOMALLOC)) {
      if (!strcmp(datarefname,customdata[offset].datarefname)) {
	found = 1;
      } else {
	offset++;
      }
    }
  } else {
    /* allocate customdata structure */
    if (verbose > 0) fprintf(logfileptr,"Allocating memory for custom dataref structure \n");

    /* allocate memory for customdata */
    /* we cannot re-allocate with each new custom data like done for the clientdata structure
       since data pointers for previously registered callback functions will become unusable.
       The strategy is thus to allocate once a maximum amount of data pointers and work from those.
       This code however is less scalable, since the maximum amount of custom datarefs has to be
       set at compile time. Any better solution is welcome */
    customdata = realloc(customdata, sizeof(customdata_struct) * MAXCUSTOMALLOC);
    if (customdata == NULL) { 
      if (verbose > 0) fprintf(logfileptr,"Memory allocation error when allocating %i custom dataref elements \n", MAXCUSTOMALLOC);
      return -1;
    }
    for (offset=0;offset<MAXCUSTOMALLOC;offset++) {
      memset(customdata[offset].datarefname,0,sizeof(customdata[offset].datarefname));
    }
  }

  if (found) {
    /* check if dataref is consistent */

    if (type != customdata[offset].type) {
      if (verbose > 0) fprintf(logfileptr,"Requested type %i does not match type %i for custom dataref %s \n",
			       type,customdata[offset].type,datarefname);
      return -1;
    }

    if (nelements != customdata[offset].nelements) {
      if (verbose > 0) fprintf(logfileptr,"Requested elements %i does not match elements %i for custom dataref %s \n",
			       nelements,customdata[offset].nelements,datarefname);
      return -1;
    }
 
    if (verbose > 0) fprintf(logfileptr,"Existing custom dataref: %s \n",datarefname);

  } else {
    /* create a new data element for this custom dataref */

    /* find first empty custom dataref element */
    offset = 0;
    while ((found == 0) && (offset<MAXCUSTOMALLOC)) {
      if (!strcmp(customdata[offset].datarefname,"")) {
	found = 1;
      } else {
	offset ++;
      }
    }

    if (found == 0) {
      if (verbose > 0) fprintf(logfileptr,"Maximum number of custom dataref elements exceeded: %i \n",MAXCUSTOMALLOC);
      return -1;
    }

    /* allocate memory for dataref */
    switch (type) {
    case XPTYPE_INT: 
      customdata[offset].data = malloc(sizeof(int));
      *(int*) customdata[offset].data = INT_MISS;
      break;
    case XPTYPE_FLT: 
      customdata[offset].data = malloc(sizeof(float));
      *(float*) customdata[offset].data = FLT_MISS;
      break;
    case XPTYPE_DBL:
      customdata[offset].data = malloc(sizeof(double));
      *(double*) customdata[offset].data = DBL_MISS;
      break;
    case XPTYPE_FLT_ARR:
      customdata[offset].data = malloc(sizeof(float)*nelements);
      for (j=0;j<nelements;j++) {
	((float*) customdata[offset].data)[j] = FLT_MISS;
      }
      break;
    case XPTYPE_INT_ARR:
      customdata[offset].data = malloc(sizeof(int)*nelements);
      for (j=0;j<nelements;j++) {
	((int*) customdata[offset].data)[j] = INT_MISS;
      }
      break;
    case XPTYPE_BYTE_ARR:
      customdata[offset].data = malloc(sizeof(unsigned char)*nelements);
      for (j=0;j<nelements;j++) {
	((unsigned char*) customdata[offset].data)[j] = 0;
      }
      break;
    default:
      if (verbose > 0) fprintf(logfileptr,"Unsupported data type %i for custom dataref %s \n",type,datarefname);
      customdata[offset].data = NULL;
      break;
    }
 
    if (customdata[offset].data == NULL) {
      if (verbose > 0) fprintf(logfileptr,"Custom dataref %s not created \n",datarefname);
      return -1;
    } else {
      strcpy(customdata[offset].datarefname,datarefname);
      customdata[offset].type = type;
      customdata[offset].nelements = nelements;
      if (verbose > 0) fprintf(logfileptr,"Created custom dataref structure: %s \n",datarefname);
    }
      
  }

  // Create our custom integer dataref
  dataref = XPLMRegisterDataAccessor(datarefname,
				     type,                     // The types we support
				     1,                        // Writable
				     GetDataiCB,  SetDataiCB,  // Integer accessors
				     GetDatafCB,  SetDatafCB,  // Float accessors
				     GetDatadCB,  SetDatadCB,  // Doubles accessors
				     GetDataviCB, SetDataviCB, // Int array accessors
				     GetDatavfCB, SetDatavfCB, // Float array accessors
				     GetDatabCB,  SetDatabCB,  // Raw data accessors
				     &customdata[offset],      // Read Refcon
				     &customdata[offset]);     // Write Refcon
  if (XPLMIsDataRefGood(dataref)) {
    /* dataref created, reset it to missing value and set it to writable */
    switch (type) {
    case XPTYPE_INT:
      XPLMSetDatai(dataref, INT_MISS);
      break;
    case XPTYPE_FLT:
      XPLMSetDataf(dataref, FLT_MISS);
      break;
    case XPTYPE_DBL:
      XPLMSetDatad(dataref, DBL_MISS);
      break;
    case XPTYPE_INT_ARR:
      datai = INT_MISS;
      for (j=0;j<nelements;j++) {
	XPLMSetDatavi(dataref, &datai,j,1);
      }
      break;
    case XPTYPE_FLT_ARR:
      dataf = FLT_MISS;
      for (j=0;j<nelements;j++) {
	XPLMSetDatavf(dataref, &dataf,j,1);
      }
      break;
    case XPTYPE_BYTE_ARR:
      datab = 0;
      for (j=0;j<nelements;j++) {
	XPLMSetDatab(dataref, &datab,j,1);
      }
      break;
    default:
      if (verbose > 0) fprintf(logfileptr,"Unsupported data type %i for custom dataref %s \n",type,datarefname);
      return -1;
    }
      
    if (verbose > 0) fprintf(logfileptr, "Registered custom dataref in X-Plane %s. \n",datarefname);
  } else {
    if (verbose > 0) fprintf(logfileptr, "Could not register custom dataref %s. \n",datarefname);
    return -1;
  }

  return 0;
}


/* custom integer dataref read callback */
int GetDataiCB(void* refcon)
{
  if (refcon != NULL) {
    if (((customdata_struct*) refcon)->data != NULL) {
      return *(int*) ((customdata_struct*) refcon)->data;
    } else {
      return INT_MISS;
    }
  } else {
    return INT_MISS;
  }
}

/* custom integer dataref write callback */
void SetDataiCB(void* refcon, int inValue)
{
  if (refcon != NULL) {
    if (((customdata_struct*) refcon)->data != NULL) {
      *(int*) ((customdata_struct*) refcon)->data = inValue;
    }
  }
}
 
/* custom float dataref read callback */
float GetDatafCB(void* refcon)
{
  if (refcon != NULL) {
    if (((customdata_struct*) refcon)->data != NULL) {
      return *(float*) ((customdata_struct*) refcon)->data;
    } else {
      return FLT_MISS;
    }
  } else {
    return FLT_MISS;
  }
}

/* custom float dataref write callback */
void SetDatafCB(void* refcon, float inValue)
{
  if (refcon != NULL) {
    if (((customdata_struct*) refcon)->data != NULL) {
      *(float*) ((customdata_struct*) refcon)->data = inValue;
    }
  }
}
 
/* custom double dataref read callback */
double GetDatadCB(void* refcon)
{
  if (refcon != NULL) {
    if (((customdata_struct*) refcon)->data != NULL) {
      return *(double*) ((customdata_struct*) refcon)->data;
    } else {
      return DBL_MISS;
    }
  } else {
    return DBL_MISS;
  }
}

/* custom double dataref write callback */
void SetDatadCB(void* refcon, double inValue)
{
  if (refcon != NULL) {
    if (((customdata_struct*) refcon)->data != NULL) {
      *(double*) ((customdata_struct*) refcon)->data = inValue;
    }
  }
}
 
/* custom integer array dataref read callback */
int GetDataviCB(void *refcon, int *outValues, int inOffset, int inMax)
{
    int n, r;

    if (refcon != NULL) {
      if (((customdata_struct*) refcon)->data != NULL) {
	// If the array ptr is null, the caller just wants to know the total item count!
	if (outValues == NULL)
	  return ((customdata_struct*) refcon)->nelements;
	
	// Calculate the number of items to return.  We must limit by both the end of 
	// our array and the total number the caller asked for - whichever is less.
	r = ((customdata_struct*) refcon)->nelements - inOffset;
	if (r > inMax) r = inMax;
	
	// Now copy the actual items from our array to the returned memory.
	for (n = 0; n < r; ++n)
	  outValues[n] = ((int*) ((customdata_struct*) refcon)->data)[n + inOffset];
	return r;
      }
    }
    return 0;
}

/* custom integer array dataref write callback */
void SetDataviCB(void *refcon, int *inValues, int inOffset, int inMax)
{
    int n, r;

    if (refcon != NULL) {
      if (((customdata_struct*) refcon)->data != NULL) {
	// Calculate the number of items to copy in.  This is the lesser of the number
	// the caller writes and the end of our array.
	r = ((customdata_struct*) refcon)->nelements - inOffset;
	if (r > inMax) r = inMax;
	
	// Copy the actual data.
	for (n = 0; n < r; ++n)
	  ((int*) ((customdata_struct*) refcon)->data)[n+ inOffset] = inValues[n];
      }
    }
}

/* custom float array dataref read callback */
int GetDatavfCB(void *refcon, float *outValues, int inOffset, int inMax)
{
    int n, r;

    if (refcon != NULL) {
      if (((customdata_struct*) refcon)->data != NULL) {
	// If the array ptr is null, the caller just wants to know the total item count!
	if (outValues == NULL)
	  return ((customdata_struct*) refcon)->nelements;
	
	// Calculate the number of items to return.  We must limit by both the end of 
	// our array and the total number the caller asked for - whichever is less.
	r = ((customdata_struct*) refcon)->nelements - inOffset;
	if (r > inMax) r = inMax;
	
	// Now copy the actual items from our array to the returned memory.
	for (n = 0; n < r; ++n)
	  outValues[n] = ((float*) ((customdata_struct*) refcon)->data)[n + inOffset];
	return r;
      }
    }
    return 0;
}

/* custom float array dataref write callback */
void SetDatavfCB(void *refcon, float *inValues, int inOffset, int inMax)
{
    int n, r;

    if (refcon != NULL) {
      if (((customdata_struct*) refcon)->data != NULL) {
	// Calculate the number of items to copy in.  This is the lesser of the number
	// the caller writes and the end of our array.
	r = ((customdata_struct*) refcon)->nelements - inOffset;
	if (r > inMax) r = inMax;
	
	// Copy the actual data.
	for (n = 0; n < r; ++n)
	  ((float*) ((customdata_struct*) refcon)->data)[n+ inOffset] = inValues[n];
      }
    }
}
/* custom byte/char array dataref read callback */
int GetDatabCB(void *refcon, void *outValues, int inOffset, int inMax)
{
    int n, r;

    if (refcon != NULL) {
      if (((customdata_struct*) refcon)->data != NULL) {
	// If the array ptr is null, the caller just wants to know the total item count!
	if (outValues == NULL)
	  return ((customdata_struct*) refcon)->nelements;
	
	// Calculate the number of items to return.  We must limit by both the end of 
	// our array and the total number the caller asked for - whichever is less.
	r = ((customdata_struct*) refcon)->nelements - inOffset;
	if (r > inMax) r = inMax;
	
	// Now copy the actual items from our array to the returned memory.
	for (n = 0; n < r; ++n)
	  ((unsigned char*) outValues)[n] = ((unsigned char*) ((customdata_struct*) refcon)->data)[n + inOffset];
	return r;
      }
    }
    return 0;
}

/* custom byte/char array dataref write callback */
void SetDatabCB(void *refcon, void *inValues, int inOffset, int inMax)
{
    int n, r;

    if (refcon != NULL) {
      if (((customdata_struct*) refcon)->data != NULL) {
	// Calculate the number of items to copy in.  This is the lesser of the number
	// the caller writes and the end of our array.
	r = ((customdata_struct*) refcon)->nelements - inOffset;
	if (r > inMax) r = inMax;
	
	// Copy the actual data.
	for (n = 0; n < r; ++n)
	  ((unsigned char*) ((customdata_struct*) refcon)->data)[n+ inOffset] = ((unsigned char*) inValues)[n];
      }
    }
}
