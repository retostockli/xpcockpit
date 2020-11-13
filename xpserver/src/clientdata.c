/* clientdata.c creates, stores, modifies X-Plane data buffers of connected clients

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

#include "clientdata.h"   

/* allocation of global variables defined in clientdata.h */
clientdata_struct *clientdata; /* pointer to dynamically allocatable xp data structure */

int numalloc; /* number of clientdata elements allocated */
int numlink; /* number of clientdata elements linked */
char clientname[100]; /* name of x-plane client application */

int allocate_clientdata(int offset, int type, int nelements, int index, int precision, char datarefname[]) {

  int found = 0;
  int j;

  /* disable array element selection for scalar data */
  if ((type == XPTYPE_INT) || (type == XPTYPE_FLT) || (type == XPTYPE_DBL) || 
      (type == XPTYPE_CMD_ONCE) || (type == XPTYPE_CMD_HOLD)) {
    nelements = 1;
    index = 0;
  }

  /* check for zero or negative number of elements elements */
  if (nelements < 1) {
    if (verbose > 0) fprintf(logfileptr,"Number of elements < 1 for dataref %s \n",datarefname);
    return -1;
  }
 
  /* check for negative elment index */
  /* index of 0 .. nelements-1 : select individual element of array */
  /* index of -1 : transfer all elements of array */
  if (index < -1) {
    if (verbose > 0) fprintf(logfileptr,"Index of element < -1 for dataref %s \n",datarefname);
    return -1;
  }


  /* check for too high elment index */
  if (index >= nelements) {
    if (verbose > 0) fprintf(logfileptr,"Index of element > %i for dataref %s \n",nelements,datarefname);
    return -1;
  }

  /* check for zero length string in datarefname */
  if (!strcmp(datarefname,"")) {
    if (verbose > 0) fprintf(logfileptr,"Dataref has no name. Not created\n");
    return -1;
  }

  /* disable precision checking for integer/char data and commands */
  if ((type == XPTYPE_INT) || (type == XPTYPE_INT_ARR) || (type == XPTYPE_BYTE_ARR) || 
      (type == XPTYPE_CMD_ONCE) || (type == XPTYPE_CMD_HOLD)) {
    precision = INT_MISS;
  }

  if (((precision > 10) || (precision < -10)) && (precision != INT_MISS)) {
    if (verbose > 0) fprintf(logfileptr,"Precision %i for dataref %s outside of range -10..10 Setting to missing \n",
			     precision, datarefname);
    precision = INT_MISS;
  }

  if (clientdata != NULL) {
    /* search for dataref */
    if (offset<numalloc) {
      if (!strcmp(datarefname,clientdata[offset].datarefname) && (index == clientdata[offset].index)) {
	found = 1;
      }
    }
  } else {
    /* allocate clientdata structure */
    if (verbose > 0) fprintf(logfileptr,"clientdata structure not yet allocated \n");
  }

  if (found) {
    /* check if dataref is consistent */

    //    if (type != clientdata[offset].type) {
    if ((clientdata[offset].type & type) != type) {
      if (verbose > 0) fprintf(logfileptr,"Requested type %i does not match type %i for dataref %s \n",
	     type,clientdata[offset].type,datarefname);
      return -1;
    }

    if (nelements != clientdata[offset].nelements) {
      if (verbose > 0) fprintf(logfileptr,"Requested elements %i does not match elements %i for dataref %s \n",
	     nelements,clientdata[offset].nelements,datarefname);
      return -1;
    }

    if (index != clientdata[offset].index) {
      if (verbose > 0) fprintf(logfileptr,"Requested index %i does not match index %i for dataref %s \n",
	     index,clientdata[offset].index,datarefname);
      return -1;
    }

    if (precision != clientdata[offset].precision) {
      if (verbose > 0) fprintf(logfileptr,"Requested precision %i does not match precision %i for dataref %s \n",
	     precision,clientdata[offset].precision,datarefname);
      return -1;
    }

    if (verbose > 0) fprintf(logfileptr,"Existing dataref: %s \n",datarefname);

  } else {
    /* create a new data element for this dataref */

    /* allocate more memory (for one more element of clientdata) */
    if (offset >= numalloc) {
      void *tmp = realloc(clientdata, sizeof(clientdata_struct) * (offset+1));
      if (tmp == NULL) { 
	if (verbose > 0) fprintf(logfileptr,"Memory allocation error when allocating %i data elements \n", offset+1);
	return -1;
      } else {
	clientdata = tmp;
	numalloc=(offset+1);
      }
    } 

    /* set status to non-allocated */
    clientdata[offset].status = XPSTATUS_UNINITIALIZED;

    /* allocate memory for dataref */
    switch (type) {
    case XPTYPE_INT: 
      clientdata[offset].data = malloc(sizeof(int));
      *(int*) clientdata[offset].data = INT_MISS;
      break;
    case XPTYPE_FLT: 
      clientdata[offset].data = malloc(sizeof(float));
      *(float*) clientdata[offset].data = FLT_MISS;
      break;
    case XPTYPE_DBL:
      clientdata[offset].data = malloc(sizeof(double));
      *(double*) clientdata[offset].data = DBL_MISS;
      break;
    case XPTYPE_FLT_ARR:
      if (index == -1) {
	clientdata[offset].data = malloc(sizeof(float)*nelements);
	for (j=0;j<nelements;j++) {
	  ((float*) clientdata[offset].data)[j] = FLT_MISS;
	}
      } else {
	clientdata[offset].data = malloc(sizeof(float));
	*(float*) clientdata[offset].data = FLT_MISS;
      }
      break;
    case XPTYPE_INT_ARR:
      if (index == -1) {
	clientdata[offset].data = malloc(sizeof(int)*nelements);
	for (j=0;j<nelements;j++) {
	  ((int*) clientdata[offset].data)[j] = INT_MISS;
	}
      } else {
	clientdata[offset].data = malloc(sizeof(int));
	*(int*) clientdata[offset].data = INT_MISS;
      }
      break;
    case XPTYPE_BYTE_ARR:
      if (index == -1) {
	clientdata[offset].data = malloc(sizeof(unsigned char)*nelements);
	for (j=0;j<nelements;j++) {
	  ((unsigned char*) clientdata[offset].data)[j] = 0;
	}
      } else {
	clientdata[offset].data = malloc(sizeof(unsigned char));
	*(unsigned char*) clientdata[offset].data = 0;
      }
      break;
    case XPTYPE_CMD_ONCE:
    case XPTYPE_CMD_HOLD:
      clientdata[offset].data = malloc(sizeof(int));
      *(int*) clientdata[offset].data = INT_MISS;
      break;
    default:
      if (verbose > 0) fprintf(logfileptr,"Unknown data type for dataref %s \n",datarefname);
      if (verbose > 0) fprintf(logfileptr,"Type %i Number of elements %i Index %i Precision %i \n", 
			       type, nelements, index, precision);
      clientdata[offset].data = NULL;
      break;
    }

    if (((precision > 10) || (precision < -10)) && (precision != INT_MISS)) {
      if (verbose > 0) fprintf(logfileptr,"Precision %i outside of range -10..10 Setting to missing \n",precision);      
      precision = INT_MISS;
    }

    if (clientdata[offset].data == NULL) {
      if (verbose > 0) fprintf(logfileptr,"Memory allocation error for dataref %s \n",datarefname);
      return -1;
    } else {
      strcpy(clientdata[offset].datarefname,datarefname);
      clientdata[offset].status = XPSTATUS_ALLOC;
      clientdata[offset].type = type;
      clientdata[offset].nelements = nelements;
      clientdata[offset].index = index;
      clientdata[offset].precision = precision;
      if (verbose > 0) fprintf(logfileptr,"Created dataref: %s \n",datarefname);
    }

  }

  return 0;

}

/* check validity of dataref, type, number of elements etc. */
/* if check fails, the datarefname field is replaced with an error string */
/* return values: 0=OK, 1=dataref not found in x-plane (continue with custom dataref creation), 2=type/nelements/etc wrong */
int check_clientdata(int offset) {

  int xptype;

  if (clientdata != NULL) {

    if (offset<numalloc) {      

      switch (clientdata[offset].type) {
	case XPTYPE_CMD_ONCE:  // special checking for commands
	case XPTYPE_CMD_HOLD:
	  clientdata[offset].dataref = XPLMFindCommand(clientdata[offset].datarefname);
	  if (clientdata[offset].dataref != NULL) {
	    clientdata[offset].write = 1;
          } else {
	    if (verbose > 0) fprintf(logfileptr, "Command %s not found in X-Plane. \n",clientdata[offset].datarefname);
	    return 1;
          }
	  break;
	default:  // normal dataref checking
	  clientdata[offset].dataref = XPLMFindDataRef(clientdata[offset].datarefname);
	  if (XPLMIsDataRefGood(clientdata[offset].dataref)) {
	    xptype = XPLMGetDataRefTypes(clientdata[offset].dataref);
	    /* float and double types can concur (type 6): type 2+4. Make it always double (type 4) */
	    /*
	    if (xptype == (XPTYPE_FLT+XPTYPE_DBL)) { 
	      xptype = XPTYPE_DBL; 
	      } */
	    //	    if (clientdata[offset].type != xptype) {
	    if ((xptype & clientdata[offset].type) != clientdata[offset].type) {
	      if (verbose > 0) fprintf(logfileptr, "Dataref %s differs in type: %i (client) vs. %i (X-Plane) \n",
		  clientdata[offset].datarefname, clientdata[offset].type, 
		  XPLMGetDataRefTypes(clientdata[offset].dataref));
	      snprintf(clientdata[offset].datarefname,sizeof(clientdata[offset].datarefname),
		       "Dataref differs in type: %i (client) vs. %i (X-Plane)",
		       clientdata[offset].type, XPLMGetDataRefTypes(clientdata[offset].dataref));
	      return -1;
	    }

	    if (clientdata[offset].nelements > 1) {
	      switch (clientdata[offset].type) {
	      case XPTYPE_FLT_ARR: 
	        if (clientdata[offset].nelements != XPLMGetDatavf(clientdata[offset].dataref, NULL,0,0)) {
	          if (verbose > 0) fprintf(logfileptr, "Dataref %s differs in # elements: %i (client) vs. %i (X-Plane) \n",
		      clientdata[offset].datarefname, clientdata[offset].nelements, 
		      (int) XPLMGetDatavf(clientdata[offset].dataref, NULL,0,0));
		  snprintf(clientdata[offset].datarefname,sizeof(clientdata[offset].datarefname),
			   "Dataref differs in # elements: %i (client) vs. %i (X-Plane)",
			   clientdata[offset].nelements, (int) XPLMGetDatavf(clientdata[offset].dataref, NULL,0,0));
	          return -1;
	        }
	        break;
	      case XPTYPE_INT_ARR:
	        if (clientdata[offset].nelements != XPLMGetDatavi(clientdata[offset].dataref, NULL,0,0)) {
	          if (verbose > 0) fprintf(logfileptr, "Dataref %s differs in # elements: %i (client) vs. %i (X-Plane) \n",
		      clientdata[offset].datarefname, clientdata[offset].nelements, 
		      (int) XPLMGetDatavi(clientdata[offset].dataref, NULL,0,0));
		  snprintf(clientdata[offset].datarefname,sizeof(clientdata[offset].datarefname),"Dataref differs in # elements: %i (client) vs. %i (X-Plane)",clientdata[offset].nelements, (int) XPLMGetDatavi(clientdata[offset].dataref, NULL,0,0));
	          return -1;
	        }
	        break;
	      case XPTYPE_BYTE_ARR:
		/* do not treat different array size for byte/char array as error since text strings
		   may vary in size also during runtime. Optimally, only valid elements will be read
		   even if the client specifies less or more elements than the dataref holds */
	        if (clientdata[offset].nelements != XPLMGetDatab(clientdata[offset].dataref, NULL,0,0)) {
	          if (verbose > 0) fprintf(logfileptr, "Dataref %s differs in # elements: %i (client) vs. %i (X-Plane) \n",
		      clientdata[offset].datarefname, clientdata[offset].nelements, 
		      (int) XPLMGetDatab(clientdata[offset].dataref, NULL,0,0));
		  /*
		    snprintf(clientdata[offset].datarefname,sizeof(clientdata[offset].datarefname),
			   "Dataref differs in # elements: %i (client) vs. %i (X-Plane)",
			   clientdata[offset].nelements, (int) XPLMGetDatab(clientdata[offset].dataref, NULL,0,0));
	          return -1;
		  */
	        }
	        break;
	      default:
	        break;
	      }

	    }

	    clientdata[offset].write = XPLMCanWriteDataRef(clientdata[offset].dataref);

          } else {
	    /* x-plane dataref not found */
	    if (verbose > 0) fprintf(logfileptr, "Dataref %s not found in X-Plane. \n",clientdata[offset].datarefname);
	    return 1;
	  }
	  break;
      } // switch command or dataref  
      
    } else {
      if (verbose > 0) fprintf(logfileptr,"Cannot check dataref with offset %i: offset does not exist \n",offset);
      return -1;
    }

  } else {
    if (verbose > 0) fprintf(logfileptr,"Cannot check dataref with offset %i: clientdata structure not allocated \n",offset);
  }

  return 0;
}

int deallocate_clientdata(int offset) {

  if (clientdata != NULL) {

    if (offset<numalloc) {      
      
      /* deallocate memory for dataref */
      
      if (clientdata[offset].data != NULL) {
	free(clientdata[offset].data);
	clientdata[offset].data = NULL;
      }
      
      clientdata[offset].type = XPTYPE_UNKNOWN;
      clientdata[offset].nelements = INT_MISS;
      clientdata[offset].index = INT_MISS;
      clientdata[offset].precision = INT_MISS;
      clientdata[offset].status = XPSTATUS_DEALLOC;
      memset(clientdata[offset].datarefname,0,sizeof(clientdata[offset].datarefname));
      
      if (verbose > 0) fprintf(logfileptr,"Deallocated dataref with offset %i \n",offset);
      
    } else {
      if (verbose > 0) fprintf(logfileptr,"Cannot deallocate dataref with offset %i: offset does not exist \n",offset);
      return -1;
    }
  } else {
    if (verbose > 0) fprintf(logfileptr,"Cannot deallocate dataref with offset %i: clientdata structure not allocated \n",offset);
    return -1;
  }

  return 0;
}  

void clear_clientdata() {

  int offset;

  /* deallocate data buffers */
  if (clientdata != NULL) {
    for (offset=0;offset<numalloc;offset++) {
      if (clientdata[offset].data != NULL) {
	if (verbose > 0) fprintf(logfileptr,"Cleared clientdata entry %i \n",offset);
	free(clientdata[offset].data);
	clientdata[offset].data = NULL;
      } 	   
    }

    /* deallocate data structure */
    free(clientdata);
    clientdata = NULL;
    numalloc = 0;
  }
  
}

/* update count of linked clientdata elements */
void count_clientdata() {

  int i;

  numlink = 0;
  if (clientdata != NULL) {    
    if (numalloc > 0) {
      for (i=0;i<numalloc;i++) {
	if (clientdata[i].status == XPSTATUS_VALID) {
	  numlink++;
	}
      }
    }
  }

}
