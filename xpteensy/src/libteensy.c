/* This is the libteensy.c code which contains all functions to interact with
   Teensys via the Ethernet Shield and the UDP protcol

   Copyright (C) 2024 - 2025 Reto Stockli

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
#include "libteensy.h"
#include "handleudp.h"
#include "serverdata.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

char teensyserver_ip[30];
int teensyserver_port;

unsigned char teensyRecvBuffer[RECVMSGLEN];
unsigned char teensySendBuffer[SENDMSGLEN];

teensy_struct teensy[MAXTEENSYS];
program_struct program[MAXTEENSYS][MAX_PROG];
mcp23017_struct mcp23017[MAXTEENSYS][MAX_DEV];
pca9685_struct pca9685[MAXTEENSYS][MAX_DEV];
pcf8591_struct pcf8591[MAXTEENSYS][MAX_DEV];
as5048b_struct as5048b[MAXTEENSYS][MAX_DEV];
ht16k33_struct ht16k33[MAXTEENSYS][MAX_DEV];

/* Send a Ping to the Teensy until it responds */
/* Makes sure Teensy is online before we send init strings */
/* since UDP does not guarantee reception */
int ping_teensy() {
  
  int ret;
  int te;
  struct timeval newtime;
  float dt;

  gettimeofday(&newtime,NULL);
 
  for (te=0;te<MAXTEENSYS;te++) {
    
    if (teensy[te].connected == 1) {
      if (teensy[te].online == 0) {

	// time difference in [ms]
	dt = ((float)(newtime.tv_sec - teensy[te].ping_time.tv_sec) +
	      ((float)(newtime.tv_usec - teensy[te].ping_time.tv_usec)) / 1000000.0)*1000.0;

	if (dt > 1000.0) {
      
	  memset(teensySendBuffer,0,SENDMSGLEN);
	  teensySendBuffer[0] = TEENSY_ID1; /* T */
	  teensySendBuffer[1] = TEENSY_ID2; /* E */
	  teensySendBuffer[2] = 0x00;
	  teensySendBuffer[3] = 0x00; 
	  teensySendBuffer[4] = TEENSY_PING;
	  teensySendBuffer[5] = TEENSY_TYPE;
	  ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	  if (ret == SENDMSGLEN) {
	    if (verbose > 0) printf("PING: Sent %i bytes to Teensy %i \n", ret,te);
	  } else {
	    printf("INCOMPLETE PING: Sent %i bytes to Teensy %i \n", ret,te);
	  }

	  teensy[te].ping_time = newtime;

	}
      } /* teensy not online */
    } /* teensy connected */
  } /* loop over teensys */

  return 0;
}

/* Initialize Teensy Inputs and Outputs and Daughter boards */
int init_teensy() {
  
  int ret;
  int te;
  int pin;
  int dev;
  int prog;

  /* first check if Teensy devices are online */
  ret = ping_teensy();
  
  for (te=0;te<MAXTEENSYS;te++) {
    if ((teensy[te].connected == 1) && (teensy[te].online == 1) && (teensy[te].initialized == 0)) {
      /* initialize teensy mother board */
      /* initialize pins selected for digital input */
      for (pin=0;pin<MAX_PINS;pin++) {
	if ((teensy[te].pinmode[pin] == PINMODE_INPUT) ||
	    (teensy[te].pinmode[pin] == PINMODE_OUTPUT) ||
	    (teensy[te].pinmode[pin] == PINMODE_PWM) ||
	    (teensy[te].pinmode[pin] == PINMODE_SERVO) ||
	    (teensy[te].pinmode[pin] == PINMODE_MOTOR) ||
	    (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEDIAN) ||
	    (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEAN) ||
	    (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUT) ||
	    (teensy[te].pinmode[pin] == PINMODE_INTERRUPT) ||
	    (teensy[te].pinmode[pin] == PINMODE_I2C)) {

	  if ((pin == I2C_SCL_PIN) || (pin == I2C_SDA_PIN)) {
	    printf("ERROR: Teensy %i Pin %i is a I2C SDA / SCL Pin \n",te,pin);
	  } else {
	    memset(teensySendBuffer,0,SENDMSGLEN);
	    if (verbose > 0) {
	      if (teensy[te].pinmode[pin] == PINMODE_INPUT) printf("Teensy %i Pin %i Initialized as Input \n",te,pin);
	      if (teensy[te].pinmode[pin] == PINMODE_OUTPUT) printf("Teensy %i Pin %i Initialized as Output with Value %i \n",
								    te,pin,teensy[te].val[pin][0]);
	      if (teensy[te].pinmode[pin] == PINMODE_PWM) printf("Teensy %i Pin %i Initialized as PWM with Value %i \n",
								 te,pin,teensy[te].val[pin][0]);
	      if (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEDIAN) printf("Teensy %i Pin %i Initialized as ANALOG INPUT (Median Filter) \n",te,pin);
	      if (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEAN) printf("Teensy %i Pin %i Initialized as ANALOG INPUT (Mean Filter) \n",te,pin);
	      if (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUT) printf("Teensy %i Pin %i Initialized as ANALOG INPUT \n",te,pin);
	      if (teensy[te].pinmode[pin] == PINMODE_SERVO) printf("Teensy %i Pin %i Initialized as SERVO with Value %i \n",
								   te,pin,teensy[te].val[pin][0]);
	      if (teensy[te].pinmode[pin] == PINMODE_MOTOR) {
		printf("Teensy %i Pin %i Initialized as Motor with Value %i \n",te,pin,teensy[te].val[pin][0]);
		printf("       with Direction Pins %i %i \n",teensy[te].arg1[pin],teensy[te].arg2[pin]);
		if (teensy[te].arg1[pin] == INITVAL) printf("ERROR: NO DIRECTION PIN 1 DEFINED!!!! \n");
		if (teensy[te].arg2[pin] == INITVAL) printf("ERROR: NO DIRECTION PIN 2 DEFINED!!!! \n");
		if (teensy[te].arg3[pin] != INITVAL) printf("       with Current Sensing Pin %i \n",teensy[te].arg3[pin]);
	      }
	      if (teensy[te].pinmode[pin] == PINMODE_INTERRUPT) printf("Teensy %i Pin %i Initialized as INTERRUPT \n",te,pin);
	      if (teensy[te].pinmode[pin] == PINMODE_I2C) printf("Teensy %i Pin %i Initialized as I2C Pin \n",te,pin);
	    }
	    teensySendBuffer[0] = TEENSY_ID1; /* T */
	    teensySendBuffer[1] = TEENSY_ID2; /* E */
	    if (teensy[te].pinmode[pin] == PINMODE_MOTOR) {
	      /* also send maximum allowable current for motor */
	      memcpy(&teensySendBuffer[2],&teensy[te].arg6[pin],sizeof(teensy[te].arg6[pin]));
	    } else {
	      teensySendBuffer[2] = 0x00;
	      teensySendBuffer[3] = 0x00;
	    }
	    teensySendBuffer[4] = TEENSY_INIT;
	    teensySendBuffer[5] = TEENSY_TYPE;
	    teensySendBuffer[6] = 0;
	    teensySendBuffer[7] = pin;
	    memcpy(&teensySendBuffer[8],&teensy[te].val[pin][0],sizeof(teensy[te].val[pin][0]));
	    teensySendBuffer[10] = teensy[te].pinmode[pin];
	    if (teensy[te].pinmode[pin] == PINMODE_MOTOR) {
	      /* also send the two direction pins for IN1 and IN2 and the Current Sense pin */
	      teensySendBuffer[11] = teensy[te].arg1[pin]; // IN1 Pin
	      teensySendBuffer[12] = teensy[te].arg2[pin]; // IN2 Pin
	      teensySendBuffer[13] = teensy[te].arg3[pin]; // Current Sense Pin
	      teensySendBuffer[14] = teensy[te].arg4[pin]; // Min speed (0..255)
	      teensySendBuffer[15] = teensy[te].arg5[pin]; // Max speed (0..255)
	    }
	    ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	    if (ret == SENDMSGLEN) {
	      if (verbose > 1) printf("INIT: Sent %i bytes to Teensy %i \n", ret,te);
	    } else {
	      printf("INCOMPLETE INIT: Sent %i bytes to Teensy %i \n", ret,te);
	    }
	  } /* pin not a I2C pin */
	} /* pin defined */
      } /* loop over pins */

      /* wait for 10 ms to make sure teensy hardware has initialized */
      usleep(10000);

      /* initialize PCA9685 boards connected via I2C */
      /* PCA9685 0x40 and 0x41 is incompatible with MCP23017 at 0x20
	 Solution is to initialize PCA9685 before MCP23017. Why???? */
      for (dev=0;dev<MAX_DEV;dev++) {
	if ((pca9685[te][dev].connected == 1) && (pca9685[te][dev].wire != INITVAL)) {
	  for (pin=0;pin<PCA9685_MAX_PINS;pin++) {
	    if ((pca9685[te][dev].pinmode[pin] == PINMODE_PWM) ||
		(pca9685[te][dev].pinmode[pin] == PINMODE_SERVO)) {
	      memset(teensySendBuffer,0,SENDMSGLEN);
	      if (verbose > 0) {
		if (pca9685[te][dev].pinmode[pin] == PINMODE_PWM)
		  printf("Teensy %i PCA9685 %i Pin %i Initialized as PWM with Value %i \n",
			 te,dev,pin,pca9685[te][dev].val[pin]);
		if (pca9685[te][dev].pinmode[pin] == PINMODE_SERVO)
		  printf("Teensy %i PCA9685 %i Pin %i Initialized as SERVO with Value %i \n",
			 te,dev,pin,pca9685[te][dev].val[pin]);
	      }
	      teensySendBuffer[0] = TEENSY_ID1; /* T */
	      teensySendBuffer[1] = TEENSY_ID2; /* E */
	      teensySendBuffer[2] = 0x00;
	      teensySendBuffer[3] = 0x00; 
	      teensySendBuffer[4] = TEENSY_INIT;
	      teensySendBuffer[5] = PCA9685_TYPE;
	      teensySendBuffer[6] = dev;
	      teensySendBuffer[7] = pin;
	      memcpy(&teensySendBuffer[8],&pca9685[te][dev].val[pin],sizeof(pca9685[te][dev].val[pin]));
	      teensySendBuffer[10] = pca9685[te][dev].pinmode[pin];
	      teensySendBuffer[11] = 0; 
	      teensySendBuffer[12] = pca9685[te][dev].wire;
	      teensySendBuffer[13] = (int8_t) pca9685[te][dev].address; 
	      ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	      if (ret == SENDMSGLEN) {
		if (verbose > 1) printf("INIT: Sent %i bytes to Teensy %i PCA9685 %i \n", ret,te,dev);
	      } else {
		printf("INCOMPLETE INIT: Sent %i bytes to Teensy %i PCA9685 %i \n", ret,te,dev);
	      }
	    } /* pin defined */
	  } /* loop over pins */
	}
      }

      /* wait for 10 ms to make sure teensy hardware has initialized */
      usleep(10000);
      
      /* initialize MCP23017 boards connected via I2C */
      for (dev=0;dev<MAX_DEV;dev++) {
	if ((mcp23017[te][dev].connected == 1) && (mcp23017[te][dev].wire != INITVAL)) {
	  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
	    if ((mcp23017[te][dev].pinmode[pin] == PINMODE_INPUT) ||
		(mcp23017[te][dev].pinmode[pin] == PINMODE_OUTPUT)) {
	      memset(teensySendBuffer,0,SENDMSGLEN);
	      if (verbose > 0) {
		if (mcp23017[te][dev].pinmode[pin] == PINMODE_INPUT)
		  printf("Teensy %i MCP23017 %i Pin %i Initialized as Input \n",te,dev,pin);
		if (mcp23017[te][dev].pinmode[pin] == PINMODE_OUTPUT)
		  printf("Teensy %i MCP23017 %i Pin %i Initialized as Output with Value %i \n",
			 te,dev,pin,mcp23017[te][dev].val[pin]);
	      }
	      if ((mcp23017[te][dev].intpin == I2C_SCL_PIN) || (mcp23017[te][dev].intpin == I2C_SDA_PIN)) {
		printf("ERROR: Teensy %i Interrupt Pin %i for MCP23017 %i is a I2C SDA / SCL Pin \n",
		       te,mcp23017[te][dev].intpin,dev);
	      } else {	      
		teensySendBuffer[0] = TEENSY_ID1; /* T */
		teensySendBuffer[1] = TEENSY_ID2; /* E */
		teensySendBuffer[2] = 0x00;
		teensySendBuffer[3] = 0x00; 
		teensySendBuffer[4] = TEENSY_INIT;
		teensySendBuffer[5] = MCP23017_TYPE;
		teensySendBuffer[6] = dev;
		teensySendBuffer[7] = pin;
		memcpy(&teensySendBuffer[8],&mcp23017[te][dev].val[pin],sizeof(mcp23017[te][dev].val[pin]));
		teensySendBuffer[10] = mcp23017[te][dev].pinmode[pin];
		teensySendBuffer[11] = mcp23017[te][dev].intpin; 
		teensySendBuffer[12] = mcp23017[te][dev].wire;
		teensySendBuffer[13] = (int8_t) mcp23017[te][dev].address; 
		ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
		if (ret == SENDMSGLEN) {
		  if (verbose > 1) printf("INIT: Sent %i bytes to Teensy %i MCP23017 %i \n", ret,te,dev);
		} else {
		  printf("INCOMPLETE INIT: Sent %i bytes to Teensy %i MCP23017 %i \n", ret,te,dev);
		}
	      } /* pin not I2C pin */
	    } /* pin defined */
	  } /* loop over pins */
	}
      }

      /* wait for 10 ms to make sure teensy hardware has initialized */
      usleep(10000);
      
      /* initialize AS5048B boards connected via I2C */
      for (dev=0;dev<MAX_DEV;dev++) {
	if ((as5048b[te][dev].connected == 1) && (as5048b[te][dev].wire != INITVAL)) {
	  memset(teensySendBuffer,0,SENDMSGLEN);
	  if (verbose > 0) {
	    if (as5048b[te][dev].type == 0)
	      printf("Teensy %i AS5048B %i Initialized as Direction Count Reporting \n",te,dev);
	    if (as5048b[te][dev].type == 1)
	      printf("Teensy %i AS5048B %i Initialized as Angle Reporting \n",te,dev);
	  }
	  teensySendBuffer[0] = TEENSY_ID1; /* T */
	  teensySendBuffer[1] = TEENSY_ID2; /* E */
	  teensySendBuffer[2] = 0x00;
	  teensySendBuffer[3] = 0x00; 
	  teensySendBuffer[4] = TEENSY_INIT;
	  teensySendBuffer[5] = AS5048B_TYPE;
	  teensySendBuffer[6] = dev;
	  teensySendBuffer[7] = 0;
	  memcpy(&teensySendBuffer[8],&as5048b[te][dev].val[0],sizeof(as5048b[te][dev].val[0]));
	  teensySendBuffer[10] = as5048b[te][dev].nangle;
	  teensySendBuffer[11] = as5048b[te][dev].type; 
	  teensySendBuffer[12] = as5048b[te][dev].wire;
	  teensySendBuffer[13] = (int8_t) as5048b[te][dev].address; 
	  ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	  if (ret == SENDMSGLEN) {
	    if (verbose > 1) printf("INIT: Sent %i bytes to Teensy %i AS5048B %i \n", ret,te,dev);
	  } else {
	    printf("INCOMPLETE INIT: Sent %i bytes to Teensy %i AS5048B %i \n", ret,te,dev);
	  }
	}
      }

      /* wait for 10 ms to make sure teensy hardware has initialized */
      usleep(10000);
     
      /* initialize HT16K33 boards connected via I2C */
      for (dev=0;dev<MAX_DEV;dev++) {
	if ((ht16k33[te][dev].connected == 1) && (ht16k33[te][dev].wire != INITVAL)) {
	  memset(teensySendBuffer,0,SENDMSGLEN);
	  if (verbose > 0) printf("Teensy %i HT16K33 %i Initialized \n",te,dev);
	  teensySendBuffer[0] = TEENSY_ID1; /* T */
	  teensySendBuffer[1] = TEENSY_ID2; /* E */
	  teensySendBuffer[2] = 0x00;
	  teensySendBuffer[3] = 0x00; 
	  teensySendBuffer[4] = TEENSY_INIT;
	  teensySendBuffer[5] = HT16K33_TYPE;
	  teensySendBuffer[6] = dev;
	  teensySendBuffer[12] = ht16k33[te][dev].wire;
	  teensySendBuffer[13] = (int8_t) ht16k33[te][dev].address; 
	  ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	  if (ret == SENDMSGLEN) {
	    if (verbose > 1) printf("INIT: Sent %i bytes to Teensy %i HT16K33 %i \n", ret,te,dev);
	  } else {
	    printf("INCOMPLETE INIT: Sent %i bytes to Teensy %i HT16K33 %i \n", ret,te,dev);
	  }
	}
      }

      /* wait for 10 ms to make sure teensy hardware has initialized */
      usleep(10000);
           
      /* initialize internal programs on Teensy */
      /* initialize last, since motors, potentiometers etc. used by programs need to be known before */
      /* for now: send 3x 16 bit values and 3x 8 bit values (may need extension for other programs) */
      /* also: programs may send values back (not currently implemented) */
      for (prog=0;prog<MAX_PROG;prog++) {
	if ((program[te][prog].connected == 1) && (program[te][prog].type != INITVAL)) {
	  memset(teensySendBuffer,0,SENDMSGLEN);
	  if (verbose > 0) {
	    if (program[te][prog].type == PROGRAM_CLOSEDLOOP) {
	      printf("Teensy %i Program %i Initialized as Closed Loop Motor Driver \n",te,prog);
	    } else {
	      printf("Teensy %i Program %i Initialized as UNDEFINED PROGRAM (PLEASE DEFINE)\n",te,prog);
	    }
	  }
	  teensySendBuffer[0] = TEENSY_ID1; /* T */
	  teensySendBuffer[1] = TEENSY_ID2; /* E */
	  teensySendBuffer[2] = program[te][prog].val8[0]; // 8 bit value #0
	  teensySendBuffer[3] = program[te][prog].val8[1]; // 8 bit value #1
	  teensySendBuffer[4] = TEENSY_INIT;
	  teensySendBuffer[5] = PROGRAM_TYPE;
	  teensySendBuffer[6] = program[te][prog].type; // type of program
	  teensySendBuffer[7] = prog;  // program number
	  memcpy(&teensySendBuffer[8],&program[te][prog].val16[0],sizeof(program[te][prog].val16[0])); // 16 bit value #0
	  memcpy(&teensySendBuffer[10],&program[te][prog].val16[1],sizeof(program[te][prog].val16[1])); // 16 bit value #1
	  memcpy(&teensySendBuffer[12],&program[te][prog].val16[2],sizeof(program[te][prog].val16[2])); // 16 bit value #2
	  teensySendBuffer[14] = program[te][prog].val8[2]; // 8 bit value #2	  
	  ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	  if (ret == SENDMSGLEN) {
	    if (verbose > 1) printf("INIT: Sent %i bytes to Teensy %i PROGRAM %i \n", ret,te,prog);
	  } else {
	    printf("INCOMPLETE INIT: Sent %i bytes to Teensy %i PROGRAM %i \n", ret,te,prog);
	  }
	}
      }
      
      teensy[te].initialized = 1;

      /* wait for 10 ms to make sure teensy hardware has initialized */
      usleep(10000);
      
    } /* teensy connected */
  } /* loop over teensys */

  return 0;
}


int send_teensy() {
  
  int ret;
  int te;
  int pin;
  int dig;
  int dev;
  int prog;

  for (te=0;te<MAXTEENSYS;te++) {
    if ((teensy[te].connected == 1) && (teensy[te].online == 1) && (teensy[te].initialized == 1)) {

      /* Send chanaged values to selected Teensy outputs */
      for (pin=0;pin<MAX_PINS;pin++) {
	if ((teensy[te].pinmode[pin] == PINMODE_OUTPUT) ||
	    (teensy[te].pinmode[pin] == PINMODE_PWM) ||
	    (teensy[te].pinmode[pin] == PINMODE_MOTOR) ||
	    (teensy[te].pinmode[pin] == PINMODE_SERVO)) {
	  if (teensy[te].val[pin][0] != teensy[te].val_save[pin]) { 
	    memset(teensySendBuffer,0,SENDMSGLEN);
	    teensySendBuffer[0] = TEENSY_ID1; /* T */
	    teensySendBuffer[1] = TEENSY_ID2; /* E */
	    teensySendBuffer[2] = 0x00;
	    teensySendBuffer[3] = 0x00; 
	    teensySendBuffer[4] = TEENSY_REGULAR;
	    teensySendBuffer[5] = TEENSY_TYPE;
	    teensySendBuffer[6] = 0;
	    teensySendBuffer[7] = pin;
	    memcpy(&teensySendBuffer[8],&teensy[te].val[pin][0],sizeof(teensy[te].val[pin][0]));
	    teensySendBuffer[10] = 0;
	    teensySendBuffer[11] = 0;
	    ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	    if (ret == SENDMSGLEN) {
	      if (verbose > 1) printf("SEND: Sent %i bytes to Teensy %i Pin %i \n", ret,te,pin);
	    } else {
	      printf("INCOMPLETE SEND: Sent %i bytes to Teensy %i Pin %i \n", ret,te,pin);
	    }
	  } /* value changed since last send */
	} /* pinmode output, pwm or servo */
      } /* loop over pins of teensy */

      /* Send changed values to MCP23017 daughter board outputs via I2C */
      for (dev=0;dev<MAX_DEV;dev++) {
	if ((mcp23017[te][dev].connected == 1) && (mcp23017[te][dev].wire != INITVAL)) {
	  for (pin=0;pin<MCP23017_MAX_PINS;pin++) {
	    if (mcp23017[te][dev].pinmode[pin] == PINMODE_OUTPUT) {
	      if (mcp23017[te][dev].val[pin] != mcp23017[te][dev].val_save[pin]) {
		memset(teensySendBuffer,0,SENDMSGLEN);
		teensySendBuffer[0] = TEENSY_ID1; /* T */
		teensySendBuffer[1] = TEENSY_ID2; /* E */
		teensySendBuffer[2] = 0x00;
		teensySendBuffer[3] = 0x00; 
		teensySendBuffer[4] = TEENSY_REGULAR;
		teensySendBuffer[5] = MCP23017_TYPE;
		teensySendBuffer[6] = dev;
		teensySendBuffer[7] = pin;
		memcpy(&teensySendBuffer[8],&mcp23017[te][dev].val[pin],sizeof(mcp23017[te][dev].val[pin]));
		teensySendBuffer[10] = 0;
		teensySendBuffer[11] = 0;
		ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
		if (ret == SENDMSGLEN) {
		  if (verbose > 1) printf("SEND: Sent %i bytes to Teensy %i MCP23017 %i Pin %i \n", ret,te,dev,pin);
		} else {
		  printf("INCOMPLETE SEND: Sent %i bytes to Teensy %i MCP23017 %i Pin %i \n", ret,te,dev,pin);
		}
	      } /* value changed since last send */
	    } /* pinmode output */
	  } /* loop over pins of MCP23017 */
	} /* MCP23017 connected */
      } /* loop over MCP23017 devices */

      /* Send changed values to PCA9685 daughter board pwm/servos via I2C */
      for (dev=0;dev<MAX_DEV;dev++) {
	if ((pca9685[te][dev].connected == 1) && (pca9685[te][dev].wire != INITVAL)) {
	  for (pin=0;pin<PCA9685_MAX_PINS;pin++) {
	    if ((pca9685[te][dev].pinmode[pin] == PINMODE_PWM) ||
		(pca9685[te][dev].pinmode[pin] == PINMODE_SERVO)) {
	      if (pca9685[te][dev].val[pin] != pca9685[te][dev].val_save[pin]) {
		memset(teensySendBuffer,0,SENDMSGLEN);
		teensySendBuffer[0] = TEENSY_ID1; /* T */
		teensySendBuffer[1] = TEENSY_ID2; /* E */
		teensySendBuffer[2] = 0x00;
		teensySendBuffer[3] = 0x00; 
		teensySendBuffer[4] = TEENSY_REGULAR;
		teensySendBuffer[5] = PCA9685_TYPE;
		teensySendBuffer[6] = dev;
		teensySendBuffer[7] = pin;
		memcpy(&teensySendBuffer[8],&pca9685[te][dev].val[pin],sizeof(pca9685[te][dev].val[pin]));
		teensySendBuffer[10] = 0;
		teensySendBuffer[11] = 0;
		ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
		if (ret == SENDMSGLEN) {
		  if (verbose > 1) printf("SEND: Sent %i bytes to Teensy %i PCA9685 %i Pin %i \n", ret,te,dev,pin);
		} else {
		  printf("INCOMPLETE SEND: Sent %i bytes to Teensy %i PCA9685 %i Pin %i \n", ret,te,dev,pin);
		}
	      } /* value changed since last send */
	    } /* pinmode pwm or servo */
	  } /* loop over pins of PCA9685 */
	} /* PCA9685 connected */
      } /* loop over PCA9685 devices */

      /* Send changed values to HT16K33 daughter board via I2C */
      for (dev=0;dev<MAX_DEV;dev++) {
	if ((ht16k33[te][dev].connected == 1) && (ht16k33[te][dev].wire != INITVAL)) {
	  for (dig=0;dig<HT16K33_MAX_DIG;dig++) {
	    if (ht16k33[te][dev].val[dig] != ht16k33[te][dev].val_save[dig]) {
	      memset(teensySendBuffer,0,SENDMSGLEN);
	      teensySendBuffer[0] = TEENSY_ID1; /* T */
	      teensySendBuffer[1] = TEENSY_ID2; /* E */
	      teensySendBuffer[2] = 0x00;
	      teensySendBuffer[3] = 0x00; 
	      teensySendBuffer[4] = TEENSY_REGULAR;
	      teensySendBuffer[5] = HT16K33_TYPE;
	      teensySendBuffer[6] = dev;
	      teensySendBuffer[7] = dig;
	      memcpy(&teensySendBuffer[8],&ht16k33[te][dev].val[dig],sizeof(ht16k33[te][dev].val[dig]));
	      teensySendBuffer[10] = ht16k33[te][dev].decimalpoint[dig];
	      teensySendBuffer[11] = ht16k33[te][dev].brightness;
	      ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	      if (ret == SENDMSGLEN) {
		if (verbose > 0) printf("SEND: Sent %i bytes to Teensy %i HT16K33 %i Pin %i \n", ret,te,dev,pin);
	      } else {
		printf("INCOMPLETE SEND: Sent %i bytes to Teensy %i HT16K33 %i Pin %i \n", ret,te,dev,pin);
	      }
	    } /* value changed since last send */
	  } /* loop over pins of HT16K33 */
	} /* HT16K33 connected */
      } /* loop over HT16K33 devices */

      /* Send changed values to internal program running on Teensy */
      for (prog=0;prog<MAX_PROG;prog++) {
	if ((program[te][prog].connected == 1) && (program[te][prog].type != INITVAL)) {
	  if ((program[te][prog].val8[0] != program[te][prog].val8_save[0]) ||
	      (program[te][prog].val8[1] != program[te][prog].val8_save[1]) ||
	      (program[te][prog].val8[2] != program[te][prog].val8_save[2]) ||
	      (program[te][prog].val16[0] != program[te][prog].val16_save[0]) ||
	      (program[te][prog].val16[1] != program[te][prog].val16_save[1]) ||
	      (program[te][prog].val16[2] != program[te][prog].val16_save[2])) {
	    memset(teensySendBuffer,0,SENDMSGLEN);
	    teensySendBuffer[0] = TEENSY_ID1; /* T */
	    teensySendBuffer[1] = TEENSY_ID2; /* E */
	    teensySendBuffer[2] = program[te][prog].val8[0]; // 8 bit value #0
	    teensySendBuffer[3] = program[te][prog].val8[1]; // 8 bit value #1
	    teensySendBuffer[4] = TEENSY_REGULAR;
	    teensySendBuffer[5] = PROGRAM_TYPE;
	    teensySendBuffer[6] = program[te][prog].type; // type of program
	    teensySendBuffer[7] = prog;  // program number
	    memcpy(&teensySendBuffer[8],&program[te][prog].val16[0],sizeof(program[te][prog].val16[0])); // 16 bit value #0
	    memcpy(&teensySendBuffer[10],&program[te][prog].val16[1],sizeof(program[te][prog].val16[1])); // 16 bit value #1
	    memcpy(&teensySendBuffer[12],&program[te][prog].val16[2],sizeof(program[te][prog].val16[2])); // 16 bit value #2
	    teensySendBuffer[14] = program[te][prog].val8[2]; // 8 bit value #2
	    teensySendBuffer[15] = program[te][prog].val8[3]; // 8 bit value #3
	    ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	    if (ret == SENDMSGLEN) {
	      if (verbose > 1) printf("SEND: Sent %i bytes to Teensy %i PROGRAM %i \n", ret,te,prog);
	    } else {
	      printf("INCOMPLETE SEND: Sent %i bytes to Teensy %i PROGRAM %i \n", ret,te,prog);
	    }
	  } /* value changed since last send */
	} /* Programm defined / connected */
      } /* loop over programs */
     
    } /* teensy connected */
  } /* loop over teensys */

  return 0;
}

int recv_teensy() {

  int te;
  int recv_type;
  int dev_type;
  int dev_num;
  int pin;
  short int val;
  struct timeval newtime;
 
  
  if (udpReadLeft >= RECVMSGLEN) {
    //while (udpReadLeft >= RECVMSGLEN) {

    //printf("Packets left to read %i \n",udpReadLeft/RECVMSGLEN);
    
    te = INITVAL;
  
    /* empty UDP receive buffer instead of directly accessing the device */
    pthread_mutex_lock(&udp_exit_cond_lock);
    /* read from start of receive buffer */
    memcpy(teensyRecvBuffer,&udpRecvBuffer[0],RECVMSGLEN);
    /* shift remaining read buffer to the left */
    memmove(&udpRecvBuffer[0],&udpRecvBuffer[RECVMSGLEN],udpReadLeft-RECVMSGLEN);
    /* decrease read buffer position and counter */
    udpReadLeft -= RECVMSGLEN;
    pthread_mutex_unlock(&udp_exit_cond_lock);

    /* decode message */

    /* check init string is TE */
    if ((teensyRecvBuffer[0] == TEENSY_ID1) && (teensyRecvBuffer[1] == TEENSY_ID2)) {
      /* check MAC Address of Teensy which did send the message */
      
      for (int i=0;i<MAXTEENSYS;i++) {
	if ((teensy[i].mac[0] == teensyRecvBuffer[2]) && (teensy[i].mac[1] == teensyRecvBuffer[3]) &&
	    (teensy[i].connected == 1)) te = i;
      }
      
      /* Device which did send data is one of our Teensy devices */
      if (te >= 0) {
	if (verbose > 1) printf("Read %i bytes from Teensy %i \n",RECVMSGLEN,te);

	/* decode data header about device etc. */
	recv_type = teensyRecvBuffer[4];
	dev_type = teensyRecvBuffer[5];
	dev_num = teensyRecvBuffer[6];
	pin = teensyRecvBuffer[7];
	
	/* Copy payload 16 bit Integer into variable */
	memcpy(&val,&teensyRecvBuffer[8],sizeof(val));

	if (recv_type == TEENSY_PING) {
	  if ((dev_type == TEENSY_TYPE) && (dev_num == 0)) {
	    if (verbose > 0) printf("PING: Received Ping Reply of Teensy %i \n",te);
	    gettimeofday(&newtime,NULL);
	    teensy[te].online = 1;
	    teensy[te].ping_time = newtime;
	  }
	} else if (recv_type == TEENSY_REGULAR) {
	  if ((dev_type == TEENSY_TYPE) && (dev_num == 0)) {
	    if ((pin>=0) && (pin<MAX_PINS)) {
	      if (teensy[te].pinmode[pin] == PINMODE_INPUT) {
		if (verbose > 1) printf("Received digital value %i for pin %i of Teensy %i \n",val,pin,te);
		teensy[te].val[pin][0] = val;
	      } else if ((teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEDIAN) ||
			 (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEAN) ||
			 (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUT))  {
		teensy[te].val[pin][0] = val;
		if (verbose > 1) printf("Received analog value %i for pin %i of Teensy %i \n",val,pin,te);
	      } else {
		printf("Received value for non-Input pin %i of Teensy %i \n",pin,te);
	      }
	    } else {
	      printf("Received value for invalid pin number %i for Teensy %i \n",pin,te);
	    }
	  } else if ((dev_type == MCP23017_TYPE) && (dev_num >= 0) && (dev_num < MAX_DEV)) {
	    if (mcp23017[te][dev_num].connected == 1) {
	      if ((pin>=0) && (pin<MCP23017_MAX_PINS)) {
		mcp23017[te][dev_num].val[pin] = val;
		if (verbose > 1) printf("Received digital value %i for pin number %i for Teensy %i MCP23017 %i \n",
					val,pin,te,dev_num);
	      } else {
		printf("Received value for invalid pin number %i for Teensy %i MCP23017 %i \n",
		       pin,te,dev_num);
	      }
	    } else {
	      printf("Received value for not connected MCP23017 device %i on Teensy %i\n",dev_num,te);
	    }
	  } else if ((dev_type == AS5048B_TYPE) && (dev_num >= 0) && (dev_num < MAX_DEV)) {
	    if (as5048b[te][dev_num].connected == 1) {
	      as5048b[te][dev_num].val[0] = val;
	      if (as5048b[te][dev_num].type == 1) {
		if (verbose > 1) printf("Received Angle %i for Teensy %i AS5048B %i \n",
					val,te,dev_num);
	      } else {
		if (verbose > 1) printf("Received Directional Impulse %i for Teensy %i AS5048B %i \n",
					val,te,dev_num);
	      }
	    } else {
	      printf("Received value for not connected AS5048B device %i on Teensy %i\n",dev_num,te);
	    }
	  } else {
	    printf("Received value for unknown device type\n");
	  }
	}
	
	// printf("Left to Read: %i \n",udpReadLeft);
	
      } else {
	printf("Teensy with MAC %02x:%02x is not defined in ini file \n",
	       teensyRecvBuffer[2],teensyRecvBuffer[3]);
      }
    } else {
      printf("Received wrong Init String: %02x %02x \n",teensyRecvBuffer[0],teensyRecvBuffer[1]);
    }

  } /* while / if UDP data present in receive buffer */

  return 0;
}



/* wrapper for digital_input with floating point values */
int digital_inputf(int te, int type, int dev, int pin, float *fvalue, int input_type)
{
  int value = INT_MISS;
  if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue);
  int ret = digital_input(te, type, dev, pin, &value, input_type);
  if (value != INT_MISS) *fvalue = (float) value;
  return ret;
    
}

/* retrieve input value from given input of the teensy */
/* Two types : */
/* 0: pushbutton */
/* 1: toggle switch */
int digital_input(int te, int type, int dev, int pin, int *value, int input_type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed,
		     and -1 if something went wrong */

  if (value != NULL) {

    if (te < MAXTEENSYS) {
      if (teensy[te].connected) {
	if (type == TEENSY_TYPE) {
	  if ((pin >= 0) && (pin < MAX_PINS)) {
	    if (teensy[te].pinmode[pin] == PINMODE_INPUT) {
	      if (input_type == 0) {
		/* simple pushbutton / switch */
		if (teensy[te].val[pin][0] != teensy[te].val_save[pin]) {
		  if (verbose > 1) printf("Pushbutton: Teensy %i Pin %i Changed to %i %i \n",
					  te, pin, *value,teensy[te].val[pin][0]);
		  retval = 1;
		}
		/* always return current value if not initial value */
		if (teensy[te].val[pin][0] != INITVAL) {
		  *value = teensy[te].val[pin][0];
		} 
	      } else {
		/* toggle state everytime you press button */
		/* check if the switch state changed from 0 -> 1 */
		if ((teensy[te].val[pin][0] == 1) && (teensy[te].val_save[pin] == 0)) {
		  /* toggle */
		  if (*value != INT_MISS) {
		    if ((*value == 0) || (*value == 1)) {
		      *value = 1 - (*value);
		      retval = 1;
		      if (verbose > 1) printf("Toogle Switch: Teensy %i Pin %i Changed to %i \n",
					      te, pin, *value);
		    } else {
		      printf("Toogle Switch: Teensy %i Pin %i Needs to be 0 or 1, but has value %i \n",
			     te, pin, *value);
		      retval = -1;
		    }
		  }
		}
	      }
	    } else {
	      if (verbose > 0) printf("Pin %i is not defined as digital input of Teensy %i \n", pin, te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("Digital Input %i above maximum # of pins %i of Teensy %i \n",
				    pin,MAX_PINS,te);
	    retval = -1;
	  }
	} else if (type == MCP23017_TYPE) {
	  if ((dev >= 0) && (dev < MAX_DEV)) {
	    if (mcp23017[te][dev].connected == 1) {
		if ((pin >= 0) && (pin < MCP23017_MAX_PINS)) {
		  if (mcp23017[te][dev].pinmode[pin] == PINMODE_INPUT) {
		    if (input_type == 0) {
		      /* simple pushbutton / switch */
		      if (mcp23017[te][dev].val[pin] != mcp23017[te][dev].val_save[pin]) {
			if (verbose > 1) printf("Pushbutton: Teensy %i MCP23017 %i Pin %i Changed to %i %i \n",
						te, dev, pin, *value,mcp23017[te][dev].val[pin]);
			*value = mcp23017[te][dev].val[pin];
			retval = 1;
		      }
		      /* always return current value if not initial value */
		      if (mcp23017[te][dev].val[pin] != INITVAL) {
			*value = mcp23017[te][dev].val[pin];
		      } 
		    } else {
		      /* toggle state everytime you press button */
		      /* check if the switch state changed from 0 -> 1 */
		      if ((mcp23017[te][dev].val[pin] == 1) && (mcp23017[te][dev].val_save[pin] == 0)) {
			/* toggle */
			if (*value != INT_MISS) {
			  if ((*value == 0) || (*value == 1)) {
			    *value = 1 - (*value);
			    retval = 1;
			    if (verbose > 1) printf("Toogle Switch: Teensy %i MCP23017 %i Pin %i Changed to %i \n",
						    te, dev, pin, *value);
			  } else {
			    printf("Toogle Switch: Teensy %i MCP23017 %i Pin %i Needs to be 0 or 1, has value %i \n",
				   te, dev, pin, *value);
			    retval = -1;
			  }
			}
		      }
		    }
		  } else {
		    if (verbose > 0) printf("Pin %i of MCP23017 %i is not defined as digital input of Teensy %i \n",
					    pin, dev, te);
		    retval = -1;
		  }
		} else {
		  if (verbose > 0) printf("Digital Input %i of MCP23017 %i above maximum # of pins %i of Teensy %i \n",
					  pin,dev,MCP23017_MAX_PINS,te);
		  retval = -1;
		}
	      } else {
		if (verbose > 0) printf("Digital Input %i cannot be read. MCP23017 %i not connected to Teensy %i \n",
					pin,dev,te);
		retval = -1;
	      }
	      } else {
		if (verbose > 0) printf("Digital Input %i for MCP23017 %i above maximum # of devices %i of Teensy %i \n",
					pin, dev,MAX_DEV,te);
		retval = -1;
	      }
	  } else {
	    if (verbose > 0) printf("Digital Input %i for Teensy %i: Please select Teensy or MCP23017 \n",pin,te);
	    retval = -1;
	  }
	} else {
	  if (verbose > 2) printf("Digital Input %i cannot be read. Teensy %i not connected \n",
				  pin,te);
	  retval = -1;
	}
      } else {
      if (verbose > 0) printf("Digital Input %i cannot be read. Teensy %i >= %i\n",pin,te,MAXTEENSYS);
      retval = -1;
    }    
  }
  
  return retval;
}

/* retrieves rotation angle or up/down trigger from AS5048B */
int angle_input(int te, int type, int dev, int input_type, int *value)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed,
		     and -1 if something went wrong */

  if (value != NULL) {

    if (te < MAXTEENSYS) {
      if (teensy[te].connected) {
	if (type == AS5048B_TYPE) {
	  if ((dev <= 0) && (dev < MAX_DEV)) {
	    if (as5048b[te][dev].connected == 1) {
	      if (input_type == as5048b[te][dev].type) {
		if (as5048b[te][dev].val[0] != as5048b[te][dev].val_save) {
		  if (input_type == 0) {
		    /* direction count */
		    if (verbose > 1) {
		      if (as5048b[te][dev].val[0] == 1) {
			printf("Teensy %i AS5048B %i DIRECTION UP \n", te, dev);
		      } else {
			printf("Teensy %i AS5048B %i DIRECTION DOWN \n", te, dev);
		      }
		    }
		    *value = as5048b[te][dev].val[0];
		    as5048b[te][dev].val[0] = 0;
		    retval = 1;
		  } else {
		    /* angle reporting */
		    if (verbose > 1) printf("Teensy %i AS5048B %i Angle %i \n", te, dev,as5048b[te][dev].val[0]);
		    *value = as5048b[te][dev].val[0];
		    retval = 1;
		  }
		}
	      } else {
		if (verbose > 0) printf("Teensy %i AS5048B %i is not configured for input type %i \n",
					te,dev,input_type);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("AS5048B %i not connected to of Teensy %i \n",
				      dev,te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("AS5048B %i above maximum # of devices %i of Teensy %i \n",
				    dev,MAX_DEV,te);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Please select AS5048B Device for Angle Input on Teensy %i \n",te);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Angle from AS5048B cannot be read. Teensy %i not connected \n",te);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Angle from AS5048B cannot be read. Teensy %i >= %i\n",te,MAXTEENSYS);
      retval = -1;
    }    
  }
  
  return retval;
}

/* retrieve value from given analog input between the range minval and maxval */
int analog_input(int te, int pin, float *value, float minval, float maxval)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed,
		     and -1 if something went wrong */

  if (value != NULL) {

    if (te < MAXTEENSYS) {
      if (teensy[te].connected) {
	if ((pin >= 0) && (pin < MAX_PINS)) {
	  if ((teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEDIAN) ||
	      (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUTMEAN) ||
	      (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUT)) {
	    /* always return actual value if valid */
	    if (teensy[te].val[pin][0] != INITVAL) {
	      *value = ((float) teensy[te].val[pin][0])
		/ (float) (pow(2,ANALOGINPUT_NBITS)-1)
		* (maxval - minval) + minval;
	    }
	    if (teensy[te].val[pin][0] != teensy[te].val_save[pin]) {
	      retval = 1;
	    }
	  } else {
	    if (verbose > 0) printf("Pin %i not defined as analog input for Teensy %i \n",
				    pin,te);
	  }
	} else {
	  if (verbose > 0) printf("Analog Input %i above maximum # of inputs %i of Teensy %i \n",
				  pin,MAX_PINS,te);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Analog Input %i cannot be read. Teensy %i not connected \n",
				pin,te);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Analog Input %i cannot be read. Teensy %i >= MAXTEENSYS\n",pin,te);
      retval = -1;
    }
    
  }

  return retval;
}

/* wrapper for encoder_input using integer values and multiplier */
int encoder_input(int te, int type, int dev, int pin1,  int pin2, int *value, int multiplier, int encoder_type)
{
  float fvalue = FLT_MISS;
  if (*value != INT_MISS) fvalue = (float) *value;
  int ret = encoder_inputf(te, type, dev, pin1, pin2, &fvalue, (float) multiplier, encoder_type);
  if (fvalue != FLT_MISS) *value = (int) lroundf(fvalue);
  return ret;
    
}

/* retrieve encoder value and for given encoder type connected to inputs 1 and 2
   Note that even though we try to capture every bit that changes, turning an
   optical encoder too fast will result in loss of states since the Teensy
   will not capture every state change due to its polling interval */
/* two types of encoders: */
/* 1: 2 bit optical rotary encoder */
/* 2: 2 bit gray type mechanical encoder */
int encoder_inputf(int te, int type, int dev, int pin1, int pin2, float *value, float multiplier, int encoder_type)
{

  // TODO: add multiplier by checking time since last move

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed,
		     and -1 if something went wrong */

  int16_t oldcount, newcount; /* encoder integer counters */
  int16_t updown = 0; /* encoder direction */
  int16_t obits[2]; /* bit arrays for 2 bit encoder */
  int16_t nbits[2]; /* bit arrays for 2 bit encoder */

  struct timeval oldtime;
  struct timeval newtime;
  float dt;

  int max_pins;
  int8_t pinmode1;
  int8_t pinmode2;
  int16_t val1;
  int16_t val2;
  int16_t val1_save;
  int16_t val2_save;
  
  if (value != NULL) {

    if (*value != FLT_MISS) {

      if ((type == TEENSY_TYPE) || (type == MCP23017_TYPE)) {
	
	if ((type == TEENSY_TYPE) || ((type == MCP23017_TYPE) && (dev >= 0) && (dev < MAX_DEV))) {

	  if (te < MAXTEENSYS) {

	    if (teensy[te].connected) {
	
	      if ((type == TEENSY_TYPE) || ((type == MCP23017_TYPE) && (mcp23017[te][dev].connected == 1))) {
		
		if (type == TEENSY_TYPE) {
		  max_pins = MAX_PINS;
		} else {
		  max_pins = MCP23017_MAX_PINS;
		}
	
		if ((pin1 >= 0) && (pin1 < max_pins) &&
		    (pin2 >= 0) && (pin2 < max_pins)) {
		  if (type == TEENSY_TYPE) {
		    pinmode1 = teensy[te].pinmode[pin1];
		    pinmode2 = teensy[te].pinmode[pin2];
		  } else {
		    pinmode1 = mcp23017[te][dev].pinmode[pin1];
		    pinmode2 = mcp23017[te][dev].pinmode[pin2];
		  }
		  if ((pinmode1 == PINMODE_INPUT) &&
		      (pinmode2 == PINMODE_INPUT)) {

		    if (type == TEENSY_TYPE) {
		      val1 = teensy[te].val[pin1][0];
		      val2 = teensy[te].val[pin2][0];
		      val1_save = teensy[te].val_save[pin1];
		      val2_save = teensy[te].val_save[pin2];
		    } else {
		      val1 = mcp23017[te][dev].val[pin1];
		      val2 = mcp23017[te][dev].val[pin2];
		      val1_save = mcp23017[te][dev].val_save[pin1];
		      val2_save = mcp23017[te][dev].val_save[pin2];
		    }
		    //printf("%i %i %i %i \n",val1,val2,val1_save,val2_save);
		    if ((val1 != INITVAL) && (val2 != INITVAL) &&
			(val1_save != INITVAL) && (val2_save != INITVAL)) {
	    
		      if ((val1 != val1_save) || (val2 != val2_save)) {
		
			/* derive last encoder bit state */
			obits[0] = val1_save;
			obits[1] = val2_save;
		  
			/* derive new encoder bit state */
			nbits[0] = val1;
			nbits[1] = val2;
		  
			//if (obits[0] == INITVAL) obits[0] = nbits[0];
			//if (obits[1] == INITVAL) obits[1] = nbits[1];

			//printf("%i %i %i %i \n",nbits[0],nbits[1],obits[0],obits[1]);

			if (encoder_type == 1) {
			  /* 2 bit optical encoder */		    
			  if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0)) {
			    updown = -1;
			  } else if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 1) && (nbits[1] == 1)) {
			    updown = 1;
			  } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1)) {
			    updown = -1;
			  } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 0) && (nbits[1] == 0)) {
			    updown = 1;
			  } else {
			    /* This encoder sends 2 bit changes per encoder change. This is an intermediate step */
			  }
	  
			  if (updown != 0) {
			    gettimeofday(&newtime,NULL);
			    if (type == TEENSY_TYPE) {
			      oldtime = teensy[te].val_time[pin1];
			      teensy[te].val_time[pin1] = newtime;
			    } else {
			      oldtime = mcp23017[te][dev].val_time[pin1];
			      mcp23017[te][dev].val_time[pin1] = newtime;
			    }
			    
			    dt = ((float)(newtime.tv_sec - oldtime.tv_sec) +
				  ((float)(newtime.tv_usec - oldtime.tv_usec)) / 1000000.0)*1000.0;
			    //printf("%f %i \n",dt,1 + (int) (50.0/MAX(dt,5.0)));
		  
			    *value = *value + ((float) updown)  * multiplier * (float) (1 + (int) (50.0/MAX(dt,5.0)));
			    retval = 1;
			  }
			} else if (encoder_type == 2) {
			  /* 2 bit gray type mechanical encoder */

			  /* derive last encoder count */
			  oldcount = obits[0]+2*obits[1];
	  
			  /* derive new encoder count */
			  newcount = nbits[0]+2*nbits[1];

			  /* forwards */
			  if (((oldcount == 0) && (newcount == 1)) ||
			      ((oldcount == 1) && (newcount == 3)) ||
			      ((oldcount == 3) && (newcount == 2)) ||
			      ((oldcount == 2) && (newcount == 0))) {
			    updown = 1;
			  }
		    
			  /* backwards */
			  if (((oldcount == 2) && (newcount == 3)) ||
			      ((oldcount == 3) && (newcount == 1)) ||
			      ((oldcount == 1) && (newcount == 0)) ||
			      ((oldcount == 0) && (newcount == 2))) {
			    updown = -1;
			  }
		    
			  if (updown != 0) {
			    gettimeofday(&newtime,NULL);
			    if (type == TEENSY_TYPE) {
			      oldtime = teensy[te].val_time[pin1];
			      teensy[te].val_time[pin1] = newtime;
			    } else {
			      oldtime = mcp23017[te][dev].val_time[pin1];
			      mcp23017[te][dev].val_time[pin1] = newtime;
			    }
			    
			    dt = ((float)(newtime.tv_sec - oldtime.tv_sec) +
				  ((float)(newtime.tv_usec - oldtime.tv_usec)) / 1000000.0)*1000.0;
			    //printf("%f %i \n",dt,1 + (int) (50.0/MAX(dt,5.0)));

			    *value = *value + ((float) updown)  * multiplier * (float) (1 + (int) (50.0/MAX(dt,5.0)));
			    retval = 1;
			  }
			} else {
			  if (verbose > 0) {
			    if (type == TEENSY_TYPE) {
			      printf("Encoder with Pins %i,%i of Teensy %i need to be of type 1 or 2 \n",
				     pin1,pin2,te);
			    } else {
			      printf("Encoder with Pins %i,%i of Teensy %i MCP23017 %i need to be of type 1 or 2 \n",
				     pin1,pin2,te,dev);
			    }
			    retval = -1;
			  }
			}
			if (type == TEENSY_TYPE) {
			  if ((retval == 1) && (verbose > 0)) printf("Encoder with Pins %i,%i of Teensy %i changed to %f \n",
								     pin1,pin2,te,*value);
			} else {
			  if ((retval == 1) && (verbose > 0))
			    printf("Encoder with Pins %i,%i of Teensy %i MCP23017 %i changed to %f \n",
				   pin1,pin2,te,dev,*value);
			}			
		      }
		    }
		  } else {
		    if (type == TEENSY_TYPE) {
		      if (verbose > 0)
			printf("Encoder with Pins %i,%i cannot be read. Pins not set as Inputs for Teensy %i \n",
			       pin1,pin2,te);
		    } else {
		      if (verbose > 0)
			printf("Encoder with Pins %i,%i cannot be read. Pins not set as Inputs for Teensy %i MCP23017 %i \n",
			       pin1,pin2,te,dev);
		    }
		    retval = -1;
		  }	    
		} else {
		  if (type == TEENSY_TYPE) {
		    if (verbose > 0) printf("Encoder with Pins %i,%i above maximum # of digital inputs %i of Teensy %i \n",
					    pin1,pin2,MAX_PINS,te);
		  } else {
		    if (verbose > 0)
		      printf("Encoder with Pins %i,%i above maximum # of digital inputs %i of Teensy %i MCP23017 %i \n",
			     pin1,pin2,MAX_PINS,te, dev);
		  }
		  retval = -1;
		}		
	      } else {
		if (verbose > 0) printf("Encoder with Pins %i %i cannot be read. MCP23017 %i not connected to Teensy %i \n",
					pin1,pin2,dev,te);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("Encoder with Pins %i,%i cannot be read. Teensy %i not connected \n",
				      pin1,pin2,te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("Encoder with Pins %i,%i cannot be read. Teensy %i >= %i\n",pin1,pin2,te,MAXTEENSYS);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Encoder with Pins %i,%i cannot be read. MCP23017 # %i >= %i \n",pin1,pin2,te,MAX_DEV);
	  retval = -1;
	}
      } else {
	if (verbose > 0) printf("Encoder with Pins %i,%i cannot be read. Device has to be Teensy or MCP23017\n",
				pin1,pin2);
	return -1;
      }    
    }
  }
  
  return retval;
}


/* wrapper for digital_output when supplying floating point value
   Values < 0.5 are set to 0 output and values >= 0.5 are set to 1 output */
int digital_outputf(int te, int type, int dev, int output, float *fvalue) {

  int value;
  
  if (*fvalue == FLT_MISS) {
    value = INT_MISS;
  } else if (*fvalue >= 0.5) {
    value = 1;
  } else {
    value = 0;
  }
    
  return digital_output(te, type, dev, output, &value);
}

/* Set Teensy or MCP23017 pin to LOW or HIGH */
int digital_output(int te, int type, int dev, int pin, int *value) {

  int retval = 0;

  if (value != NULL) {

    if (*value != INT_MISS) {
      if ((*value == 1) || (*value == 0)) {
	if (te < MAXTEENSYS) {
	  if (teensy[te].connected) {
	    if (type == TEENSY_TYPE) {
	      /* set pin of teensy host controller */
	      if ((pin >= 0) && (pin < MAX_PINS)) {
		if (teensy[te].pinmode[pin] == PINMODE_OUTPUT) {
		  if (*value != teensy[te].val[pin][0]) {
		    teensy[te].val[pin][0] = *value;
		    if (verbose > 1) printf("Digital Output %i of Teensy %i changed to %i \n", pin,te,*value);
		  }
		} else {
		  if (verbose > 0) printf("Pin %i is not defined as digital output of Teensy %i \n", pin, te);
		  retval = -1;
		}
	      } else {
		if (verbose > 0) printf("Digital Output %i above maximum # of outputs %i of Teensy %i \n",
					pin,MAX_PINS,te);
		retval = -1;
	      }
	    } else if (type == MCP23017_TYPE) {
	      /* set pin of teensy host controller */
	      if ((dev >= 0) && (dev < MAX_DEV)) {
		if (mcp23017[te][dev].connected == 1) {
		  if ((pin >= 0) && (pin < MCP23017_MAX_PINS)) {
		    if (mcp23017[te][dev].pinmode[pin] == PINMODE_OUTPUT) {
		      if (*value != mcp23017[te][dev].val[pin]) {
			mcp23017[te][dev].val[pin] = *value;
			if (verbose > 1) printf("Digital Output %i of Teensy %i MCP23017 %i changed to %i \n",
						pin,te,dev,*value);
		      }
		    } else {
		      if (verbose > 0) printf("Pin %i is not defined as digital output of Teensy %i MCP23017 %i \n",
					      pin, te, dev);
		      retval = -1;
		    }
		  } else {
		    if (verbose > 0) printf("Digital Output %i above maximum # of outputs %i of MCP23017 \n",
					    pin,MCP23017_MAX_PINS);
		    retval = -1;
		  }
		} else {
		  if (verbose > 0) printf("Digital Output %i cannot be written. MCP23017 %i not connected to Teensy %i \n",
					  pin,dev,te);
		  retval = -1;
		}
	      } else {
		if (verbose > 0) printf("Digital Output %i cannot be written. MCP23017 %i above maximum device number %i \n",
					pin,dev,MAX_DEV);
		retval = -1;
	      }	
	    } else {
	      if (verbose > 0) printf("Digital Outputs are supported only for Teensy and MCP23017 \n");
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("Digital Output %i cannot be written. Teensy %i not connected \n",
				    pin,te);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Digital Ouput %i cannot be written. Teensy %i >= %i \n",pin,te, MAXTEENSYS);
	  retval = -1;
	}
      } else {
	printf("Digital Output %i of Teensy %i should be 0 or 1, but is %i \n", pin,te,*value);
	retval = -1;
      }
    } // Output value not missing
    
  } // Output value pointer not null

  return retval;
}
int display_outputf(int te, int type, int dev, int pos, int n, float *fvalue, int dp, int brightness) {
  int value;
  
  if (*fvalue == FLT_MISS) {
    value = INT_MISS;
  } else {
    value = (int) lroundf(*fvalue);
  }
    
  return display_output(te, type, dev, pos, n, &value, dp, brightness);

}

int display_output(int te, int type, int dev, int pos, int n, int *value, int dp, int brightness) {

  int retval = 0;
  int count;
  int changed;
  int tempval;
  int negative;
  int16_t single;

  /* You can use special signs for single digits (n=1) : */
  /* Single Digits go from 0-F, 16-18 are blank, 19 is negative sign, 20 is degree sign, 21 is underscore */
  /* This way you can also blank individual or all digits */

  if (value != NULL) {

    if (*value != INT_MISS) {
      if (te < MAXTEENSYS) {
	if (teensy[te].connected) {
	  if (type == HT16K33_TYPE) {
	    if ((dev >= 0) && (dev < MAX_DEV)) {
	      if (ht16k33[te][dev].connected == 1) {
		if ((pos >= 0) && (pos < HT16K33_MAX_DIG) && (n > 0) && ((pos+n) <= HT16K33_MAX_DIG)) {

		  changed = 0;

		  ht16k33[te][dev].brightness = brightness;
		
		  if (n > 1) {
		    
		    /* Regular Case with integer number distributed over n 7-segment displays */
		    /* generate temporary storage of input value */
		    tempval = *value;

		    /* reverse negative numbers: find treatment for - sign */
		    /* use first digit for negative sign */
		    if (tempval < 0) {
		      tempval = -tempval;
		      negative = 1;
		    } else {
		      negative = 0;
		    }
		
		    /* read individual digits from integer */
		    /* blank leftmost 0 values except if it is the first one */ 
		    count = 0;
		    while ((tempval) && (count<n))
		      {
			if (dp == count) {
			  ht16k33[te][dev].decimalpoint[pos+count] = 1;
			} else {
			  ht16k33[te][dev].decimalpoint[pos+count] = 0;
			}
		      
			single = tempval % 10;
			if (ht16k33[te][dev].val[pos+count] != single) {
			  ht16k33[te][dev].val[pos+count] = single;
			  changed = 1;
			}
			tempval /= 10;
			count++;
		      }
		    
		    while (count<n)
		      {
			if (negative) {
			  /* set negative sign */
			  if (count > dp) {
			    single = 19;
			    if (ht16k33[te][dev].val[pos+count] != single) {
			      ht16k33[te][dev].val[pos+count] = single;
			      changed = 1;
			    }
			    negative = 0;
			  }
			} else {
			  if ((count == 0) || (dp >= count)) {
			    /* do not blank leftmost 0 display or if it has a decimal point */
			    /* this allows to display for instance 0.04 */
			    if (dp == count) {
			      ht16k33[te][dev].decimalpoint[pos+count] = 1;
			    } else {
			      ht16k33[te][dev].decimalpoint[pos+count] = 0;
			    }
			
			    single = 0;
			    if (ht16k33[te][dev].val[pos+count] != single) {
			      ht16k33[te][dev].val[pos+count] = single;
			      changed = 1;
			    }
			  } else {
			    /* blank all other displays in front of number */
			    single = 16;
			    if (ht16k33[te][dev].val[pos+count] != single) {
			      ht16k33[te][dev].val[pos+count] = single;
			      changed = 1;
			    }
			  }
			}
			count++;
		      }

		    if (changed == 1) {
		      if (verbose > 0) printf("Display Output Digit range %i-%i of Teensy %i HT16K33 %i changed to %i \n",
					      pos,pos+n-1,te,dev,*value);
		    }

		  } else {
		    /* special case: single digit (n=1): also plot special symbols */
		    if (dp == pos) {
		      ht16k33[te][dev].decimalpoint[pos] = 1;
		    } else {
		      ht16k33[te][dev].decimalpoint[pos] = 0;
		    }
		    
		    if (ht16k33[te][dev].val[pos] != *value) {
		      ht16k33[te][dev].val[pos] = *value;
		      if (verbose > 0) printf("Display Output Digit %i of Teensy %i HT16K33 %i changed to %i \n",
					      pos,te,dev,*value);
		    }		    
		  }		  
		} else {
		  if (verbose > 0) printf("Display Output digit range %i-%i above maximum # of Digits %i of HT16K33 \n",
					  pos,pos+n-1,HT16K33_MAX_DIG);
		  retval = -1;
		}
	      } else {
		if (verbose > 0) printf("Display Output range %i-%i cannot be written. HT16K33 %i not connected to Teensy %i \n",
					pos,pos+n-1,dev,te);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("Display Output range %i-%i cannot be written. HT16K33 %i above maximum # of devices %i of Teensy %i \n",
				      pos,pos+n-1,dev,MAX_DEV,te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("Display Outputs are supported only for HT16K33 \n");
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Display Output range %i-%i cannot be written. Teensy %i not connected \n",
				  pos,pos+n-1,te);
	  retval = -1;
	}
      } else {
	if (verbose > 0) printf("Display Output range %i-%i cannot be written. Teensy %i >= %i \n",
				pos,pos+n-1,te, MAXTEENSYS);
	retval = -1;
      }
    } // Output value not missing
    
  } // Output value pointer not null

  return retval;
}

/* Create PWM signal on Teensy pin */
int pwm_output(int te, int type, int dev, int pin, float *fvalue, float minval, float maxval) {

  int retval = 0;
  int ival;

  if (fvalue != NULL) {

    if (*fvalue != FLT_MISS) {
      
      if (te < MAXTEENSYS) {
	if (teensy[te].connected) {
	  if (type == TEENSY_TYPE) {
	    if ((pin >= 0) && (pin < MAX_PINS)) {
	      if (teensy[te].pinmode[pin] == PINMODE_PWM) {
		/* scale value to PWM output range */
		ival = (int) (MIN(MAX(0.0,(*fvalue - minval) / (maxval - minval)),1.0) *
			      (pow(2,ANALOGOUTPUT_NBITS)-1.0));
		if (ival != teensy[te].val[pin][0]) {
		  teensy[te].val[pin][0] = ival;
		  if (verbose > 2) printf("PWM Output %i of Teensy %i changed to %i \n", pin, te, ival);
		}
	      } else {
		if (verbose > 0) printf("Pin %i of Teensy %i is not defined as PWM Output \n", pin, te);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("PWM Output %i above maximum # of outputs %i of Teensy %i \n", pin,
				      MAX_PINS, te);
	      retval = -1;
	    }
	  } else if (type == PCA9685_TYPE) {
	    if ((dev >= 0) && (dev < MAX_DEV)) {
	      if (pca9685[te][dev].connected == 1) {
		if ((pin >= 0) && (pin < PCA9685_MAX_PINS)) {
		  if (pca9685[te][dev].pinmode[pin] == PINMODE_PWM) {
		    /* scale value to servo output range */
		    ival = (int) ((MIN(MAX(0.0,(*fvalue - minval) / (maxval - minval)),1.0)) *
				  (pow(2,PCA9685_PWM_NBITS)-1.0));
		    if (ival != pca9685[te][dev].val[pin]) {
		      pca9685[te][dev].val[pin] = ival;
		      if (verbose > 0) printf("PWM Output %i of Teensy %i PCA9685 %i changed to %i \n",
					      pin, te, dev,ival);
		    }
		  } else {
		    if (verbose > 0) printf("Pin %i of Teensy %i PCA9685 %i is not defined as PWM Output \n",
					    pin, te, dev);
		    retval = -1;
		  }
		} else {
		  if (verbose > 0) printf("PWM Output %i above maximum # of outputs %i of Teensy %i PCA9685 %i \n",
					  pin, PCA9685_MAX_PINS, te, dev);
		  retval = -1;
		}

	      } else {
		if (verbose > 0) printf("PCA9685 %i not connected to of Teensy %i \n",
					dev,te);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("PCA9685 %i above maximum # of devices %i of Teensy %i \n",
				      dev,MAX_DEV,te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("PWM Outputs are supported only for Teensy and PCA9685 \n");
	    retval = -1;
	  }
	} else {
	  if (verbose > 2) printf("PWM Output %i cannot be written. Teensy %i not connected \n", pin, te);
	  retval = -1;
	}
      } else {
	if (verbose > 0) printf("PWM Ouput %i cannot be written. Teensy %i >= %i \n", pin, te, MAXTEENSYS);
	retval = -1;
      }

    }
    
  }

  return retval;
}

/* Writes Servo Signal to given pin of Teensy (needs to be a PWM capable pin) */
/* minval / maxval are the maximum range of the input fvalue that we want the servo to travel */
/* servo_minval and servo_maxval is the normalized range for the servo to travel in the input range.
   Example: The servo would normally go from normalized 0.0 to 1.0, you can limit it between e.g. the range 0.25 to 0.725 */
int servo_output(int te, int type, int dev, int pin, float *fvalue,
		 float minval, float maxval, float servo_minval, float servo_maxval) {

  int retval = 0;
  int ival;

  if (fvalue != NULL) {

    if (*fvalue != FLT_MISS) {

      if (te < MAXTEENSYS) {
	if (teensy[te].connected) {
	  if (type == TEENSY_TYPE) {
	    if ((pin >= 0) && (pin < MAX_PINS)) {
	      if (teensy[te].pinmode[pin] == PINMODE_SERVO) {
		/* scale value to servo output range */
		ival = (int) ((MIN(MAX(0.0,(*fvalue - minval) / (maxval - minval)
				       * (servo_maxval - servo_minval) + servo_minval),1.0))
			      * (SERVO_MAXANGLE - SERVO_MINANGLE) + SERVO_MINANGLE);
		if (ival != teensy[te].val[pin][0]) {
		  teensy[te].val[pin][0] = ival;
		  if (verbose > 0) printf("Servo Output %i of Teensy %i changed to %i \n", pin, te, ival);
		}
	      } else {
		if (verbose > 0) printf("Pin %i of Teensy %i is not defined as Servo Output \n", pin, te);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("Servo Output %i above maximum # of outputs %i of Teensy %i \n", pin,
				      MAX_PINS, te);
	      retval = -1;
	    }
	  } else if (type == PCA9685_TYPE) {
	    if ((dev >= 0) && (dev < MAX_DEV)) {
	      if (pca9685[te][dev].connected == 1) {
		if ((pin >= 0) && (pin < PCA9685_MAX_PINS)) {
		  if (pca9685[te][dev].pinmode[pin] == PINMODE_SERVO) {
		    /* scale value to servo output range */
		    ival = (int) ((MIN(MAX(0.0,(*fvalue - minval) / (maxval - minval)
					   * (servo_maxval - servo_minval) + servo_minval),1.0))
				  * (PCA9685_SERVO_MAXPULSE - PCA9685_SERVO_MINPULSE) + PCA9685_SERVO_MINPULSE);
		    if (ival != pca9685[te][dev].val[pin]) {
		      pca9685[te][dev].val[pin] = ival;
		      if (verbose > 0) printf("Servo Output %i of Teensy %i PCA9685 %i changed to %i \n",
					      pin, te, dev,ival);
		    }
		  } else {
		    if (verbose > 0) printf("Pin %i of Teensy %i PCA9685 %i is not defined as Servo Output \n",
					    pin, te, dev);
		    retval = -1;
		  }
		} else {
		  if (verbose > 0) printf("Servo Output %i above maximum # of outputs %i of Teensy %i PCA9685 %i \n",
					  pin, PCA9685_MAX_PINS, te, dev);
		  retval = -1;
		}
	      } else {
		if (verbose > 0) printf("PCA9685 %i not connected to of Teensy %i \n",
					dev,te);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("PCA9685 %i above maximum # of devices %i of Teensy %i \n",
				      dev,MAX_DEV,te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("Servo Outputs are supported only for Teensy and PCA9685 \n");
	    retval = -1;
	  }
	} else {
	  if (verbose > 2) printf("Servo Output %i cannot be written. Teensy %i not connected \n", pin, te);
	  retval = -1;
	}
      } else {
	if (verbose > 0) printf("Servo Ouput %i cannot be written. Teensy %i >= MAXTEENSYS\n", pin, te);
	retval = -1;
      }

    }

  }

  return retval;
}

/* Create Motor Driver Signal (for L298) on Teensy */
int motor_output(int te, int type, int dev, int pin, float *fvalue, float minval, float maxval, int brake) {

  int retval = 0;
  int ival;

  if (fvalue != NULL) {

    if (*fvalue != FLT_MISS) {
      
      if (te < MAXTEENSYS) {
	if (teensy[te].connected) {
	  if (type == TEENSY_TYPE) {
	    if ((pin >= 0) && (pin < MAX_PINS)) {
	      if (teensy[te].pinmode[pin] == PINMODE_MOTOR) {
		/* scale value to PWM output range */
		if (brake == 1) {
		  /* set value to 1000 if motor brake is needed */
		  ival = 1000;
		} else {
		  if (*fvalue >= 0.0) {
		    ival = (int) (MIN(MAX(0.0,(*fvalue - minval) / (maxval - minval)),1.0) *
				  (pow(2,ANALOGOUTPUT_NBITS)-1.0));
		  } else {
		    ival = (int) -(MIN(MAX(0.0,(-*fvalue - minval) / (maxval - minval)),1.0) *
				   (pow(2,ANALOGOUTPUT_NBITS)-1.0));
		  }
		  if (ival == INITVAL) ival = 0; // -1 as motor value corresponds to the init value */
		}
		if (ival != teensy[te].val[pin][0]) {
		  teensy[te].val[pin][0] = ival;
		  if (verbose > 0) printf("MOTOR Output %i of Teensy %i changed to %i \n", pin, te, ival);
		}
	      } else {
		if (verbose > 0) printf("Pin %i of Teensy %i is not defined as MOTOR Output \n", pin, te);
		retval = -1;
	      }
	    } else {
	      if (verbose > 0) printf("MOTOR Output %i above maximum # of outputs %i of Teensy %i \n", pin,
				      MAX_PINS, te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("MOTOR Outputs are supported only for Teensy \n");
	    retval = -1;
	  }
	} else {
	  if (verbose > 2) printf("MOTOR Output %i cannot be written. Teensy %i not connected \n", pin, te);
	  retval = -1;
	}
      } else {
	if (verbose > 0) printf("MOTOR Ouput %i cannot be written. Teensy %i >= MAXTEENSYS\n", pin, te);
	retval = -1;
      }

    }
    
  }

  return retval;
}

/* This program simulates a servo using a real motor and a potentiometer */
int program_closedloop(int te, int prog, int active, float *fvalue, float minval, float maxval) {

  /* program variable storage:
     16 bit (0) : Required Potentiometer Value for motor to reach
     16 bit (1) : Minimum Potentiometer Value
     16 bit (2) : Maximum Potentiometer Value
     8 bit (0) : closed loop active (1) or not (0)
     8 bit (1) : pin number of Servo Potentiometer (potentiometer has to be defined as regular potentiometer)
     8 bit (2) : pin number of Motor EN (other pins of motor have to be defined as regular motor) */

  int retval = 0;
  int ival;

  if (fvalue != NULL) {

    if ((*fvalue != FLT_MISS) && (active != INT_MISS)) {

      if (te < MAXTEENSYS) {
	if (teensy[te].connected) {
	  if ((prog >= 0) && (prog < MAX_PROG)) {
	    if (program[te][prog].type == PROGRAM_CLOSEDLOOP) {

	      float servo_min = (float) program[te][prog].val16[1];
	      float servo_max = (float) program[te][prog].val16[2];
	      
	      /* scale value to servo output range */
	      ival = (int16_t) ((MIN(MAX(0.0,(*fvalue - minval) / (maxval - minval)),1.0)) *
				(servo_max - servo_min) + servo_min);

	      if (ival != program[te][prog].val16[0]) {
		  program[te][prog].val16[0] = ival;
		  if (verbose > 0) printf("Teensy %i Closed Loop Program %i Servo Position changed to %i \n", te, prog, ival);
		}
	      if (active != program[te][prog].val8[0]) {
		program[te][prog].val8[0] = active;
		if (verbose > 0) printf("Teensy %i Closed Loop Program %i Active Status set to %i \n", te, prog, active);
	      }
	    } else {
	      if (verbose > 0) printf("Program %i of Teensy %i is not defined as Closed Loop \n", prog, te);
	      retval = -1;
	    }
	  } else {
	    if (verbose > 0) printf("Program %i above maximum # of programs %i of Teensy %i \n", prog,
				    MAX_PROG, te);
	    retval = -1;
	  }
	} else {
	  if (verbose > 2) printf("Program %i cannot be executed. Teensy %i not connected \n", prog, te);
	  retval = -1;
	}
      } else {
	if (verbose > 0) printf("Program %i cannot be executed. Teensy %i >= MAXTEENSYS\n", prog, te);
	retval = -1;
      }

    }

  }

  return retval;
}
