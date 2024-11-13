void mcp23017_init(int8_t dev, int8_t pin, int8_t pinmode, int8_t wirenum, uint8_t address, int8_t intpin, int16_t initval) {
  Adafruit_MCP23X17 mcp;
  if ((pin >=0) && (pin < MAX_MCP23017_PINS)) {
    if ((dev >=0) && (dev < MAX_DEV)) {
      // Connect board to I2C bus if not already connected
      if (mcp23017_data[dev].connected == 0) {
        int ret = 0;
        if (wirenum == 0) {
          ret = mcp.begin_I2C(address, &Wire);
        } else if (wirenum == 1) {
          ret = mcp.begin_I2C(address, &Wire1);
        } else if (wirenum == 2) {
          ret = mcp.begin_I2C(address, &Wire2);
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
          mcp23017_data[dev].connected = 1;
          mcp23017_data[dev].wire = wirenum;
          mcp23017_data[dev].address = address;
          mcp23017_data[dev].intpin = intpin;
          mcp.setupInterrupts(true, true, LOW); /* configure Interrupt mode of MCP23017 */
          if (teensy_data.pinmode[intpin] == PINMODE_INTERRUPT) {
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
  
      if (pinmode == PINMODE_INPUT) {
        mcp23017_data[dev].pinmode[pin] = pinmode;
        mcp.pinMode(pin, INPUT_PULLUP);  // set all pins on board #1 as inputs
        mcp.setupInterruptPin(pin, CHANGE);
      } else if (pinmode == PINMODE_OUTPUT) {
        mcp23017_data[dev].pinmode[pin] = pinmode;
        mcp.pinMode(pin, OUTPUT);
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

void mcp23017_write(int8_t dev, int8_t pin, int16_t val) {
  Adafruit_MCP23X17 mcp;
  uint8_t bitstate = 0;
  uint8_t mask = 0;
  if ((pin >=0) && (pin < MAX_MCP23017_PINS)) {
    if ((dev >=0) && (dev < MAX_DEV)) {
      if (mcp23017_data[dev].connected == 1) {
        if (val != mcp23017_data[dev].val[pin]) { 
          if ((val == 0) || (val == 1)) {
            mcp23017_data[dev].val_save[pin] = mcp23017_data[dev].val[pin];
            mcp23017_data[dev].val[pin] = val;
            if (pin < 8) {
              for (int b=0;b<8;b++) {
                /* write all 8 bits */
                if (mcp23017_data[dev].val[b]  == 1) {
                  mask = 1 << b;
                  bitstate = bitstate | mask;
                }
              }
              mcp.writeGPIOA(bitstate);
            } else {
              for (int b=0;b<8;b++) {
                /* write all 8 bits */
                if (mcp23017_data[dev].val[b+8]  == 1) {
                  mask = 1 << b;
                  bitstate = bitstate | mask;
                }
              }
              mcp.writeGPIOB(bitstate);
            }
          }
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
 
