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

  /*

     NEW PROTOCOL: 10 Bytes on send and receive
     BYTE   | MEANING
     0-1    | Identifier (Characters TE for Teensy)
     2-3    | Last two Bytes of MAC address
     4      | Init Send, Regular Send, Shutdown etc.
     --- FIRST DATA PACKET 5 bytes
     5      | Device Type
     6      | Device Number
     7      | Pin Number
     8-9    | Value (16 bit signed Integer Value)
 
     In case the server sends an initialization as input to a specific pin
     Teensy will send back the current input value. This is important to get
     initial states after every new connect 

  */

int init_teensy() {
  
  int ret;
  int tee;
  int k;

  for (tee=0;tee<MAXTEENSYS;tee++) {
    if (teensy[tee].connected == 1) {

      /* initialize pins selected for digital input */
      memset(teensySendBuffer,0,SENDMSGLEN);
      for (k=0;k<teensy[tee].num_pins;k++) {
	if (teensy[tee].type[k] != INITVAL) {
	  if (verbose > 1) {
	    if (teensy[tee].type[k] == PINMODE_INPUT) printf("Teensy %i Pin %i Initialized as Input \n",tee,k);
	    if (teensy[tee].type[k] == PINMODE_OUTPUT) printf("Teensy %i Pin %i Initialized as Output \n",tee,k);
	    if (teensy[tee].type[k] == PINMODE_PWM) printf("Teensy %i Pin %i Initialized as PWM \n",tee,k);
	    if (teensy[tee].type[k] == PINMODE_ANALOGINPUT) printf("Teensy %i Pin %i Initialized as ANALOG INPUT \n",tee,k);
	    if (teensy[tee].type[k] == PINMODE_INTERRUPT) printf("Teensy %i Pin %i Initialized as INTERRUPT \n",tee,k);
	    if (teensy[tee].type[k] == PINMODE_I2C) printf("Teensy %i Pin %i Initialized as I2C Pin \n",tee,k);
	  }
	  teensySendBuffer[0] = TEENSY_ID1; /* T */
	  teensySendBuffer[1] = TEENSY_ID2; /* E */
	  teensySendBuffer[2] = 0x00; /* does not make sense to set MAC Address of sender since not needed by teensy */
	  teensySendBuffer[3] = 0x00; /* does not make sense to set MAC Address of sender since not needed by teensy */
	  teensySendBuffer[4] = TEENSY_INIT;
	  teensySendBuffer[5] = teensy[tee].version;
	  teensySendBuffer[6] = teensy[tee].type[k];
	  teensySendBuffer[7] = k;
	  teensySendBuffer[8] = 0x00;
	  teensySendBuffer[9] = 0x00;
	  ret = send_udp(teensy[tee].ip,teensy[tee].port,teensySendBuffer,SENDMSGLEN);
	  if (verbose > 2) printf("Sent %i bytes to teensy %i \n", ret,tee);
	  memset(teensySendBuffer,0,SENDMSGLEN);
	} /* pin defined */
      } /* loop over pins */
    } /* teensy connected */
  } /* loop over teensys */

  return 0;
}

#define PINMODE_INPUT 1
#define PINMODE_OUTPUT 2
#define PINMODE_PWM 3
#define PINMODE_ANALOGINPUT 4
#define PINMODE_INTERRUPT 5
#define PINMODE_I2C 10

/* int read_teensy() { */

/*   int tee; */
/*   short int val; */
/*   int input; */
/*   int i,s,p; */
 
/*   //printf("Packets left to read %i \n",udpReadLeft/RECVMSGLEN); */
  
/*   while (udpReadLeft >= RECVMSGLEN) { */
    
/*     tee = -1; */
    
/*     /\* empty UDP receive buffer instead of directly accessing the device *\/ */

/*     pthread_mutex_lock(&udp_exit_cond_lock);     */
/*     /\* read from start of receive buffer *\/ */
/*     memcpy(teensyRecvBuffer,&udpRecvBuffer[0],RECVMSGLEN); */
/*     /\* shift remaining read buffer to the left *\/ */
/*     memmove(&udpRecvBuffer[0],&udpRecvBuffer[RECVMSGLEN],udpReadLeft-RECVMSGLEN);     */
/*     /\* decrease read buffer position and counter *\/ */
/*     udpReadLeft -= RECVMSGLEN; */
/*     pthread_mutex_unlock(&udp_exit_cond_lock); */

/*     /\* decode message *\/ */

/*     /\* check init string is TE *\/ */
/*     if ((teensyRecvBuffer[0] == 0x54) && (teensyRecvBuffer[1] == 0x45)) { */
/*       /\* check MAC Address of Teensy which did send the message *\/ */
      
/*       for (i=0;i<MAXTEENSYS;i++) { */
/* 	if ((teensy[i].mac[0] == teensyRecvBuffer[2]) && (teensy[i].mac[1] == teensyRecvBuffer[3]) && */
/* 	    (teensy[i].connected == 1)) tee = i; */
/*       } */
      
/*       /\* tee found *\/ */
/*       if (tee >= 0) { */

/* 	if (verbose > 3) printf("Read %i bytes from Teensy %i \n",RECVMSGLEN,tee); */

/* 	for (p=0;p<MAXPACKETRECV;p++) { */
	
/* 	  /\* Copy payload 16 bit Integer into variable *\/ */
/* 	  memcpy(&val,&teensyRecvBuffer[p*4+6],sizeof(val)); */

/* 	  input = teensyRecvBuffer[p*4+4]; */
	
/* 	  /\* check type of input *\/ */
/* 	  if (teensyRecvBuffer[p*4+5] == 0x01) { */
/* 	    /\* Digital Input *\/ */
	  
/* 	    if (val != teensy[tee].inputs[input][0]) { */
/* 	      /\* shift all inputs in history array by one *\/ */
/* 	      for (i=0;i<teensy[tee].ninputs;i++) { */
/* 		for (s=MAXSAVE-2;s>=0;s--) { */
/* 		  teensy[tee].inputs[i][s+1] = teensy[tee].inputs[i][s]; */
/* 		} */
/* 	      } */
/* 	      teensy[tee].inputs[input][0] = val;  */
/* 	      if (verbose > 2) printf("Teensy %i Input %i Changed to: %i \n",tee,input,val); */

/* 	      /\* update number of history values per input *\/ */
/* 	      if (teensy[tee].inputs_nsave < MAXSAVE) { */
/* 		teensy[tee].inputs_nsave += 1; */
/* 		if (verbose > 2) printf("Teensy %i Input %i # of History Values %i \n", */
/* 					tee,input,teensy[tee].inputs_nsave); */
/* 	      } else { */
/* 		if (verbose > 0) printf("Teensy %i Input %i Maximum # of History Values %i Reached \n", */
/* 					tee,input,MAXSAVE); */
/* 	      }	       */
/* 	    } */

/* 	  } else if (teensyRecvBuffer[p*4+5] == 0x02) { */
/* 	    /\* Analog Input *\/ */
	  
/* 	    if (val != teensy[tee].analoginputs[input][0]) { */
/* 	      if (verbose > 2) printf("Teensy %i Analog Input %i Changed to %i \n", */
/* 				      tee,input,val); */
/* 	    } */
/* 	    teensy[tee].analoginputs[input][0] = val;	   */
/* 	  } else { */
/* 	    /\* ANY OTHER TYPE OF INPUT OR DATA PACKET: DO NOTHING *\/ */
/* 	  } */

/* 	} /\* loop through data packets in message *\/ */
	
/* 	//printf("Left to Read: %i \n",udpReadLeft); */
	
/*       } else { */
/* 	printf("Teensy with MAC %02x:%02x is not defined in ini file \n", */
/* 	       teensyRecvBuffer[2],teensyRecvBuffer[3]); */
/*       } */
/*     } else { */
/*       printf("Received wrong Init String: %02x %02x \n",teensyRecvBuffer[0],teensyRecvBuffer[1]); */
/*     } */

/*   } /\* while UDP data present in receive buffer *\/ */

/*   return 0; */
/* } */

/* int write_teensy() { */
  
/*   int ret; */
/*   int tee; */
/*   int output; */
/*   int analogoutput; */
/*   short int val; */
/*   int p; */

/*   for (tee=0;tee<MAXTEENSYS;tee++) { */
/*     if (teensy[tee].connected == 1) { */

/*       p = 0; /\* start with first data packet *\/ */
      
/*       /\* Check Digital Outputs for Changes and Send them *\/       */
/*       memset(teensySendBuffer,0,SENDMSGLEN); */
/*       for (output=0;output<teensy[tee].noutputs;output++) { */
/* 	if (teensy[tee].outputs_changed[output] == CHANGED) { */
/* 	  if (verbose > 2) printf("Teensy %i Output %i changed to: %i \n", */
/* 				  tee,output,teensy[tee].outputs[output]); */
/* 	  teensySendBuffer[p*4+4] = output; */
/* 	  teensySendBuffer[p*4+5] = 0x01; */
/* 	  val = teensy[tee].outputs[output]; */
/* 	  memcpy(&teensySendBuffer[p*4+6],&val,sizeof(val)); */
/* 	  teensy[tee].outputs_changed[output] = UNCHANGED; */
/* 	  p++; */
/* 	} */
/* 	if (p == MAXPACKETSEND) { */
/* 	  teensySendBuffer[0] = 0x54; /\* T *\/ */
/* 	  teensySendBuffer[1] = 0x45; /\* E *\/ */
/* 	  teensySendBuffer[2] = 0x00; /\* does not make sense to set MAC Address of sender since not needed by teensy *\/ */
/* 	  teensySendBuffer[3] = 0x00; /\* does not make sense to set MAC Address of sender since not needed by teensy *\/ */
/* 	  ret = send_udp(teensy[tee].ip,teensy[tee].port,teensySendBuffer,SENDMSGLEN); */
/* 	  if (verbose > 2) printf("Sent %i bytes to teensy %i \n", ret,tee); */
/* 	  p=0; */
/* 	  memset(teensySendBuffer,0,SENDMSGLEN); */
/* 	} */
/*       } */
      
/*       /\* Check Analog Outputs (PWM) for Changes and Send them *\/       */
/*       for (analogoutput=0;analogoutput<teensy[tee].nanalogoutputs;analogoutput++) { */
/* 	if (teensy[tee].analogoutputs_changed[analogoutput] == CHANGED) { */
/* 	  if (verbose > 2) printf("Teensy %i Analogoutput %i changed to: %i \n", */
/* 				  tee,analogoutput,teensy[tee].analogoutputs[analogoutput]); */
/* 	  teensySendBuffer[p*4+4] = analogoutput; */
/* 	  teensySendBuffer[p*4+5] = 0x02; */
/* 	  val = teensy[tee].analogoutputs[analogoutput]; */
/* 	  memcpy(&teensySendBuffer[p*4+6],&val,sizeof(val)); */
/* 	  teensy[tee].analogoutputs_changed[analogoutput] = UNCHANGED; */
/* 	  p++; */
/* 	} */
/* 	if ((p == MAXPACKETSEND) || ((p>0)&&(analogoutput==teensy[tee].nanalogoutputs-1))) { */
/* 	  teensySendBuffer[0] = 0x54; /\* T *\/ */
/* 	  teensySendBuffer[1] = 0x45; /\* E *\/ */
/* 	  teensySendBuffer[2] = 0x00; /\* does not make sense to set MAC Address of sender since not needed by teensy *\/ */
/* 	  teensySendBuffer[3] = 0x00; /\* does not make sense to set MAC Address of sender since not needed by teensy *\/ */
/* 	  ret = send_udp(teensy[tee].ip,teensy[tee].port,teensySendBuffer,SENDMSGLEN); */
/* 	  if (verbose > 2) printf("Sent %i bytes to teensy %i \n", ret,tee); */
/* 	  p=0; */
/* 	  memset(teensySendBuffer,0,SENDMSGLEN); */
/* 	} */
/*       } */

/*     } */
/*   } */
  
/*   return 0; */
/* } */



/* /\* wrapper for digital_input with floating point values *\/ */
/* int digital_inputf(int tee, int input, float *fvalue, int type) */
/* { */
/*   int value = INT_MISS; */
/*   if (*fvalue != FLT_MISS) value = (int) lroundf(*fvalue); */
/*   int ret = digital_input(tee, input, &value, type); */
/*   if (value != INT_MISS) *fvalue = (float) value; */
/*   return ret; */
    
/* } */

/* /\* retrieve input value from given input of the teensy *\/ */
/* /\* Two types : *\/ */
/* /\* 0: pushbutton *\/ */
/* /\* 1: toggle switch *\/ */
/* int digital_input(int tee, int input, int *value, int type) */
/* { */

/*   int retval = 0; /\* returns 1 if something changed, and 0 if nothing changed,  */
/* 		     and -1 if something went wrong *\/ */

/*   int s; */

/*   if (value != NULL) { */

/*     if (tee < MAXTEENSYS) { */
/*       if (teensy[tee].connected) { */
/* 	if ((input >= 0) && (input < teensy[tee].ninputs)) { */
/* 	  if (teensy[tee].inputs_nsave != 0) { */
/* 	    s = teensy[tee].inputs_nsave - 1; /\* history slot to read *\/ */
/* 	    if (type == 0) { */
/* 	      /\* simple pushbutton / switch *\/ */
/* 	      if (*value != teensy[tee].inputs[input][s]) { */
/* 		if (verbose > 1) printf("Pushbutton: Teensy %i Input %i Changed to %i %i \n", */
/* 					tee, input, *value,teensy[tee].inputs[input][s]); */
/* 		*value = teensy[tee].inputs[input][s]; */
/* 		retval = 1; */
/* 	      } */

/* 	    } else { */
/* 	      /\* toggle state everytime you press button *\/ */
/* 	      if (s < (MAXSAVE-1)) { */
/* 		/\* check if the switch state changed from 0 -> 1 *\/ */
/* 		if ((teensy[tee].inputs[input][s] == 1) && (teensy[tee].inputs[input][s+1] == 0)) { */
/* 		  /\* toggle *\/ */
/* 		  if (*value != INT_MISS) { */
/* 		    if ((*value == 0) || (*value == 1)) { */
/* 		      *value = 1 - (*value); */
/* 		      retval = 1; */
/* 		      if (verbose > 1) printf("Toogle Switch: Teensy %i Input %i Changed to %i \n", */
/* 					      tee, input, *value); */
/* 		    } else { */
/* 		      printf("Toogle Switch: Teensy %i Input %i Needs to be 0 or 1, but has value %i \n", */
/* 					      tee, input, *value); */
/* 		      retval = -1; */
/* 		    } */
/* 		  } */
/* 		} */
/* 	      } */
/* 	    } */

/* 	  } */
/* 	} else { */
/* 	  if (verbose > 0) printf("Digital Input %i above maximum # of inputs %i of Teensy %i \n", */
/* 				  input,teensy[tee].ninputs,tee); */
/* 	  retval = -1; */
/* 	} */
/*       } else { */
/* 	if (verbose > 2) printf("Digital Input %i cannot be read. Teensy %i not connected \n", */
/* 				input,tee); */
/* 	retval = -1; */
/*       } */
/*     } else { */
/*       if (verbose > 0) printf("Digital Input %i cannot be read. Teensy %i >= MAXTEENSYS\n",input,tee); */
/*       retval = -1; */
/*     } */
    
/*   } */

/*   return retval; */
/* } */


/* /\* wrapper for digital_output when supplying floating point value  */
/*    Values < 0.5 are set to 0 output and values >= 0.5 are set to 1 output *\/ */
/* int digital_outputf(int tee, int output, float *fvalue) { */

/*   int value; */
  
/*   if (*fvalue == FLT_MISS) { */
/*     value = INT_MISS; */
/*   } else if (*fvalue >= 0.5) { */
/*     value = 1; */
/*   } else { */
/*     value = 0; */
/*   } */
    
/*   return digital_output(tee, output, &value); */
/* } */

/* int digital_output(int tee, int output, int *value) */
/* { */
/*   int retval = 0; */

/*   if (value != NULL) { */

/*     if (tee < MAXTEENSYS) { */
/*       if (teensy[tee].connected) { */
/* 	if ((output >= 0) && (output < teensy[tee].noutputs)) { */

/* 	  if (*value != INT_MISS) { */
/* 	    if ((*value == 1) || (*value == 0)) { */
/* 	      if (*value != teensy[tee].outputs[output]) { */
/* 		teensy[tee].outputs[output] = *value; */
/* 		teensy[tee].outputs_changed[output] = CHANGED; */
/* 		if (verbose > 2) printf("Digital Output %i of Teensy %i changed to %i \n", */
/* 					output,tee,*value); */
/* 	      } */
/* 	    } else { */
/* 	      printf("Digital Output %i of Teensy %i should be 0 or 1, but is %i \n", */
/* 		     output,tee,*value); */
/* 	      retval = -1; */
/* 	    } */
/* 	  } */
/* 	} else { */
/* 	  if (verbose > 0) printf("Digital Output %i above maximum # of outputs %i of Teensy %i \n", */
/* 				  output,teensy[tee].noutputs,tee); */
/* 	  retval = -1; */
/* 	} */
/*       } else { */
/* 	if (verbose > 2) printf("Digital Output %i cannot be written. Teensy %i not connected \n", */
/* 				output,tee); */
/* 	retval = -1; */
/*       } */
/*     } else { */
/*       if (verbose > 0) printf("Digital Ouputt %i cannot be written. Teensy %i >= MAXTEENSYS\n",output,tee); */
/*       retval = -1; */
/*     } */

/*   } */

/*   return retval; */
/* } */

/* int analog_output(int tee, int analogoutput, int *value) */
/* { */
/*   int retval = 0; */

/*   if (value != NULL) { */

/*     if (tee < MAXTEENSYS) { */
/*       if (teensy[tee].connected) { */
/* 	if ((analogoutput >= 0) && (analogoutput < teensy[tee].nanalogoutputs)) { */

/* 	  if (*value != INT_MISS) { */
/* 	    if ((*value >= 0) && (*value < (int) pow(2,ANALOGOUTPUTNBITS))) { */
/* 	      if (*value != teensy[tee].analogoutputs[analogoutput]) { */
/* 		teensy[tee].analogoutputs[analogoutput] = *value; */
/* 		teensy[tee].analogoutputs_changed[analogoutput] = CHANGED; */
/* 		if (verbose > 2) printf("Analog Output %i of Teensy %i changed to %i \n", */
/* 					analogoutput,tee,*value); */
/* 	      } */
/* 	    } else { */
/* 	      printf("Analog Output %i of Teensy %i should be between 0 and %i, but is %i \n", */
/* 		     analogoutput,tee,(int) pow(2,ANALOGOUTPUTNBITS)-1,*value); */
/* 	      retval = -1; */
/* 	    } */
/* 	  } */
/* 	} else { */
/* 	  if (verbose > 0) printf("Analog Output %i above maximum # of outputs %i of Teensy %i \n", */
/* 				  analogoutput,teensy[tee].nanalogoutputs,tee); */
/* 	  retval = -1; */
/* 	} */
/*       } else { */
/* 	if (verbose > 2) printf("Analog Output %i cannot be written. Teensy %i not connected \n", */
/* 				analogoutput,tee); */
/* 	retval = -1; */
/*       } */
/*     } else { */
/*       if (verbose > 0) printf("Analog Ouput %i cannot be written. Teensy %i >= MAXTEENSYS\n",analogoutput,tee); */
/*       retval = -1; */
/*     } */

/*   } */

/*   return retval; */
/* } */

/* /\* retrieve value from given analog input between the range minval and maxval *\/ */
/* int analog_input(int tee, int input, float *value, float minval, float maxval) */
/* { */

/*   int retval = 0; /\* returns 1 if something changed, and 0 if nothing changed,  */
/* 		     and -1 if something went wrong *\/ */
/*   int found = 0; */

/*   if (value != NULL) { */

/*     if (tee < MAXTEENSYS) { */
/*       if (teensy[tee].connected) { */
/* 	if ((input >= 0) && (input < teensy[tee].nanaloginputs)) { */

/* 	  /\* Analog input values are flickering with +/-2 (out of a range of 1023) */
/* 	     which is because of imprecision of potentiometers etc. */
/* 	     A change does not necessarily mean that we turned the potentiometer, */
/* 	     so check whether the present value can be found in the history values. */
/* 	     if not: we have a real change *\/ */
/* 	  /\* TBD: maybe check each value against +/- 1 of all history values? *\/ */
/* 	  // for (int s=1;s<MAXSAVE;s++) { */
/* 	  for (int s=1;s<2;s++) { */
/* 	    if (teensy[tee].analoginputs[input][0] == teensy[tee].analoginputs[input][s]) found = 1; */
/* 	  } */
/*           /\* First Time Read *\/ */
/*           if ((teensy[tee].analoginputs[input][0] != INPUTINITVAL) && */
/* 	      (teensy[tee].analoginputs[input][1] == INPUTINITVAL)) { */
/* 	    teensy[tee].analoginputs[input][1] = teensy[tee].analoginputs[input][0]; */
/* 	    found = 0; */
/* 	  } */

/* 	  /\* */
/* 	  printf("%i %i %i %i %i %i %i \n",teensy[tee].analoginputs[input][0], */
/* 		 teensy[tee].analoginputs[input][1],teensy[tee].analoginputs[input][2], */
/* 		 teensy[tee].analoginputs[input][3],teensy[tee].analoginputs[input][4], */
/* 		 teensy[tee].analoginputs[input][5],teensy[tee].analoginputs[input][6] */
/* 		 );  */
/* 	  *\/ */

/* 	  if (!found) { */
/* 	    *value = ((float) teensy[tee].analoginputs[input][0]) / (float) pow(2,ANALOGINPUTNBITS) *  */
/* 	    (maxval - minval) + minval; */
/* 	    retval = 1; */
/* 	  } */
	  
/* 	} else { */
/* 	  if (verbose > 0) printf("Analog Input %i above maximum # of analog inputs %i of Teensy %i \n", */
/* 				  input,teensy[tee].nanaloginputs,tee); */
/* 	  retval = -1; */
/* 	} */
/*       } else { */
/* 	if (verbose > 2) printf("Analog Input %i cannot be read. Teensy %i not connected \n", */
/* 				input,tee); */
/* 	retval = -1; */
/*       } */
/*     } else { */
/*       if (verbose > 0) printf("Analog Input %i cannot be read. Teensy %i >= MAXTEENSYS\n",input,tee); */
/*       retval = -1; */
/*     } */
    
/*   } */

/*   return retval; */
/* } */


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
/*       if (teensy[tee].connected) { */
/* 	if ((input1 >= 0) && (input1 < teensy[tee].ninputs) && */
/* 	    (input2 >= 0) && (input2 < teensy[tee].ninputs)) { */
/* 	  if (*value != FLT_MISS) { */

/* 	    //	    printf("%i %i %i %i \n",teensy[tee].inputs[input1][s],teensy[tee].inputs[input2][s], */
/* 	    //		   teensy[tee].inputs[input1][s+1], teensy[tee].inputs[input2][s+1]); */
	    
/* 	    if (teensy[tee].inputs_nsave != 0) { */
/* 	      s = teensy[tee].inputs_nsave - 1; /\* history slot to read *\/ */
/* 	      if (s < (MAXSAVE-1)) { */
/* 		/\* if not first read, and if any of the inputs have changed then the encoder was moved *\/ */
/* 		if ((((teensy[tee].inputs[input1][s] != teensy[tee].inputs[input1][s+1]) && */
/* 		      (teensy[tee].inputs[input1][s+1] != INPUTINITVAL)) || */
/* 		     ((teensy[tee].inputs[input2][s] != teensy[tee].inputs[input2][s+1]) && */
/* 		      (teensy[tee].inputs[input2][s+1] != INPUTINITVAL))) && */
/* 		    (teensy[tee].inputs[input1][s] != INPUTINITVAL) && */
/* 		    (teensy[tee].inputs[input2][s] != INPUTINITVAL)) { */

/* 		  /\* derive last encoder bit state *\/ */
/* 		  obits[0] = teensy[tee].inputs[input1][s+1]; */
/* 		  obits[1] = teensy[tee].inputs[input2][s+1]; */
		  
/* 		  /\* derive new encoder bit state *\/ */
/* 		  nbits[0] = teensy[tee].inputs[input1][s]; */
/* 		  nbits[1] = teensy[tee].inputs[input2][s]; */
		  
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
/* 				  input1,input2,teensy[tee].ninputs,tee); */
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
