/* This is the libteensy.h header to the libteensy.c code which contains all functions to interact with
   Teensy over the Ethernet Shield

   Copyright (C) 2020-2024 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#define MAXTEENSYS 5             /* maximum number of teensys that we expect */
#define MAXPACKETRECV 1          /* maximum number of data points to receive per packet */
#define MAXPACKETSEND 10         /* maximum number of data points to send per packet */
#define RECVMSGLEN 4+4*MAXPACKETRECV /* number of bytes in received UDP packet */
#define SENDMSGLEN 4+4*MAXPACKETSEND /* number of bytes in sent UDP packet */
#define MAXANALOGINPUTS 42       /* 42 on Teensy  */
#define MAXANALOGOUTPUTS 42      /* 42 on Teensy, not all can have PWM output though  */
#define MAXINPUTS 42             /* All inputs can be used as outputs. Some are reserved */
#define MAXOUTPUTS 42            /* All inputs can be used as outputs. Some are reserved */
#define MAXSAVE 30               /* maximum number of history values in data structure */
#define INPUTINITVAL -1          /* initial value of inputs upon startup */
#define OUTPUTINITVAL -1         /* initial value of outputs upon startup (OFF) */
#define ANALOGINPUTNBITS 10      /* number of bits of analog inputs */
#define ANALOGOUTPUTNBITS 8      /* number of bits of analog outputs */
#define UNCHANGED 0              /* flag for unchanged input / output */
#define CHANGED 1                /* flag for changed input / output. 
				    Can actually also be > 1 for e.g. analog inputs that change faster than xpteensy cycle */

#include "teensy_config.h"

/* Teensy UDP SERVER (this code here) */
extern char teensyserver_ip[30];
extern int teensyserver_port;
extern unsigned char teensyRecvBuffer[RECVMSGLEN];
extern unsigned char teensySendBuffer[SENDMSGLEN];

/* Teensy data structure */
extern int nteensys;

typedef struct {
  int connected;         /* specific teensy is connected or not */
  char ip[30];           /* IP address of teensy */
  int port;              /* UDP port teensy is listening */
  unsigned char mac[2];  /* last two bytes of MAC address */
  int ninputs;           /* actual number of activated inputs */
  int nanaloginputs;     /* actual number of activated analoginputs */
  int noutputs;          /* actual number of activated outputs */
  int nanalogoutputs;    /* actual number of activated analog outputs (INOP) */
  char inputs[MAXINPUTS][MAXSAVE];
  char inputs_nsave; /* number of history saves for inputs */
  char inputs_isinput[MAXINPUTS];  /* which of the pins is inialized as input sends input data back? */
  int analoginputs[MAXANALOGINPUTS][MAXSAVE];
  char analoginputs_isinput[MAXANALOGINPUTS];  /* which of the analog inputs should send data back? */
  char outputs[MAXOUTPUTS];
  char outputs_changed[MAXOUTPUTS]; /* unchanged = 0, changed = 1 */
  int analogoutputs[MAXOUTPUTS];
  int analogoutputs_changed[MAXOUTPUTS]; /* unchanged = 0, changed = 1 */

  
} teensy_struct;

extern teensy_struct teensy[MAXTEENSYS];

/* Prototype Functions */
int read_teensy(void);
int write_teensy(void);
int init_teensy(void);
int digital_inputf(int teensy, int input, float *fvalue, int type);
int digital_input(int teensy, int input, int *value, int type);
int digital_outputf(int teensy, int output, float *fvalue);
int digital_output(int teensy, int output, int *value);
int analog_output(int teensy, int analogoutput, int *value);
int analog_input(int teensy, int input, float *value, float minval, float maxval);
int encoder_input(int teensy, int input1, int input2, int *value, int multiplier, int type);
int encoder_inputf(int teensy, int input1, int input2, float *value, float multiplier, int type);
