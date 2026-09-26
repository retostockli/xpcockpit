/*
 * Write Network and Daughter Configuration to the SISMO SC-MB card with the new Firmware
 * Will flash this data onto the PIC18F86J60 Chip. Boards needs to be re-powered in order
 * for the new network configuration to become effective.
 *
 * Run:
 *     ./writeconfig INIFILE BOARD-IP BOARD-PORT
 *
 * Where:
 *   - INIFILE: the prefix of the .cfg file located in ../inidata
 *   - BOARD-IP: the IP Address where the SC-MB Card is currently reachable. Format: 192.168.1.55
 *   - BOARD-PORT: the UDP Port where the SC-MB Card is currently reachable. Format: 1024
 *
 * The board will respond with a confirmation that this data was written. Without confirmation the code will print an error
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 1026
#define CLIENT_PORT 1024
#define CLIENT_IP "192.168.1.55"
#define BUFFER_SIZE 1000

int serverSocket;

struct timeval oldtime;
struct timeval newtime;


/* send datagram to UDP client */
int send_udp(char client_ip[], int client_port, unsigned char data[], int len) {

  struct sockaddr_in udpClientAddr;     /* Client address structure */
  int n;

  /* Construct the client address structure */
 
  memset(&udpClientAddr, 0, sizeof(udpClientAddr));       /* Zero out structure */
  udpClientAddr.sin_family      = AF_INET;                /* Internet address family */
  udpClientAddr.sin_addr.s_addr = inet_addr(client_ip);   /* Server IP address */
  udpClientAddr.sin_port        = htons(client_port);     /* Server port */
 
  /*
  printf("%i \n",udpClientAddr.sin_family);
  printf("%i \n",udpClientAddr.sin_addr.s_addr);
  printf("%i \n",ntohs(udpClientAddr.sin_port));
  printf("%s \n",udpClientAddr.sin_zero);
  */
  
  n = sendto(serverSocket, data, len, 0,
	     (struct sockaddr *) &udpClientAddr, sizeof(udpClientAddr));

  return n;
}

int main(void)
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    socklen_t clientLen = sizeof(clientAddr);

    char buffer[BUFFER_SIZE];

    int ret;
    int len;

    //----------------------------------------------------------------------
    // Create UDP socket
    //----------------------------------------------------------------------

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if(serverSocket < 0)
    {
        perror("socket");
        return 1;
    }

    //----------------------------------------------------------------------
    // Configure server address
    //----------------------------------------------------------------------

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    //----------------------------------------------------------------------
    // Bind socket
    //----------------------------------------------------------------------

    if(bind(serverSocket,
            (struct sockaddr*)&serverAddr,
            sizeof(serverAddr)) < 0)
    {
        perror("bind");
        close(serverSocket);
        return 1;
    }

    printf("UDP server listening on port %d...\n", SERVER_PORT);

    //----------------------------------------------------------------------
    // Receive loop
    //----------------------------------------------------------------------

    gettimeofday(&oldtime,NULL);

    
    while(1)
    {
        ssize_t receivedBytes;

        memset(buffer, 0, BUFFER_SIZE);

        receivedBytes = recvfrom(serverSocket,
                                 buffer,
                                 BUFFER_SIZE - 1,
                                 0,
                                 (struct sockaddr*)&clientAddr,
                                 &clientLen);

        if(receivedBytes < 0)
        {
            perror("recvfrom");
            continue;
        }

        //------------------------------------------------------------------
        // Print sender info
        //------------------------------------------------------------------
	gettimeofday(&newtime,NULL);
	float dt = ((newtime.tv_sec - oldtime.tv_sec) +
		    (newtime.tv_usec - oldtime.tv_usec) / 1000000.0)*1000.0;

	oldtime.tv_sec = newtime.tv_sec;
	oldtime.tv_usec = newtime.tv_usec;
	
        printf("Received %zd bytes after dt %f \n", receivedBytes, dt);

        /* printf("From %s:%d\n", */
        /*        inet_ntoa(clientAddr.sin_addr), */
        /*        ntohs(clientAddr.sin_port)); */

        /* printf("Message: %s\n", buffer); */

	//len = strlen(buffer);
	
	/* ret = send_udp(CLIENT_IP,CLIENT_PORT,buffer,len); */
	/* if (ret == len) { */
	/*   printf("COMPLETE: Sent %i of %i bytes to Client \n", ret,len); */
	/* } else { */
	/*   printf("INCOMPLETE: Sent %i of %i bytes to Client \n", ret,len); */
	/* } */

    }

    close(serverSocket);

    return 0;
}
