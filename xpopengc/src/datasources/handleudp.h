/* This is the handleudp.h header to the handleudp.c code

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
extern char udpServerIP[30];
extern int udpServerPort;
extern int udpSocket;

/* thread parameters */
extern pthread_t poll_thread;                /* read thread */
extern int poll_thread_exit_code;            /* read thread exit code */
extern pthread_mutex_t exit_cond_lock;

extern struct sockaddr_in udpServerAddr;     /* Server address structure */
extern struct sockaddr_in udpClientAddr;     /* Client address structure */

/* Prototype functions for Network communication */
int init_udp_server(void);
int init_udp_client(void);
int init_udp_receive();
void *poll_thread_main();
void exit_udp_server(void);
void exit_udp_client(void);
int send_udp_to_server(void);
int recv_udp_from_server(void);
int send_udp_to_client(char client_ip[],int client_port,unsigned char data[], int len);
int recv_udp_from_client(void);
