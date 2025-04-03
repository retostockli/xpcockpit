Adafruit_PWMServoDriver pca9685[MAX_DEV];

void pca9685_init(int8_t dev, int8_t pin, int8_t pinmode, int8_t wirenum, uint8_t address, int16_t val) {

  if ((pin >= 0) && (pin < PCA9685_MAX_PINS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      // Connect board to I2C bus if not already connected
      if (pca9685_data[dev].connected == 0) {

        if (wirenum == 0) {
          pca9685[dev] = Adafruit_PWMServoDriver(address, Wire);
        } else if (wirenum == 1) {
          pca9685[dev] = Adafruit_PWMServoDriver(address, Wire1);
        } else if (wirenum == 2) {
          pca9685[dev] = Adafruit_PWMServoDriver(address, Wire2);
        }

        if ((wirenum < 0) || (wirenum > 2)) {
          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
          }
        } else {
          if (pca9685[dev].begin()) {
            /* reset wire speed since pca9685 begin sets it to 100 kHz */
            if (wirenum == 0) {
              Wire.setClock(WIRESPEED);
            } else if (wirenum == 1) {
              Wire1.setClock(WIRESPEED);
            } else if (wirenum == 2) {
              Wire2.setClock(WIRESPEED);
            }

            pca9685[dev].setOscillatorFrequency(27000000);
            pca9685[dev].setPWMFreq(50);  // Maximum PWM Frequency for PWM operation
            //pca9685[dev].setPWMFreq(1600);  // Maximum PWM Frequency for PWM operation

            pca9685_data[dev].connected = 1;
            pca9685_data[dev].wire = wirenum;
            pca9685_data[dev].address = address;

            delay(10);

            if (DEBUG > 0) {
              Serial.printf("INIT: PCA9685 Device %i connected to I2C bus %i with address 0x%02x \n", dev, wirenum, address);
            }
          } else {
            if (DEBUG > 0) {
              Serial.printf("INIT: PCA9685 Device %i could not be connected to I2C bus %i with address 0x%02x \n", dev, wirenum, address);
            }
          }
        }
      }

      /* Only initialize pins of connected devices */
      if (pca9685_data[dev].connected == 1) {

        if (pinmode == PINMODE_PWM) {
          pca9685_data[dev].val_save[pin] = INITVAL;  // reset pin state so that it will be sent to client after read
          pca9685_data[dev].pinmode[pin] = pinmode;

          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Device %i Pin %i initialized as PWM with value %i \n", dev, pin, val);
          }
        } else if (pinmode == PINMODE_SERVO) {
          pca9685_data[dev].val_save[pin] = INITVAL;  // reset pin state so that it will be sent to client after read
          pca9685_data[dev].pinmode[pin] = pinmode;

          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Device %i Pin %i initialized as SERVO with value %i \n", dev, pin, val);
          }
        } else {
          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Device %i Pin %i can only be PWM or SERVO\n", dev, pin);
          }
        }
        pca9685_data[dev].val[pin] = INITVAL;
        pca9685_write(dev, pin, val);
      }
    } else {
      if (DEBUG > 0) {
        Serial.printf("INIT: PCA9685 Device Number %i out of range \n", dev);
      }
    }
  } else {
    if (DEBUG > 0) {
      Serial.printf("INIT: PCA9685 Device %i Pin %i out of range \n", dev, pin);
    }
  }
}

void pca9685_write(int8_t dev, int8_t pin, int16_t val) {

  if ((pin >= 0) && (pin < PCA9685_MAX_PINS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      if (pca9685_data[dev].connected == 1) {
        if (pca9685_data[dev].val[pin] != val) {
          if (val != INITVAL) {
            if (pca9685_data[dev].pinmode[pin] == PINMODE_PWM) {

              if ((val >= 0) && (val < pow(2, PCA9685_PWM_NBITS))) {
                pca9685_data[dev].val[pin] = val;
                pca9685[dev].setPin(pin, val);
                //pca9685[dev].setPWM(pin, 0, val);
                if (DEBUG > 0) {
                  Serial.printf("WRITE: PCA9685 Device %i PWM %i has value %i \n", dev, pin, val);
                }
              } else {
                if (DEBUG > 0) {
                  Serial.printf("WRITE: PCA9685 Device %i PWM %i value out of range %i \n", dev, pin, val);
                }
              }
            } else if (pca9685_data[dev].pinmode[pin] == PINMODE_SERVO) {

              if ((val >= PCA9685_SERVO_MINPULSE) && (val <= PCA9685_SERVO_MAXPULSE)) {
                pca9685_data[dev].val[pin] = val;
                pca9685[dev].writeMicroseconds(pin, val);
                if (DEBUG > 0) {
                  Serial.printf("WRITE: PCA9685 Device %i SERVO %i has value %i \n", dev, pin, val);
                }
              } else {
                if (DEBUG > 0) {
                  Serial.printf("WRITE: PCA9685 Device %i SERVO %i value out of range %i \n", dev, pin, val);
                }
              }
            } else {
              if (DEBUG > 0) {
                Serial.printf("WRITE: PCA9685 Device %i Pin %i can only be PWM or SERVO\n", dev, pin);
              }
            }
          }  // not initialization value
        }    // value has changed
      } else {
        if (DEBUG > 0) {
          Serial.printf("WRITE: PCA9685 Device Number %i out of range \n", dev);
        }
      }
    } else {
      if (DEBUG > 0) {
        Serial.printf("WRITE: PCA9685 Device %i Pin %i out of range \n", dev, pin);
      }
    }
  }
}