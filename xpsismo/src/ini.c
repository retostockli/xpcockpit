/* This is the ini.c code which reads the ini files to configure the sismo modules

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
#include "libsismo.h"
#include "ini.h"

/* this routine parses the usbiocards.ini file and reads its values */
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

  /* sismo server */
  char default_sismoserver_ip[] = "ANY";
  int default_sismoserver_port = 1026;

  /* sismo cards */
  char default_sismocard_ip[] = "NA";
  int default_sismocard_port = 0;
  int default_sismocard_mac = 0;
 

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
    sprintf(filename,"../share/xpsismo/%s.ini",ininame);
  }

  ini = iniparser_load(filename);

  if (ini != NULL) {
    verbose = iniparser_getint(ini,"general:verbose", default_verbose);
 
    strncpy(clientname,iniparser_getstring(ini,"xpserver:Name", ""),sizeof(clientname));
    if (! strcmp(clientname,"")) strncpy(clientname,PACKAGE_NAME,sizeof(clientname));
    printf("XPSERVER Client name: %s\n", clientname);
    strcpy(xpserver_ip,iniparser_getstring(ini,"xpserver:Address", default_xpserver_ip));
    xpserver_port = iniparser_getint(ini,"xpserver:Port", default_xpserver_port);
    printf("XPSERVER Address %s Port %i \n",xpserver_ip, xpserver_port);

    strcpy(sismoserver_ip,iniparser_getstring(ini,"sismoserver:Address", default_sismoserver_ip));
    sismoserver_port = iniparser_getint(ini,"sismoserver:Port", default_sismoserver_port);
    printf("SISMOSERVER Address %s Port %i \n",sismoserver_ip, sismoserver_port);

    ncards = 0;
    for(i=0;i<MAXCARDS;i++) {
      sprintf(tmp,"card%i:Address",i);
      strcpy(sismo[i].ip,iniparser_getstring(ini,tmp, default_sismocard_ip));
      sprintf(tmp,"card%i:Port",i);
      sismo[i].port = iniparser_getint(ini,tmp, default_sismocard_port);
      sprintf(tmp,"card%i:Mac1",i);
      sismo[i].mac[0] = iniparser_getint(ini,tmp, default_sismocard_mac);
      sprintf(tmp,"card%i:Mac2",i);
      sismo[i].mac[1] = iniparser_getint(ini,tmp, default_sismocard_mac);
      printf("%02x:%02x \n",sismo[i].mac[0],sismo[i].mac[1]);
      if (sismo[i].port == default_sismocard_port) {
	printf("SISMOCARD %i NA \n",i);
      } else {
	printf("SISMOCARD %i Address %s Port %i Mac %02x:%02x \n",i,sismo[i].ip, sismo[i].port,
	       sismo[i].mac[0],sismo[i].mac[1]);
	ncards++;
      }

      /* only mark as connected once we receive the first packet */
      sismo[i].connected = 0;

      /* assume that no daughter card is attached */
      sismo[i].daughter_output1 = 0;
      sismo[i].daughter_output2 = 0;
      sismo[i].daughter_servo = 0;
      sismo[i].daughter_analogoutput = 0;
      sismo[i].daughter_display1 = 0;
      sismo[i].daughter_display2 = 0;
      sismo[i].ninputs = MAXINPUTS;
      sismo[i].nanaloginputs = 5;
      sismo[i].noutputs = 64;  
      sismo[i].ndisplays = 32;
      sismo[i].nservos = 0;
      sismo[i].nanalogoutputs = 0;
    }
    
    printf("\n");
    iniparser_freedict(ini);
    
  } else {    
    ret = -1;
  }

  return ret;
}

int ini_sismodata()
{
  int i,j,k;
  
  for(i=0;i<MAXCARDS;i++) {
    for(j=0;j<MAXANALOGINPUTS;j++) {
      sismo[i].analoginputs[j] = INPUTINITVAL;
      sismo[i].analoginputs_changed[j] = UNCHANGED;
    }
    for(j=0;j<MAXINPUTS;j++) {
      for(k=0;k<MAXSAVE;k++) {
	sismo[i].inputs[j][k] = INPUTINITVAL;
      }
      sismo[i].inputs_changed[j] = UNCHANGED;
    }
    for(j=0;j<MAXOUTPUTS;j++) {
      sismo[i].outputs[j] = OUTPUTSINITVAL;
      sismo[i].outputs_changed[j] = CHANGED;
    }
    for(j=0;j<MAXDISPLAYS;j++) {
      sismo[i].displays[j] = DISPLAYSINITVAL;
      sismo[i].displays_changed[j] = CHANGED;
    }
    for(j=0;j<MAXSERVOS;j++) {
      sismo[i].servos[j] = SERVOSINITVAL;
      sismo[i].servos_changed[j] = UNCHANGED;
    }
 }

  return 0;
}

int reset_sismodata()
{

  int i,j;
  
  for(i=0;i<MAXCARDS;i++) {

    if (sismo[i].connected == 1) {
      for(j=0;j<MAXANALOGINPUTS;j++) {
	sismo[i].analoginputs_changed[j] = UNCHANGED;
      }
      for(j=0;j<MAXINPUTS;j++) {
	sismo[i].inputs_changed[j] = UNCHANGED;
      }
      for(j=0;j<MAXOUTPUTS;j++) {
	sismo[i].outputs_changed[j] = UNCHANGED;
      }
      for(j=0;j<MAXDISPLAYS;j++) {
	sismo[i].displays_changed[j] = UNCHANGED;
      }
      for(j=0;j<MAXSERVOS;j++) {
	sismo[i].servos_changed[j] = UNCHANGED;
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
  
  if (signal(SIGINT, exit_sismo) == SIG_ERR) {
    printf("Could not establish new signal handler.\n");
    ret = -1;
  }
  return ret;
}

/* Exiting */
void exit_sismo(int ret)
{
  if (ret != 2) {
    printf("Exiting with status %i \n",ret);
    exit(ret);
  } else {
    /* A signal 2 means CTRL-C by user. All is fine */
    exit(0);
  }
}
