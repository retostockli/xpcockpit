/* This is the libarduino.c code which contains all functions to interact with
   Arduinos via the Ethernet Shield and the UDP protcol

   Copyright (C) 2020 Reto Stockli

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
#include "libarduino.h"
#include "handleudp.h"
#include "serverdata.h"

int verbose;

//char xpserver_ip[30];
//int xpserver_port;
//char clientname[100];

char arduinoserver_ip[30];
int arduinoserver_port;

unsigned char arduinoRecvBuffer[RECVMSGLEN];
unsigned char arduinoSendBuffer[SENDMSGLEN];

int nards;
arduino_struct arduino[MAXARDS];

int read_arduino() {

  /* PROTOCOL: 8 Bytes on send and receive
     BYTE | MEANING
     0-1  | Identifier (Characters AR for Arduino)
     2-3  | Last two Bytes of MAC address
     4    | Input / Output Number (0-13)
     5    | Input / Output Type (0: digital, 1: analog)
     6-7  | Value (16 bit signed Integer Value) */
    
  int ard;
  short int val;
  int input;
  int i,s;
 
  //printf("Packets left to read %i \n",udpReadLeft/RECVMSGLEN);
  
  while (udpReadLeft >= RECVMSGLEN) {
    
    ard = -1;
    
    /* empty UDP receive buffer instead of directly accessing the device */

    pthread_mutex_lock(&exit_cond_lock);    
    /* read from start of receive buffer */
    memcpy(arduinoRecvBuffer,&udpRecvBuffer[0],RECVMSGLEN);
    /* shift remaining read buffer to the left */
    memmove(&udpRecvBuffer[0],&udpRecvBuffer[RECVMSGLEN],udpReadLeft-RECVMSGLEN);    
    /* decrease read buffer position and counter */
    udpReadLeft -= RECVMSGLEN;
    pthread_mutex_unlock(&exit_cond_lock);

    /* decode message */

    /* check init string AR */
    if ((arduinoRecvBuffer[0] == 0x41) && (arduinoRecvBuffer[1] == 0x52)) {
      /* check MAC Address of Arduino which did send the message */
      
      for (i=0;i<MAXARDS;i++) {
	if ((arduino[i].mac[0] == arduinoRecvBuffer[2]) && (arduino[i].mac[1] == arduinoRecvBuffer[3]) &&
	    (arduino[i].connected == 1)) ard = i;
      }
      
      /* ard found */
      if (ard >= 0) {

	if (verbose > 3) printf("Read %i bytes from Arduino %i \n",RECVMSGLEN,ard);
	
	/* Copy payload 16 bit Integer into variable */
	memcpy(&val,&arduinoRecvBuffer[6],sizeof(val));

	input = arduinoRecvBuffer[4];
	
	/* check type of input */
	if (arduinoRecvBuffer[5] == 0x0) {
	  /* Digital Input */
	  
	  if (val != arduino[ard].inputs[input][0]) {
	    /* shift all inputs in history array by one */
	    for (s=MAXSAVE-2;s>=0;s--) {
	      arduino[ard].inputs[input][s+1] = arduino[ard].inputs[input][s];
	    }
	    arduino[ard].inputs[input][0] = val; 
	    if (verbose > 0) printf("Arduino %i Input %i Changed to: %i \n",ard,input,val);

	    /* update number of history values per input */
	    if (arduino[ard].inputs_nsave[input] < MAXSAVE) {
	      arduino[ard].inputs_nsave[input] += 1;
	      if (verbose > 0) printf("Arduino %i Input %i # of History Values %i \n",
				      ard,input,arduino[ard].inputs_nsave[input]);
	    } else {
	      if (verbose > 0) printf("Arduino %i Input %i Maximum # of History Values %i Reached \n",
				      ard,input,MAXSAVE);
	    }	      
	  }

	} else {
	  /* Analog Input */
	  
	  /* shift all inputs in history array by one */
	  for (s=MAXSAVE-2;s>=0;s--) {
	    arduino[ard].analoginputs[input][s+1] = arduino[ard].analoginputs[input][s];
	  }
	  if (val != arduino[ard].analoginputs[input][0]) {
	    if (verbose > 0) printf("Arduino %i Analog Input %i Changed to %i \n",
				    ard,input,val);
	  }
	  arduino[ard].analoginputs[input][0] = val;	  
	}
	
	//printf("Left to Read: %i \n",udpReadLeft);
	
      } else {
	printf("Arduino with MAC %02x:%02x is not defined in ini file \n",
	       arduinoRecvBuffer[2],arduinoRecvBuffer[3]);
      }
    } else {
      printf("Received wrong Init String: %02x %02x \n",arduinoRecvBuffer[0],arduinoRecvBuffer[1]);
    }

  } /* while UDP data present in receive buffer */

  return 0;
}

int write_arduino() {
  
  int ret;
  int ard;
  int output;
  int analogoutput;
  int connected;
  short int val;

  for (ard=0;ard<MAXARDS;ard++) {
    if (arduino[ard].connected == 1) {

      memset(arduinoSendBuffer,0,SENDMSGLEN);
      arduinoSendBuffer[0] = 0x41;
      arduinoSendBuffer[1] = 0x52;
      arduinoSendBuffer[2] = 0x00;
      arduinoSendBuffer[3] = 0x00;

      /* Check Digital Outputs for Changes and Send them */
      arduinoSendBuffer[5] = 0x00;
      
      for (output=0;output<arduino[ard].noutputs;output++) {
	if (arduino[ard].outputs_changed[output] == CHANGED) {
	  if (verbose > 0) printf("Arduino %i Output %i changed to: %i \n",
				  ard,output,arduino[ard].outputs[output]);
	  arduinoSendBuffer[4] = output;
	  val = arduino[ard].outputs[output];
	  memcpy(&arduinoSendBuffer[6],&val,sizeof(val));
	  ret = send_udp(arduino[ard].ip,arduino[ard].port,arduinoSendBuffer,SENDMSGLEN);
	  if (verbose > 0) printf("Sent %i bytes to ard %i \n", ret,ard);
	  arduino[ard].outputs_changed[output] = UNCHANGED;
	}
      }
      
      /* Check Analog Outputs (PWM) for Changes and Send them */
      arduinoSendBuffer[5] = 0x01;
      
      for (analogoutput=0;analogoutput<arduino[ard].nanalogoutputs;analogoutput++) {
	if (arduino[ard].analogoutputs_changed[analogoutput] == CHANGED) {
	  if (verbose > 0) printf("Arduino %i Analogoutput %i changed to: %i \n",
				  ard,analogoutput,arduino[ard].analogoutputs[analogoutput]);
	  arduinoSendBuffer[4] = analogoutput;
	  val = arduino[ard].analogoutputs[analogoutput];
	  memcpy(&arduinoSendBuffer[6],&val,sizeof(val));
	  ret = send_udp(arduino[ard].ip,arduino[ard].port,arduinoSendBuffer,SENDMSGLEN);
	  if (verbose > 0) printf("Sent %i bytes to ard %i \n", ret,ard);
	  arduino[ard].analogoutputs_changed[analogoutput] = UNCHANGED;
	}
      }

    }
  }
  
  return 0;
}


/* wrapper for digital_input with floating point values */
int digital_inputf(int ard, int input, float *fvalue, int type)
{
  int value = INT_MISS;
  if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue);
  int ret = digital_input(ard, input, &value, type);
  if (value != INT_MISS) *fvalue = (float) value;
  return ret;
    
}

/* retrieve input value from given input position of SISMO ard */
/* master ard has 64 inputs (0..63)
   daughter ard 1 and 2 have another 64 inputs (64..127 and 128..191) */
/* Two types : */
/* 0: pushbutton */
/* 1: toggle switch */
int digital_input(int ard, int input, int *value, int type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */

  int s;

  if (value != NULL) {

    if (ard < MAXARDS) {
      if (arduino[ard].connected) {
	if ((input >= 0) && (input < arduino[ard].ninputs)) {
	  
	  if (arduino[ard].inputs_nsave[input] != 0) {
	    s = arduino[ard].inputs_nsave[input] - 1; /* history slot to read */
	    if (type == 0) {
	      /* simple pushbutton / switch */
	      if (*value != arduino[ard].inputs[input][s]) {
		*value = arduino[ard].inputs[input][s];
		retval = 1;
		if (verbose > 1) printf("Pushbutton: Arduino %i Input %i Changed to %i \n",
					ard, input, *value);
	      }

	    } else {
	      /* toggle state everytime you press button */
	      if (s < (MAXSAVE-1)) {
		/* check if the switch state changed from 0 -> 1 */
		if ((arduino[ard].inputs[input][s] == 1) && (arduino[ard].inputs[input][s+1] == 0)) {
		  /* toggle */
		  if (*value != INT_MISS) {
		    if ((*value == 0) || (*value == 1)) {
		      *value = 1 - (*value);
		      retval = 1;
		      if (verbose > 1) printf("Toogle Switch: Arduino %i Input %i Changed to %i \n",
					      ard, input, *value);
		    } else {
		      printf("Toogle Switch: Arduino %i Input %i Needs to be 0 or 1, but has value %i \n",
					      ard, input, *value);
		      retval = -1;
		    }
		  }
		}
	      }
	    }

	  }
	} else {
	  if (verbose > 0) printf("Digital Input %i above maximum # of inputs %i of Arduino %i \n",
				  input,arduino[ard].ninputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Input %i cannot be read. Arduino %i not connected \n",
				input,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Input %i cannot be read. Arduino %i >= MAXARDS\n",input,ard);
      retval = -1;
    }
    
  }

  return retval;
}

/* wrapper for digital_output when supplying floating point value 
   Values < 0.5 are set to 0 output and values >= 0.5 are set to 1 output */
int digital_outputf(int ard, int output, float *fvalue) {

  int value;
  
  if (*fvalue == FLT_MISS) {
    value = INT_MISS;
  } else if (*fvalue >= 0.5) {
    value = 1;
  } else {
    value = 0;
  }
    
  return digital_output(ard, output, &value);
}

int digital_output(int ard, int output, int *value)
{
  int retval = 0;

  if (value != NULL) {

    if (ard < MAXARDS) {
      if (arduino[ard].connected) {
	if ((output >= 0) && (output < arduino[ard].noutputs)) {

	  if (*value != INT_MISS) {
	    if ((*value == 1) || (*value == 0)) {
	      if (*value != arduino[ard].outputs[output]) {
		arduino[ard].outputs[output] = *value;
		arduino[ard].outputs_changed[output] = CHANGED;
		if (verbose > 2) printf("Digital Output %i of Arduino %i changed to %i \n",
					output,ard,*value);
	      }
	    } else {
	      printf("Digital Output %i of Arduino %i should be 0 or 1, but is %i \n",
		     output,ard,*value);
	      retval = -1;
	    }
	  }
	} else {
	  if (verbose > 0) printf("Digital Output %i above maximum # of outputs %i of Arduino %i \n",
				  output,arduino[ard].noutputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Digital Output %i cannot be written. Arduino %i not connected \n",
				output,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Digital Ouputt %i cannot be written. Arduino %i >= MAXARDS\n",output,ard);
      retval = -1;
    }

  }

  return retval;
}

int analog_output(int ard, int analogoutput, int *value)
{
  int retval = 0;

  if (value != NULL) {

    if (ard < MAXARDS) {
      if (arduino[ard].connected) {
	if ((analogoutput >= 0) && (analogoutput < arduino[ard].nanalogoutputs)) {

	  if (*value != INT_MISS) {
	    if ((*value >= 0) && (*value < (int) pow(2,ANALOGOUTPUTNBITS))) {
	      if (*value != arduino[ard].analogoutputs[analogoutput]) {
		arduino[ard].analogoutputs[analogoutput] = *value;
		arduino[ard].analogoutputs_changed[analogoutput] = CHANGED;
		if (verbose > 2) printf("Analog Output %i of Arduino %i changed to %i \n",
					analogoutput,ard,*value);
	      }
	    } else {
	      printf("Analog Output %i of Arduino %i should be between 0 and %i, but is %i \n",
		     analogoutput,ard,(int) pow(2,ANALOGOUTPUTNBITS)-1,*value);
	      retval = -1;
	    }
	  }
	} else {
	  if (verbose > 0) printf("Analog Output %i above maximum # of outputs %i of Arduino %i \n",
				  analogoutput,arduino[ard].nanalogoutputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Analog Output %i cannot be written. Arduino %i not connected \n",
				analogoutput,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Analog Ouput %i cannot be written. Arduino %i >= MAXARDS\n",analogoutput,ard);
      retval = -1;
    }

  }

  return retval;
}

/* retrieve value from given analog input between the range minval and maxval */
int analog_input(int ard, int input, float *value, float minval, float maxval)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */
  int found = 0;

  if (value != NULL) {

    if (ard < MAXARDS) {
      if (arduino[ard].connected) {
	if ((input >= 0) && (input < arduino[ard].nanaloginputs)) {

	  /* Analog input values are flickering with +/-2 (out of a range of 1023)
	     which is because of imprecision of potentiometers etc.
	     A change does not necessarily mean that we turned the potentiometer,
	     so check whether the present value can be found in the history values.
	     if not: we have a real change */
	  /* TBD: maybe check each value against +/- 1 of all history values? */
	  for (int s=1;s<MAXSAVE;s++) {
	    if (arduino[ard].analoginputs[input][0] == arduino[ard].analoginputs[input][s]) found = 1;
	  }

	  /*
	  printf("%i %i %i %i %i %i %i \n",arduino[ard].analoginputs[input][0],
		 arduino[ard].analoginputs[input][1],arduino[ard].analoginputs[input][2],
		 arduino[ard].analoginputs[input][3],arduino[ard].analoginputs[input][4],
		 arduino[ard].analoginputs[input][5],arduino[ard].analoginputs[input][6]
		 ); 
	  */
	  if (!found) {
	    *value = ((float) arduino[ard].analoginputs[input][0]) / (float) pow(2,ANALOGINPUTNBITS) * 
	    (maxval - minval) + minval;
	    retval = 1;
	  }
	  
	} else {
	  if (verbose > 0) printf("Analog Input %i above maximum # of analog inputs %i of Arduino %i \n",
				  input,arduino[ard].nanaloginputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Analog Input %i cannot be read. Arduino %i not connected \n",
				input,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Analog Input %i cannot be read. Arduino %i >= MAXARDS\n",input,ard);
      retval = -1;
    }
    
  }

  return retval;
}


/* wrapper for encoder_input using integer values and multiplier */
int encoder_input(int ard, int input1,  int input2, int *value, int multiplier, int type)
{
  float fvalue = FLT_MISS;
  if (*value != INT_MISS) fvalue = (float) *value;
  int ret = encoder_inputf(ard, input1, input2, &fvalue, (float) multiplier, type);
  if (fvalue != FLT_MISS) *value = (int) lroundf(fvalue);
  return ret;
    
}

/* retrieve encoder value and for given encoder type connected to inputs 1 and 2
   inputs 1 and 2 need to be on the same input bank (each bank has 64 inputs).
   Note that even though we try to capture every bit that changes, turning an 
   optical encoder too fast will result in loss of states since the SISMO ard
   will not capture every state change correctly */
/* two types of encoders: */
/* 1: 2 bit optical rotary encoder (type 3 in xpusb) */
/* 2: 2 bit gray type mechanical encoder */
int encoder_inputf(int ard, int input1, int input2, float *value, float multiplier, int type)
{

  int retval = 0; /* returns 1 if something changed, and 0 if nothing changed, 
		     and -1 if something went wrong */

  char oldcount, newcount; /* encoder integer counters */
  char updown = 0; /* encoder direction */
  char obits[2]; /* bit arrays for 2 bit encoder */
  char nbits[2]; /* bit arrays for 2 bit encoder */
  int s;
  int bank;

  if (value != NULL) {

    if (ard < MAXARDS) {
      if (arduino[ard].connected) {
	if ((input1 >= 0) && (input1 < arduino[ard].ninputs) &&
	    (input2 >= 0) && (input2 < arduino[ard].ninputs)) {
	  if (input1/64 == input2/64) {

	    if (*value != FLT_MISS) {
	    
	      bank = input1/64;
	      if (arduino[ard].inputs_nsave[bank] != 0) {
		s = arduino[ard].inputs_nsave[bank] - 1; /* history slot to read */
		if (s < (MAXSAVE-1)) {
		  /* if not first read, and if any of the inputs have changed then the encoder was moved */
		  if (((arduino[ard].inputs[input1][s] != arduino[ard].inputs[input1][s+1]) ||
		       (arduino[ard].inputs[input2][s] != arduino[ard].inputs[input2][s+1])) &&
		      (arduino[ard].inputs[input1][s+1] != INPUTINITVAL) &&
		      (arduino[ard].inputs[input2][s+1] != INPUTINITVAL)) {
		  
		    if (type == 1) {
		      /* 2 bit optical encoder */

		      /* derive last encoder bit state */
		      obits[0] = arduino[ard].inputs[input1][s+1];
		      obits[1] = arduino[ard].inputs[input2][s+1];
		      /* derive new encoder bit state */
		      nbits[0] = arduino[ard].inputs[input1][s];
		      nbits[1] = arduino[ard].inputs[input2][s];
		      
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
			/* add accelerator by using s as number of queued encoder changes */
			*value = *value + ((float) updown)  * multiplier * (float) (s*2+1);
			retval = 1;
		      }		    
		    } else if (type == 2) {
		      /* 2 bit gray type mechanical encoder */

		      /* derive last encoder count */
		      oldcount = arduino[ard].inputs[input1][s+1]+2*arduino[ard].inputs[input2][s+1];
	  
		      /* derive new encoder count */
		      newcount = arduino[ard].inputs[input1][s]+2*arduino[ard].inputs[input2][s];

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
			/* add accelerator by using s as number of queued encoder changes */
			*value = *value + ((float) updown) * multiplier * (float) (s+1);
			retval = 1;
		      }		    
		    } else {
		      if (verbose > 0) printf("Encoder with Input %i,%i of ard %i need to be of type 1 or 2 \n",
					      input1,input2,ard);
		      retval = -1;
		    }
		  }
		}
	      }
	      if ((retval == 1) && (verbose > 2)) printf("Encoder with Input %i,%i of Arduino %i changed to %f \n",
							 input1,input2,ard,*value);
	    }
	    
	  } else {
	    if (verbose > 0) printf("Encoder with Input %i,%i need to be on same input bank of Arduino %i \n",
				    input1,input2,ard);
	    retval = -1;
	  }
	} else {
	  if (verbose > 0) printf("Encoder with Input %i,%i above maximum # of digital inputs %i of Arduino %i \n",
				  input1,input2,arduino[ard].ninputs,ard);
	  retval = -1;
	}
      } else {
	if (verbose > 2) printf("Encoder with Input %i,%i cannot be read. Arduino %i not connected \n",
				input1,input2,ard);
	retval = -1;
      }
    } else {
      if (verbose > 0) printf("Encoder with Input %i,%i cannot be read. Arduino %i >= MAXARDS\n",input1,input2,ard);
      retval = -1;
    }
    
  }

  return retval;
}
