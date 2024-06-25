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


#define MAX_DEV 10
#define MAX_HIST 10

#define TEENSY_20_TYPE 0
#define TEENSY_20PP_TYPE 1
#define TEENSY_LC_TYPE 2
#define TEENSY_32_TYPE 3
#define TEENSY_35_TYPE 4
#define TEENSY_36_TYPE 5
#define TEENSY_40_TYPE 6
#define TEENSY_41_TYPE 7

#define MCP23008_TYPE 100    // 8 I/O Extension via I2C
#define MCP23017_TYPE 101    // 16 I/O Extension via I2C
#define PCF8591_TYPE 110     // 8 bit DAC/DAC via I2C

#define PINMODE_INPUT 0
#define PINMODE_OUTPUT 1
#define PINMODE_PWM 2
#define PINMODE_ANALOGINPUT 3
#define PINMODE_INTERRUPT 4
#define PINMODE_I2C 10

#include <stdint.h>

typedef struct {
  uint8_t connected;    // Device connected (1) or not (0)
  uint16_t values[42][MAX_HIST];      // new values on device pins (input or output)
  uint16_t values_save[42][MAX_HIST]; // previous values on device pins (input or output)
  uint8_t type[42];        // what type of pin (input/output/pwm/interrup/i2c etc.)
  uint8_t int_device[42];  // for interrupt pins: for which device type
  uint8_t int_number[42];  // for interrupt pins: for which device number of above type
} teensy41_struct;

typedef struct {
  uint8_t connected; // Device connected (1) or not (0)
  uint8_t values[8][MAX_HIST];       // new values on device pins (input or output)
  uint8_t values_save[8][MAX_HIST];  // previous values on device pins (input or output)
  uint8_t type[8];   // I/O type: Input or output
  uint8_t intpin;    // Interrupt pin on teensy to read this device
  uint8_t wire;      // I2C bus (0,1,2)
  uint8_t address;   // I2C address 0x00 - 0xff
} mcp23008_struct;

typedef struct {
  uint8_t connected; // Device connected (1) or not (0)
  uint8_t values[16][MAX_HIST];       // new values on device pins (input or output)
  uint8_t values_save[16][MAX_HIST];  // previous values on device pins (input or output)
  uint8_t type[16];   // I/O type: Input or output
  uint8_t intpin;    // Interrupt pin on teensy to read this device
  uint8_t wire;      // I2C bus (0,1,2)
  uint8_t address;   // I2C address 0x00 - 0xff
} mcp23017_struct;
