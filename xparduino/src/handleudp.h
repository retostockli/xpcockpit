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

#include <arpa/inet.h>

/* posix tread headers */
#include <assert.h>
#include <pthread.h>

#define UDPRECVBUFLEN 30*100
#define UDPSENDBUFLEN 28

/* UDP CLIENT PARAMETERS */
extern unsigned char *udpSendBuffer;         /* buffer containing data to send to udp */
extern unsigned char *udpRecvBuffer;         /* buffer containing data that was read from udp */
extern int udpReadLeft;                      /* counter of bytes to read from receive thread */

/* thread parameters */
extern pthread_t poll_thread;                /* read thread */
extern int poll_thread_exit_code;            /* read thread exit code */
extern pthread_mutex_t exit_cond_lock;

/* Prototype functions for Network communication */
int init_udp_server(char server_ip[],int server_port);
int init_udp_receive(void);
void exit_udp(void);
int send_udp(char client_ip[],int client_port,unsigned char data[], int len);
int recv_udp(void);
