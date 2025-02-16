#define RECVMSGLEN 16
#define SENDMSGLEN 16

struct timeval current_time;  // stores time of current loop
int DEBUG;

// define buffers for receiving and sending data
uint8_t recvBuffer[RECVMSGLEN];  // buffer to hold receive packet
uint8_t sendBuffer[SENDMSGLEN];  // buffer to hold send packet

/* Prototypes */
void udp_receive(void);
int udp_send(int8_t dev_type, int8_t dev_num, int8_t pin_num, int16_t val);