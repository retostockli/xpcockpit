/* This is the libiocards.c code which contains all functions to interact with
   the IOCARDS devices via USB interface 

   Copyright (C) 2009 - 2013 Reto Stockli

   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation
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
#include "iniparser.h"
#include "libleos.h"
#include "handleserver.h"
#include "serverdata.h"

#ifdef LIBUSB
#include "libusb_interface.h"
#endif

#ifdef HIDAPI
#include "hidapi_interface.h"
#endif

extern float roundf(float);

/* allocation of global variables from libleo.h */
leo_struct leo[MAXDEVICES];
int initial; 

/* this routine prints the GNU license information */
void print_license(void)
{
  printf("XPLEO Copyright (C) 2025 Reto Stockli\n");
  printf("\n");
  printf("This program comes with ABSOLUTELY NO WARRANTY\n");
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under the terms of the GNU General Public License as published by\n");
  printf("the Free Software Foundation, either version 3 of the License, or\n");
  printf("any later version. For details see http://www.gnu.org/licenses/gpl.html.\n");
  printf("\n");

}

/* this routine parses the xyz.ini file and reads its values */
int read_ini(char* programPath, char* iniName)
{
  int ret = 0;
  int device;
  char sdevice[50];
  dictionary *ini;
  char filename[255];
  char cwd[200];
  char *pch;

  char default_server_ip[] = "";
  int default_server_port = 8091;	// port 8090 traditionally used by other SIOC plugins, like uipcx!
  int default_verbose = 0;

  /* default Leo values (not connected) */
  char default_name[] = "none";
  char default_serial[] = "";
  uint16_t default_vendor = 0xFFFF;
  uint16_t default_product = 0xFFFF;
  uint8_t default_bus = 0xFF;
  uint8_t default_address = 0xFF;
  char default_path[] = "";
  int default_naxes = 0;
  int default_nbits = 8;
  int default_ninputs = 0;
  int default_status = -1;

  memset(XPlaneServerIP,0,sizeof(XPlaneServerIP));

  /* check if we are in the source code directory or in the binary installation path */
  if (strncmp("/",programPath,1)==0) {
    /* we have full path starting code */
    printf("Starting with full program path \n");
    /* remove program name from path */
    pch = strrchr(programPath,'/');
    *pch = '\0';
    printf("%s\n",programPath);
#ifdef WIN
    pch = strstr (programPath,"\\bin");
#else
    pch = strstr (programPath,"/bin");
#endif
    if (pch == NULL) {
      snprintf (filename, sizeof(filename), "%s/../../inidata/%s.ini",programPath,iniName);
    } else {
      snprintf (filename, sizeof(filename), "%s/../share/xpleo/%s.ini",programPath,iniName);
    }
  } else {
    /* assume we start from source or bin dir */
    printf("Starting from bin/ or src/ directory \n");
    if (getcwd(cwd, sizeof(cwd)) == NULL) return -2;
#ifdef WIN
    pch = strstr (cwd,"\\bin");
#else
    pch = strstr (cwd,"/bin");
#endif
    if (pch == NULL) {
      snprintf(filename, sizeof(filename),"../inidata/%s.ini",iniName);
    } else {
      snprintf(filename, sizeof(filename),"../share/xpleo/%s.ini",iniName);
    }
  }

  ini = iniparser_load(filename);

  if (ini != NULL) {

    strcpy(XPlaneServerIP,iniparser_getstring(ini,"network:Server Address", default_server_ip));
    XPlaneServerPort = iniparser_getint(ini,"network:Server Port", default_server_port);

    verbose = iniparser_getint(ini,"general:verbose", default_verbose);
    
    /* Set up the client name for the server */
    strncpy(clientname,iniparser_getstring(ini,"general:Name", ""),sizeof(clientname));
    if (! strcmp(clientname,"")) strncpy(clientname,PACKAGE_NAME,sizeof(clientname));
    printf("XPLEO Client name: %s\n", clientname);

    /* read Leo connection list */
    printf("\n");
    printf("User-defined Devices: \n");
    for(device=0;device<MAXDEVICES;device++) {
      memset(leo[device].name,0,sizeof(leo[device].name));
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Name",device);
      strcpy(leo[device].name,iniparser_getstring(ini,sdevice, default_name));
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Vendor",device);
      leo[device].vendor = iniparser_getint(ini,sdevice, default_vendor);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Product",device);
      leo[device].product = iniparser_getint(ini,sdevice, default_product);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Bus",device);
      leo[device].bus = iniparser_getint(ini,sdevice, default_bus);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Address",device);
      leo[device].address = iniparser_getint(ini,sdevice, default_address);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Path",device);
      strcpy(leo[device].path,iniparser_getstring(ini,sdevice, default_path));
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Serial",device);
      strcpy(leo[device].serial,iniparser_getstring(ini,sdevice, default_serial));
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"device%i:Naxes",device);
      leo[device].naxes = iniparser_getint(ini,sdevice, default_naxes);

      leo[device].status = default_status;
      
      if (!strcmp(leo[device].name,default_name)) {
	// printf("Device %i NO \n",device);
      } else {
	printf("Device %i: \n",device);
	printf("Name %s \n",leo[device].name);
	printf("Vendor ID %i Product ID %i \n",leo[device].vendor,leo[device].product);
	//	printf("USB Bus %i USB Address %i \n",leo[device].bus,leo[device].address);
	printf("Serial No. %s \n",leo[device].serial);
	printf("USB Path %s \n",leo[device].path);
	printf("\n");
      }
    }

    printf("\n");
    iniparser_freedict(ini);

  } else {    
    ret = -1;
  }

  return ret;
}

/* this routine calculates the acceleration of rotary encoder based on last rotation time */
int get_acceleration (int device, int card, int input, int accelerator) 
{
  struct timeval t2;
  struct timeval t1;
  double dt;
  float acceleration;

  double mintime = 0.01;
  double maxtime = 0.05;

  /* get new time */
  gettimeofday(&t1,NULL);
  t2 = leo[device].time_enc[card][input];
  
  dt = - ((t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0);

  if (dt > mintime) {

    acceleration = 1 + (int) ( (maxtime / dt) * (float) accelerator);

    leo[device].time_enc[card][input] = t1;
    
  } else {
    /* likely a switch bounce during change, do not count! */

    acceleration = 0;
    
  }

  if (verbose > 2) {
    printf("LIBLEO: difference=%f [seconds], acceleration=%f for input=%i device=%i card=%i\n", 
	   dt, acceleration,input,device,card);
  }

  return acceleration;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {

    // Initialize pivot to be the first element
    int p = arr[low];
    int i = low;
    int j = high;

    while (i < j) {

        // Find the first element greater than
        // the pivot (from starting)
        while (arr[i] <= p && i <= high - 1) {
            i++;
        }

        // Find the first element smaller than
        // the pivot (from last)
        while (arr[j] > p && j >= low + 1) {
            j--;
        }
        if (i < j) {
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[low], &arr[j]);
    return j;
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {

        // call partition function to find Partition Index
        int pi = partition(arr, low, high);

        // Recursively call quickSort() for left and right
        // half based on Partition Index
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

/* initialize data arrays with default values */
/* flight data for USB and TCP/IP communication */
int initialize_leodata(void) 
{
  int device;
  int count;
  int hist;
  struct timeval time_new;

  /* set initialize stage */
  initial = 1;
  printf("LIBLEO: Initialization started \n");	

  /* get new time */
  gettimeofday(&time_new,NULL);

  /* reset all inputs, axes, outputs and displays */
  for (device=0;device<MAXDEVICES;device++) {
    //printf("%i %s %i \n",device,leo[device].name,leo[device].naxes);
    for (count=0;count<leo[device].ninputs;count++) {
      leo[device].inputs[count]=0;
      leo[device].inputs_old[count]=INITVAL;
      leo[device].time_enc[count] = time_new;
      leo[device].inputs_read[count] = 0;
    }
    for (count=0;count<leo[device].naxes;count++) {
      for (hist=0;hist<MAX_HIST;hist++) {
	leo[device].axes_hist[count][hist]=INITVAL;
      }
      leo[device].axes[count]=INITVAL;
      leo[device].axes_old[count]=INITVAL;
    }
  }

  return 0;
}

/* saves a copy of all LEO I/O states */
/* this is needed because, at each step, only the modified values */
/* are communicated either via TCP/IP or USB to X-Plane and LEO */
int copy_iocardsdata(void)
{
  int device;
  int count;
  
  for (device=0;device<MAXDEVICES;device++) {

    /* only copy new to old input data if the input data was read at least once */
    /* else keep the new input data */
    
    for (count=0;count<leo[device].ninputs;count++) {
      if (leo[device].inputs_read[card][count] == 1) {
	leo[device].inputs_old[card][count] = leo[device].inputs[card][count];
	leo[device].inputs_read[card][count] = 0;
      }
    }

    if (leo[device].naxes>0) {
      memcpy(leo[device].axes_old,leo[device].axes,sizeof(leo[device].axes));
    }

  }

  //  printf("%i %i %i %i \n",leo[7].axes[0],leo[7].axes[1],leo[7].axes[2],sizeof(leo[7].axes));

  /* finalized initial USB data retrieval */
  if (initial == 1) {
    printf("LIBLEO: Initialization completed \n");	  
    initial = 0;   
  }

  return(0);

}

/* retrieve input value from given input position on MASTERCARD or BU0836X/A Interface */
/* Two types : */
/* 0: pushbutton */
/* 1: toggle switch */
int digital_input(int device, int card, int input, int *value, int type)
{
  char device_name1[] = "BU0836X Interface";
  char device_name2[] = "BU0836A Interface";
  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, and -1 if something went wrong */

  if (value != NULL) {

    /* check if we have a connected and initialized device */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2)) 
	&& (iocard[device].status == 1)) {

      
	if ((input >=0) && (input<iocard[device].ninputs)) {
	  iocard[device].inputs_read[card][input] = 1;
	  if (type == 0) {
	    /* simple pushbutton / switch */
	    if (iocard[device].inputs_old[card][input] != iocard[device].inputs[card][input]) {
	      /* something changed */
	      *value = iocard[device].inputs[card][input];
	      retval = 1;
	      if (verbose > 1) {
		printf("LIBIOCARDS: Pushbutton                  : device=%i card=%i input=%i value=%i \n",
		       device, card, input, iocard[device].inputs[card][input]);
	      }
	    } else {
	      /* nothing changed */
	      *value = iocard[device].inputs[card][input];
	      retval = 0;
	    }
	  } else if (type == 1) {
	    /* toggle state everytime you press button */
	    /* inputs_old[input] always stores last state */
	    /* new state is then stored in inputs[input] */
	    if ((iocard[device].inputs_old[card][input] == 0) && (iocard[device].inputs[card][input] == 1)) {
	      /* toggle */
	      if (*value != INT_MISS) {
		if ((*value == 0) || (*value == 1)) {
		  *value = 1 - (*value);
		  retval = 1;
		  if (verbose > 1) {
		    printf("LIBIOCARDS: Toggle Switch               :  device=%i card=%i input=%i value=%i \n",
			   device, card, input, *value);
		  }
		} else {
		  printf("LIBIOCARDS: Toggle Switch Needs to have 0 or 1  :  device=%i card=%i input=%i value=%i \n",
			 device, card, input, *value);
		}
	      }
	    } else {
	      /* retain */
	      retval = 0;
	    }	
	  } else if (type == 2) {
	    /* simple pushbutton / switch like type 0
	       BUT ONLY THE STATE CHANGE FROM 0 TO 1 IS RETURNED AS VALUE, 
	       FOR PERMANENT SWITCH STATE OF 0 OR 1 OR A SWITH STATE 
	       CHANGE FROM 1 TO 0 THE VALUE IS PRESERVED.
	       THIS IS GOOD FOR EXECUTING COMMANDS WITH LINK_DATAREF_CMD_ONCE  */
	    if ((iocard[device].inputs_old[card][input] == 0) &&
		(iocard[device].inputs[card][input] == 1)) {
	      /* something changed */
	      *value = iocard[device].inputs[card][input];
	      retval = 1;
	      if (verbose > 1) {
		printf("LIBIOCARDS: Pushbutton                  : device=%i card=%i input=%i value=%i \n",
		  device, card, input, iocard[device].inputs[card][input]);
	      }
	    } else {
	      /* nothing changed */
	      retval = 0;
	    }
	  }
	} else {
	  retval = -1;
	  if (verbose > 0) printf("LIBIOCARDS: Invalid input position detected: %i \n", input);
	}

    } else {
      retval = -1;
      if (verbose > 1) printf("LIBIOCARDS: Device either not a MASTERCARD or BU0836X/A Interface or not ready: %i \n", device);
    }
  
  }

  return(retval);
}

/* retrieve encoder value and for given encoder type from given input position on MASTERCARD OR BU0836X/A Interface */
/* three type of encoders: */
/* 0: 1x12 rotary switch, wired like demonstrated on OpenCockpits website */
/* 1: optical rotary encoder using the Encoder II card */
/* 2: 2 bit gray type mechanical encoder */
/* 3: optical rotary encoder without the Encoder II card */
int mastercard_encoder(int device, int card, int input, float *value, float multiplier, int accelerator, int type)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  char device_name3[] = "BU0836X Interface";
  char device_name4[] = "BU0836A Interface";
  int oldcount, newcount; /* encoder integer counters */
  int updown = 0; /* encoder direction */
  int retval = 0; /* returns 1 if something changed, 0 if nothing changed and -1 if something went wrong */
  int obits[2]; /* bit arrays for 2 bit gray encoder */
  int nbits[2]; /* bit arrays for 2 bit gray encoder */
  
  if (value != NULL) {

    /* check if we have a connected and initialized device */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2) ||
	 !strcmp(iocard[device].name,device_name3) ||
	 !strcmp(iocard[device].name,device_name4)) 
	&& (iocard[device].status == 1)) {

      if ((card >=0) && (card<iocard[device].ncards)) {

	if (*value != FLT_MISS) {

	  if (((input >=0) && (input<(iocard[device].ninputs-2)) && (type==0)) || 
	      ((input >=0) && (input<(iocard[device].ninputs-1)) && (type>0))) {

	    iocard[device].inputs_read[card][input] = 1;
	    iocard[device].inputs_read[card][input+1] = 1;
	    if (type == 0) iocard[device].inputs_read[card][input+2] = 1;
	    
	    if (type == 0) { 
	      /* simulated encoder out of a 1x12 rotary switch */
	
	      if ((iocard[device].inputs[card][input]+
		   iocard[device].inputs[card][input+1]+
		   iocard[device].inputs[card][input+2]) == 0)
		/* 0 0 0 is a wrong measurement due to switch mechanics: do not count */
		{

		  iocard[device].inputs[card][input] = iocard[device].inputs_old[card][input];
		  iocard[device].inputs[card][input+1] = iocard[device].inputs_old[card][input+1];
		  iocard[device].inputs[card][input+2] = iocard[device].inputs_old[card][input+2];
		} else {
	  
		if (((iocard[device].inputs[card][input] != iocard[device].inputs_old[card][input]) ||
		     (iocard[device].inputs[card][input+1] != iocard[device].inputs_old[card][input+1]) ||
		     (iocard[device].inputs[card][input+2] != iocard[device].inputs_old[card][input+2])) &&
		    (iocard[device].inputs_old[card][input] != INITVAL) && 
		    (iocard[device].inputs_old[card][input+1] != INITVAL) &&
		    (iocard[device].inputs_old[card][input+2] != INITVAL)) {
		  /* something has changed */
	    
		  if (verbose > 1) {
		    printf("LIBIOCARDS: Rotary Encoder    1x12 Type : device=%i card=%i inputs=%i-%i values=%i %i %i \n",
			   device, card, input, input+2, iocard[device].inputs[card][input],
			   iocard[device].inputs[card][input+1], iocard[device].inputs[card][input+2]);
		  }
	    
		  newcount = iocard[device].inputs[card][input] + 
		    iocard[device].inputs[card][input+1]*2 + iocard[device].inputs[card][input+2]*3;
		  oldcount = iocard[device].inputs_old[card][input] + 
		    iocard[device].inputs_old[card][input+1]*2 + iocard[device].inputs_old[card][input+2]*3;
	    
		  if (newcount > oldcount) {
		    updown = 1;
		  } else {
		    updown = -1;
		  }
	    
		  if ((oldcount == 3) && (newcount == 1)) {
		    updown = 1;
		  }
		  if ((oldcount == 1) && (newcount == 3)) {
		    updown = -1;
		  }	    

		  if (updown != 0) {
		    *value = *value + (float) (updown * get_acceleration(device, card, input, accelerator)) * multiplier;
		    retval = 1;
		  }

		} 
	      }
	    }
      
	    if (type == 1) {
	      /* optical rotary encoder using the Encoder II card */
		
	      if (((iocard[device].inputs[card][input] != iocard[device].inputs_old[card][input]) || 
		   (iocard[device].inputs[card][input+1] != iocard[device].inputs_old[card][input+1]))
		  && (iocard[device].inputs_old[card][input] != INITVAL) && (iocard[device].inputs_old[card][input+1] != INITVAL)) {
		/* something has changed */
	  
		if (verbose > 1) {
		  printf("LIBIOCARDS: Rotary Encoder Optical Type : device=%i card=%i inputs=%i-%i values=%i %i \n",
			 device,card,input,input+1,iocard[device].inputs[card][input],iocard[device].inputs[card][input+1]);
		}

		if (iocard[device].inputs[card][input+1] == 1) {
		  updown = 1;
		} else {
		  updown = -1;
		}

		if (updown != 0) {
		  *value = *value + (float) (updown * get_acceleration(device, card, input, accelerator)) * multiplier;
		  retval = 1;
		}

	      }
	
	    }
      
	    if (type == 2) {
	      /* 2 bit gray type encoder */

	      if (((iocard[device].inputs[card][input] != iocard[device].inputs_old[card][input]) || 
		   (iocard[device].inputs[card][input+1] != iocard[device].inputs_old[card][input+1])) 
		  && (iocard[device].inputs_old[card][input] != INITVAL) && (iocard[device].inputs_old[card][input+1] != INITVAL)) {
		/* something has changed */
	
		/*
		  printf("%i %i %i %i %i %i %i %i\n",device,card,input,input+1,
		  iocard[device].inputs_old[card][input],
		  iocard[device].inputs_old[card][input+1],
		  iocard[device].inputs[card][input],
		  iocard[device].inputs[card][input+1]);
		*/

		if (verbose > 1) {
		  printf("LIBIOCARDS: Rotary Encoder    Gray Type : device=%i card=%i inputs=%i-%i values=%i %i \n",
			 device,card,input,input+1,iocard[device].inputs[card][input],iocard[device].inputs[card][input+1]);
		}

		/* derive last encoder count */
		obits[0] = iocard[device].inputs_old[card][input];
		obits[1] = iocard[device].inputs_old[card][input+1];
		oldcount = obits[0]+2*obits[1];
	  
		/* derive new encoder count */
		nbits[0] = iocard[device].inputs[card][input];
		nbits[1] = iocard[device].inputs[card][input+1];
		newcount = nbits[0]+2*nbits[1];

		/* forward */
		if (((oldcount == 0) && (newcount == 1)) ||
		    ((oldcount == 1) && (newcount == 3)) ||
		    ((oldcount == 3) && (newcount == 2)) ||
		    ((oldcount == 2) && (newcount == 0))) {
		  updown = 1;
		}
	  
		/* backward */
		if (((oldcount == 2) && (newcount == 3)) ||
		    ((oldcount == 3) && (newcount == 1)) ||
		    ((oldcount == 1) && (newcount == 0)) ||
		    ((oldcount == 0) && (newcount == 2))) {
		  updown = -1;
		}
	  
		if (updown != 0) {
		  *value = *value + (float) (updown * get_acceleration(device, card, input, accelerator)) * multiplier;
		  retval = 1;
		}
	  
	      } 
	
	    }

	    if (type == 3) {
	      /* 2 bit optical encoder: phase e.g. EC11 from ALPS */
	
	      if (((iocard[device].inputs[card][input] != iocard[device].inputs_old[card][input]) || 
		   (iocard[device].inputs[card][input+1] != iocard[device].inputs_old[card][input+1]))
		  && (iocard[device].inputs_old[card][input] != INITVAL) && (iocard[device].inputs_old[card][input+1] != INITVAL)) {
		/* something has changed */
	  
		if (verbose > 1) {
		  printf("LIBIOCARDS: Rotary Encoder  Phased Type : device=%i card=%i inputs=%i-%i values=%i %i \n",
			 device,card,input,input+1,iocard[device].inputs[card][input],iocard[device].inputs[card][input+1]);
		}

		/* derive last encoder count */
		obits[0] = iocard[device].inputs_old[card][input];
		obits[1] = iocard[device].inputs_old[card][input+1];
		/* derive new encoder count */
		nbits[0] = iocard[device].inputs[card][input];
		nbits[1] = iocard[device].inputs[card][input+1];
	  
		if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0)) {
		  updown = -1;
		} else if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 1) && (nbits[1] == 1)) {
		  updown = 1;
		} else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1)) {
		  updown = -1;
		} else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 0) && (nbits[1] == 0)) {
		  updown = 1;
		}
	  
		if (updown != 0) {
		  *value = *value + (float) (updown * get_acceleration(device, card, input, accelerator)) * multiplier;
		  retval = 1;
		}
	  
	      } 
	
	    }

	  } else {
	    retval = -1;
	    if (type == 0) {
	      printf("LIBIOCARDS: Invalid input position detected: %i - %i \n", input,input+2);
	    } else {
	      printf("LIBIOCARDS: Invalid input position detected: %i - %i \n", input,input+1);
	    }
	  }

	} /* input encoder value not missing */
      } else {
	retval = -1;
	if (verbose > 0) printf("LIBIOCARDS: Invalid card number detected: %i \n", card);
      }

      if ((type<0) || (type>3)) {
	retval = -1;
	if (verbose > 0) printf("LIBIOCARDS: Invalid encoder type detected: %i \n", type);
      }

    } else {
      retval = -1;
      if (verbose > 1)	printf("LIBIOCARDS: Device either not a MASTERCARD or BU0836X/A Interface or not ready: %i \n", device);
    }

  }

  return(retval);
}

/* retrieve input value from given axis on USB expander, USBServos, DCMotors PLUS or BU0836X/A Interface card */
int axis_input(int device, int input, float *value, float minval, float maxval)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  char device_name3[] = "IOCard-USBServos";
  char device_name4[] = "USBServos v3";
  char device_name5[] = "DCMotors PLUS";
  char device_name6[] = "BU0836X Interface";
  char device_name7[] = "BU0836A Interface";
  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, and -1 if something went wrong */

  if (value != NULL) {

    /* check if we have a connected and initialized Expansion card */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2) ||
	 !strcmp(iocard[device].name,device_name3) || 
	 !strcmp(iocard[device].name,device_name4) || 
	 !strcmp(iocard[device].name,device_name5) || 
	 !strcmp(iocard[device].name,device_name6) || 
	 !strcmp(iocard[device].name,device_name7)) &&
	(iocard[device].status == 1)) {

      if ((input>=0) && (input<iocard[device].naxes)) {
	
	if (iocard[device].axes_old[input] != iocard[device].axes[input]) {
	  /* something changed */
	  *value = ((float) iocard[device].axes[input]) / (float) pow(2,iocard[device].nbits) * 
	    (maxval - minval) + minval;
	  retval = 1;
	  if (verbose > 3) {
	    printf("LIBIOCARDS: Axis                        : device=%i input=%i value=%i %i \n",
		   device, input, iocard[device].axes[input],iocard[device].axes_old[input]);
	  }
	} else {
	  /* nothing changed */
	  if (iocard[device].axes[input] != INITVAL) {
	    *value = ((float) iocard[device].axes[input]) / (float) pow(2,iocard[device].nbits) * (maxval - minval) + minval;
	  }
	  retval = 0;
	}
      } else {
	retval = -1;
	if (verbose > 0) printf("LIBIOCARDS: Invalid axis number %i on device %i detected \n",input,device);
      }
    } else {
      retval = -1;
      if (verbose > 1)	printf("LIBIOCARDS: Device either not a USB Expansion, USBServos, DCMotors PLUS or BU0836X/A Interface card or not ready: %i \n", device);
    }

  }

  return(retval);
}

/* receive USB data from a BU0836X Interface or BU0836A Interface card */
/* containing the data from the analog axes and the buttons of */
/* Leo Bodnar's Joystick card */
/* BU0836X Interface Card: */
/* The configuration of the read data really depends very much on the */
/* configuration of the card. This read script is configured for: */
/* naxes analog inputs and 32 buttons */
/* BU0836A Interface Card: */
/* naxes analog inputs and 32 buttons */
int receive_bu0836(void)
{
  char device_name1[] = "BU0836X Interface";
  char device_name2[] = "BU0836A Interface";
  int device;
  int result = 0;
  int recv_status;
  int buffersize = 32;
  int axis;
  int button;
  int byte;
  int bit;
  int nbutton = 32;
  int card = 0;
  unsigned char recv_data[buffersize];	/* BU0836X/A Interface raw IO data */

  int val;
  int median;
  int noise = 2; // out of 1024
  int temparr[MAX_HIST];
  int h;

  for (device=0;device<MAXDEVICES;device++) {

    /* check if we have a IOCard-USBServos card */
    if ((!strcmp(iocard[device].name,device_name1) || !strcmp(iocard[device].name,device_name2))
	&& (iocard[device].status == 1)) {
      
      /* check whether there is new data on the read buffer */
      do {

	recv_status = read_usb(device,recv_data, buffersize);

	if (recv_status > 0) {

	  if (verbose > 3) printf("LIBIOCARDS: received %i bytes from BU0836X/A Interface \n",recv_status);

	  /* read analog axis (2 bytes per axis) */
	  for (axis=0;axis<iocard[device].naxes;axis++) {
	    
	    val = recv_data[2*axis] + recv_data[2*axis+1]*256;
	    
	    if (verbose > 3) printf("LIBIOCARDS: Device %i Axis %i Value %i \n",device,axis,val);
	    
	    /* Analog input values are flickering with spikes (up to 10 out of a range of 1023)
	       which is because of imprecision of potentiometers and power supply.
	       A change does not necessarily mean that we turned the potentiometer,
	       Here we use a median filter */
      
	    memcpy(temparr,&iocard[device].axes_hist[axis],MAX_HIST*sizeof(int));
	    quicksort(temparr,0,MAX_HIST-1);
	    median = temparr[MAX_HIST/2];
	    
	    //if (axis == 0)
	    //  printf("%i %i %i \n",val,iocard[device].axes_old[axis],iocard[device].axes[axis]);
	    
	    if ((val != INITVAL) && (median != INITVAL)) {
	      /* compare to median of history values */
	      if ((val < (median - noise)) ||
		  (val > (median + noise))) {
		/* save current value */
	        //if (axis == 0) printf("%i %i \n",median,val);
		iocard[device].axes[axis] = val;		
	      }      	  
	    } else {
	      /* initialize current value */
	      iocard[device].axes[axis] = val;		
	    }

	    /* Shift History of analog inputs and update current value */
	    for (h = MAX_HIST-2; h >= 0; h--) {
	      iocard[device].axes_hist[axis][h+1] = iocard[device].axes_hist[axis][h];
	    }
	    iocard[device].axes_hist[axis][0] = recv_data[2*axis] + recv_data[2*axis+1]*256;

	  }
	    
	  for (button=0;button<nbutton;button++) {
	    if (strcmp(iocard[device].serial,"B37271")==0) {
	      /* BU0836X card in my CFY TQ */
	      byte = 2*iocard[device].naxes + 1 + button/8;
	    } else {
	      /* All other BU0836X/A */
	      byte = 2*iocard[device].naxes + button/8;
	    }
	    bit = button - (button/8)*8;
	    iocard[device].inputs[card][button] = (recv_data[byte] >> bit) & 0x01;
	    if (verbose > 2) printf("LIBIOCARDS: Device %i Button %i Value %i %i %i \n",device,button,
				    iocard[device].inputs[card][button], byte, bit);
	  }
	}

      } while (recv_status > 0);
    }

  }

  return result;

}

/* initialize Leo Bodnar's BU0836X Interface card (do not send anything, this card just has an Input descriptor) */
int initialize_bu0836(int device)
{
  char device_name1[] = "BU0836X Interface";
  char device_name2[] = "BU0836A Interface";
  int result = 0;
  int buffersize = 32;

  /* check if we have a connected BU0836X Interface card */
  if ((!strcmp(iocard[device].name,device_name1) || (!strcmp(iocard[device].name,device_name2))) &&
      (iocard[device].status == 0)) {

    /* allocate input buffer */
    result = setbuffer_usb(device,buffersize);

    iocard[device].ncards = 1;
    // iocard[device].naxes = 2; // given in ini file
    iocard[device].noutputs = 0;
    iocard[device].ninputs = 32;
    iocard[device].nservos = 0;
    iocard[device].nmotors = 0;
    iocard[device].ndisplays = 0;
    iocard[device].nbits = 12;

    if (verbose > 0) printf("LIBIOCARDS: Initialized BU0836X/A Interface (device %i) with %i axes \n",
			    device, iocard[device].naxes);

    result = 1;

  }

  return result;
}

/* loop through selected devices and check if some have been connected
   or disconnected. Initialize or free devices as needed */
int initialize_leo(void)
{
  int device;
  int ret;
  char device_none[] = "none";
  int result = 0;

  if (verbose > 0) printf("\n");
  for (device=0;device<MAXDEVICES;device++) {

    if (strcmp(iocard[device].name,device_none) && (iocard[device].status == -1)) {
      if (verbose > 0) {
	printf("\n");
	printf("LIBIOCARDS: Initialize device: %i Name: %s \n",device,iocard[device].name);
      }
	
      ret = check_usb(iocard[device].name,device,iocard[device].vendor,iocard[device].product,
		      iocard[device].bus, iocard[device].address,
		      iocard[device].path, iocard[device].serial);

      if (ret<0) {
	result = ret;
	break;
      } else {
	iocard[device].status = 0;
	
	if (initialize_bu0836(device) == 1) {
	  iocard[device].status = 1;
	} else {
	  if (verbose > 0) printf("LIBIOCARDS: No initialization code for device: %i \n",device);
	  iocard[device].status = 0;
	  result = -1;
	}
      }
    }
  }

  return result;
}

/* Initializes USB driver */
int initialize_usb(void)
{
  int ret;

  /* initialize USB driver */
  ret = init_usb();

  return ret;
}


/* Terminates USB connections */
void terminate_usb(void)
{

  /* stop usb driver */
  exit_usb();

}



