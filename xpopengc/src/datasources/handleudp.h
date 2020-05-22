/* This is the handleserver.h header to the handleserver.c code

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

#ifdef WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

/* UDP CLIENT PARAMETERS */
char udpServerIP[30];
int udpServerPort;
int udpSocket;

struct sockaddr_in udpServAddr;     /* Server address structure */

/* Prototype functions for Network communication */
int init_udp(void);
void exit_udp(void);
int send_udp(void);
int recv_udp(void);
