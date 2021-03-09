/* This is the ini.c code which reads the ini files to configure the arduino modules

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
#include <float.h>
#include <sys/types.h>
#include <signal.h>

#include "common.h"
#include "iniparser.h"
#include "libarduino.h"
#include "handleudp.h"
#include "ini.h"
#include "handleserver.h"
#include "serverdata.h"

/* this routine parses the usbioards.ini file and reads its values */
int ini_read(char ininame[])
{
  int ret = 0;
 dictionary *ini;
  char filename[255];
  char cwd[200];
  char *pch;
  int i;
  char tmp[50];

  /* general */
  int default_verbose = 0;

  /* xpserver */
 char default_xpserver_ip[] = "127.0.0.1";
  int default_xpserver_port = 8091;

  /* arduino server */
  char default_arduinoserver_ip[] = "ANY";
  int default_arduinoserver_port = 1026;

  /* arduino ards */
  char default_arduinoard_ip[] = "NA";
  int default_arduinoard_port = 0;
  int default_arduinoard_mac = 0;
 

  /* check if we are in the source code directory or in the binary installation path */
  if (getcwd(cwd, sizeof(cwd)) == NULL) return -2;
#ifdef WIN
  pch = strstr (cwd,"\\bin");
#else
  pch = strstr (cwd,"/bin");
#endif
  if (pch == NULL) {
    sprintf(filename,"../inidata/%s.ini",ininame);
  } else {
    sprintf(filename,"../share/xparduino/%s.ini",ininame);
  }

  ini = iniparser_load(filename);

  if (ini != NULL) {
    verbose = iniparser_getint(ini,"general:verbose", default_verbose);
 
    strncpy(clientname,iniparser_getstring(ini,"xpserver:Name", ""),sizeof(clientname));
    if (! strcmp(clientname,"")) strncpy(clientname,PACKAGE_NAME,sizeof(clientname));
    printf("XPSERVER Client name: %s\n", clientname);
    strcpy(server_ip,iniparser_getstring(ini,"xpserver:Address", default_xpserver_ip));
    server_port = iniparser_getint(ini,"xpserver:Port", default_xpserver_port);
    printf("XPSERVER Address %s Port %i \n",server_ip, server_port);

    strcpy(arduinoserver_ip,iniparser_getstring(ini,"arduinoserver:Address", default_arduinoserver_ip));
    arduinoserver_port = iniparser_getint(ini,"arduinoserver:Port", default_arduinoserver_port);
    printf("SISMOSERVER Address %s Port %i \n",arduinoserver_ip, arduinoserver_port);

    nards = 0;
    for(i=0;i<MAXCARDS;i++) {
      sprintf(tmp,"ard%i:Address",i);
      strcpy(arduino[i].ip,iniparser_getstring(ini,tmp, default_arduinoard_ip));
      sprintf(tmp,"ard%i:Port",i);
      arduino[i].port = iniparser_getint(ini,tmp, default_arduinoard_port);
      sprintf(tmp,"ard%i:Mac1",i);
      arduino[i].mac[0] = iniparser_getint(ini,tmp, default_arduinoard_mac);
      sprintf(tmp,"ard%i:Mac2",i);
      arduino[i].mac[1] = iniparser_getint(ini,tmp, default_arduinoard_mac);
      printf("%02x:%02x \n",arduino[i].mac[0],arduino[i].mac[1]);
      if (arduino[i].port == default_arduinoard_port) {
	printf("SISMOCARD %i NA \n",i);
      } else {
	printf("SISMOCARD %i Address %s Port %i Mac %02x:%02x \n",i,arduino[i].ip, arduino[i].port,
	       arduino[i].mac[0],arduino[i].mac[1]);
	nards++;
      }

      /* only mark as connected once we receive the first packet */
      arduino[i].connected = 0;

      /* assume that no daughter ard is attached */
      arduino[i].daughter_output1 = 0;
      arduino[i].daughter_output2 = 0;
      arduino[i].daughter_servo = 0;
      arduino[i].daughter_analogoutput = 0;
      arduino[i].daughter_display1 = 0;
      arduino[i].daughter_display2 = 0;
      arduino[i].ninputs = MAXINPUTS;
      arduino[i].nanaloginputs = 5;
      arduino[i].noutputs = 64;  
      arduino[i].ndisplays = 32;
      arduino[i].nservos = 0;
      arduino[i].nanalogoutputs = 0;
    }
    
    printf("\n");
    iniparser_freedict(ini);
    
  } else {    
    ret = -1;
  }

  return ret;
}

int ini_arduinodata()
{
  int i,j,k;

  for(i=0;i<MAXCARDS;i++) {
    for(j=0;j<MAXANALOGINPUTS;j++) {
      for(k=0;k<MAXSAVE;k++) {
	arduino[i].analoginputs[j][k]= INPUTINITVAL;
      }
    }
    for(j=0;j<MAXINPUTS/64;j++) {
      arduino[i].inputs_nsave[j] = 0;
    }
    for(j=0;j<MAXINPUTS;j++) {
      for(k=0;k<MAXSAVE;k++) {
	arduino[i].inputs[j][k] = INPUTINITVAL;
      }
    }
    for(j=0;j<MAXOUTPUTS;j++) {
      arduino[i].outputs[j] = OUTPUTSINITVAL;
      arduino[i].outputs_changed[j] = CHANGED;
    }
    for(j=0;j<MAXDISPLAYS;j++) {
      arduino[i].displays[j] = DISPLAYSINITVAL;
      arduino[i].displays_changed[j] = CHANGED;
    }
    for(j=0;j<MAXSERVOS;j++) {
      arduino[i].servos[j] = SERVOSINITVAL;
      arduino[i].servos_changed[j] = UNCHANGED;
    }
 }

  return 0;
}

int reset_arduinodata()
{
  /* The changed flag is modified by the read function that first reads the input 
     or writes the output etc. */
  
  int i,j;
  
  for(i=0;i<MAXCARDS;i++) {

    if (arduino[i].connected == 1) {
      for(j=0;j<(MAXINPUTS/64);j++) {
	if (arduino[i].inputs_nsave[j] > 0) arduino[i].inputs_nsave[j] -= 1;
      }

      /* not needed since these flags are reset during send */
      /*
      for(j=0;j<MAXOUTPUTS;j++) {
	arduino[i].outputs_changed[j] = UNCHANGED;
      }
      for(j=0;j<MAXDISPLAYS;j++) {
	arduino[i].displays_changed[j] = UNCHANGED;
      }
      for(j=0;j<MAXSERVOS;j++) {
	arduino[i].servos_changed[j] = UNCHANGED;
      }
      */
    }
  }
  return 0;
}

/* signal handler in order to clean up when we're interrupted */
/* from the command line (ctrl-c) */
int ini_signal_handler(void)
{
  int ret = 0;
  
  if (signal(SIGINT, exit_arduino) == SIG_ERR) {
    printf("Could not establish new signal handler.\n");
    ret = -1;
  }
  return ret;
}

/* Exiting */
void exit_arduino(int ret)
{

  /* Terminated UDP read thread */
  exit_udp();

  /* cancel tcp/ip connection */
  exit_tcpip();

  /* free local dataref structure */
  clear_dataref();

  if (ret != 2) {
    printf("Exiting with status %i \n",ret);
    exit(ret);
  } else {
    /* A signal 2 means CTRL-C by user. All is fine */
    exit(0);
  }
}
