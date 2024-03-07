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
#include "ini.h"
#include "handleserver.h"
#include "serverdata.h"
#include "xplanebeacon.h"

int verbose;
int is_copilot;

/* this routine parses the usbioards.ini file and reads its values */
int ini_read(char* programPath, char* iniName)
{
  int ret = 0;
  dictionary *ini;
  char filename[255];
  char cwd[200];
  char *pch;
 
  /* general */
  int default_verbose = 0;

  /* xpserver */
  char default_xpserver_ip[] = "127.0.0.1";
  int default_xpserver_port = 8091;

  /* Captain / Copilot Panel */
  int default_copilot = 0;


  /* check if we are in the source code directory or in the binary installation path */
  if (strncmp("/",programPath,1)==0) {
    /* we have full path starting code */
    printf("Starting with full program path \n");
    /* remove program name from path */
#ifdef WIN
    pch = strrchr(programPath,'\\');
#else
    pch = strrchr(programPath,'/');
#endif
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
      snprintf (filename, sizeof(filename), "%s/../share/xppi/%s.ini",programPath,iniName);
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
      snprintf(filename, sizeof(filename),"../share/xppi/%s.ini",iniName);
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
    is_copilot = iniparser_getint(ini,"panel:copilot", default_copilot);
    printf("Hardware runs for Copilot: %i \n",is_copilot);

    printf("\n");
    iniparser_freedict(ini);
    
  } else {    
    ret = -1;
  }

  return ret;
}

/* signal handler in order to clean up when we're interrupted */
/* from the command line (ctrl-c) or by Kill from Terminal */
int ini_signal_handler(void)
{
  int ret = 0;
  
  if (signal(SIGINT, exit_pi) == SIG_ERR) {
    printf("Could not establish new Interrupt signal handler.\n");
    ret = -1;
  }
  if (signal(SIGTERM, exit_pi) == SIG_ERR) {
    printf("Could not establish new Termination signal handler.\n");
    ret = -1;
  }
  return ret;
}

/* Initializing wiringPi */
int init_pi(void)
{
#ifdef PIGPIO

  /* Add this for pigpio not to interfere with signal handlling */
  /* Does not work yet */
/*
  int cfg = gpioCfgGetInternals();
  cfg |= PI_CFG_NOSIGHANDLER;  // (1<<10)
  gpioCfgSetInternals(cfg);
*/

  if (gpioInitialise() < 0) {
    return -1;
  } else {
    return 0;
  }
#else
  // wiringPiSetup(); /* Use Wiring Pi Numbering DEPRECATED */
  wiringPiSetupGpio(); /* Use GPIO / BCM Numbering */
  return 0;
#endif
}

/* Exiting */
void exit_pi(int ret)
{

  /* cancel tcp/ip connection */
  exit_tcpip_client();

  /* cancel beacon client */
  exit_beacon_client();

  /* free local dataref structure */
  clear_dataref();

#ifdef PIGPIO
  /* fix for running soft pwm in fmc: turn off background lighting */
  gpioPWM(19, 0);
  
  /* free gpio handling */
  gpioTerminate();
#endif
  
  if ((ret != SIGINT) && (ret != SIGTERM)) {
    printf("Exiting with status %i \n",ret);
    exit(ret);
  } else {
    /* A signal 2 means CTRL-C by user. All is fine */
    exit(0);
  }
}
