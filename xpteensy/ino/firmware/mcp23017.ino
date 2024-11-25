Adafruit_MCP23X17 mcp23017[MAX_DEV];

void mcp23017_init(int8_t dev, int8_t pin, int8_t pinmode, int8_t wirenum, uint8_t address, int8_t intpin, int16_t initval) {

  if ((pin >= 0) && (pin < MAX_MCP23017_PINS)) {
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
          if (DEBUG) {
            Serial.print("Init: MCP23017 Dev # ");
            Serial.print(dev);
            Serial.print(" Wire Number # ");
            Serial.print(wirenum);
            Serial.println(" Out of Range. Can only be 0, 1 or 2");
          }
        }

        if (ret == 0) {
          if (DEBUG) {
            Serial.print("Init: MCP23017 Device Could not be connected to I2C bus: ");
            Serial.println(dev);
          }
        } else {
          if (teensy_data.pinmode[intpin] == INITVAL) {
            teensy_data.pinmode[intpin] = PINMODE_INTERRUPT;
            pinMode(intpin, INPUT_PULLUP);
          }

          if (teensy_data.pinmode[intpin] == PINMODE_INTERRUPT) {
            if (DEBUG) {
              Serial.print("Init: MCP23017 Device ");
              Serial.print(dev);
              Serial.print(" connected to I2C bus # ");
              Serial.println(wirenum);
            }
            mcp23017_data[dev].connected = 1;
            mcp23017_data[dev].wire = wirenum;
            mcp23017_data[dev].address = address;
            mcp23017_data[dev].intpin = intpin;
            mcp23017[dev].setupInterrupts(true, true, LOW); /* configure Interrupt mode of MCP23017 */

            teensy_data.arg1[intpin] = MCP23017_TYPE;
            teensy_data.arg2[intpin] = dev;
          } else {
            if (DEBUG) {
              Serial.print("Init: MCP23017 Dev # ");
              Serial.print(dev);
              Serial.print(" Interrupt pin # ");
              Serial.print(intpin);
              Serial.println(" Not defined as Interrupt on Teensy");
            }
          }
        }
      }

      /* Only initialize pins of connected devices */
      if (mcp23017_data[dev].connected == 1) {

        if (pinmode == PINMODE_INPUT) {
          mcp23017_data[dev].val_save[pin] = INITVAL;  // reset pin state so that it will be sent to client after read
          mcp23017_data[dev].pinmode[pin] = pinmode;
          mcp23017[dev].pinMode(pin, INPUT_PULLUP);      // set pin as input with pullup resistor
          mcp23017[dev].setupInterruptPin(pin, CHANGE);  // send an interrupt when pin changes state
          if (DEBUG) {
            Serial.print("Init: MCP23017 Dev # ");
            Serial.print(dev);
            Serial.print(" Pin # ");
            Serial.print(pin);
            Serial.println(" initialized as INPUT");
          }
          /* read input asap when initialized */
          mcp23017_read(dev);
        } else if (pinmode == PINMODE_OUTPUT) {
          mcp23017_data[dev].pinmode[pin] = pinmode;
          mcp23017[dev].pinMode(pin, OUTPUT);
          if (DEBUG) {
            Serial.print("Init: MCP23017 Dev # ");
            Serial.print(dev);
            Serial.print(" Pin # ");
            Serial.print(pin);
            Serial.println(" initialized as OUTPUT");
          }
        } else {
          if (DEBUG) {
            Serial.print("Init: MCP23017 Dev # ");
            Serial.print(dev);
            Serial.print(" Pin # ");
            Serial.print(pin);
            Serial.println(" can only be INPUT or OUTPUT.");
          }
        }
      } else {
        if (DEBUG) {
          Serial.print("Init: MCP23017 Device Number out of range: ");
          Serial.println(dev);
        }
      }
    } else {
      if (DEBUG) {
        Serial.print("Init: MCP23017 Dev # ");
        Serial.print(dev);
        Serial.print("Pin # out of range: ");
        Serial.println(pin);
      }
    }
  }
}

void mcp23017_write(int8_t dev, int8_t pin, int16_t val) {

  uint8_t bitstate = 0;
  uint8_t mask = 0;
  if ((pin >= 0) && (pin < MAX_MCP23017_PINS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      if (mcp23017_data[dev].connected == 1) {
        if (val != mcp23017_data[dev].val[pin]) {
          if ((val == 0) || (val == 1)) {
            mcp23017_data[dev].val_save[pin] = mcp23017_data[dev].val[pin];
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
            if (DEBUG) {
              Serial.print("WRITE: MCP23017 Dev # ");
              Serial.print(dev);
              Serial.print(" Pin # ");
              Serial.print(pin);
              Serial.print(" Data: ");
              for (int i = 0; i < 8; i++) {
                bool b = bitstate & 0x80;
                Serial.print(b);
                bitstate = bitstate << 1;
              }
              Serial.println("");
            }
          }
        }
      } else {
        if (DEBUG) {
          Serial.print("Init: MCP23017 Device Number not connected: ");
          Serial.println(dev);
        }
      }
    } else {
      if (DEBUG) {
        Serial.print("Init: MCP23017 Device Number out of range: ");
        Serial.println(dev);
      }
    }
  } else {
    if (DEBUG) {
      Serial.print("Init: MCP23017 Dev # ");
      Serial.print(dev);
      Serial.print("Pin # out of range: ");
      Serial.println(pin);
    }
  }
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

      //if (DEBUG) {
      //  Serial.print("MCP23017 Read Time (us): ");
      //  Serial.println(Time3);
      //}

      for (pin = 0; pin < MAX_MCP23017_PINS; pin++) {
        if (mcp23017_data[dev].pinmode[pin] == PINMODE_INPUT) {
          val = 1 - ((bitstate >> pin) & 1); /* HIGH = open (0), LOW: closed (1) */
          mcp23017_data[dev].val[pin] = val;
          if (mcp23017_data[dev].val[pin] != mcp23017_data[dev].val_save[pin]) {
            if (DEBUG) {
              Serial.print("INTERRUPT: MCP23017 ");
              Serial.print(dev);
              Serial.print(" Pin # ");
              Serial.print(pin);
              Serial.print(" digital value changed to: ");
              Serial.println(mcp23017_data[dev].val[pin]);
            }
            ret = udp_send(MCP23017_TYPE, dev, pin, mcp23017_data[dev].val[pin]);
            if (ret == SENDMSGLEN) {
              mcp23017_data[dev].val_save[pin] = mcp23017_data[dev].val[pin];
            }
          }
        }
      }
    } else {
      if (DEBUG) {
        Serial.print("Init: MCP23017 Device Number not connected: ");
        Serial.println(dev);
      }
    }
  } else {
    if (DEBUG) {
      Serial.print("Init: MCP23017 Device Number out of range: ");
      Serial.println(dev);
    }
  }
}