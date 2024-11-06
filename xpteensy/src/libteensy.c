/* This is the libteensy.c code which contains all functions to interact with
   Teensys via the Ethernet Shield and the UDP protcol

   Copyright (C) 2024 - 2024 Reto Stockli

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

int verbose;

char teensyserver_ip[30];
int teensyserver_port;

unsigned char teensyRecvBuffer[RECVMSGLEN];
unsigned char teensySendBuffer[SENDMSGLEN];

teensy_struct teensy[MAXTEENSYS];
teensyvar_struct teensyvar[MAXTEENSYS];
mcp23008_struct mcp23008[MAXTEENSYS][MAX_DEV];
mcp23017_struct mcp23017[MAXTEENSYS][MAX_DEV];
pcf8591_struct pcf8591[MAXTEENSYS][MAX_DEV];

int init_teensy() {
  
  int ret;
  int te;
  int pin;

  for (te=0;te<MAXTEENSYS;te++) {
    if (teensy[te].connected == 1) {

      /* initialize pins selected for digital input */
      memset(teensySendBuffer,0,SENDMSGLEN);
      for (pin=0;pin<teensy[te].num_pins;pin++) {
	if (teensy[te].pinmode[pin] != INITVAL) {
	  if (verbose > 1) {
	    if (teensy[te].pinmode[pin] == PINMODE_INPUT) printf("Teensy %i Pin %i Initialized as Input \n",te,pin);
	    if (teensy[te].pinmode[pin] == PINMODE_OUTPUT) printf("Teensy %i Pin %i Initialized as Output \n",te,pin);
	    if (teensy[te].pinmode[pin] == PINMODE_PWM) printf("Teensy %i Pin %i Initialized as PWM \n",te,pin);
	    if (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUT) printf("Teensy %i Pin %i Initialized as ANALOG INPUT \n",te,pin);
	    if (teensy[te].pinmode[pin] == PINMODE_INTERRUPT) printf("Teensy %i Pin %i Initialized as INTERRUPT \n",te,pin);
	    if (teensy[te].pinmode[pin] == PINMODE_I2C) printf("Teensy %i Pin %i Initialized as I2C Pin \n",te,pin);
	  }
	  teensySendBuffer[0] = TEENSY_ID1; /* T */
	  teensySendBuffer[1] = TEENSY_ID2; /* E */
	  teensySendBuffer[2] = 0x00;
	  teensySendBuffer[3] = 0x00; 
	  teensySendBuffer[4] = TEENSY_INIT;
	  teensySendBuffer[5] = TEENSY_TYPE;
	  teensySendBuffer[6] = 0;
	  teensySendBuffer[7] = pin;
	  memcpy(&teensySendBuffer[8],&teensy[te].val[pin][0],sizeof(teensy[te].val[pin][0]));
	  teensySendBuffer[10] = teensy[te].pinmode[pin];
	  teensySendBuffer[11] = 0; 
	  ret = send_udp(teensy[te].ip,teensy[te].port,teensySendBuffer,SENDMSGLEN);
	  if (verbose > 2) printf("Sent %i bytes to teensy %i \n", ret,te);
	  memset(teensySendBuffer,0,SENDMSGLEN);
	} /* pin defined */
      } /* loop over pins */
    } /* teensy connected */
  } /* loop over teensys */

  return 0;
}


int write_teensy() {
  
  int ret;
  int te;
  int pin;

  for (te=0;te<MAXTEENSYS;te++) {
    if (teensy[te].connected == 1) {

      /* initialize pins selected for digital input */
      memset(teensySendBuffer,0,SENDMSGLEN);
      for (pin=0;pin<teensy[te].num_pins;pin++) {
	if ((teensy[te].pinmode[pin] == PINMODE_OUTPUT) ||
	    (teensy[te].pinmode[pin] == PINMODE_PWM)) {
	  if (teensy[te].val[pin][0] != teensy[te].val_save[pin]) { 
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
	    if (verbose > -1) printf("Sent %i bytes to teensy %i \n", ret,te);
	    memset(teensySendBuffer,0,SENDMSGLEN);
	  } /* value changed since last send */
	} /* output or pwm */
      } /* loop over pins */
    } /* teensy connected */
  } /* loop over teensys */

  return 0;
}


int read_teensy() {

  int te;
  int recv_type;
  int dev_type;
  int dev_num;
  int pin;
  short int val;
  int i,p;
 
  //printf("Packets left to read %i \n",udpReadLeft/RECVMSGLEN);
  
  while (udpReadLeft >= RECVMSGLEN) {
    
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

	if (recv_type == TEENSY_REGULAR) {
	  if ((dev_type == TEENSY_TYPE) && (dev_num == 0)) {
	    if ((pin>=0) && (pin<teensy[te].num_pins)) {
	      if (teensy[te].pinmode[pin] == PINMODE_INPUT) {
		if (teensy[te].nhist > 0) {
		  /* digital inputs: we want to catch all changes especially for rotary encoders
		     first shift history values of all pins and store the new value in position 0 */
		  for (p=0;p<teensy[te].num_pins;p++) {
		    for (i=teensy[te].nhist;i>=1;i--) {
		      if (i<MAX_HIST) {
			teensy[te].val[p][i] = teensy[te].val[p][i-1];
		      }
		    }
		  }
		}
		if (teensy[te].nhist < MAX_HIST) {
		  teensy[te].nhist++;
		} else {
		  printf("Maximum Number %i of history values reached for Teensy %i \n",MAX_HIST,te);
		}
		if (verbose > 0) printf("Received digital value %i for pin %i of Teensy %i \n",val,pin,te);
		teensy[te].val[pin][0] = val;
	      } else if (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUT) {
		teensy[te].val[pin][0] = val;
		if (verbose > 0) printf("Received analog value %i for pin %i of Teensy %i \n",val,pin,te);
	      } else {
		printf("Received value for non-Input pin %i of Teensy %i \n",pin,te);
	      }
	    } else {
	      printf("Received value for invalid pin number %i for Teensy %i \n",pin,te);
	    }
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

  } /* while UDP data present in receive buffer */

  return 0;
}



/* wrapper for digital_input with floating point values */
int digital_inputf(int te, int pin, float *fvalue, int type)
{
  int value = INT_MISS;
  if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue);
  int ret = digital_input(te, pin, &value, type);
  if (value != INT_MISS) *fvalue = (float) value;
  return ret;
    
}

/* retrieve input value from given input of the teensy */
/* Two types : */
/* 0: pushbutton */
/* 1: toggle switch */
int digital_input(int te, int pin, int *value, int type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed,
		     and -1 if something went wrong */

  int s;

  if (value != NULL) {

    if (te < MAXTEENSYS) {
      if (teensy[te].connected) {
	if ((pin >= 0) && (pin < teensy[te].num_pins)) {
	  if (teensy[te].pinmode[pin] == PINMODE_INPUT) {
	    if (teensy[te].nhist != 0) {
	      s = teensy[te].nhist - 1; /* history slot to read */
	      if (type == 0) {
		/* simple pushbutton / switch */
		if (*value != teensy[te].val[pin][s]) {
		  if (verbose > 1) printf("Pushbutton: Teensy %i Pin %i Changed to %i %i \n",
					  te, pin, *value,teensy[te].val[pin][s]);
		  *value = teensy[te].val[pin][s];
		  retval = 1;
		}

	      } else {
		/* toggle state everytime you press button */
		/* check if the switch state changed from 0 -> 1 */
		if ((teensy[te].val[pin][s] == 1) && (teensy[te].val_save[pin] == 0)) {
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
	    }
	  } else {
	    if (verbose > 0) printf("Pin %i is not defined as digital input of Teensy %i \n", pin, te);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Digital Input %i above maximum # of pins %i of Teensy %i \n",
				  pin,teensy[te].num_pins,te);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Input %i cannot be read. Teensy %i not connected \n",
				pin,te);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Input %i cannot be read. Teensy %i >= MAXTEENSYS\n",pin,te);
      retval = -1;
    }
    
  }

  return retval;
}


/* wrapper for digital_output when supplying floating point value
   Values < 0.5 are set to 0 output and values >= 0.5 are set to 1 output */
int digital_outputf(int te, int output, float *fvalue) {

  int value;
  
  if (*fvalue == FLT_MISS) {
    value = INT_MISS;
  } else if (*fvalue >= 0.5) {
    value = 1;
  } else {
    value = 0;
  }
    
  return digital_output(te, output, &value);
}

int digital_output(int te, int pin, int *value)
{
  int retval = 0;
  int i;

  if (value != NULL) {

    if (te < MAXTEENSYS) {
      if (teensy[te].connected) {
	if ((pin >= 0) && (pin < teensy[te].num_pins)) {
	  if (teensy[te].pinmode[pin] == PINMODE_OUTPUT) {
	    if (*value != INT_MISS) {
	      if ((*value == 1) || (*value == 0)) {
		if (*value != teensy[te].val[pin][0]) {
		  for (i=0;i<MAX_HIST;i++) {
		    teensy[te].val[pin][i] = *value;
		  }
		  if (verbose > 1) printf("Digital Output %i of Teensy %i changed to %i \n", pin,te,*value);
		}
	      } else {
		printf("Digital Output %i of Teensy %i should be 0 or 1, but is %i \n", pin,te,*value);
		retval = -1;
	      }
	    }
	  } else {
	    if (verbose > 0) printf("Pin %i is not defined as digital output of Teensy %i \n", pin, te);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Digital Output %i above maximum # of outputs %i of Teensy %i \n",
				  pin,teensy[te].num_pins,te);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Output %i cannot be written. Teensy %i not connected \n",
				pin,te);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Ouput %i cannot be written. Teensy %i >= MAXTEENSYS\n",pin,te);
      retval = -1;
    }

  }

  return retval;
}

int analog_output(int te, int pin, float *fvalue, float minval, float maxval)
{
  int retval = 0;
  int ival;

  if (fvalue != NULL) {

    if (te < MAXTEENSYS) {
      if (teensy[te].connected) {
	if ((pin >= 0) && (pin < teensy[te].num_pins)) {
	  if (teensy[te].pinmode[pin] == PINMODE_PWM) {
	    if (*fvalue != FLT_MISS) {
	      /* scale value to analog output range */
	      ival = (int) (MIN(MAX(0.0,(*fvalue - minval) / (maxval - minval)),1.0)*(pow(2,ANALOGOUTPUTNBITS)-1.0));
	      if (ival != teensy[te].val[pin][0]) {
		teensy[te].val[pin][0] = ival;
		if (verbose > 2) printf("Analog Output %i of Teensy %i changed to %i \n", pin, te, ival);
	      }
	    }
	  } else {
	    if (verbose > 0) printf("Pin %i of Teensy %i is not defined as PWM Output \n", pin, te);
	    retval = -1;
	  }
 	} else {
	  if (verbose > 0) printf("Analog Output %i above maximum # of outputs %i of Teensy %i \n", pin,
				  teensy[te].num_pins, te);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Analog Output %i cannot be written. Teensy %i not connected \n", pin, te);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Analog Ouput %i cannot be written. Teensy %i >= MAXTEENSYS\n", pin, te);
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
	if ((pin >= 0) && (pin < teensy[te].num_pins)) {
	  if (teensy[te].pinmode[pin] == PINMODE_ANALOGINPUT) {

	    if (teensy[te].val[pin][0] != teensy[te].val_save[pin]) {
	      *value = ((float) teensy[te].val[pin][0])
		/ (float) pow(2,ANALOGINPUTNBITS)
		* (maxval - minval) + minval;
	      retval = 1;
	    }
	  } else {
	    if (verbose > 0) printf("Pin %i not defined as analog input for Teensy %i \n",
				    pin,te);
	  }
	} else {
	  if (verbose > 0) printf("Analog Input %i above maximum # of inputs %i of Teensy %i \n",
				  pin,teensy[te].num_pins,te);
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


/* /\* wrapper for encoder_input using integer values and multiplier *\/ */
/* int encoder_input(int tee, int input1,  int input2, int *value, int multiplier, int type) */
/* { */
/*   float fvalue = FLT_MISS; */
/*   if (*value != INT_MISS) fvalue = (float) *value; */
/*   int ret = encoder_inputf(tee, input1, input2, &fvalue, (float) multiplier, type); */
/*   if (fvalue != FLT_MISS) *value = (int) lroundf(fvalue); */
/*   return ret; */
    
/* } */

/* /\* retrieve encoder value and for given encoder type connected to inputs 1 and 2 */
/*    Note that even though we try to capture every bit that changes, turning an  */
/*    optical encoder too fast will result in loss of states since the Teensy */
/*    will not capture every state change due to its polling interval *\/ */
/* /\* two types of encoders: *\/ */
/* /\* 1: 2 bit optical rotary encoder (type 3 in xpusb) *\/ */
/* /\* 2: 2 bit gray type mechanical encoder *\/ */
/* int encoder_inputf(int tee, int input1, int input2, float *value, float multiplier, int type) */
/* { */

/*   int retval = 0; /\* returns 1 if something changed, and 0 if nothing changed,  */
/* 		     and -1 if something went wrong *\/ */

/*   char oldcount, newcount; /\* encoder integer counters *\/ */
/*   char updown = 0; /\* encoder direction *\/ */
/*   char obits[2]; /\* bit arrays for 2 bit encoder *\/ */
/*   char nbits[2]; /\* bit arrays for 2 bit encoder *\/ */
/*   int s; */

/*   if (value != NULL) { */

/*     if (tee < MAXTEENSYS) { */
/*       if (teensy[te].connected) { */
/* 	if ((input1 >= 0) && (input1 < teensy[te].ninputs) && */
/* 	    (input2 >= 0) && (input2 < teensy[te].ninputs)) { */
/* 	  if (*value != FLT_MISS) { */

/* 	    //	    printf("%i %i %i %i \n",teensy[te].inputs[input1][s],teensy[te].inputs[input2][s], */
/* 	    //		   teensy[te].inputs[input1][s+1], teensy[te].inputs[input2][s+1]); */
	    
/* 	    if (teensy[te].inputs_nsave != 0) { */
/* 	      s = teensy[te].inputs_nsave - 1; /\* history slot to read *\/ */
/* 	      if (s < (MAXSAVE-1)) { */
/* 		/\* if not first read, and if any of the inputs have changed then the encoder was moved *\/ */
/* 		if ((((teensy[te].inputs[input1][s] != teensy[te].inputs[input1][s+1]) && */
/* 		      (teensy[te].inputs[input1][s+1] != INPUTINITVAL)) || */
/* 		     ((teensy[te].inputs[input2][s] != teensy[te].inputs[input2][s+1]) && */
/* 		      (teensy[te].inputs[input2][s+1] != INPUTINITVAL))) && */
/* 		    (teensy[te].inputs[input1][s] != INPUTINITVAL) && */
/* 		    (teensy[te].inputs[input2][s] != INPUTINITVAL)) { */

/* 		  /\* derive last encoder bit state *\/ */
/* 		  obits[0] = teensy[te].inputs[input1][s+1]; */
/* 		  obits[1] = teensy[te].inputs[input2][s+1]; */
		  
/* 		  /\* derive new encoder bit state *\/ */
/* 		  nbits[0] = teensy[te].inputs[input1][s]; */
/* 		  nbits[1] = teensy[te].inputs[input2][s]; */
		  
/* 		  if (obits[0] == INPUTINITVAL) obits[0] = nbits[0]; */
/* 		  if (obits[1] == INPUTINITVAL) obits[1] = nbits[1]; */

/* 		  printf("%i %i %i %i \n",nbits[0],nbits[1],obits[0],obits[1]); */
		  
/* 		  if (type == 1) { */
/* 		    /\* 2 bit optical encoder *\/ */
		    
/* 		    if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 0) && (nbits[1] == 0)) { */
/* 		      updown = -1; */
/* 		    } else if ((obits[0] == 0) && (obits[1] == 1) && (nbits[0] == 1) && (nbits[1] == 1)) { */
/* 		      updown = 1; */
/* 		    } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 1) && (nbits[1] == 1)) { */
/* 		      updown = -1; */
/* 		    } else if ((obits[0] == 1) && (obits[1] == 0) && (nbits[0] == 0) && (nbits[1] == 0)) { */
/* 		      updown = 1; */
/* 		    } */
	  
/* 		    if (updown != 0) { */
/* 		      /\* add accelerator by using s as number of queued encoder changes *\/ */
/* 		      *value = *value + ((float) updown)  * multiplier * (float) (s*2+1); */
/* 		      retval = 1; */
/* 		    }		     */
/* 		  } else if (type == 2) { */
/* 		    /\* 2 bit gray type mechanical encoder *\/ */

/* 		    /\* derive last encoder count *\/ */
/* 		    oldcount = obits[0]+2*obits[1]; */
	  
/* 		    /\* derive new encoder count *\/ */
/* 		    newcount = nbits[0]+2*nbits[1]; */

/* 		    /\* forwtee *\/ */
/* 		    if (((oldcount == 0) && (newcount == 1)) || */
/* 			((oldcount == 1) && (newcount == 3)) || */
/* 			((oldcount == 3) && (newcount == 2)) || */
/* 			((oldcount == 2) && (newcount == 0))) { */
/* 		      updown = 1; */
/* 		    } */
		    
/* 		    /\* backwtee *\/ */
/* 		    if (((oldcount == 2) && (newcount == 3)) || */
/* 			((oldcount == 3) && (newcount == 1)) || */
/* 			((oldcount == 1) && (newcount == 0)) || */
/* 			((oldcount == 0) && (newcount == 2))) { */
/* 		      updown = -1; */
/* 		    } */
		    
/* 		    if (updown != 0) { */
/* 		      /\* add accelerator by using s as number of queued encoder changes *\/ */
/* 		      *value = *value + ((float) updown) * multiplier * (float) (s+1); */
/* 		      retval = 1; */
/* 		    }		     */
/* 		  } else { */
/* 		    if (verbose > 0) printf("Encoder with Input %i,%i of tee %i need to be of type 1 or 2 \n", */
/* 					    input1,input2,tee); */
/* 		    retval = -1; */
/* 		  } */
/* 		} */
/* 	      } */
/* 	    } */
/* 	    if ((retval == 1) && (verbose > 2)) printf("Encoder with Input %i,%i of Teensy %i changed to %f \n", */
/* 						       input1,input2,tee,*value); */
/* 	  } */
	    
/* 	} else { */
/* 	  if (verbose > 0) printf("Encoder with Input %i,%i above maximum # of digital inputs %i of Teensy %i \n", */
/* 				  input1,input2,teensy[te].ninputs,tee); */
/* 	  retval = -1; */
/* 	} */
/*       } else { */
/* 	if (verbose > 2) printf("Encoder with Input %i,%i cannot be read. Teensy %i not connected \n", */
/* 				input1,input2,tee); */
/* 	retval = -1; */
/*       } */
/*     } else { */
/*       if (verbose > 0) printf("Encoder with Input %i,%i cannot be read. Teensy %i >= MAXTEENSYS\n",input1,input2,tee); */
/*       retval = -1; */
/*     } */
    
/*   } */

/*   return retval; */
/* } */
