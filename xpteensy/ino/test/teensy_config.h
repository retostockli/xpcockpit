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

#define TEENSY_ID1 0x54 /* T */
#define TEENSY_ID2 0x45 /* E */

#define MAX_DEV 10        // maximum number of teensy controllers
#define MAX_HIST 10       // maximum number of history variables to save
#define MAX_VARS 20       // maximum number of internal variables on teensy
#define MAX_PINS 100      // maximum number of pins on a teensy
#define MAX_MCP23008_PINS 8
#define MAX_MCP23017_PINS 16
#define MAX_PCF8591_PINS 4

#define INITVAL -1          /* initial value of inputs/outputs upon startup */
#define ANALOGINPUTNBITS 10      /* number of bits of analog inputs */
#define ANALOGOUTPUTNBITS 8      /* number of bits of PWM outputs */

#define TEENSY_INIT 1       // Initialization data packet
#define TEENSY_REGULAR 2    // Regular data packet with data
#define TEENSY_RESEND 3     // Request to resend states
#define TEENSY_SHUTDOWN 10  // Request to shutdown

#define TEENSY_20_TYPE 0
#define TEENSY_20PP_TYPE 1
#define TEENSY_LC_TYPE 2
#define TEENSY_32_TYPE 3
#define TEENSY_35_TYPE 4
#define TEENSY_36_TYPE 5
#define TEENSY_40_TYPE 6
#define TEENSY_41_TYPE 7
#define VARIABLE_TYPE 20     // Internal variable storage

#define MCP23008_TYPE 100    // 8 I/O Extension via I2C
#define MCP23017_TYPE 101    // 16 I/O Extension via I2C
#define PCF8591_TYPE 110     // 8 bit DAC/DAC via I2C

#define PINMODE_INPUT 1
#define PINMODE_OUTPUT 2
#define PINMODE_PWM 3
#define PINMODE_ANALOGINPUT 4
#define PINMODE_INTERRUPT 5
#define PINMODE_I2C 10

#include <stdint.h>

typedef struct {
  int8_t connected;    // Device connected (1) or not (0)
  int16_t val[MAX_PINS][MAX_HIST];      // new values on device pins (input or output)
  int16_t val_save[MAX_PINS][MAX_HIST]; // previous values on device pins (input or output)
  int8_t type[MAX_PINS];        // what type of pin (input/output/pwm/interrup/i2c etc.)
  int8_t int_dev[MAX_PINS];     // for interrupt pins: for which device type
  int8_t int_dev_num[MAX_PINS]; // for interrupt pins: for which device number of above type
  char ip[30];            // IP address of teensy / server
  int port;               // UDP port teensy is listening / sending
  unsigned char mac[2];   // last two bytes of MAC address  
  int8_t version;         // teensy version
  int8_t num_pins;        // number of I/O pins on this teensy
} teensy_struct;

typedef struct {
  int16_t val[MAX_VARS];      // new values for internal variables
  int16_t val_save[MAX_VARS]; // previous values for internal variables
} teensyvar_struct;

typedef struct {
  int8_t connected; // Device connected (1) or not (0)
  int16_t val[MAX_MCP23008_PINS][MAX_HIST];       // new values on device pins (input or output)
  int16_t val_save[MAX_MCP23008_PINS][MAX_HIST];  // previous values on device pins (input or output)
  int8_t type[MAX_MCP23008_PINS];   // I/O type: Input or output
  int8_t intpin;    // Interrupt pin on teensy to read this device
  int8_t wire;      // I2C bus (0,1,2)
  uint8_t address;   // I2C address 0x00 - 0xff
} mcp23008_struct;

typedef struct {
  int8_t connected; // Device connected (1) or not (0)
  int16_t val[MAX_MCP23017_PINS][MAX_HIST];       // new values on device pins (input or output)
  int16_t val_save[MAX_MCP23017_PINS][MAX_HIST];  // previous values on device pins (input or output)
  int8_t type[MAX_MCP23017_PINS];   // I/O type: Input or output
  int8_t intpin;     // Interrupt pin on teensy to read this device
  int8_t wire;       // I2C bus (0,1,2)
  uint8_t address;   // I2C address 0x00 - 0xff
} mcp23017_struct;

typedef struct {
  int8_t connected;    // Device connected (1) or not (0)
  int16_t val[MAX_PCF8591_PINS];       // new values on device pins (Analog Inputs)
  int16_t val_save[MAX_PCF8591_PINS];  // previous values on device pins (Analog Inputs)
  int16_t dac;          // Digital to Analog pin Output
  int8_t wire;         // I2C bus (0,1,2)
  uint8_t address;     // I2C address 0x00 - 0xff
} pcf8591_struct;
