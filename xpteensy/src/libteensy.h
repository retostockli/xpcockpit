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
#define RECVMSGLEN 16            /* number of bytes in received UDP packet */
#define SENDMSGLEN 16            /* number of bytes in sent UDP packet */
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
extern program_struct program[MAXTEENSYS][MAX_PROG];
extern mcp23017_struct mcp23017[MAXTEENSYS][MAX_DEV];
extern pca9685_struct pca9685[MAXTEENSYS][MAX_DEV];
extern pcf8591_struct pcf8591[MAXTEENSYS][MAX_DEV];
extern as5048b_struct as5048b[MAXTEENSYS][MAX_DEV];
extern ht16k33_struct ht16k33[MAXTEENSYS][MAX_DEV];

/* Prototype Functions */
int ping_teensy(void);
int recv_teensy(void);
int send_teensy(void);
int init_teensy(void);

int digital_inputf(int te, int type, int dev, int pin, float *fvalue, int input_type);
int digital_input(int te, int type, int dev, int pin, int *value, int input_type);
int analog_input(int te, int pin, float *value, float minval, float maxval);
int encoder_input(int te, int type, int dev, int pin1, int pin2, int *value, int multiplier, int encoder_type);
int encoder_inputf(int te, int type, int dev, int pin1, int pin2, float *value, float multiplier, int encoder_type);
int digital_outputf(int te, int type, int dev, int pin, float *fvalue);
int digital_output(int te, int type, int dev, int pin, int *value);
int display_outputf(int te, int type, int dev, int pos, int n, float *fvalue, int dp, int brightness);
int display_output(int te, int type, int dev, int pos, int n, int *value, int dp, int brightness);
int pwm_output(int te, int type, int dev, int pin, float *fvalue, float minval, float maxval);
int servo_output(int te, int type, int dev, int pin, float *fvalue, float minval, float maxval, float servo_min, float servo_max);
int motor_output(int te, int type, int dev, int pin, float *fvalue, float minval, float maxval, int brake);
int angle_input(int te, int type, int dev, int input_type, int *value);
int program_closedloop(int te, int prog, int active, float *fvalue, float minval, float maxval);

