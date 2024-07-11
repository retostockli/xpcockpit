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
#define RECVMSGLEN 10            /* number of bytes in received UDP packet */
#define SENDMSGLEN 10            /* number of bytes in sent UDP packet */
#define UNCHANGED 0              /* flag for unchanged input / output */
#define CHANGED 1                /* flag for changed input / output. 
				    Can actually also be > 1 for e.g. analog inputs that change faster than xpteensy cycle */

#include "teensy_config.h"

/* Teensy UDP SERVER (this code here) */
extern char teensyserver_ip[30];
extern int teensyserver_port;
extern unsigned char teensyRecvBuffer[RECVMSGLEN];
extern unsigned char teensySendBuffer[SENDMSGLEN];

/* DATA STRUCTURE */
extern teensy_struct teensy[MAXTEENSYS];
extern teensyvar_struct teensyvar[MAXTEENSYS];
extern mcp23008_struct mcp23008[MAXTEENSYS][MAX_DEV];
extern mcp23017_struct mcp23017[MAXTEENSYS][MAX_DEV];
extern pcf8591_struct pcf8591[MAXTEENSYS][MAX_DEV];

/* Prototype Functions */
int read_teensy(void);
int write_teensy(void);
int init_teensy(void);

/*
int digital_inputf(int teensy, int input, float *fvalue, int type);
int digital_input(int teensy, int input, int *value, int type);
int digital_outputf(int teensy, int output, float *fvalue);
int digital_output(int teensy, int output, int *value);
int analog_output(int teensy, int analogoutput, int *value);
int analog_input(int teensy, int input, float *value, float minval, float maxval);
int encoder_input(int teensy, int input1, int input2, int *value, int multiplier, int type);
int encoder_inputf(int teensy, int input1, int input2, float *value, float multiplier, int type);
*/
