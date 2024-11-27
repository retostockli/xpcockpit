Adafruit_PWMServoDriver pca9685[MAX_DEV];

void pca9685_init(int8_t dev, int8_t pin, int8_t pinmode, int8_t wirenum, uint8_t address, int8_t intpin, int16_t initval) {

  if ((pin >= 0) && (pin < MAX_PCA9685_PINS)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      // Connect board to I2C bus if not already connected
      if (pca9685_data[dev].connected == 0) {

        if (wirenum == 0) {
          pca9685[dev] = Adafruit_PWMServoDriver(address, Wire);
        } else if (wirenum == 1) {
          pca9685[dev] = Adafruit_PWMServoDriver(address, Wire1);
        } else if (wirenum == 2) {
          pca9685[dev] = Adafruit_PWMServoDriver(address, Wire2);
        } else {
          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
          }
        }

        pca9685_data[dev].connected = 1;
        pca9685_data[dev].wire = wirenum;
        pca9685_data[dev].address = address;
      }

      /* Only initialize pins of connected devices */
      if (pca9685_data[dev].connected == 1) {

        if (pinmode == PINMODE_PWM) {
          pca9685_data[dev].val_save[pin] = INITVAL;  // reset pin state so that it will be sent to client after read
          pca9685_data[dev].pinmode[pin] = pinmode;

          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Device %i Pin %i initialized as PWM \n", dev, pin);
          }
        } else if (pinmode == PINMODE_SERVO) {
          pca9685[dev].begin();
          pca9685[dev].setOscillatorFrequency(27000000);
          pca9685[dev].setPWMFreq(50);  // Analog servos run at ~50 Hz updates

          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Device %i Pin %i initialized as SERVO \n", dev, pin);
          }
        } else {
          if (DEBUG > 0) {
            Serial.printf("INIT: PCA9685 Device %i Pin %i can only be PWM or SERVO\n", dev, pin);
          }
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
}

void pca9685_write(int8_t dev, int8_t pin, int16_t val) {
  
          pca9685[dev].writeMicroseconds(pin, 2400);
}