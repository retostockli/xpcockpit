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
  int i,j,k,l;
  char tmp[50];
  char version[10];
  int ival;
 
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
  int default_teensy_daughter = 0;
  char default_teensy_version[] = "NA";
  

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

    for(i=0;i<MAXTEENSYS;i++) {
      sprintf(tmp,"teensy%i:Address",i);
      strcpy(teensy[i].ip,iniparser_getstring(ini,tmp, default_teensy_ip));
      sprintf(tmp,"teensy%i:Port",i);
      teensy[i].port = iniparser_getint(ini,tmp, default_teensy_port);
      sprintf(tmp,"teensy%i:Mac1",i);
      teensy[i].mac[0] = iniparser_getint(ini,tmp, default_teensy_mac);
      sprintf(tmp,"teensy%i:Mac2",i);
      teensy[i].mac[1] = iniparser_getint(ini,tmp, default_teensy_mac);
      sprintf(tmp,"teensy%i:Version",i);
      strcpy(version,iniparser_getstring(ini,tmp, default_teensy_version));

      for (k=0;k<MAX_PINS;k++) {
	for (l=0;l<MAX_HIST;l++) {
	  teensy[i].val[k][l] = INITVAL;
	  teensy[i].val_save[k][l] = INITVAL;
	}
	teensy[i].type[k] = INITVAL;
	teensy[i].int_dev[k] = INITVAL;
	teensy[i].int_dev_num[k] = INITVAL;
      }
       
       if (teensy[i].port == default_teensy_port) {
	printf("Teensy %i Not Connected \n",i);
	teensy[i].connected = 0;
      } else if (strcmp(version,default_teensy_version)==0) {
	printf("Teensy %i No Version Number \n",i);
	teensy[i].connected = 0;
      } else {
	if (strcmp(version,"4.1")==0) {
	  teensy[i].version = TEENSY_41_TYPE;
	  teensy[i].num_pins = 42;
	} else {
	  teensy[i].version = INITVAL;
	  teensy[i].num_pins = INITVAL;
	}
	printf("Teensy %i Version %s Address %s Port %i Mac %02x:%02x \n",i,
	       version,
	       teensy[i].ip, teensy[i].port,
	       teensy[i].mac[0],teensy[i].mac[1]);
	teensy[i].connected = 1;

	printf("Connected Daughter Boards:\n");
	sprintf(tmp,"teensy%i:MCP23008",i);
	ival = iniparser_getint(ini,tmp, default_teensy_daughter);
 	printf("MCP23008: %i\n",ival);
	for (j=0;j<MAX_DEV;j++) {
	  for (k=0;k<MAX_MCP23008_PINS;k++) {
	    for (l=0;l<MAX_HIST;l++) {
	      mcp23008[i][j].val[k][l] = INITVAL;
	      mcp23008[i][j].val_save[k][l] = INITVAL;
	    }
	    mcp23008[i][j].type[k] = INITVAL;
	  }
	  mcp23008[i][j].intpin = INITVAL;
	  mcp23008[i][j].wire = INITVAL;
	  mcp23008[i][j].address = 0;
	  if (j<ival) {
	    mcp23008[i][j].connected = 1;
	  } else {
	    mcp23008[i][j].connected = 0;
	  }
	}

	sprintf(tmp,"teensy%i:MCP23017",i);
	ival = iniparser_getint(ini,tmp, default_teensy_daughter);
 	printf("MCP23017: %i\n",ival);
	for (j=0;j<MAX_DEV;j++) {
	  for (k=0;k<MAX_MCP23017_PINS;k++) {
	    for (l=0;l<MAX_HIST;l++) {
	      mcp23017[i][j].val[k][l] = INITVAL;
	      mcp23017[i][j].val_save[k][l] = INITVAL;
	    }
	    mcp23017[i][j].type[k] = INITVAL;
	  }
	  mcp23017[i][j].intpin = INITVAL;
	  mcp23017[i][j].wire = INITVAL;
	  mcp23017[i][j].address = 0;
	  if (j<ival) {
	    mcp23017[i][j].connected = 1;
	  } else {
	    mcp23017[i][j].connected = 0;
	  }
	}

	sprintf(tmp,"teensy%i:PCF8591",i);
	ival = iniparser_getint(ini,tmp, default_teensy_daughter);
 	printf("PCF8591: %i\n",ival);
	for (j=0;j<MAX_DEV;j++) {
	  for (k=0;k<MAX_PCF8591_PINS;k++) {
	    pcf8591[i][j].val[k] = INITVAL;
	    pcf8591[i][j].val_save[k] = INITVAL;
	  }
	  pcf8591[i][j].dac = INITVAL;
	  pcf8591[i][j].wire = INITVAL;
	  pcf8591[i][j].address = 0;
	  if (j<ival) {
	    pcf8591[i][j].connected = 1;
	  } else {
	    pcf8591[i][j].connected = 0;
	  }
	}

      }
	
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

  return 0;
}

int reset_teensydata()
{
  /* The changed flag is modified by the read function that first reads the input 
     or writes the output etc. */
  
  int i;
  
  for(i=0;i<MAXTEENSYS;i++) {

    /* if (teensy[i].connected == 1) { */
    /*   if (teensy[i].inputs_nsave > 0) teensy[i].inputs_nsave -= 1; */
    /* } */

    /* /\* shift all analog inputs in history array by one *\/ */
    /* for (j=0;j<teensy[i].nanaloginputs;j++) { */
    /*   for (s=MAXSAVE-2;s>=0;s--) { */
    /* 	teensy[i].analoginputs[j][s+1] = teensy[i].analoginputs[j][s]; */
    /*   } */
    /* } */
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
