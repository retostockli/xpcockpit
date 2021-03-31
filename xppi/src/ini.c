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

int verbose;
int is_copilot;

/* this routine parses the usbioards.ini file and reads its values */
int ini_read(char ininame[])
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
/* from the command line (ctrl-c) */
int ini_signal_handler(void)
{
  int ret = 0;
  
  if (signal(SIGINT, exit_pi) == SIG_ERR) {
    printf("Could not establish new signal handler.\n");
    ret = -1;
  }
  return ret;
}

/* Initializing wiringPi */
int init_pi(void)
{
  wiringPiSetup();
  return 0;
}

/* Exiting */
void exit_pi(int ret)
{

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
