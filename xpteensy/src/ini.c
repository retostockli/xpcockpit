/* This is the ini.c code which reads the ini files to configure the teensy modules

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
#include "libteensy.h"
#include "handleudp.h"
#include "ini.h"
#include "handleserver.h"
#include "serverdata.h"
#include "xplanebeacon.h"

/* this routine parses the usbioards.ini file and reads its values */
int ini_read(char* programPath, char* iniName)
{
  int ret = 0;
  dictionary *ini;
  char filename[255];
  char cwd[200];
  char *pch;
  int i,j;
  char tmp[50];
  char sinput[50];
  char sanaloginput[50];
  const char s[2] = ",";
  char *token;
 
  /* general */
  int default_verbose = 0;

  /* xpserver */
 char default_xpserver_ip[] = "";
  int default_xpserver_port = 8091;

  /* teensy server */
  char default_teensyserver_ip[] = "ANY";
  int default_teensyserver_port = 1032;

  /* teensys */
  char default_teensy_ip[] = "NA";
  int default_teensy_port = 0;
  int default_teensy_mac = 0;
  char default_teensy_input[] = "";
 

  /* check if we are in the source code directory or in the binary installation path */
  if (strncmp("/",programPath,1)==0) {
    /* we have full path starting code */
    printf("Starting with full program path \n");
    /* remove program name from path */
    pch = strrchr(programPath,'/');
    *pch = '\0';
    printf("%s\n",programPath);
#ifdef WIN
    pch = strstr (programPath,"\\bin");
#else
    pch = strstr (programPath,"/bin");
#endif
    if (pch == NULL) {
      snprintf (filename, sizeof(filename), "%s/../../inidata/%s.ini",programPath,iniName);
    } else {
      snprintf (filename, sizeof(filename), "%s/../share/xpteensy/%s.ini",programPath,iniName);
    }
  } else {
    /* assume we start from source or bin dir */
    printf("Starting from bin/ or src/ directory \n");
    if (getcwd(cwd, sizeof(cwd)) == NULL) return -2;
#ifdef WIN
    pch = strstr (cwd,"\\bin");
#else
    pch = strstr (cwd,"/bin");
#endif
    if (pch == NULL) {
      snprintf(filename, sizeof(filename),"../inidata/%s.ini",iniName);
    } else {
      snprintf(filename, sizeof(filename),"../share/xpteensy/%s.ini",iniName);
    }
  }

  ini = iniparser_load(filename);

  if (ini != NULL) {
    verbose = iniparser_getint(ini,"general:verbose", default_verbose);
 
    strncpy(clientname,iniparser_getstring(ini,"xpserver:Name", ""),sizeof(clientname));
    if (! strcmp(clientname,"")) strncpy(clientname,PACKAGE_NAME,sizeof(clientname));
    printf("XPSERVER Client name: %s\n", clientname);
    strcpy(XPlaneServerIP,iniparser_getstring(ini,"xpserver:Address", default_xpserver_ip));
    XPlaneServerPort = iniparser_getint(ini,"xpserver:Port", default_xpserver_port);

    strcpy(teensyserver_ip,iniparser_getstring(ini,"teensyserver:Address", default_teensyserver_ip));
    teensyserver_port = iniparser_getint(ini,"teensyserver:Port", default_teensyserver_port);
    printf("SISMOSERVER Address %s Port %i \n",teensyserver_ip, teensyserver_port);

    nteensys = 0;
    for(i=0;i<MAXTEENSYS;i++) {
      sprintf(tmp,"teensy%i:Address",i);
      strcpy(teensy[i].ip,iniparser_getstring(ini,tmp, default_teensy_ip));
      sprintf(tmp,"teensy%i:Port",i);
      teensy[i].port = iniparser_getint(ini,tmp, default_teensy_port);
      sprintf(tmp,"teensy%i:Mac1",i);
      teensy[i].mac[0] = iniparser_getint(ini,tmp, default_teensy_mac);
      sprintf(tmp,"teensy%i:Mac2",i);
      teensy[i].mac[1] = iniparser_getint(ini,tmp, default_teensy_mac);
      sprintf(tmp,"teensy%i:Inputs",i);
      strcpy(sinput,iniparser_getstring(ini,tmp, default_teensy_input));
      sprintf(tmp,"teensy%i:Analoginputs",i);
      strcpy(sanaloginput,iniparser_getstring(ini,tmp, default_teensy_input));
      
      if (teensy[i].port == default_teensy_port) {
	printf("Teensy %i NA \n",i);
	teensy[i].connected = 0;
      } else {
	  
	printf("Teensy %i Address %s Port %i Mac %02x:%02x \n",i,teensy[i].ip, teensy[i].port,
	       teensy[i].mac[0],teensy[i].mac[1]);
	nteensys++;
	teensy[i].connected = 1;

	/* evaluate selected digital inputs */
	for (j=0;j<MAXINPUTS;j++) {
	  teensy[i].inputs_isinput[j] = 0;
	}
	token = strtok(sinput, s);
        while (token) {
	  j = atoi(token);
	  if (j < MAXINPUTS) {
	    printf("Digital Input %i sends data to Server\n",j);
	    teensy[i].inputs_isinput[j] = 1;
	  }
	  token=strtok(NULL,","); 
	}
	
	/* evaluate selected analog inputs */
	for (j=0;j<MAXANALOGINPUTS;j++) {
	  teensy[i].analoginputs_isinput[j] = 0;
	}
	token = strtok(sanaloginput, s);
        while (token) {
	  j = atoi(token);
	  if (j < MAXANALOGINPUTS) {
	    printf("Analog Input %i sends data to Server\n",j);
	    teensy[i].analoginputs_isinput[j] = 1;
	  }
	  token=strtok(NULL,","); 
	}
	
      }

      /* assume that no daughter ard is attached */
      teensy[i].ninputs = MAXINPUTS;
      teensy[i].nanaloginputs = MAXANALOGINPUTS;
      teensy[i].noutputs = MAXOUTPUTS;  
      teensy[i].nanalogoutputs = MAXANALOGOUTPUTS;
      printf("\n");
    }
    
    printf("\n");
    printf("\n");
    iniparser_freedict(ini);
    
  } else {    
    ret = -1;
  }

  return ret;
}

int ini_teensydata()
{
  int i,j,k;

  for(i=0;i<MAXTEENSYS;i++) {
    for(j=0;j<MAXANALOGINPUTS;j++) {
      for(k=0;k<MAXSAVE;k++) {
	teensy[i].analoginputs[j][k]= INPUTINITVAL;
      }
    }
    
    teensy[i].inputs_nsave = 0;

    for(j=0;j<MAXINPUTS;j++) {
      for(k=0;k<MAXSAVE;k++) {
	teensy[i].inputs[j][k] = INPUTINITVAL;
      }
    }
    for(j=0;j<MAXOUTPUTS;j++) {
      teensy[i].outputs[j] = OUTPUTINITVAL;
      teensy[i].outputs_changed[j] = UNCHANGED;
    }
    for(j=0;j<MAXANALOGOUTPUTS;j++) {
      teensy[i].analogoutputs[j] = OUTPUTINITVAL;
      teensy[i].analogoutputs_changed[j] = UNCHANGED;
    }
    
 }

  return 0;
}

int reset_teensydata()
{
  /* The changed flag is modified by the read function that first reads the input 
     or writes the output etc. */
  
  int i,j,s;
  
  for(i=0;i<MAXTEENSYS;i++) {

    if (teensy[i].connected == 1) {
      if (teensy[i].inputs_nsave > 0) teensy[i].inputs_nsave -= 1;
    }

    /* shift all analog inputs in history array by one */
    for (j=0;j<teensy[i].nanaloginputs;j++) {
      for (s=MAXSAVE-2;s>=0;s--) {
	teensy[i].analoginputs[j][s+1] = teensy[i].analoginputs[j][s];
      }
    }
  }
  return 0;
}

/* signal handler in order to clean up when we're interrupted */
/* from the command line (ctrl-c) */
int ini_signal_handler(void)
{
  int ret = 0;
  
 if (signal(SIGINT, exit_teensy) == SIG_ERR) {
    printf("Could not establish new Interrupt signal handler.\n");
    ret = -1;
  }
  if (signal(SIGTERM, exit_teensy) == SIG_ERR) {
    printf("Could not establish new Termination signal handler.\n");
    ret = -1;
  }
  return ret;
}

/* Exiting */
void exit_teensy(int ret)
{

  /* Terminated UDP read thread */
  exit_udp();

  /* cancel tcp/ip connection */
  exit_tcpip_client();

  /* cancel beacon client */
  exit_beacon_client();

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