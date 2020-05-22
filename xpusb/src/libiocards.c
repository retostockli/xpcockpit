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
#include "libiocards.h"
#include "handleserver.h"
#include "serverdata.h"

#ifdef LIBUSB
#include "libusb_interface.h"
#endif

#ifdef HIDAPI
#include "hidapi_interface.h"
#endif

extern float roundf(float);

/* this routine prints the GNU license information */
void print_license(void)
{
  printf("USBIOCARDS Copyright (C) 2009-2013 Reto Stockli\n");
  printf("\n");
  printf("Analog axes code added 2011 by Hans Jansen\n");
  printf("A320 modules Copyright (C) 2013-2016 Hans Jansen\n");
  printf("\n");
  printf("This program comes with ABSOLUTELY NO WARRANTY\n");
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under the terms of the GNU General Public License as published by\n");
  printf("the Free Software Foundation, either version 3 of the License, or\n");
  printf("any later version. For details see http://www.gnu.org/licenses/gpl.html.\n");
  printf("\n");

}

/* this routine parses the usbiocards.ini file and reads its values */
int read_ini(char ininame[])
{
  int ret = 0;
  int device;
  char sdevice[50];
  dictionary *ini;
  char filename[255];
  char cwd[200];
  char *pch;

  char default_server_ip[] = "127.0.0.1";
  int default_server_port = 8091;	// port 8090 traditionally used by other SIOC plugins, like uipcx!
  int default_verbose = 0;

  /* default IOCard values (not connected) */
  char default_name[] = "none";
  uint16_t default_vendor = 0xFFFF;
  uint16_t default_product = 0xFFFF;
  uint8_t default_bus = 0xFF;
  uint8_t default_address = 0xFF;
  char default_path[] = "";
  // unsigned char default_ep = 0xFF;
  int default_ncards = 0;
  int default_naxes = 0;
  int default_nbits = 8;
  int default_ninputs = 0;
  int default_noutputs = 0;
  int default_nmotors = 0;
  int default_nservos = 0;
  int default_status = -1;

  memset(server_ip,0,sizeof(server_ip));

  /* check if we are in the source code directory or in the binary installation path */
  if (getcwd(cwd, sizeof(cwd)) == NULL) return -2;
#ifdef WIN
  pch = strstr (cwd,"\\bin");
#else
  pch = strstr (cwd,"/bin");
#endif
  if (pch == NULL) {
    sprintf(filename,"../inidata/%s.ini",ininame);
  } else {
    sprintf(filename,"../share/xpusb/%s.ini",ininame);
  }

  ini = iniparser_load(filename);

  if (ini != NULL) {

    strcpy(server_ip,iniparser_getstring(ini,"network:Server Address", default_server_ip));
    server_port = iniparser_getint(ini,"network:Server Port", default_server_port);

    verbose = iniparser_getint(ini,"general:verbose", default_verbose);
    printf("X-Plane Server Plugin Address:Port is %s:%i \n",server_ip, server_port);
    
    /* Set up the client name for the server */
    strncpy(clientname,iniparser_getstring(ini,"general:Name", ""),sizeof(clientname));
    if (! strcmp(clientname,"")) strncpy(clientname,PACKAGE_NAME,sizeof(clientname));
    printf("Usbiocards Client name: %s\n", clientname);

    /* read IOCard connection list */
    printf("\n");
    printf("User-defined Devices: \n");
    for(device=0;device<MAXDEVICES;device++) {
      memset(iocard[device].name,0,sizeof(iocard[device].name));
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Name",device);
      strcpy(iocard[device].name,iniparser_getstring(ini,sdevice, default_name));
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Vendor",device);
      iocard[device].vendor = iniparser_getint(ini,sdevice, default_vendor);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Product",device);
      iocard[device].product = iniparser_getint(ini,sdevice, default_product);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Bus",device);
      iocard[device].bus = iniparser_getint(ini,sdevice, default_bus);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Address",device);
      iocard[device].address = iniparser_getint(ini,sdevice, default_address);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Path",device);
      strcpy(iocard[device].path,iniparser_getstring(ini,sdevice, default_path));
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Ncards",device);
      iocard[device].ncards = iniparser_getint(ini,sdevice, default_ncards);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Naxes",device);
      iocard[device].naxes = iniparser_getint(ini,sdevice, default_naxes);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Nbits",device);
      iocard[device].nbits = iniparser_getint(ini,sdevice, default_nbits);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Ninputs",device);
      iocard[device].ninputs = iniparser_getint(ini,sdevice, default_ninputs);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Noutputs",device);
      iocard[device].noutputs = iniparser_getint(ini,sdevice, default_noutputs);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Nmotors",device);
      iocard[device].nmotors = iniparser_getint(ini,sdevice, default_nmotors);
      memset(sdevice,0,sizeof(sdevice));
      sprintf(sdevice,"iocard:device%i:Nservos",device);
      iocard[device].nservos = iniparser_getint(ini,sdevice, default_nservos);

      iocard[device].status = default_status;
      
      if (!strcmp(iocard[device].name,default_name)) {
	// printf("Device %i NO \n",device);
      } else {
	printf("Device %i: \n",device);
	printf("Name %s \n",iocard[device].name);
	printf("Vendor ID %i Product ID %i \n",iocard[device].vendor,iocard[device].product);
	//	printf("USB Bus %i USB Address %i \n",iocard[device].bus,iocard[device].address);
	printf("USB Path %s \n",iocard[device].path);
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
  t2 = iocard[device].time_enc[card][input];
  
  dt = - ((t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0);

  if (dt > mintime) {

    acceleration = 1 + (int) ( (maxtime / dt) * (float) accelerator);

    iocard[device].time_enc[card][input] = t1;
    
  } else {
    /* likely a switch bounce during change, do not count! */

    acceleration = 0;
    
  }

  if (verbose > 2) {
    printf("LIBIOCARDS: difference=%f [seconds], acceleration=%f for input=%i device=%i card=%i\n", 
	   dt, acceleration,input,device,card);
  }

  return acceleration;
}

/* initialize data arrays with default values */
/* flight data for  USB and TCP/IP communication */
int initialize_iocardsdata(void) 
{
  int device;
  int count;
  int card;
  struct timeval time_new;

  /* set initialize stage */
  initial = 1;
  printf("LIBIOCARDS: Initialization started \n");	

  /* get new time */
  gettimeofday(&time_new,NULL);

  /* reset all inputs, axes, outputs and displays */
  for (device=0;device<MAXDEVICES;device++) {
    //printf("%i %s %i \n",device,iocard[device].name,iocard[device].naxes);
    for (card=0;card<iocard[device].ncards;card++) {
      for (count=0;count<iocard[device].ninputs;count++) {
	iocard[device].inputs[card][count]=0;
	iocard[device].inputs_old[card][count]=-1;
	iocard[device].time_enc[card][count] = time_new;
	iocard[device].inputs_read[card][count] = 0;
      }
      for (count=0;count<iocard[device].noutputs;count++) {
	iocard[device].outputs[card][count]=0;
	iocard[device].outputs_old[card][count]=-1;
      }
      for (count=0;count<iocard[device].ndisplays;count++) {
	iocard[device].displays[card][count]=0;
	iocard[device].displays_old[card][count]=-1;
      }
    }
    for (count=0;count<iocard[device].naxes;count++) {
      iocard[device].axes[count]=-1;
      iocard[device].axes_old[count]=-1;
    }
    for (count=0;count<iocard[device].nservos;count++) {
      iocard[device].servos[count]=SERVOPARK;
      iocard[device].servos_old[count]=-1;
      iocard[device].time_servos[count] = FLT_MISS;
    }
    for (count=0;count<iocard[device].nmotors;count++) {
      iocard[device].motors[count]=MOTORPARK;
      iocard[device].motors_old[count]=-1;
    }
  }

  return 0;
}

/* saves a copy of all IOCARDS I/O states */
/* this is needed because, at each step, only the modified values */
/* are communicated either via TCP/IP or USB to X-Plane and IOCARDS */
int copy_iocardsdata(void)
{
  int device;
  int count;
  int card;
 
  for (device=0;device<MAXDEVICES;device++) {

    /* only copy new to old input data if the input data was read at least once */
    /* else keep the new input data */
    
    for (card=0;card<iocard[device].ncards;card++) {
	for (count=0;count<iocard[device].ninputs;count++) {
	  if (iocard[device].inputs_read[card][count] == 1) {
	    iocard[device].inputs_old[card][count] = iocard[device].inputs[card][count];
	    iocard[device].inputs_read[card][count] = 0;
	  }
	}
	for (count=0;count<iocard[device].noutputs;count++) {
	  iocard[device].outputs_old[card][count]=iocard[device].outputs[card][count];
	}
	for (count=0;count<iocard[device].ndisplays;count++) {
	  iocard[device].displays_old[card][count]=iocard[device].displays[card][count];
	}
    }
    /*
    for (count=0;count<iocard[device].naxes;count++) {
      iocard[device].axes_old[count]=iocard[device].axes[count];
    }
    for (count=0;count<iocard[device].nservos;count++) {
      iocard[device].servos_old[count]=iocard[device].servos[count];
    }
    for (count=0;count<iocard[device].nmotors;count++) {
      iocard[device].motors_old[count]=iocard[device].motors[count];
      } */
    if (iocard[device].naxes>0) {
      memcpy(iocard[device].axes_old,iocard[device].axes,sizeof(iocard[device].axes));
    }
    if (iocard[device].nservos>0) {
      memcpy(iocard[device].servos_old,iocard[device].servos,sizeof(iocard[device].servos));
    }
    if (iocard[device].nmotors>0) {
      memcpy(iocard[device].motors_old,iocard[device].motors,sizeof(iocard[device].motors));
    }


  }

  //  printf("%i %i %i %i \n",iocard[7].axes[0],iocard[7].axes[1],iocard[7].axes[2],sizeof(iocard[7].axes));

  /* finalized initial USB data retrieval */
  if (initial == 1) {
    printf("LIBIOCARDS: Initialization completed \n");	  
    initial = 0;

    /* set all MASTERCARD inputs to 0 which have not been explicitly 
       set to 0 or 1 by the first read of MASTERCARD initial values */   
    /*
    for (device=0;device<MAXDEVICES;device++) {
      for (card=0;card<iocard[device].ncards;card++) {
	for (count=0;count<iocard[device].ninputs;count++) {
	  if (iocard[device].inputs[card][count] == -1) iocard[device].inputs[card][count] = 0 ;
	  if (iocard[device].inputs_old[card][count] == -1) iocard[device].inputs_old[card][count] = 0 ;
	}
      }
    }
    */
    
  }

  return(0);

}

/* fill output value for given output position on MASTERCARD or on DCMotor PLUS Card
   Argument 'card' is not used for DCMotor PLUS Card since it specifies the MASTERCARD
   number for the USB Expansion card*/
int digital_output(int device, int card, int output, int *value)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  char device_name3[] = "DCMotors PLUS";
  int retval = 0;
  int firstoutput; /* On Mastercard output channels start at 11, and go to 55 */

  if (value != NULL) {

    /* check if we have a connected and initialized mastercard */
    if ((!strcmp(iocard[device].name,device_name1) || 
	 !strcmp(iocard[device].name,device_name2) || 
	 !strcmp(iocard[device].name,device_name3)) &&
	(iocard[device].status == 1)) {

      if (*value != INT_MISS) {
	
	if (!strcmp(iocard[device].name,device_name1) || 
	    !strcmp(iocard[device].name,device_name2)) {
	  firstoutput = 11;
	} else {
	  firstoutput = 0;
	}

	if ((card >=0) && (card<iocard[device].ncards)) {
	  if ((output >= firstoutput) && (output <= (firstoutput+iocard[device].noutputs))) {
	    if (*value >= 1) {
	      iocard[device].outputs[card][output-firstoutput]=1;
	    } else if (*value == 0) {
	      iocard[device].outputs[card][output-firstoutput]=0;
	    } else {
	      retval = -1;
	    }
	  } else {
	    retval = -1;
	    if (verbose > 0) printf("LIBIOCARDS: Invalid output position detected: %i \n", output);
	  }
	} else {
	  retval = -1;
	  if (verbose > 0) printf("LIBIOCARDS: Invalid card number detected: %i \n", card);
	}
      }

    } else {
      retval = -1;
      if (verbose > 1) printf("LIBIOCARDS: Device not a MASTERCARD, DCMotors PLUS card or not ready: %i \n", 
			      device);
    }

  }

  return (retval);
}

/* retrieve input value from given input position on MASTERCARD or BU0836X/A Interface */
/* Two types : */
/* 0: pushbutton */
/* 1: toggle switch */
int digital_input(int device, int card, int input, int *value, int type)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  char device_name3[] = "BU0836X Interface";
  char device_name4[] = "BU0836A Interface";
  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, and -1 if something went wrong */

  if (value != NULL) {

    /* check if we have a connected and initialized device */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2) ||
	 !strcmp(iocard[device].name,device_name3) ||
	 !strcmp(iocard[device].name,device_name4)) 
	&& (iocard[device].status == 1)) {

      if ((card >=0) && (card<iocard[device].ncards)) {
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
		*value = 1 - (*value);
		retval = 1;
		if (verbose > 1) {
		  printf("LIBIOCARDS: Toggle Switch               :  device=%i card=%i input=%i value=%i \n",
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
	if (verbose > 0) printf("LIBIOCARDS: Invalid card number detected: %i \n", card);
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
		    (iocard[device].inputs_old[card][input] != -1) && 
		    (iocard[device].inputs_old[card][input+1] != -1) &&
		    (iocard[device].inputs_old[card][input+2] != -1)) {
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
		  && (iocard[device].inputs_old[card][input] != -1) && (iocard[device].inputs_old[card][input+1] != -1)) {
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
		  && (iocard[device].inputs_old[card][input] != -1) && (iocard[device].inputs_old[card][input+1] != -1)) {
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
		  && (iocard[device].inputs_old[card][input] != -1) && (iocard[device].inputs_old[card][input+1] != -1)) {
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

/* fill display at given output position on MASTERCARD */
int mastercard_display(int device, int card, int pos, int n, int *value, int hasspecial)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  int retval = 0;
  int single;
  int count;
  int power;
  int tempval;
  int negative = 0;
  int nt;

  /* special value codes for display II card (set hasspecial to 1)
     10 = Blank the digit			--> 0xf7 (also 0x0a)
     11 = Put the "-" sign			--> 0xf8
     12 = Put a Special 6			--> 0xf9
     13 = Put a "t"					--> 0xfa
     14 = Put a "d"					--> 0xfb
     15 = Put a "_" (Underscore)	--> 0xfc (does not work; rather, has something to do with dimming...)
  */

  //printf ("pos = %i, length = %i, value = %i\n", pos, n, *value);
  
  if (value != NULL) {

    /* check if we have a connected and initialized mastercard */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2)) &&
	(iocard[device].status == 1)) {

      if ((card >=0) && (card<iocard[device].ncards)) {
	if ((pos >=0) && ((pos+n)<=iocard[device].ndisplays)) {
 
	  if (*value != INT_MISS) {
	    
	    /* generate temporary storage of input value */
	    tempval = *value;

	    nt = n;
	    
	    /* handle special codes */
	    if (hasspecial) {
	      for (count=nt-1;count>=0;count--) {
		if (tempval == 10) {
		  // blank digit
		  //iocard[device].displays[card][count+pos] = 0xf7;
		  iocard[device].displays[card][count+pos] = 0x0a;
		} else if (tempval == 11) {
		  // - sign
		  iocard[device].displays[card][count+pos] = 0xf8;
		} else if (tempval == 12) {
		  // b
		  iocard[device].displays[card][count+pos] = 0xf9;
		} else if (tempval == 13) {
		  // t
		  iocard[device].displays[card][count+pos] = 0xfa;
		} else if (tempval == 14) {
		  // d
		  iocard[device].displays[card][count+pos] = 0xfb;
		} else if (tempval == 15) {
		  // change intensity
		  iocard[device].displays[card][count+pos] = 0xfc;
		} else if ((tempval >= 20) && (tempval <= 50)) {
		  // intensity value: 0-15 (use special value 20-35
		  iocard[device].displays[card][count+pos] = tempval-20;
		}
	      }
	    } else {
	      /* normal processing */

	      /* reverse negative numbers: find treatment for - sign */
	      /* use first digit for negative sign */
	      if (tempval < 0) {
		tempval = -tempval;
		negative = 1;
	      }
      
	      /*cut high values */
	      power = roundf(pow(10,nt));
	      if (tempval >= power) {
		tempval = tempval - (tempval / power) * power;
	      }
		  
	      /* read individual digits from integer */
	      /* blank leftmost 0 displays except the first one */ 
	      int firstpos = -1;
	      for (count=nt-1;count>=0;count--) {
		power = roundf(pow(10,count));
		single = tempval / power;
		tempval = tempval - single * power;
		if ((firstpos < 0) && (single == 0) && (count != 0)) {
		  /* blank display #count */
		  iocard[device].displays[card][count+pos] = 0x0a;
		} else {
		  /* print display #count */
		  if (firstpos < 0) firstpos = count;
		  iocard[device].displays[card][count+pos] = single;
		}
		// printf("count: %i, power: %i, tempval: %i, digit: %i\n", count, power, tempval, single);
	      }
	   
	      if (firstpos != (nt-1)) {
		/* deal with negative sign if it is not left to the leftmost display */
		if (negative == 1) {
		  /* put minus sign in the leftmost display */
		  iocard[device].displays[card][pos+firstpos+1] = 0xf8;
		}
	      }

	    } // end "normal processing"
	  }

	} else {
	  retval = -1;
	  if (verbose > 0) printf("LIBIOCARDS: Invalid display range detected: %i - %i \n",pos,pos+n);
	}
      } else {
	retval = -1;
	if (verbose > 0) printf("LIBIOCARDS: Invalid card number detected: %i \n", card);
      }

    } else {
      retval = -1;
      if (verbose > 1) printf("LIBIOCARDS: Device either not a MASTERCARD or not ready: %i \n", device);
    }

  }

  return (retval);
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
	  if (iocard[device].axes[input] != -1) {
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

/* read single key from IOCardS-Keys card */
/* function returns 1 if key is pressed or 0 if not */
int keys_input(int device, int key)
{
  char device_name[] = "IOCardS-Keys";
  int retval = 0;
  int card = 0;

  /* check if we have a connected and initialized IOCardS-Keys */
  if (!strcmp(iocard[device].name,device_name) && (iocard[device].status == 1)) {    
    if ((key>=0) && (key<iocard[device].ninputs)) {
      if (iocard[device].inputs[card][key] == 1) {
	retval = 1;
	if (verbose > 1) printf("LIBIOCARDS: Key Pressed                 :  device=%i key=%i \n",device, key); 

	/* reset key */
	iocard[device].inputs[card][key] = 0;
      }
    } else {
      retval = -1;
      if (verbose > 0) printf("LIBIOCARDS: Invalid key number supplied to keys_input: %i \n", key);
    }   
  } else {
    retval = -1;
    if (verbose > 1) printf("LIBIOCARDS: Device either not a IOCardS-Keys or not ready: %i \n", device);
  }

  return (retval);
}

/* output value to control servo motor 
   Define a minimum and a maximum output value (minval, maxval) 
   with corresponding servo minimum and maximum value (servominval, servomaxval)
   Servo minimum and maximum values can go from SERVOMIN to SERVOMAX.
   If below SERVOMIN or above SERVOMAX, the servo is parked.
   In parking position the servo is not active and the servo wheel can be freely
   turned even when the power is connected */
int servos_output(int device, int servo, float *value, float minval, float maxval,
		  int servominval, int servomaxval)
{
  char device_name1[] = "IOCard-USBServos";
  char device_name2[] = "USBServos v3";
  char device_name3[] = "DCMotors PLUS";
  int retval = 0;
  int data;           /* integer servo value */
 
  /* check if we have a connected and initialized USBServos or DCMotors PLUS card */
  if ((!strcmp(iocard[device].name,device_name1) ||
       !strcmp(iocard[device].name,device_name2) ||
       !strcmp(iocard[device].name,device_name3)) && (iocard[device].status == 1)) {    
 
    if (value != NULL) {

      if ((servo>=0) && (servo<iocard[device].nservos)) {
	if (*value != FLT_MISS) {
	  
	  /* scale input data to servo data range */
	  data = (int) ((*value - minval)/(maxval - minval)*
			((float) (servomaxval - servominval)) + (float) servominval);
	  if (data < SERVOMIN) data = SERVOPARK;
	  if (data > SERVOMAX) data = SERVOPARK;
	  
	  /* update servo internal data array for sending */
	  if (data != iocard[device].servos[servo]) {	  
	    iocard[device].servos[servo] = data;
	    if (verbose > 1) printf("LIBIOCARDS: Device %i Servo %i has value %i \n",
				    device, servo, data);	  
	  }
	    
	}
      } else {
	retval = -1;
	if (verbose > 0) printf("LIBIOCARDS: Servo number outside allowed range: %i \n", servo);
      }
    }
    
  } else {
    retval = -1;
    if (verbose > 1) printf("LIBIOCARDS: Device not a IOCard-USBServos, DCMotors PLUS or not ready: %i \n", device);
  }

  return (retval);
}

/* output value to control servo motor */
/* the motor speed can go from -range to +range */
/* In parking position (outside of +/-range or at 0) */
/* the motor is not active and the motor wheel can be freely  */
/* turned even when the power is connected */
int motors_output(int device, int motor, float *value, float range)
{
  char device_name[] = "DCMotors PLUS";
  int retval = 0;
  int data;           /* integer motor value */
  int motor_range = 127;
 
  /* check if we have a connected and initialized DCMotors PLUS */
  if (!strcmp(iocard[device].name,device_name) && (iocard[device].status == 1)) {    
 
    if (value != NULL) {

      if ((motor>=0) && (motor<iocard[device].nmotors)) {

	if (*value != FLT_MISS) {
	
	  /* scale input data to motor data range */
	  if ((*value < (-range-FLT_EPSILON)) || (*value > (range+FLT_EPSILON)) || (fabs(*value) < FLT_EPSILON)) {
	    data = MOTORPARK;
	  } else {
	    if (*value > 0.0) {
	      data = (int) (*value * (float) motor_range / range);
	    } else {
	      data = (int) (-1.0 * *value * (float) motor_range / range) + motor_range + 1;
	    }
	  }
	  /* update motor internal data array for sending */
	  if (data != iocard[device].motors[motor]) {	  
	    iocard[device].motors[motor] = data;
	    if (verbose > 2) printf("LIBIOCARDS: Device %i Motor %i has value %i \n",device, motor, data);	  
	  }

	}

      } else {
	retval = -1;
	if (verbose > 0) printf("LIBIOCARDS: Motor number outside allowed range: %i \n", motor);
      }
      
    }

  } else {
    retval = -1;
    if (verbose > 1) printf("LIBIOCARDS: Device either not a IOCard-USBMotors or not ready: %i \n", device);
  }

  return (retval);
}

/* send changes in the outputs array (outputs and displays) to MASTERCARD */
/* MASTERCARD is connected to USB EXPANSION CARD */
int send_mastercard(void)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  int device;
  int result = 0;
  int buffersize = 8;
  unsigned char send_data[] = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
  int send_status = 0;
  int count;
  int firstoutput = 11; /* output channels start at 11, and go to 55 */
  int totchannels = 64; /* total channels per card (second card starts at 64(+11), 3rd at 128(+11), 4th at 192+11))*/
  int channelspersegment = 8; // each segment can hold 8 outputs (and one display)
  int slotspercycle = 4; // each cycle can hold 4 slots (1 byte with address + 1 byte with values)
  int card;
  int channel;
  int changed;
  int changedinsegment;
  int segment; /* we have to write full segments of 8 outputs each */
  int slot;

  for (device=0;device<MAXDEVICES;device++) {

    /* check if we have a connected and initialized mastercard */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2)) &&
	(iocard[device].status == 1)) {
	
      for (card=0;card<iocard[device].ncards;card++) {

	/* fill send data with output information */
	slot = 0;
	changed = 0;
	memset(send_data,0xff,sizeof(send_data));
	for (segment=0;segment<(totchannels/channelspersegment);segment++) {

	  changedinsegment = 0;
	  
	  if ((segment*channelspersegment) < iocard[device].noutputs) {
	    /* 8-byte segment transfers */
	    for (count=0;count<channelspersegment;count++) {

	      channel = segment*channelspersegment + count;

	      if (channel < iocard[device].noutputs) {
		if (iocard[device].outputs[card][channel] != iocard[device].outputs_old[card][channel]) {
		  changed = 1;
		  changedinsegment = 1;
		}
		if (count == 0) {
		  send_data[slot*2+1] = iocard[device].outputs[card][channel]*pow(2,count);
		} else {
		  send_data[slot*2+1] += iocard[device].outputs[card][channel]*pow(2,count);
		}
	      } else {
		if (count == 0) {
		  send_data[slot*2+1] = pow(2,count);
		} else {
		  send_data[slot*2+1] += pow(2,count);
		}
	      }
	    }

	    if (changedinsegment == 1) {

	      send_data[slot*2] = card*totchannels + segment*channelspersegment + firstoutput;
	      changedinsegment = 0;
	      slot++;
	      
	      if (verbose > 2) {
		for (count = 0;count<channelspersegment;count++){	      
		  channel = count + segment*channelspersegment;
		  if (channel < iocard[device].noutputs) {
		    printf("LIBIOCARDS: send output to MASTERCARD device=%i card=%i output=%i value=%i \n",
			   device, card, firstoutput+channel, iocard[device].outputs[card][channel]);
		  }
		}
	      }
	    }
	  }
	  
	  if ((slot == slotspercycle) ||
	      ((changed == 1) && (segment==(totchannels/channelspersegment-1)))) {
	    send_status = write_usb(device, send_data, buffersize);
	    if ((send_status) < 0) {
	      result = send_status;
	      break;
	    }
	    
	    slot = 0;
	    changed = 0;
	    memset(send_data,0xff,sizeof(send_data));
	  }
	}

	/*
	for (segment=0;segment<(totchannels/channelspersegment);segment++) {

	  changed = 0;

	  if ((segment*channelspersegment) < iocard[device].noutputs) {

	    memset(send_data,0xff,sizeof(send_data));
	    for (count=0;count<channelspersegment;count++) {

	      channel = segment*channelspersegment + count;

	      if (channel < iocard[device].noutputs) {
		if (iocard[device].outputs[card][channel] != iocard[device].outputs_old[card][channel]) {
		  changed = 1;
		}
		if (count == 0) {
		  send_data[1] = iocard[device].outputs[card][channel]*pow(2,count);
		} else {
		  send_data[1] += iocard[device].outputs[card][channel]*pow(2,count);
		}
	      } else {
		if (count == 0) {
		  send_data[1] = pow(2,count);
		} else {
		  send_data[1] += pow(2,count);
		}
	      }
	    }

	    if (changed == 1) {

	      send_data[0] = card*totchannels + segment*channelspersegment + firstoutput;
	      
	      send_status = write_usb(device, send_data, buffersize);
	      if ((send_status) < 0) {
		result = send_status;
		break;
	      }
	      
	      if (verbose > 2) {
		for (count = 0;count<channelspersegment;count++){	      
		  channel = count + segment*channelspersegment;
		  if (channel < iocard[device].noutputs) {
		    printf("LIBIOCARDS: send output to MASTERCARD device=%i card=%i output=%i value=%i \n",
			   device, card, firstoutput+channel, iocard[device].outputs[card][channel]);
		  }
		}
	      } 
	    }
	  }
	}
	*/
	
	/* fill send data with display information */
	slot = 0;
	changed = 0;
	for (count=0;count<iocard[device].ndisplays;count++) {
	  if (iocard[device].displays[card][count] != iocard[device].displays_old[card][count]) {
	    changed = 1;
	    if (slot == 0) {
	      memset(send_data,0xff,sizeof(send_data));
	    }
	    send_data[slot*2] = iocard[device].displays[card][count];
	    send_data[slot*2+1] = card*iocard[device].ndisplays + count;
	    slot++;

	    if (verbose > 2) {
	      printf("LIBIOCARDS: send display to MASTERCARD device=%i card=%i display=%i value=%i \n",
		     device, card, count, iocard[device].displays[card][count]);
	    }
	    
	  }
	  if ((slot == slotspercycle) ||
	      ((changed == 1) && (count == (iocard[device].ndisplays-1)))) {
	    send_status = write_usb(device,send_data, buffersize);
	    if ((send_status) < 0) {
	      result = send_status;
	      break;
	    }
	    changed = 0;
	    slot = 0;
	  }
	}
	/*
	for (count=0;count<iocard[device].ndisplays;count++) {
	  if (iocard[device].displays[card][count] != iocard[device].displays_old[card][count]) {

	    memset(send_data,0xff,sizeof(send_data));
	    send_data[0] = iocard[device].displays[card][count];
	    send_data[1] = card*iocard[device].ndisplays + count;
	
	    send_status = write_usb(device,send_data, buffersize);
	    if ((send_status) < 0) {
	      result = send_status;
	      break;
	    }

	    if (verbose > 2) {
	      printf("LIBIOCARDS: send display to MASTERCARD device=%i card=%i display=%i value=%i \n",
		     device, card, count, iocard[device].displays[card][count]);
	    }
	  }
	}
	*/

	if (result < 0) break;

      } // cards on master card

    } // master card?

  } // devices
  return result;
}

/* updates the inputs array with new data from MASTERCARD */
/* MASTERCARD is connected to USB EXPANSION CARD */
int receive_mastercard(void)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  int result = 0;
  int recv_status;
  int input[8][8]; /* 8 byte bit storage */
  int byte;        /* byte count */
  int bit;         /* bit count within byte */
  int index;       /* mastercard input number */
  int device;      /* device number */
  int card;	   /* mastercard counter: maximum 4 MAXMASTERCARDS per USB expansion card */
  int slot;        /* slot counter (each mastercard has 8 slots with 9 inputs) */
  int axis;	   /* number of active A/D converter */

  int found;       /* found the next slot to be read */
  int readleft;    /* yet to be read slot data for first 8 bits */
  int readnine;    /* read slot data for last 9th bit in this byte */
  int sumnine;     /* cumulated read slot data for last 9th bit */
  int sumslots;	   /* maximum number of 9bit slots present in this transmission */
  int sumcards;    /* maximum number of cards present in this transmission */

  char x;
  int buffersize = 8;
  unsigned char recv_data[buffersize];	/* mastercard raw IO data */

  /* PROTOCOL */
  /* Each Mastercard has 72 Inputs, distributed over 8 slots with 9 inputs
     The first 8 inputs of each slot are read first, followed by an optional vector of the 9th input for each slot
     The Mastercard No. is given in the first byte (input 0-3)
     If any slots have inputs set to 1, these slots are set to 1 in byte 1
     The data of the present slots will then follow in the next bytes (bytes 2-7) 
     If there are more slots than fit into a single message, a followup message must be read; such a message is
     identified by bit 7 of byte 0 being set.
     Note: if any A/D converters are turned on, by a "naxes" item in the usbiocards.ini file, this is reflected
     in the bits 4-6 of the first byte (1x, 2x, 3x or 4x giving the number of the currently reporting converter).
     In this case, the byte 1 is the output of the A/D converter, and the slot bits are moved to byte 2. 
     Data of the present slots will then follow in bytes 3-7.
     Note also that the A/D converters will report continually, every 10msec or so, not only on value change!
  */
  
  for (device=0;device<MAXDEVICES;device++) {

    /* check if we have a connected USB expander card */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2)) &&
	(iocard[device].status == 1)) {
      
      do {
      
      /* check whether there is new data on the read buffer */
      recv_status = read_usb(device,recv_data, buffersize);

      if (recv_status > 0) {

	if (verbose > 2) printf("LIBIOCARDS: received %i bytes from MASTERCARD \n",recv_status);

	/* fill the input array by bitshifting the first eight bytes */
	for (byte=0;byte<8;byte++) {
	  x = recv_data[byte];
	  for (bit=0;bit<8;bit++) {
	    if ( x & 01) {
	      input[byte][bit]=1;
	    } else {
	      input[byte][bit]=0;
	    }
	    x = x >> 1;    
	  }
	  if (verbose > 3) {
	    printf("LIBIOCARDS: Received %i %i %i %i %i %i %i %i \n",input[byte][7], \
		   input[byte][6],input[byte][5],input[byte][4],input[byte][3],input[byte][2], \
		   input[byte][1],input[byte][0]);
	  }
	}

	/* examine first byte: bits 0-3: Mastercard, 4-6: A/D, 7: continuation */

	/* determine whether an A/D converter is active and read its value */
	axis = (recv_data[0] >> 4) & 7;	/* extract the A/D number from bits 6-4 */

	if (axis > 0) {
	  if (verbose > 3) printf("LIBIOCARDS: Axis %i reports %i \n", axis, recv_data[1]);
	  /* suppress noise +/-1 in axis input to avoid high frequency flipping of input */
	  if ((abs(recv_data[1] - iocard[device].axes_old[axis-1]) > 1) ||
	      (recv_data[1] == 0) || (recv_data[1] == 255)) {
	    iocard[device].axes[axis-1] = recv_data[1];
	  }
	}

	if (axis == 0) {
	  /* no analog axis value: start slot indicator at second byte */
	  byte = 1;
	} else {
	  /* axis value present in second byte: start slot indicator at third byte */
	  byte = 2;
	}

	if (input[0][7] == 0) {
	  /* OPTION #1 */
	  /* new transmission (first 8 byte packet) */

	  /* clean slot index data */
	  for (card=0;card<MAXMASTERCARDS;card++) {
	    for (slot=0;slot<8;slot++) {
	      iocard[device].slotdata[card][slot] = 0;
	    }
	  }

	  /* fill slot index data with slot indices of first transmission */
	  sumslots = 0;	  
	  sumcards = 0;
	  for (card=0;card<MAXMASTERCARDS;card++) {
	    if (input[0][card] == 1) {
	      for (slot=0;slot<8;slot++) {
		if (input[byte+sumcards][slot] == 1) {
		  iocard[device].slotdata[card][slot] = 1;
		  sumslots++;
		}
	      }
	      sumcards++;
	    } 
	  }

	  if (verbose > 3) {
	    printf("LIBIOCARDS: new transmission with %i cards and %i slots present. \n",sumcards,sumslots);
	  
	    for (card=0;card<MAXMASTERCARDS;card++) {
	      printf("card %i slots: %i %i %i %i %i %i %i %i \n",card,
		     iocard[device].slotdata[card][7],
		     iocard[device].slotdata[card][6],
		     iocard[device].slotdata[card][5],
		     iocard[device].slotdata[card][4],
		     iocard[device].slotdata[card][3],
		     iocard[device].slotdata[card][2],
		     iocard[device].slotdata[card][1],
		     iocard[device].slotdata[card][0]);
	    }
	  }
	  
	  /* augment byte count to position of first data packet */
	  byte += sumcards;

	} else {
	  /* OPTION #2 */
	  /* continuation of previous transmission (another 8 byte packet) */

	  if (verbose > 3) printf("LIBIOCARDS: continuation transmission. \n");

	}

	while ((byte<8) && (byte>=0)) {

	  /* read slotwise input data for first 8 bits of each slot */
	  found = 0;
	  readleft = 0;
	  for (card=0;card<MAXMASTERCARDS;card++) {
	    for (slot=0;slot<8;slot++) {
	      if (iocard[device].slotdata[card][slot] == 1) {
		readleft++;
		if (!found) {
		  found = 1;
		  iocard[device].slotdata[card][slot] = 2;
		  if (card<iocard[device].ncards) {
		    for (bit=0;bit<8;bit++) {
		      index = 9*slot + bit;
		      iocard[device].inputs[card][index] = input[byte][bit];
		      if (verbose > 3)
			printf("LIBIOCARDS: device=%i card=%i byte=%i slot=%i input=%i value=%i \n",
					      device, card, byte, slot, index, input[byte][bit]);
		    }
		  } else {
		    if (verbose > 0) printf("LIBIOCARDS: device %i, card %i sent input but is not defined.\n", device, card);
		  }
		}
	      }
	    }
	  }
	      
	  /* read slotwise input data for last 9th bit of each slot */
	  if (readleft == 0) {
	    readnine = 0;
	    sumnine = 0;
	    for (card=0;card<MAXMASTERCARDS;card++) {
	      for (slot=0;slot<8;slot++) {
		if (iocard[device].slotdata[card][slot] == 3) {
		  sumnine++;
		}
		if (iocard[device].slotdata[card][slot] == 2) {
		  if (readnine < 8) {
		    iocard[device].slotdata[card][slot] = 3;
		    if (card<iocard[device].ncards) {
		      bit = (sumnine + readnine) % 8; /* present slots fill up subsequent bytes with their 9th bit data */
		      index = 9*slot + 8;
		      iocard[device].inputs[card][index] = input[byte][bit];
		      if (verbose > 3)
			printf("LIBIOCARDS: device=%i card=%i byte=%i slot=%i input=%i value=%i \n",
			       device, card, byte, slot, index, input[byte][bit]);
		    }
		  }
		  readnine++;
		}
	      }
	    }
	  }

	  /* next byte */
	  byte++;
	}
	
      }
      
      } while (recv_status > 0);

      result = recv_status;

    } /* is a master card */

  } /* device loop */

  return result;

}

int receive_keys(void)
{
  char device_name[] = "IOCardS-Keys";
  int device;
  int result = 0;
  int recv_status;
  int buffersize = 8;
  int byte;
  int card = 0;
  unsigned char recv_data[buffersize];	/* IOCardS-Keys raw IO data */

  for (device=0;device<MAXDEVICES;device++) {

    /* check if we have a IOCardS-Keys card */
    if (!strcmp(iocard[device].name,device_name) && (iocard[device].status == 1)) {

      do {
      
	/* check whether there is new data on the read buffer */
	recv_status = read_usb(device,recv_data, buffersize);
	
	if (recv_status > 0) {
	  
	  if (verbose > 2) printf("LIBIOCARDS: received %i bytes from IOCardS-Keys \n",recv_status);
	  
	  /* evaluate all 8 bytes for checking if many keys are pressed simultaneously */
	  /* CHECK: does this really work and how many keys can be pressed simultaneously? */
	  /* are all key matrix combinations possible? */
	  for (byte=0;byte<buffersize;byte++) {
	    if ((recv_data[byte] >= 1) && (recv_data[byte] <= iocard[device].ninputs)) {
	      iocard[device].inputs[card][recv_data[byte]-1] = 1;
	      if (verbose > 2) {
		printf("LIBIOCARDS: Key %i pressed on device %i byte %i \n",recv_data[byte],device,byte);
	      }
	      if (recv_data[byte] > iocard[device].ninputs) {
		if (verbose > 0) {
		  printf("LIBIOCARDS: Invalid key number %i received from device %i byte %i \n",recv_data[byte],device, byte);
		}
	      }
	    }
	  }
	}
      } while (recv_status > 0);

    }

  }

  return result;

}

/* send servo value to IOCard-USBServos card */
int send_servos(void)
{
  char device_name1[] = "IOCard-USBServos";
  char device_name2[] = "USBServos v3";
  int device;
  int result = 0;
  int buffersize = 8;
  unsigned char send_data[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  int send_status = 0;
  int servo;
  int data;
  int changed;
  struct timeval time;
  double dt;
  double dtmax = 1.0; /* Seconds to allow Servos to turn 
			 into position until they are parked */

  gettimeofday(&time,NULL);

  for (device=0;device<MAXDEVICES;device++) {

    /* check if we have a connected and initialized IOCard-USBServos card */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2)) &&
	(iocard[device].status == 1)) {
      
      changed = 0;

      for (servo=0;servo<iocard[device].nservos;servo++) {
	  
	if (iocard[device].servos[servo] != iocard[device].servos_old[servo]) {

	  /* something changed, reset timer of that servo */
	  iocard[device].time_servos[servo] = time.tv_sec + time.tv_usec/1000000.0;
	  
	  changed = 1;	
	}

	if (iocard[device].time_servos[servo] != FLT_MISS) {
	  dt = time.tv_sec + time.tv_usec/1000000.0 - iocard[device].time_servos[servo];
	  if (dt > dtmax) {
	    // After some time: Put Servo into parking position
	    changed = 1;
	  }
	} else {
	  dt = FLT_MISS;
	}

	if ((changed == 1) && (verbose > 2)) {
	  printf("LIBIOCARDS: send servo data device=%i servo=%i value=%i dt=%f \n",
		 device, servo+1, iocard[device].servos[servo],dt);
	}
      }

      if (changed) {
	/* ERASE SEND DATA FOR EACH DEVICE SINCE WE'RE ADDING BITS BELOW */
	memset(send_data,0,sizeof(send_data));

	for (servo=0;servo<iocard[device].nservos;servo++) {
	  /* Servo value is 2nd byte (minor) + first byte (major) of 2-byte couple by servo + 
	     2 bits of last two bytes, with each servo advancing two bits. */
	  
	  /* servo data is SERVOMAX - servo value. This is to guarantee that the parking value of
	     0 at the same end as the minimum servo value. This is also consistent with OpenCockpits IOCP */
	  //if (iocard[device].servos[servo] != iocard[device].servos_old[servo]) {
	  if (iocard[device].time_servos[servo] != FLT_MISS) {
	    dt = time.tv_sec + time.tv_usec/1000000.0 - iocard[device].time_servos[servo];
	    if (dt > dtmax) {
	      /* PARK to Servo after adjustment time */
	      data = SERVOMAX - SERVOPARK;
	      /* Set timer to missing (do not send PARK any more until servo is updated */
	      iocard[device].time_servos[servo] = FLT_MISS;
	    } else {
	      /* Within Servo adjustment time: send servo value */
	      data = SERVOMAX - iocard[device].servos[servo];
	    } 
	  } else {
	    /* PARK Servo */
	    data = SERVOMAX - SERVOPARK;
	  }
	  //printf("%i %i %04x \n",device,servo,data);

	  /* transfer first 255 values */
	  send_data[servo] = data & 0xff;

	  /* transfer the remaining two bits into bytes 7 and 8 */	  
	  if (servo == 0) {
	    send_data[6] = send_data[6] + (((data >> 8) & 0x03) << 6);
	  } else if (servo == 1) {
	    send_data[6] = send_data[6] + (((data >> 8) & 0x03) << 4);
	  } else if (servo == 2) {
	    send_data[6] = send_data[6] + (((data >> 8) & 0x03) << 2);
	  } else if (servo == 3) {
	    send_data[6] = send_data[6] + (((data >> 8) & 0x03) << 0);
	  } else if (servo == 4) {	  
	    send_data[7] = send_data[7] + (((data >> 8) & 0x03) << 6);
	  } else if (servo == 5) {
	    send_data[7] = send_data[7] + (((data >> 8) & 0x03) << 4);
	  }
	  	  
	}

	/* send data to USB */
	send_status = write_usb(device,send_data, buffersize);
	if (send_status < 0) {
	  result = send_status;
	}
	
	if (verbose > 2) {
	  printf("Servo HEX values: %02x %02x %02x %02x %02x %02x %02x %02x \n",
		 send_data[0],send_data[1],send_data[2],send_data[3],
		 send_data[4],send_data[5],send_data[6],send_data[7]);
	  printf("Bytes sent (Device %i): %i \n",device,send_status);
	}

      } /* at least one servo value changed for this device */

    } /* device is a USBServos card */

  } /* loop through devices */

  return result;
}

/* send motor, output and servos values to DCMotors PLUS card */
int send_motors(void)
{
  char device_name[] = "DCMotors PLUS";
  int device;
  int result = 0;
  int buffersize = 16;
  unsigned char send_data[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
  int send_status = 0;
  int servo;
  int output;
  int motor;
  int data;
  int changed;
  int card = 0;
  int count;

  for (device=0;device<MAXDEVICES;device++) {

    /* check if we have a connected and DCMotors PLUS card */
    if (!strcmp(iocard[device].name,device_name) && (iocard[device].status == 1)) {
      
      changed = 0;

      for (servo=0;servo<iocard[device].nservos;servo++) {
	if (iocard[device].servos[servo] != iocard[device].servos_old[servo]) {	  
	  changed = 1;	
	  if (verbose > 2) {
	    printf("LIBIOCARDS: send servo data to device=%i servo=%i value=%i \n",
		   device, servo+1, iocard[device].servos[servo]);
	  }
	}
      }
      for (output=0;output<iocard[device].noutputs;output++) {
	if (iocard[device].outputs[card][output] != iocard[device].outputs_old[card][output]) {	  
	  changed = 1;	
	  if (verbose > 2) {
	    printf("LIBIOCARDS: send output data to device=%i output=%i value=%i \n",
		   device, output+1, iocard[device].outputs[card][output]);
	  }
	}
      }
      for (motor=0;motor<iocard[device].nmotors;motor++) {
	if (iocard[device].motors[motor] != iocard[device].motors_old[motor]) {	  
	  changed = 1;	
	  if (verbose > 2) {
	    printf("LIBIOCARDS: send motor data to device=%i motor=%i value=%i \n",
		   device, motor+1, iocard[device].motors[motor]);
	  }
	}
      }

      if (changed) {

	for (servo=0;servo<iocard[device].nservos;servo++) {
	  /* Servo value is 2nd byte (minor) + first byte (major) of 2-byte couple by servo + 
	     2 bits of fifth byte, with each servo advancing two bits. */
	  
	  /* servo data is SERVOMAX - servo value. This is to guarantee that the parking value of
	     0 at the same end as the minimum servo value. This is also consistent with OpenCockpits IOCP */
	  data = SERVOMAX - iocard[device].servos[servo];
	  
	  /* transfer first 255 values */
	  send_data[servo] = data & 0xff;

	  /* transfer the remaining two bits into byte 5 */
	  if (servo == 0) {
	    send_data[4] = send_data[4] + (((data >> 8) & 0x03) << 6);
	  } else if (servo == 1) {
	    send_data[4] = send_data[4] + (((data >> 8) & 0x03) << 4);
	  } else if (servo == 2) {
	    send_data[4] = send_data[4] + (((data >> 8) & 0x03) << 2);
	  } else if (servo == 3) {
	    send_data[4] = send_data[4] + (((data >> 8) & 0x03) << 0);
	  }
	  
	}
	for (output=0;output<iocard[device].noutputs;output++) {
	  if (iocard[device].outputs[card][output] == 1) {
	    send_data[9] = send_data[9] + (iocard[device].outputs[card][output] << output);
	  }
	}	
	for (motor=0;motor<iocard[device].nmotors;motor++) {
	  if ((iocard[device].motors[motor] > 0) && (iocard[device].motors[motor] <= 255)) {
	    send_data[5+motor] = iocard[device].motors[motor];
	  }
	}	

	/* send data to USB */
	send_status = write_usb(device,send_data, buffersize);
	count = 1;
	while ((send_status < 0) && (count < 10))  {
	  count++;
	  usleep(50000);
	  printf("DCMotors PLUS Retry Sending (previous send incomplete!)\n");
	  send_status = write_usb(device,send_data, buffersize);
	  result = send_status;
	}
	
	if (verbose > 2) {
	  printf("DCMotors PLUS HEX values: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
		 send_data[0],send_data[1],send_data[2],send_data[3],
		 send_data[4],send_data[5],send_data[6],send_data[7],
		 send_data[8],send_data[9],send_data[10],send_data[11],
		 send_data[12],send_data[13],send_data[14],send_data[15]);
	  printf("Bytes sent: %i \n",send_status);
	}

      } /* at least one servo, motor or output value changed for this DCMotors PLUS device */

    }

  }

  return result;

}

/* receive USB data from a IOCard-USBServos card */
/* containing the data from the 4 analog axes of the card */
int receive_axes(void)
{
  char device_name1[] = "IOCard-USBServos";
  char device_name2[] = "USBServos v3";
  char device_name3[] = "DCMotors PLUS";
  int device;
  int result = 0;
  int recv_status;
  int buffersize1 = 8;
  int buffersize2 = 16;
  int byte;
  unsigned char recv_data1[buffersize1];	/* IOCard-USBServos raw IO data */
  unsigned char recv_data2[buffersize2];	/* DCMotors PLUS raw IO data */

  for (device=0;device<MAXDEVICES;device++) {

    /* check if we have a IOCard-USBServos or USBServos v3 card */
    if ((!strcmp(iocard[device].name,device_name1) ||
	 !strcmp(iocard[device].name,device_name2)) &&
	(iocard[device].status == 1)) {
      
      do {
	/* check whether there is new data on the read buffer */
	recv_status = read_usb(device,recv_data1, buffersize1);
	
	if (recv_status > 0) {
	  
	  if (verbose > 2) printf("LIBIOCARDS: received %i bytes from IOCard-USBServos \n",recv_status);
	  
	  for (byte=0;byte<4;byte++) {
	    if (verbose > 3) printf("LIBIOCARDS: Device %i Axis %i value %i \n",device,byte,recv_data1[byte]);
	    /* suppress noise +/-1 in axis input to avoid high frequency flipping of input */
	    if ((abs(recv_data1[byte] - iocard[device].axes_old[byte]) > 5) ||
		(recv_data1[byte] == 0) || (recv_data1[byte] == 255)) {
	      // printf("%i %i %i %i \n",byte,recv_data1[byte],recv_data1[byte+4],recv_status);
	      iocard[device].axes[byte] = recv_data1[byte];
	    }
	  }
	}
      } while (recv_status > 0);

    }

    /* check if we have a DCMotors PLUS card */
    if (!strcmp(iocard[device].name,device_name3) && (iocard[device].status == 1)) {

      do {
      
	/* check whether there is new data on the read buffer */
	recv_status = read_usb(device,recv_data2, buffersize2);
	
	if (recv_status > 0) {
	  
	  if (verbose > 2) printf("LIBIOCARDS: received %i bytes from USBMotors PLUS \n",recv_status);
	  
	  for (byte=0;byte<8;byte++) {
	    if (verbose > 3) printf("LIBIOCARDS: Device %i Axis %i value %i \n",device,byte,recv_data2[byte]);
	    /* suppress noise +/-1 in axis input to avoid high frequency flipping of input */
	    if ((abs(recv_data2[byte] - iocard[device].axes_old[byte]) > 5) ||
		(recv_data2[byte] == 0) || (recv_data2[byte] == 255)) {
	      //if (byte == 0) printf("%i %i \n",iocard[device].axes[byte],recv_data2[byte]);
	      iocard[device].axes[byte] = recv_data2[byte];
	    }
	  }
	}
      } while (recv_status > 0);
    }
  }

  return result;

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
	    iocard[device].axes[axis] = recv_data[2*axis] + recv_data[2*axis+1]*256;
	    if (verbose > 3) printf("LIBIOCARDS: Device %i Axis %i Value %i \n",device,axis,iocard[device].axes[axis]);
	  }
	  for (button=0;button<nbutton;button++) {
	    if (!strcmp(iocard[device].name,device_name1)) {
	      // BU0836X Interface: a byte between axes and buttons (maybe it is the # of bytes to reach max 8 inputs) 
	      byte = 2*iocard[device].naxes + 1 + button/8;
	    } else {
	      // BU0836A Interface
	      byte = 2*iocard[device].naxes + button/8;
	    }
	    bit = button - (button/8)*8;
	    iocard[device].inputs[card][button] = (recv_data[byte] >> bit) & 0x01;
	    if (verbose > 3) printf("LIBIOCARDS: Device %i Button %i Value %i %i %i \n",device,button,
				    iocard[device].inputs[card][button], byte, bit);
	  }
	}

      } while (recv_status > 0);
    }

  }

  return result;

}

/* sends initialization string to the MASTERCARD */
/* MASTERCARD is connected to USB EXPANSION CARD */
int initialize_mastercard(int device)
{
  char device_name1[] = "USB-Expancion V3";
  char device_name2[] = "USBexp V2";
  int send_status;
  int result = 0;
  int buffersize = 8;
  unsigned char send_data[] = { 0x3d,0x00,0x3a,0x00,0x39,0x00,0xff,0xff };

  /* check if we have a connected mastercard */
  if ((!strcmp(iocard[device].name,device_name1) ||
       !strcmp(iocard[device].name,device_name2)) &&
      (iocard[device].status == 0)) {

    /* allocate input/output buffer */
    result = setbuffer_usb(device,buffersize);

    iocard[device].noutputs = 45;
    iocard[device].ninputs = 72;
    iocard[device].nservos = 0;
    iocard[device].nmotors = 0;
    iocard[device].ndisplays = 64;
    iocard[device].nbits = 8;

    if (iocard[device].ncards == 1) send_data[3] = 0x01;
    if (iocard[device].ncards == 2) send_data[3] = 0x02;
    if (iocard[device].ncards == 3) send_data[3] = 0x03; /* test first, might be: 0x04 */
    if (iocard[device].ncards == 4) send_data[3] = 0x04; /* test first, might be: 0x08 */

    /* set number of analog axes of USB expansion card, if selected */
    if (iocard[device].naxes > 0) send_data[5] = iocard[device].naxes;

    /* initialize */
    send_status = write_usb(device, send_data, buffersize);
      
    if (verbose > 0) printf("LIBIOCARDS: Initialized MASTERCARD (device %i) with %i bytes \n",
	     device,send_status );
    
    if (send_status == sizeof(send_data)) result = 1;

  }
 
  return result;

}

/* sends initialization string to the IOCardS-Keys */
int initialize_keys(int device)
{
  char device_name[] = "IOCardS-Keys";
  int send_status;
  int result = 0;
  int buffersize = 8;
  unsigned char send_data[] = { 0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

  /* check if we have a connected IOCardS-Keys card */
  if (!strcmp(iocard[device].name,device_name) && (iocard[device].status == 0)) {

    /* allocate input/output buffer */
    result = setbuffer_usb(device,buffersize);

    iocard[device].ncards = 1;
    iocard[device].naxes = 0;
    iocard[device].noutputs = 0;
    iocard[device].ninputs = 88;
    iocard[device].nservos = 0;
    iocard[device].nmotors = 0;
    iocard[device].ndisplays = 0;

    /* initialize */
    send_status = write_usb(device, send_data, buffersize);
      
    if (verbose > 0) printf("LIBIOCARDS: Initialized IOCardS-Keys (device %i) with %i bytes \n",
			    device,send_status );

    if (send_status == sizeof(send_data)) result = 1;

  }
 
  return result;

}

/* send initialization string to IOCard-USBServos card */
/* the initialization string sets all servos to position 0 */
int initialize_servos(int device)
{
  char device_name1[] = "IOCard-USBServos";
  char device_name2[] = "USBServos v3";
  int send_status;
  int result = 0;
  int buffersize = 8;
  /* set all servos to 0: servo parking value. This is where the servo motor can be turned manually */
  unsigned char send_data[] = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0 };

  /* check if we have a connected IOCard-USBServos or USBServos v3 card */
  if ((!strcmp(iocard[device].name,device_name1) ||
       !strcmp(iocard[device].name,device_name2)) &&
      (iocard[device].status == 0)) {

    /* allocate input/output buffer */
    result = setbuffer_usb(device,buffersize);

    iocard[device].ncards = 0;
    iocard[device].naxes = 4;
    iocard[device].noutputs = 0;
    iocard[device].ninputs = 0;
    iocard[device].nservos = 6;
    iocard[device].nmotors = 0;
    iocard[device].ndisplays = 0;
    iocard[device].nbits = 8;

    /* initialize */
    send_status = write_usb(device, send_data, buffersize);
      
    if (verbose > 0) printf("LIBIOCARDS: Initialized IOCard-USBServos (device %i) with %i bytes \n",
			    device,send_status );

    if (send_status == sizeof(send_data)) result = 1;

  }

  return result;
}

/* send initialization string to DCMotors PLUS card */
/* NOTE: digital inputs of DCMotors PLUS not yet implemented! */
int initialize_motors(int device)
{
  char device_name[] = "DCMotors PLUS";
  int send_status;
  int result = 0;
  int buffersize = 16;
  /* set all servos to 0: servo parking value. This is where the servo motor can be turned manually */
  unsigned char send_data[] = { 0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

  /* check if we have a connected DCMotors PLUS card */
  if (!strcmp(iocard[device].name,device_name) && (iocard[device].status == 0)) {

    /* allocate input/output buffer */
    result = setbuffer_usb(device,buffersize);

    iocard[device].ncards = 1;
    iocard[device].naxes = 8;
    iocard[device].noutputs = 8;
    iocard[device].ninputs = 10;
    iocard[device].nservos = 4;
    iocard[device].nmotors = 4;
    iocard[device].ndisplays = 0;
    iocard[device].nbits = 8;

    /* initialize */
    send_status = write_usb(device, send_data, buffersize);
      
    if (verbose > 0) printf("LIBIOCARDS: Initialized DCMotors PLUS (device %i) with %i bytes \n",
			    device,send_status );

    if (send_status == sizeof(send_data)) result = 1;

    /* try to resolve an incomplete write to the DCMotors PLUS card which happens now and then */
    usleep(100000);
    
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
    iocard[device].naxes = 7; // Be Careful here: We might need to be prepared to have
    // a variable number of axes depending on BU0836 configuration.
    iocard[device].noutputs = 0;
    iocard[device].ninputs = 32;
    iocard[device].nservos = 0;
    iocard[device].nmotors = 0;
    iocard[device].ndisplays = 0;
    iocard[device].nbits = 12;

    if (verbose > 0) printf("LIBIOCARDS: Initialized BU0836X/A Interface (device %i) \n",
			    device );

    result = 1;

  }

  return result;
}

/* loop through selected devices and check if some have been connected
   or disconnected. Initialize or free devices as needed */
int initialize_iocards(void)
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
		      iocard[device].bus, iocard[device].address, iocard[device].path);
	
      if (ret<0) {
	result = ret;
	break;
      } else {
	iocard[device].status = 0;
	
	if (initialize_mastercard(device) == 1) {
	  iocard[device].status = 1;
	} else if (initialize_keys(device) == 1) {
	  iocard[device].status = 1;
	} else if (initialize_servos(device) == 1) {
	  iocard[device].status = 1;
	} else if (initialize_motors(device) == 1) {
	  iocard[device].status = 1;
	} else if (initialize_bu0836(device) == 1) {
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



