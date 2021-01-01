/* This is the libsismo.h header to the libsismo.c code which contains all functions to interact with
   the SISMO SOLUCIONES hardware

   Copyright (C) 2020 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#define MAXCARDS 5           /* maximum number of cards that we expect */
#define RECVMSGLEN 30        /* number of bytes in received UDP packet */
#define SENDMSGLEN 28        /* number of bytes in sent UDP packet */
#define MAXANALOGINPUTS 5+10         /* 5 on master and 10 on daughter */
#define MAXINPUTS 64+64+64   /* 64 on master and 2x64 on daughters */
#define MAXOUTPUTS 64+64+64  /* 64 on master and 2x64 on daughters */
#define MAXDISPLAYS 32+32+32 /* 32 on master and 2x32 on daughters */
#define DISPLAYBRIGHTNESS 15 /* 0-15 Brightness of 7 segment displays */
#define MAXSERVOS 14         /* 14 on daughter */
#define MAXSAVE 20           /* maximum number of history values in data structure */
#define INPUTINITVAL -1      /* initial value of inputs upon startup */
#define DISPLAYSINITVAL -1   /* initial value of displays upon startup (BLANK) */
#define OUTPUTSINITVAL 0     /* initial value of outputs upon startup (OFF) */
#define SERVOSINITVAL 1023   /* initial value of servos upon startup (PARK) */
#define UNCHANGED 0          /* flag for unchanged input / output */
#define CHANGED 1            /* flag for changed input / output. 
				Can actually also be > 1 for e.g. analog inputs that change faster than xpsismo cycle */

/* XPSERVER */
extern char xpserver_ip[30];
extern int xpserver_port;
extern char clientname[100];

/* SISMO UDP SERVER (this code here) */
extern char sismoserver_ip[30];
extern int sismoserver_port;
extern unsigned char sismoRecvBuffer[RECVMSGLEN];
extern unsigned char sismoSendBuffer[SENDMSGLEN];

/* SISMO data structure */
extern int ncards;

typedef struct {
  int connected;         /* specific card is connected or not */
  char ip[30];           /* IP address of card */
  int port;              /* UDP port card is listening */
  unsigned char mac[2];  /* last two bytes of MAC address */
  int daughter_output1;       /* connected daughter card outputs 1 (outputs 65-128) */
  int daughter_output2;       /* connected daughter card outputs 2 (outputs 129-192) */
  int daughter_servo;         /* connected daughter card with servos (servos 1-14) */
  int daughter_display1;      /* connected daughter card with displays 1 (displays 33-64) */
  int daughter_display2;      /* connected daughter card with displays 2 (displays 65-92) */
  int daughter_analoginput;   /* connected daughter card with analog inputs (analoginputs 6-15) */
  int daughter_analogoutput;  /* connected daughter card with analog outputs (Not documented yet) */
  int ninputs;           /* actual number of activated inputs */
  int nanaloginputs;     /* actual number of activated analoginputs */
  int nanalogoutputs;    /* actual number of activated analog outputs (INOP) */
  int noutputs;          /* actual number of activated outputs */
  int ndisplays;         /* actual number of activated displays */
  int nservos;           /* actual number of activated servos */
  int analoginputs[MAXANALOGINPUTS];
  unsigned char analoginputs_changed[MAXANALOGINPUTS]; /* unchanged = 0, changed >= 1 */
  int inputs[MAXINPUTS][MAXSAVE];
  unsigned char inputs_changed[MAXINPUTS]; /* unchanged = 0, changed >= 1 */
  int outputs[MAXOUTPUTS];
  unsigned char outputs_changed[MAXOUTPUTS]; /* unchanged = 0, changed = 1 */
  int displays[MAXDISPLAYS];
  unsigned char displays_changed[MAXDISPLAYS]; /* unchanged = 0, changed = 1 */
  int servos[MAXSERVOS];
  unsigned char servos_changed[MAXSERVOS]; /* unchanged = 0, changed = 1 */
} sismo_struct;

extern sismo_struct sismo[MAXCARDS];

/* Prototype Functions */
int read_sismo(void);
int write_sismo(void);
int digital_inputf(int card, int input, float *fvalue, int type);
int digital_input(int card, int input, int *value, int type);
int digital_outputf(int card, int output, float *fvalue);
int digital_output(int card, int output, int *value);
