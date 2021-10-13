#include <stdlib.h>
#include "udpdata.h"

char *udpSendBuffer;
char *udpRecvBuffer;
int udpSendBufferLen;
int udpRecvBufferLen;
int udpReadLeft;                      /* counter of bytes to read from receive thread */

void allocate_udpdata(int sendlen, int recvlen) {

  // initialize udp buffers
  udpSendBufferLen = sendlen;
  udpRecvBufferLen = recvlen;
  udpSendBuffer=malloc(udpSendBufferLen);
  udpRecvBuffer=malloc(udpRecvBufferLen);
}

void deallocate_udpdata() {
  free(udpSendBuffer);
  free(udpRecvBuffer);
}


void get_udpdata(char *data, int len) {

  data = udpRecvBuffer;
  len = udpRecvBufferLen;

}
