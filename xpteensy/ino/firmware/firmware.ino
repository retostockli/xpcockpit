/*
   Firmware to communicate to the xpteensy X-Plane Client.
   Made for Teensy 4.1 with ethernet controller on board.
*/

/* USER PARAMETERS PER TEENSY CONTROLLER */
#define PORT 1030
#define MASK "255.255.255.0"
#define GATEWAY "192.168.1.1"

//#define IP "192.168.1.66" /* Boeing 737 Throttle Quadrant */
#define IP "192.168.1.67" /* Boeing 737 Forward Overhead */

//#define WIRESPEED 1000000  /* 1 MHz mostly too fast for some devices */
#define WIRESPEED 400000  /* 400 kHz mostly ok */
//#define WIRESPEED 200000  /* 200 kHz always ok for all devices */
//#define WIRESPEED 100000  /* 100 kHz always ok for all devices */

//#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <QNEthernet.h>
#include <Servo.h>
#include "common.h"
#include "teensy_config.h"
#include <Adafruit_MCP23X17.h>
#include <Adafruit_PWMServoDriver.h>
#include "ht16k33.h"
#include <Wire.h>

using namespace qindesign::network;

extern TwoWire Wire0;
//extern TwoWire Wire1;
//extern TwoWire Wire2;

// data storage
teensy_struct teensy_data;
mcp23017_struct mcp23017_data[MAX_DEV];
pca9685_struct pca9685_data[MAX_DEV];
as5048b_struct as5048b_data[MAX_DEV];
ht16k33_struct ht16k33_data[MAX_DEV];
program_struct program_data[MAX_PROG];

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

IPAddress remoteIP;
int16_t remotePort;

unsigned long StartTime;
unsigned long CurrentTime;
unsigned long ElapsedTime;

void getAddr(const char saddr[], uint8_t addr[4]) {
  const char s[2] = ".";
  char tmp[30];
  strcpy(tmp, saddr);
  char *token;
  int i = 0;
  token = strtok(tmp, s);
  while (token) {
    addr[i] = atoi(token);
    token = strtok(NULL, s);
    i++;
  }
}

void setup() {

  // define storage for MAC address
  uint8_t mac[6];

  while (!Serial && millis() < 3000) {
    // wait up to 15 seconds for Arduino Serial Monitor
  }
  
  Serial.printf("Starting Setup ...\n");

  // start the Ethernet and fetch MAC address of interface
  init_ethernet();

  Ethernet.macAddress(mac);

  // start UDP service
  teensy_data.port = PORT;  // local port to listen on
  teensy_data.mac[0] = mac[4];
  teensy_data.mac[1] = mac[5];
  Udp.begin(teensy_data.port);
  Udp.setReceiveQueueCapacity(1000);
  Serial.printf("UDP: initialized.\n");

  // init teensy data structure
  gettimeofday(&current_time, NULL);
  init_data();

  // Start I2C busses (one for now)
  // Note: if you use a 3.3V --> 5.0V level shifter to run the I2C bus
  // on 5.0V, then please cut the red wire on the I2C bus 5.0V side
  // and use a separate 5.0V power supply. The power pump of the
  // https://www.adafruit.com/product/5649 level shifter draws too much
  // current of the Teensy controller and leads to large noise in the
  // analog inputs
  Wire.begin();
  Wire.setClock(WIRESPEED);
  
  // Wire1.begin();
  // Wire1.setClock(WIRESPEED);
  // Wire2.begin();
  // Wire2.setClock(WIRESPEED);
  
}

void loop() {

  StartTime = micros();

  if (Serial) {
    // Send Debug info if serial monitor is connected
    DEBUG = 1;
  } else {
    DEBUG = 0;
  }

  gettimeofday(&current_time, NULL);


  /* check if new data is available via UDP and update Teensy Outputs with it */
  udp_receive();
  /* check if Teensy inputs have changed and send it out via UDP */
  teensy_read();
  /* read magnetic encoder and send update via UDP if needed */
  as5048b_read();
  /* execute programs if needed */
  for (int prog = 0; prog < MAX_PROG; prog++) {
    if (program_data[prog].connected == 1) {
      if (program_data[prog].type == PROGRAM_CLOSEDLOOP) {
        program_closedloop(prog);
      }
    }
  }

  CurrentTime = micros();
  ElapsedTime = CurrentTime - StartTime;

  if ((ElapsedTime > 1000) && (DEBUG > 0)) {
    Serial.printf("Long Loop Time: %i us\n", ElapsedTime);
  }

  delay(1); /* We need a maximum of 1 ms delay to capture fast changing encoders */
}
