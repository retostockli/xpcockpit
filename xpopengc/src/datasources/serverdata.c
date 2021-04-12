/* This is the serverdata.c which contains all functions that generate and access
   the X-Plane data stream

   Copyright (C) 2009 - 2013  Reto Stockli
   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more sdetails.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

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

#include "serverdata.h"
#include "common.h"

int predict = 1;

serverdata_struct *serverdata; /* pointer to dynamically allocatable xp data structure */
int numalloc; /* number of serverdata elements allocated */
int numlink; /* number of serverdata elements linked */
char clientname[100]; /* name of x-plane client package */
int lastindex; /* stores the index of the last Dataref called */
int numreceived; /* stores number of received datarefs in this loop */
struct timeval time_start; /* stores the time when we initialized the dataref structure */

int time_subtract (struct timeval *x, struct timeval *y, struct timeval *result) {
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

/* initialize local dataref structure */
int initialize_dataref() {

  serverdata = NULL;
  numalloc = 0;
  numlink = 0;
  numreceived = 0;
  
  gettimeofday(&time_start, NULL);

  return 0;
}


/* deallocate local dataref structure */
void clear_dataref() {

  int i;
  struct timeval time_end;
  struct timeval time_elapsed;
  float seconds_elapsed;
  float fsend;
  float frecv;
  gettimeofday(&time_end, NULL);
  //  timersub(&time_end, &time_start, &time_elapsed);
  time_subtract(&time_end, &time_start, &time_elapsed);
  seconds_elapsed = (float) time_elapsed.tv_sec + ((float) time_elapsed.tv_usec) / 1000000.0;
  
  if (serverdata != NULL) {    
    if (numalloc > 0) {
      if (verbose > 0) {
	printf("\n");
	printf("Time elapsed: %f seconds\n",seconds_elapsed);
	printf("Statistics on Dataref exchange \n");
	printf("No.    send/s    recv/s   datarefname \n");
      }
      for (i=0;i<numalloc;i++) {
	if (serverdata[i].data != NULL) {
	  if (verbose > 0) {
	    fsend = (float) serverdata[i].nsend / seconds_elapsed;
	    frecv = (float) serverdata[i].nrecv / seconds_elapsed;
	    if ((fsend > 15.0) || (frecv > 15.0)) {
	      printf("\033[1;31m");
	    } else if ((fsend > 5.0) || (frecv > 5.0)) {
	      printf("\033[1;33m");
	    } else if ((fsend > 1.0) || (frecv > 1.0)) {
	      printf("\033[1;32m");
	    }
	    printf("%4i %8.3f %8.3f  %s \n",i,fsend,frecv,serverdata[i].datarefname);
	    printf("\033[0m");
	  }
	  free(serverdata[i].data);
	  serverdata[i].data = NULL;
	}
	if (serverdata[i].data_old != NULL) {
	  free(serverdata[i].data_old);
	  serverdata[i].data_old = NULL;
	}
	
      }
      
      free(serverdata);
      serverdata = NULL;
      numalloc = 0;
      numlink = 0;
      numreceived = 0;

      printf("serverdata structure deallocated\n");
    }
  }
}

/* update count of linked serverdata elements */
void count_dataref() {

  int i;

  numlink = 0;
  if (serverdata != NULL) {    
    if (numalloc > 0) {
      for (i=0;i<numalloc;i++) {
	if (serverdata[i].status == XPSTATUS_VALID) {
	  numlink++;
	}
      }
    }
  }

}

/* update count of freshly received datarefs */
void count_received() {

  int i;

  numreceived = 0;
  if (serverdata != NULL) {    
    if (numalloc > 0) {
      for (i=0;i<numalloc;i++) {
	if (serverdata[i].received == 1) {
	  numreceived++;
	}
      }
    }
  }

}

/* reset received flag of datarefs */
void reset_received() {

  int i;

  if (serverdata != NULL) {    
    if (numalloc > 0) {
      for (i=0;i<numalloc;i++) {
	serverdata[i].received = 0;
      }
    }
  }

}

int *link_dataref_int(const char datarefname[]) {
  return (int *) link_dataref(datarefname, XPTYPE_INT, 1, 0, INT_MISS);
}
float *link_dataref_flt(const char datarefname[], int precision) {
  return (float *) link_dataref(datarefname, XPTYPE_FLT, 1, 0, precision);
}
double *link_dataref_dbl(const char datarefname[], int precision) {
  return (double *) link_dataref(datarefname, XPTYPE_DBL, 1, 0, precision);
}
float *link_dataref_flt_arr(const char datarefname[], int nelements, int index, int precision) {
  return (float *) link_dataref(datarefname, XPTYPE_FLT_ARR, nelements, index, precision);
}
int *link_dataref_int_arr(const char datarefname[], int nelements, int index) {
  return (int *) link_dataref(datarefname, XPTYPE_INT_ARR, nelements, index, INT_MISS);
}
unsigned char *link_dataref_byte_arr(const char datarefname[], int nelements, int index) {
  return (unsigned char *) link_dataref(datarefname, XPTYPE_BYTE_ARR, nelements, index, INT_MISS);
}
int *link_dataref_cmd_once(const char datarefname[]) {
  return (int *) link_dataref(datarefname, XPTYPE_CMD_ONCE, 1, 0, INT_MISS);
}
int *link_dataref_cmd_hold(const char datarefname[]) {
  return (int *) link_dataref(datarefname, XPTYPE_CMD_HOLD, 1, 0, INT_MISS);
}

/* allocate memory for local dataref copy */
void *link_dataref(const char datarefname[], int type, int nelements, int index, int precision) {
  
  int i,j;
  int found = 0;

  /* disable array element selection for scalar data */
  if ((type == XPTYPE_INT) || (type == XPTYPE_FLT) || (type == XPTYPE_DBL) || 
      (type == XPTYPE_CMD_ONCE) || (type == XPTYPE_CMD_HOLD)) {
    nelements = 1;
    index = 0;
  }

  /* check for zero or negative number of elements elements */
  if (nelements < 1) {
    printf("Number of elements < 1 for dataref %s \n",datarefname);
    return NULL;
  }
 
  /* check for negative elment index */
  /* index of 0 .. nelements-1 : select individual element of array */
  /* index of -1 : transfer all elements of array */
  if (index < -1) {
    printf("Index of element < -1 for dataref %s \n",datarefname);
    return NULL;
  }

  /* check for too high elment index */
  if (index >= nelements) {
    printf("Index of element > %i for dataref %s \n",nelements,datarefname);
    return NULL;
  }

  /* check for zero length string in datarefname */
  if (!strcmp(datarefname,"")) {
    printf("Dataref has no name. Not created \n");      
    return NULL;
  }

  /* disable precision checking for integer/char data and commands */
  if ((type == XPTYPE_INT) || (type == XPTYPE_INT_ARR) || (type == XPTYPE_BYTE_ARR) || 
      (type == XPTYPE_CMD_ONCE) || (type == XPTYPE_CMD_HOLD)) {
    precision = INT_MISS;
  }

  if (((precision > 10) || (precision < -10)) && (precision != INT_MISS)) {
    printf("Precision %i for dataref %s outside of range -10..10 Setting to missing \n",precision, datarefname);
    precision = INT_MISS;
  }

  if (serverdata != NULL) {
    /* search for dataref */

    if (predict) {
      if ((lastindex != INT_MISS) && (lastindex < numalloc)) {
	if (serverdata[lastindex].nextindex != INT_MISS) {
	  i = serverdata[lastindex].nextindex;
	  if (!strcmp(datarefname,serverdata[i].datarefname) && (type == serverdata[i].type)
	      && (index == serverdata[i].index) && (nelements == serverdata[i].nelements)) {
	    found = 1;
	    //printf("FOUND %i %i %s \n",lastindex,serverdata[lastindex].nextindex,datarefname);
	    lastindex = i;
	  }
	}
      }
      if (!found) {
	i = 0;
	while (i<numalloc) {
	  if (!strcmp(datarefname,serverdata[i].datarefname) && (type == serverdata[i].type)
	      && (index == serverdata[i].index) && (nelements == serverdata[i].nelements)) {
	    found = 1;
	    if ((lastindex != INT_MISS) && (lastindex < numalloc)) {
	      serverdata[lastindex].nextindex = i;
	    }
	    //printf("MISSED %i %i %s \n",lastindex,serverdata[lastindex].nextindex,datarefname);
	    lastindex = i;
	    break;
	  }
	  i++;
	}	  
      }
    } else {
      i = 0;
      while (i<numalloc) {
	if (!strcmp(datarefname,serverdata[i].datarefname) && (type == serverdata[i].type)
	    && (index == serverdata[i].index) && (nelements == serverdata[i].nelements)) {
	  found = 1;
	  break;
	}
	i++;
      }
    }
  } 

  if (found) {
    /* check if dataref is consistent */
    
    if (type != serverdata[i].type) {
      printf("Warning: Requested type %i does not match type %i for dataref %s \n",
	     type,serverdata[i].type,datarefname);
    }

    if (nelements != serverdata[i].nelements) {
      printf("Warning: Requested elements %i does not elements %i for dataref %s \n",
	     nelements,serverdata[i].nelements,datarefname);
    }

    if (index != serverdata[i].index) {
      printf("Warning: Requested index %i does not match index %i for dataref %s \n",
	     index,serverdata[i].index,datarefname);
    }

    if (precision != serverdata[i].precision) {
      printf("Warning: Requested precision %i does not match precision %i for dataref %s \n",
	     precision,serverdata[i].precision,datarefname);
    }
  } else {
    /* create a new data element for this dataref */

    /* search for first free data reference within allocated data elements */    
    i = 0;
    lastindex = 0;
    while (i<numalloc) {
      if (!strcmp("",serverdata[i].datarefname)) {
	break;
      }
      i++;
    }

    /* no free data element found */
    /* allocate more memory (for one more element of serverdata) */
    if (i == numalloc) {
      numalloc++;
      void *tmp = realloc(serverdata, sizeof(serverdata_struct)*numalloc);
      if (tmp == NULL) { 
	printf("Memory allocation error when allocating %i data elements \n",numalloc);
	numalloc--;
	return NULL;
      } else {
	serverdata = tmp;
      }

    } 

    /* set status to non-allocated */
    serverdata[i].status = XPSTATUS_UNINITIALIZED;

    /* allocate memory for dataref */
    switch (type) {
    case XPTYPE_INT: 
      serverdata[i].data = malloc(sizeof(int));
      serverdata[i].data_old = malloc(sizeof(int));
      *((int*) serverdata[i].data) = INT_MISS;
      *((int*) serverdata[i].data_old) = INT_MISS;
      break;
    case XPTYPE_FLT: 
      serverdata[i].data = malloc(sizeof(float));
      serverdata[i].data_old = malloc(sizeof(float));
      *((float*) serverdata[i].data) = FLT_MISS;
      *((float*) serverdata[i].data_old) = FLT_MISS;
      break;
    case XPTYPE_DBL:
      serverdata[i].data = malloc(sizeof(double));
      serverdata[i].data_old = malloc(sizeof(double));
      *((double*) serverdata[i].data) = DBL_MISS;
      *((double*) serverdata[i].data_old) = DBL_MISS;
      break;
    case XPTYPE_FLT_ARR:
      if (index == -1) {
	serverdata[i].data = malloc(sizeof(float)*nelements);
	serverdata[i].data_old = malloc(sizeof(float)*nelements);
	for (j=0;j<nelements;j++) {
	  *((float*) serverdata[i].data + j) = FLT_MISS;
	  *((float*) serverdata[i].data_old + j) = FLT_MISS;
	}
      } else {
	serverdata[i].data = malloc(sizeof(float));
	serverdata[i].data_old = malloc(sizeof(float));
	*((float*) serverdata[i].data) = FLT_MISS;
	*((float*) serverdata[i].data_old) = FLT_MISS;
      }
      break;
    case XPTYPE_INT_ARR:
      if (index == -1) {
	serverdata[i].data = malloc(sizeof(int)*nelements);
	serverdata[i].data_old = malloc(sizeof(int)*nelements);
	for (j=0;j<nelements;j++) {
	  *((int*) serverdata[i].data + j) = INT_MISS;
	  *((int*) serverdata[i].data_old + j) = INT_MISS;
	}
      } else {
	serverdata[i].data = malloc(sizeof(int));
	serverdata[i].data_old = malloc(sizeof(int));
	*((int*) serverdata[i].data) = INT_MISS;
	*((int*) serverdata[i].data_old) = INT_MISS;
      }
      break;
    case XPTYPE_BYTE_ARR:
      if (index == -1) {
	serverdata[i].data = malloc(sizeof(unsigned char)*nelements);
	serverdata[i].data_old = malloc(sizeof(unsigned char)*nelements);
	for (j=0;j<nelements;j++) {
	  *((unsigned char*) serverdata[i].data + j) = 0;
	  *((unsigned char*) serverdata[i].data_old + j) = 0;
	}
      } else {
	serverdata[i].data = malloc(sizeof(unsigned char));
	serverdata[i].data_old = malloc(sizeof(unsigned char));
	*((unsigned char*) serverdata[i].data) = 0;
	*((unsigned char*) serverdata[i].data_old) = 0;
      }
      break;
    case XPTYPE_CMD_ONCE:
    case XPTYPE_CMD_HOLD:
	serverdata[i].data = malloc(sizeof(int));
	serverdata[i].data_old = malloc(sizeof(int));
	*((int*) serverdata[i].data) = INT_MISS;
	*((int*) serverdata[i].data_old) = INT_MISS;
      break;
    default:
      printf("Unknown data type for dataref %s \n",datarefname);
      serverdata[i].data = NULL;
      serverdata[i].data_old = NULL;
      break;
    }

    if ((serverdata[i].data == NULL) || (serverdata[i].data_old == NULL)) {
      printf("Memory allocation error for dataref %s \n",datarefname);
      return NULL;
    } else {
      memset(serverdata[i].datarefname,0,sizeof(serverdata[i].datarefname));
      strcpy(serverdata[i].datarefname,datarefname);
      serverdata[i].status = XPSTATUS_ALLOC;
      serverdata[i].type = type;
      serverdata[i].nelements = nelements;
      serverdata[i].index = index;
      serverdata[i].precision = precision;
      serverdata[i].nextindex = INT_MISS;
      serverdata[i].nsend = 0;
      serverdata[i].nrecv = 0;
      serverdata[i].received = 0;
      if (type >= XPTYPE_CMD_ONCE) {
	printf("Created commandref %s \n",datarefname);
      } else {
	printf("Created dataref %s \n",datarefname);
      } 
      fflush(stdout);
    }

  }

  return serverdata[i].data;

}

/* deallocate memory for local dataref copy */
int unlink_dataref(const char datarefname[]) {
  
  int i=0;
  int found=0;

  if (serverdata != NULL) {
    /* search for dataref */

    while (i<numalloc) {      
      if (!strcmp(datarefname,serverdata[i].datarefname)) {
	found++;
	/* deallocate memory for dataref */
	
	if (serverdata[i].data != NULL) {
	  free(serverdata[i].data);
	  serverdata[i].data = NULL;
	}
	
	if (serverdata[i].data_old != NULL) {
	  free(serverdata[i].data_old);
	  serverdata[i].data_old = NULL;
	}
	
	serverdata[i].type = XPTYPE_UNKNOWN;
	serverdata[i].nelements = INT_MISS;
	serverdata[i].index = INT_MISS;
	serverdata[i].precision = INT_MISS;
	serverdata[i].status = XPSTATUS_DEALLOC;
	serverdata[i].nextindex = INT_MISS;
	serverdata[i].nsend = INT_MISS;
	serverdata[i].nrecv = INT_MISS;
	serverdata[i].received = INT_MISS;
	
	printf("Unlink: Deallocated dataref: %s \n",datarefname);
      }
      i++;
    }
  }

  if (found == 0) {
    printf("Cannot Unlink: not allocated dataref: %s \n",datarefname);
  }

  /* returns the number of deallocated datarefs */
  return found;
}
