Adafruit_MCP23X17 mcp23017[MAX_DEV];

/* Use 10k pull down resistor on interrupt if you run MCP23017 on 5V */

int16_t mcp23017_init(int8_t dev, int8_t pin, int8_t pinmode, int8_t intpin, int8_t wirenum, uint8_t address, int16_t val) {

  int16_t retval = 0;

  if ((pin >= 0) && (pin < MCP23017_MAX_PINS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      // Connect board to I2C bus if not already connected
      if (mcp23017_data[dev].connected == 0) {
        int ret = 0;
        if (wirenum == 0) {
          ret = mcp23017[dev].begin_I2C(address, &Wire);
        } else if (wirenum == 1) {
          ret = mcp23017[dev].begin_I2C(address, &Wire1);
        } else if (wirenum == 2) {
          ret = mcp23017[dev].begin_I2C(address, &Wire2);
        } else {
          retval = ERROR_WIRE_RANGE;
          if (DEBUG > 0) {
            Serial.printf("INIT: MCP23017 Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
          }
        }

        /* reset wire speed since mcp23017 begin may reset it to 100 kHz */
        if (wirenum == 0) {
          Wire.setClock(WIRESPEED);
        } else if (wirenum == 1) {
          Wire1.setClock(WIRESPEED);
        } else if (wirenum == 2) {
          Wire2.setClock(WIRESPEED);
        }

        if (ret == 0) {
          retval = ERROR_INIT;
          if (DEBUG > 0) {
            Serial.printf("INIT: MCP23017 Device %i could not be connected to I2C bus %i with address 0x%02x \n", dev, wirenum, address);
          }
        } else {
          if (intpin == INITVAL) {
            /* No interrupt pin: only outputs */
            mcp23017_data[dev].connected = 1;
            mcp23017_data[dev].wire = wirenum;
            mcp23017_data[dev].address = address;
            if (DEBUG > 0) {
              Serial.printf("INIT: MCP23017 Device %i connected to I2C bus %i with address 0x%02x (OUTPUT ONLY)\n", dev, wirenum, address);
            }
          } else {
            /* Interrupt pin: used for inputs */
            if (teensy_data.pinmode[intpin] == INITVAL) {
              teensy_data.pinmode[intpin] = PINMODE_INTERRUPT;
              pinMode(intpin, INPUT_PULLUP);
            }

            if (teensy_data.pinmode[intpin] == PINMODE_INTERRUPT) {
              if (DEBUG > 0) {
                Serial.printf("INIT: MCP23017 Device %i connected to I2C bus %i with address 0x%02x with Teensy Interrupt Pin %i \n", dev, wirenum, address, intpin);
              }
              mcp23017_data[dev].connected = 1;
              mcp23017_data[dev].wire = wirenum;
              mcp23017_data[dev].address = address;
              mcp23017_data[dev].intpin = intpin;
              // mirror INTA/B so only one wire required
              // active drive so INTA/B will not be floating
              // INTA/B will be signaled with a LOW
              mcp23017[dev].setupInterrupts(true, false, LOW); /* configure Interrupt mode of MCP23017 */

              teensy_data.arg1[intpin] = MCP23017_TYPE;
              teensy_data.arg2[intpin] = dev;
            } else {
              retval = ERROR_INIT_INTERRUPT;
              if (DEBUG > 0) {
                Serial.printf("INIT: MCP23017 Device %i Interrupt pin %i was not defined as Interrupt on Teensy \n", dev, intpin);
              }
            }
          } /* INTERRUPT PIN DEFINED */
        }   /* I2C device could be initialized */
      }

      /* Only initialize pins of connected devices */
      if (mcp23017_data[dev].connected == 1) {

        if (pinmode == PINMODE_INPUT) {
          mcp23017_data[dev].val_save[pin] = INITVAL;  // reset pin state so that it will be sent to client after read
          mcp23017_data[dev].pinmode[pin] = pinmode;
          mcp23017[dev].pinMode(pin, INPUT_PULLUP);      // set pin as input with pullup resistor
          mcp23017[dev].setupInterruptPin(pin, CHANGE);  // send an interrupt when pin changes state
          if (DEBUG > 0) {
            Serial.printf("INIT: MCP23017 Device %i Pin %i initialized as INPUT \n", dev, pin);
          }
          /* read input asap when initialized */
          mcp23017_read(dev);
        } else if (pinmode == PINMODE_OUTPUT) {
          if (DEBUG > 0) {
            Serial.printf("INIT: MCP23017 Device %i Pin %i initialized as OUTPUT \n", dev, pin);
          }
          mcp23017_data[dev].pinmode[pin] = pinmode;
          mcp23017[dev].pinMode(pin, OUTPUT);
          mcp23017_write(dev, pin, val);
        } else {
          retval = ERROR_PINMODE;
          if (DEBUG > 0) {
            Serial.printf("INIT: MCP23017 Device %i Pin %i can only be INPUT or OUTPUT \n", dev, pin);
          }
        }
      } 
    } else {
      retval = ERROR_DEV_RANGE;
      if (DEBUG > 0) {
        Serial.printf("INIT: MCP23017 Device %i out of range \n", dev);
      }
    }
  } else {
    retval = ERROR_PIN_RANGE;
    if (DEBUG > 0) {
      Serial.printf("INIT: MCP23017 Device %i Pin %i out of range \n", dev, pin);
    }
  }

  return retval;
}

int16_t mcp23017_write(int8_t dev, int8_t pin, int16_t val) {

  int16_t retval = 0;

  uint8_t bitstate = 0;
  uint8_t mask = 0;
  if ((pin >= 0) && (pin < MCP23017_MAX_PINS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      if (mcp23017_data[dev].connected == 1) {
        if (val != mcp23017_data[dev].val[pin]) {
          if ((val == 0) || (val == 1)) {
            mcp23017_data[dev].val[pin] = val;
            if (pin < 8) {
              for (int b = 0; b < 8; b++) {
                /* write all 8 bits */
                if (mcp23017_data[dev].val[b] == 1) {
                  mask = 1 << b;
                  bitstate = bitstate | mask;
                }
              }
              mcp23017[dev].writeGPIOA(bitstate);
            } else {
              for (int b = 0; b < 8; b++) {
                /* write all 8 bits */
                if (mcp23017_data[dev].val[b + 8] == 1) {
                  mask = 1 << b;
                  bitstate = bitstate | mask;
                }
              }
              mcp23017[dev].writeGPIOB(bitstate);
            }
            if (DEBUG > 1) {
              Serial.printf("WRITE: MCP23017 Device %i Bit Data ", dev);
              for (int i = 0; i < 8; i++) {
                bool b = bitstate & 0x80;
                Serial.printf("%i", b);
                bitstate = bitstate << 1;
              }
              Serial.printf("\n");
            } else if (DEBUG > 0) {
              Serial.printf("WRITE: MCP23017 Device %i Pin %i Digital Output Value %i \n", dev, pin, val);
            }
          }  // value is 0 or 1
        }    // value has changed
      } else {
        retval = ERROR_NOT_CONNECTED;
        if (DEBUG > 0) {
          Serial.printf("WRITE: MCP23017 Device %i not connected \n", dev);
        }
      }
    } else {
      retval = ERROR_DEV_RANGE;
      if (DEBUG > 0) {
        Serial.printf("WRITE: MCP23017 Device Number %i out of range \n", dev);
      }
    }
  } else {
    retval = ERROR_PIN_RANGE;
    if (DEBUG > 0) {
      Serial.printf("WRITE: MCP23017 Device %i Pin %i out of range \n", dev, pin);
    }
  }

  return retval;
}

void mcp23017_read(int8_t dev) {
  uint16_t bitstate;
  int8_t pin;
  uint16_t val;
  int ret;

  //unsigned long Time1;
  //unsigned long Time2;
  //unsigned long Time3;

  if ((dev >= 0) && (dev < MAX_DEV)) {
    if (mcp23017_data[dev].connected == 1) {
      //Time1 = micros();
      bitstate = mcp23017[dev].readGPIOAB();
      //Time2 = micros();
      //Time3 = Time2 - Time1;

      //if (DEBUG > 0) {
      //  Serial.print("MCP23017 Read Time (us): ");
      //  Serial.println(Time3);
      //}

      for (pin = 0; pin < MCP23017_MAX_PINS; pin++) {
        if (mcp23017_data[dev].pinmode[pin] == PINMODE_INPUT) {
          val = 1 - ((bitstate >> pin) & 1); /* HIGH = open (0), LOW: closed (1) */
          mcp23017_data[dev].val[pin] = val;
          if (mcp23017_data[dev].val[pin] != mcp23017_data[dev].val_save[pin]) {
            if (DEBUG > 0) {
              Serial.printf("READ: MCP23017 Device %i Pin %i New Digital Value %i \n", dev, pin, mcp23017_data[dev].val[pin]);
            }
            ret = udp_send(MCP23017_TYPE, dev, TEENSY_REGULAR, pin, mcp23017_data[dev].val[pin]);
            if (ret == SENDMSGLEN) {
              mcp23017_data[dev].val_save[pin] = mcp23017_data[dev].val[pin];
            }
          }
        }
      }
    } else {
      if (DEBUG > 0) {
        Serial.printf("READ: MCP23017 Device %i not connected \n", dev);
      }
    }
  } else {
    if (DEBUG > 0) {
      Serial.printf("INIT: MCP23017 Device Number %i out of range \n", dev);
    }
  }
}