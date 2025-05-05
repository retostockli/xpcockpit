/* This is the libleo.h header to the libleo.c library 

   Copyright (C) 2025 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

/* INCLUDES */

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#include <errno.h>
#include <stdlib.h>

/* LIBLEO DEFINE STATEMENTS */

#define MAXDEVICES 10     /* maximum number of simultaneous USB devices */

#define INITVAL -1        /* initial value for inputs and outputs */
#define MAX_HIST 10       /* number of history values for analog input median filter */
#define MAXAXES 8         /* number of analog axes available on the BU0836X/A card */
#define MAXINPUTS 32      /* number of digital inputs available on the BU0836X/A card */

/* LIBLEO PARAMETERS */

typedef struct {
  /* prescribe for each card in usbiocards.ini file (mandatory) */
  char name[30];         /* USB Device name, e.g. BU0836X */
  uint16_t vendor;       /* 16 bit vendor code */
  uint16_t product;      /* 16 bit product code */
  /* prescribe for each card in .ini file (voluntary) */
  uint8_t bus;           /* 8 bit bus address number */
  uint8_t address;       /* 8 bit device address number */
  char path[255];        /* Platform specific device / USB connector path:
                            Changes when you plug device to different USB port
			    May also change when you unplug an upstream device
			    (Optional) */
  char serial[50];        /* Serial Number (Optional) */
  /* the numbers below should be determined either automatically */
  /* or from the initialization script for each card */
  /* memory for input/outputs/servos etc. should then be dynamically allocated */
  int ninputs;           /* number of inputs used on the Leo Bodnar Card */
  int naxes;             /* number of analog axes used on the Leo Bodnar Card */
  int nbits;             /* number of bits for analog axes */
  int status;
  /* status values:
     =-1 : disconnected
     = 0 : connected and initialized
     = 1 : connected and not initialized
     = 2 : read error
     = 3 : write error */

  /* variables holding current and previous USB digital and analog I/O states */

  struct timeval time_enc[MAXINPUTS];
  int inputs[MAXINPUTS];
  int inputs_old[MAXINPUTS];
  int inputs_read[MAXINPUTS];
  /* --> only copy new inputs to old ones if they were read at least once */
  /* --> allow sending initial hardware states with commands that have no state */

  int axes_hist[MAXAXES][MAX_HIST];
  int axes[MAXAXES];
  int axes_old[MAXAXES];

} leo_struct;

extern leo_struct leo[MAXDEVICES];

extern int initial; /* initialization status: */

/* Prototypes */

/* Prototype user space functions for reading inputs and writing outputs */
int initialize_leodata(void);
int copy_leodata(void);

int get_acceleration (int device, int input, int accelerator);
int digital_inputf(int device, int input, float *fvalue, int type);
int digital_input(int device, int input, int *value, int type);
int encoder_inputf(int device, int input, float *fvalue, float fmultiplier, int accelerator, int type);
int encoder_input(int device, int input, int *value, int multiplier, int accelerator, int type);
int axis_input(int device, int input, float *value, float minval, float maxval);

/* Prototype functions for Iocards communication (do not use directly in your code) */
int receive_bu0836(void);

/* Prototype functions for general purposes */
void print_license(void);
int read_ini(char* programPath, char* iniName);

/* Prototype functions for initializations and exiting */
int initialize_leo(void);
int initialize_usb(void);
void terminate_usb(void);
