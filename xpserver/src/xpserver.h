/* This is the xpserver.h header file to xpserver.c

   Copyright (C) 2009 - 2014  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#ifndef XPSERVER_EXPORT
#ifdef WIN
#define XPSERVER_EXPORT __declspec(dllexport)
#else
#define XPSERVER_EXPORT __attribute__((visibility("default")))
#endif
#endif

/* TCP/IP Server Definitions */
#define MAXPENDING 5         /* Maximum outstanding connection requests */
#define BLOCKING 0
#define NON_BLOCKING 1

/* TCP/IP server/client definitions */
struct sockaddr_in echoServAddr;    /* Local address */
struct sockaddr_in echoClntAddr;    /* Client address */
int servSock;                       /* Socket descriptor for listening server */
int servSockMax;                    /* Maximum Socket descriptor (depends on number of clients) */
int nConnect;                       /* number of concurrent sockets connected */
fd_set socketSetMaster;             /* Master file descriptor list */
fd_set socketSetRead;               /* temporary file descriptor list */
int clntSockArray[MAXCONNECT];      /* client socket array */           
char clntAddrArray[MAXCONNECT][16]; /* client IP Address array */

/* dynamic data structures for all clients */
clientdata_struct *clientdataArray[MAXCONNECT];
int numallocArray[MAXCONNECT];
int numlinkArray[MAXCONNECT];
char clientnameArray[MAXCONNECT][100];

/* window control */
XPLMWindowID	xpserverWindow = NULL;
int             xpserverCloseWindow = 0;
