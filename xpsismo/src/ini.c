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

#include "common.h"
#include "iniparser.h"
#include "libsismo.h"

/* this routine parses the usbiocards.ini file and reads its values */
int read_ini(char ininame[])
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
  char default_sismocard_mac[] = "00:00";
  int default_sismocard_daughter = 0;
 

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
      strcpy(sismocard[i].ip,iniparser_getstring(ini,tmp, default_sismocard_ip));
      sprintf(tmp,"card%i:Port",i);
      sismocard[i].port = iniparser_getint(ini,tmp, default_sismocard_port);
      sprintf(tmp,"card%i:Mac",i);
      strcpy(sismocard[i].mac,iniparser_getstring(ini,tmp, default_sismocard_mac));
      if (sismocard[i].port == default_sismocard_port) {
	printf("SISMOCARD %i NA \n",i);
	sismocard[i].connected = 0;
      } else {
	printf("SISMOCARD %i Address %s Port %i Mac %s \n",i,sismocard[i].ip, sismocard[i].port, sismocard[i].mac);
	ncards++;
	sismocard[i].connected = 1;
      }
    }
    
    printf("\n");
    iniparser_freedict(ini);

  } else {    
    ret = -1;
  }

  return ret;
}
