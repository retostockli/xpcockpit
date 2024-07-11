#define DEBUG 1
#define RECVMSGLEN 10
#define SENDMSGLEN 10



// define buffers for receiving and sending data
char recvBuffer[RECVMSGLEN];  // buffer to hold receive packet
char sendBuffer[SENDMSGLEN];  // buffer to hold send packet


/* prototypes */
void init(void);
void udp_receive(void);
void udp_send(void);