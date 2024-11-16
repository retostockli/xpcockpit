#define DEBUG 1
#define RECVMSGLEN 12
#define SENDMSGLEN 12



// define buffers for receiving and sending data
char recvBuffer[RECVMSGLEN];  // buffer to hold receive packet
char sendBuffer[SENDMSGLEN];  // buffer to hold send packet

/* Prototypes */
void udp_receive(void);
int udp_send(int8_t dev_type, int8_t dev_num, int8_t pin_num, int16_t val);