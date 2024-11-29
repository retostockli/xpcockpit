/* This is the teensy_config.h header which contains all data structures that
   define the hardware configuration of the teensy controller

   Copyright (C) 2024-2024 Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

  /*

     NEW PROTOCOL: 10 Bytes on send and receive
     BYTE   | MEANING
     0-1    | Identifier (Characters TE for Teensy)
     2-3    | Last two Bytes of MAC address
     4      | Init Send, Regular Send, Shutdown etc.
     5      | Device Type
     6      | Device Number
     7      | Pin Number
     8-11   | 32 bit integer data storage
 
     In case the server sends an initialization as input to a specific pin
     Teensy will send back the current input value. This is important to get
     initial states after every new connect 

  */

#define TEENSY_ID1 0x54 /* T */
#define TEENSY_ID2 0x45 /* E */

#define MAX_DEV 10        // maximum number of i2c / spi devices per type
#define MAX_HIST 20       // maximum number of history variables to save

#define INITVAL -1          /* initial value of inputs/outputs upon startup */

/* Teensy Defines */
#define ANALOGINPUT_NBITS 10      /* number of bits of analog inputs */
#define ANALOGOUTPUT_NBITS 8      /* number of bits of PWM outputs */
#define SERVO_MINANGLE 0       /* minimum angle of servos (deg) */
#define SERVO_MAXANGLE 180     /* maximum angle of servos (deg)*/
#define SERVO_MINPULSE 600     /* minimum pulse width of servos (us) default 544 */
#define SERVO_MAXPULSE 2600    /* maximum pulse width of servos (us) default 2400 */
#define MAX_VARS 20       // maximum number of internal variables on teensy
#define MAX_PINS 42       // maximum number of pins on a teensy
#define MAX_SERVO 10      // maximum number of servos on a teensy

/* PCA9685 PWM/Servo Defines */
#define PCA9685_SERVO_MINPULSE 600   // minimum pulse width of servos (us) on PCA9685
#define PCA9685_SERVO_MAXPULSE 2400  // maximum pulse width of servos (us) on PCA9685
#define PCA9685_PWM_NBITS 12  // number of bits of PWM on PCA9685
#define PCA9685_MAX_PINS 16   // maximum number of servos/pwm outputs on a PCA9685

/* MCP23017 16 I/O Defines */
#define MCP23017_MAX_PINS 16  // maximum number of inputs/outputs on a MCP23017

/* PCF8591 4 Analog Inputs Defines */
#define PCF8591_MAX_PINS 4    // maximum number of analog inputs on a PCF8591

#define TEENSY_PING 0       // PING data packet
#define TEENSY_INIT 1       // Initialization data packet
#define TEENSY_REGULAR 2    // Regular data packet with data
#define TEENSY_RESEND 3     // Request to resend states /* INOP */
#define TEENSY_SHUTDOWN 10  // Request to shutdown /* INOP */

#define TEENSY_TYPE 0        // Host Teensy Microcontroller
#define VARIABLE_TYPE 20     // Internal variable storage

#define MCP23017_TYPE 101    // 16 I/O Extension via I2C
#define PCA9685_TYPE 102     // 16 PWM/Servo Extension via I2C
#define PCF8591_TYPE 110     // 8 bit DAC/DAC via I2C


#define PINMODE_INPUT 1
#define PINMODE_OUTPUT 2
#define PINMODE_PWM 3
#define PINMODE_ANALOGINPUT 4
#define PINMODE_INTERRUPT 5
#define PINMODE_SERVO 6
#define PINMODE_I2C 10

#include <stdint.h>

typedef struct {
  int8_t connected;    // Device connected (1) or not (0) as defined in ini file
  int8_t online;       // Device is replying to ping's from xpteensy via UDP
  int8_t initialized;  // Device is initialized
  struct timeval ping_time;  // stores time of last ping reply of teensy (to check if online)
  int16_t val[MAX_PINS][MAX_HIST];    // new values on device pins (input or output)
  int16_t val_save[MAX_PINS];         // previous values on device pins (input or output)
  struct timeval val_time[MAX_PINS];  // stores time since last change (needed for encoder speed multiplier)
  int8_t pinmode[MAX_PINS];    // what type of pin (input/output/pwm/interrup/i2c etc.)
  int8_t arg1[MAX_PINS];       // for Teensy interrupt pins: interrupt for which device type.
                               // for Servos: which servo instance (teensy) or min pulsewidth (us)
  int8_t arg2[MAX_PINS];       // for Teensy interrupt pins: for which device number of above type
                               // For Servos: max pulse width (us)
  char ip[30];                 // IP address of teensy / server
  int port;               // UDP port teensy is listening / sending
  unsigned char mac[2];   // last two bytes of MAC address  
  int16_t num_servo;      // number of initialized servos
} teensy_struct;

typedef struct {
  int16_t val[MAX_VARS];      // new values for internal variables
  int16_t val_save[MAX_VARS]; // previous values for internal variables
} teensyvar_struct;

typedef struct {
  int8_t connected; // Device connected (1) or not (0)
  int16_t val[MCP23017_MAX_PINS];       // new values on device pins (input or output)
  int16_t val_save[MCP23017_MAX_PINS];  // previous values on device pins (input or output)
  struct timeval val_time[MCP23017_MAX_PINS];    // stores time since last change (needed for encoder speed multiplier)
  int8_t pinmode[MCP23017_MAX_PINS];   // I/O type: Input or output
  int8_t intpin;     // Interrupt pin on teensy to read this device
  int8_t wire;       // I2C bus (0,1,2)
  uint8_t address;   // I2C address (0x20 - 0x027)
} mcp23017_struct;

typedef struct {
  int8_t connected; // Device connected (1) or not (0)
  int16_t val[PCA9685_MAX_PINS];       // new values on device pins (input or output)
  int16_t val_save[PCA9685_MAX_PINS];  // previous values on device pins (input or output)
  int8_t pinmode[PCA9685_MAX_PINS];   // I/O type: Input or output
  int8_t wire;       // I2C bus (0,1,2)
  uint8_t address;   // I2C address (0x20 - 0x027)
} pca9685_struct;

typedef struct {
  int8_t connected;    // Device connected (1) or not (0)
  int16_t val[PCF8591_MAX_PINS][MAX_HIST];       // new values on device pins (Analog Inputs)
  int16_t val_save[PCF8591_MAX_PINS];  // previous values on device pins (Analog Inputs)
  int16_t dac;          // Digital to Analog pin Output
  int8_t wire;         // I2C bus (0,1,2)
  uint8_t address;     // I2C address 0x00 - 0xff
} pcf8591_struct;
