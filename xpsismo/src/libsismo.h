/* This is the libsismo.h header to the libsismo.c code which contains all functions to interact with
   the SISMO SOLUCIONES hardware

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

#define MAXCARDS 10
#define RECVMSGLEN 28
#define SENDMSGLEN 30
#define RECVBUFLEN RECVMSGLEN*100
#define SENDBUFLEN RECVMSGLEN

extern char xpserver_ip[30];
extern int xpserver_port;
extern char clientname[100];

extern char sismoserver_ip[30];
extern int sismoserver_port;

extern unsigned char recvBuffer[RECVBUFLEN];
extern unsigned char sendBuffer[SENDBUFLEN];

extern int ncards;

typedef struct {
  int connected;
  char ip[30];
  int port;
  char mac[5];
} sismocard_struct;

extern sismocard_struct sismocard[MAXCARDS];

/* Prototype Functions */

void test(void);
