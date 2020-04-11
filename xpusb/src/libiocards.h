/* This is the libiocards.h header to the libiocards.c library 

   Copyright (C) 2009 - 2013 Reto Stockli
   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

/* INCLUDES */

#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <errno.h>
#include <stdlib.h>

/* LIBIOCARDS DEFINE STATEMENTS */

#define MAXMASTERCARDS 4     /* maximum number of master cards per USB expansion card */
#define MAXDEVICES 10     /* maximum number of simultaneous USB IOCARD devices */

#define MAXAXES 8         /* number of axes available on the USB expansion, IOCard-USBServos or BU0836X/A card */
#define MAXINPUTS 72      /* number of binary input channels per MASTERCARD 0-35 on J3 and 36-71 on J4 */
#define MAXOUTPUTS 45     /* number of binary output channels per MASTERCARD 11-48 on J2 (work fine), 49-55 on P2 (?) */
#define MAXDISPLAYS 64    /* number of 7-segment displays per MASTERCARD 0-15 on J1 */
#define MAXKEYS 88        /* number of key matrix combinations per IOCardS-Keys card */
#define MAXSERVOS 6       /* number of servo motors per IOCard-USBServos  */
#define MAXMOTORS 4       /* number of DC motors in DCMotors PLUS card */
#define MOTORPARK 0       /* park value for DC Motors */
#define SERVOPARK 0       /* park value for servo motors (this value applies to HITEC servos) */
#define SERVOMIN 200      /* minimum value for servo motor (this value applies to HITEC servos) */
#define SERVOMAX 1023     /* maximum value for servo motor (this value applies to HITEC servos) */

/* LIBIOCARDS PARAMETERS */

typedef struct {
  /* prescribe for each card in usbiocards.ini file (mandatory) */
  char name[30];         /* IOCard name e.g. IOCardUSB, IOCardStepper */
  uint16_t vendor;       /* 16 bit vendor code */
  uint16_t product;      /* 16 bit product code */
  /* prescribe for each card in usbiocards.ini file (voluntary) */
  uint8_t bus;           /* 8 bit bus address number */
  uint8_t address;       /* 8 bit device address number */
  char path[255];        /* Platform specific device / USB connector path:
                            Changes when you plug device to different USB port
			    May also change when you unplug an upstream device (TBD) */
  /* the numbers below should be determined either automatically */
  /* or from the initialization script for each card */
  /* memory for input/outputs/servos etc. should then be dynamically allocated */
  int ncards;            /* number of sub-cards on USB connection */
  int naxes;             /* number of axes used on the USB expansion card */
  int ninputs;           /* number of inputs (INOP) */
  int noutputs;          /* number of outputs (INOP) */
  int ndisplays;         /* number of displays (INOP) */
  int nservos;           /* number of servos (INOP) */
  int nmotors;           /* number of motors (INOP) */
  int nbits;             /* number of bits used for analog inputs */
  int status;
  /* status values:
     =-1 : disconnected
     = 0 : connected and initialized
     = 1 : connected and not initialized
     = 2 : read error
     = 3 : write error */

  /* variables holding current and previous USB digital and analog I/O states */

  struct timeval time_enc[MAXINPUTS][MAXMASTERCARDS];
  int slotdata[8][MAXMASTERCARDS];     /* stores slots present in mastercard USB read */

  int inputs[MAXINPUTS][MAXMASTERCARDS];
  int inputs_old[MAXINPUTS][MAXMASTERCARDS];
  int inputs_read[MAXINPUTS][MAXMASTERCARDS];
  /* --> only copy new inputs to old ones if they were read at least once */
  /* --> allow sending initial hardware states with commands that have no state */
  
  int axes[MAXAXES];
  int axes_old[MAXAXES];
  
  int outputs[MAXOUTPUTS][MAXMASTERCARDS];
  int outputs_old[MAXOUTPUTS][MAXMASTERCARDS];
  
  int displays[MAXDISPLAYS][MAXMASTERCARDS];
  int displays_old[MAXDISPLAYS][MAXMASTERCARDS];

  int servos[MAXSERVOS];
  int servos_old[MAXSERVOS];
  double time_servos[MAXSERVOS];

  int motors[MAXMOTORS];
  int motors_old[MAXMOTORS];

} iocard_struct;

iocard_struct iocard[MAXDEVICES];

int initial; /* initialization status: */ 

/* Prototypes */

/* Prototype functions for MASTERCARD input / output signal decoding */
int initialize_iocardsdata(void);
int copy_iocardsdata(void);

int get_acceleration (int device, int card, int input, int accelerator);
int digital_output(int device, int card, int output, int *value);
int digital_input(int device, int card, int input, int *value, int type);
int mastercard_encoder(int device, int card, int input, float *value, float multiplier, int accelerator, int type);
int mastercard_display(int device, int card, int pos, int n, int *value, int hasnegative);
int axis_input(int device, int input, float *value, float minval, float maxval);
int keys_input(int device, int key);
int servos_output(int device, int servo, float *value, float minval, float maxval,
		  int servominval, int servomaxval);
int motors_output(int device, int motor, float *value, float range);

/* Prototype functions for Iocards communication */
int initialize_mastercard(int device);
int initialize_keys(int device);
int initialize_servos(int device);
int initialize_motors(int device);
int send_mastercard(void);
int receive_mastercard(void);
int receive_keys(void);
int receive_axes(void);
int receive_bu0836(void);
int send_servos(void);
int send_motors(void);

/* Prototype functions for general purposes */
void print_license(void);
int read_ini(char ininame[]);

/* Prototype functions for initializations and exiting */
int check_iocards(void);
int initialize_iocards(void);
void exit_iocards(void);
