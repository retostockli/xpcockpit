// Create an instance of the Servo library
Servo servo[MAX_SERVO];

void teensy_init(int8_t pin, int8_t pinmode, int16_t val, int8_t arg1, int8_t arg2, int8_t arg3) {
  if ((pin >= 0) && (pin < MAX_PINS)) {

    /* store pinmode */
    teensy_data.pinmode[pin] = pinmode;

    if (pinmode == PINMODE_INPUT) {
      pinMode(pin, INPUT_PULLUP);
      teensy_data.val[pin][0] = INITVAL;
      teensy_data.val_save[pin] = INITVAL;
      if (DEBUG > 0) {
        Serial.printf("INIT: Teensy pin %i initialized as Digital Input\n", pin);
      }
    } else if (pinmode == PINMODE_OUTPUT) {
      pinMode(pin, OUTPUT);
      teensy_data.val[pin][0] = val;
      if (DEBUG > 0) {
        Serial.printf("INIT: Teensy pin %i initialized as Digital Output\n", pin);
      }
      teensy_write(pin, val);
    } else if (pinmode == PINMODE_PWM) {
      pinMode(pin, OUTPUT);
      teensy_data.val[pin][0] = val;
      // See: https://www.pjrc.com/teensy/td_pulse.html
      // You can't have the chicken and egg at the same time ...
      //analogWriteFrequency(pin, 375000); // Teensy 3.0 pin 3 also changes to 375 kHz
      //analogWriteResolution(12);  // analogWrite value 0 to 4095, or 4096 for high
      if (DEBUG > 0) {
        Serial.printf("INIT: Teensy pin %i initialized as PWM Output\n", pin);
      }
      teensy_write(pin, val);
    } else if (pinmode == PINMODE_ANALOGINPUT) {
      pinMode(pin, INPUT);
      teensy_data.val[pin][0] = INITVAL;
      teensy_data.val_save[pin] = INITVAL;
      if (DEBUG > 0) {
        Serial.printf("INIT: Teensy pin %i initialized as Analog Input\n", pin);
      }
    } else if (pinmode == PINMODE_INTERRUPT) {
      pinMode(pin, INPUT_PULLUP);
      if (DEBUG > 0) {
        Serial.printf("INIT: Teensy pin %i initialized as Interrupt\n", pin);
      }
    } else if (pinmode == PINMODE_I2C) {
      if (DEBUG > 0) {
        Serial.printf("INIT: Teensy pin %i initialized as I2C Interface\n", pin);
      }
    } else if (pinmode == PINMODE_SERVO) {
      if (teensy_data.pinmode[pin] != pinmode) {
        /* Specify the pin number where the motor's control signal is connected, and the motor's minimum and maximum control pulse width, in microseconds. */
        servo[teensy_data.num_servo].attach(pin, SERVO_MINPULSE, SERVO_MAXPULSE);
        teensy_data.arg1[pin] = teensy_data.num_servo;
        /* store servo instance number for this pin */
        teensy_data.num_servo++;
        teensy_data.val[pin][0] = val;
        if (DEBUG > 0) {
          Serial.printf("INIT: Teensy pin %i initialized as Servo Output\n", pin);
        }
        teensy_write(pin, val);
      }
    } else if (pinmode == PINMODE_MOTOR) {
      pinMode(pin, OUTPUT);
      if (arg1 != INITVAL) pinMode(arg1, OUTPUT);
      if (arg2 != INITVAL) pinMode(arg2, OUTPUT);
      if (arg3 != INITVAL) pinMode(arg3, INPUT);
      teensy_data.arg1[pin] = arg1;
      teensy_data.arg2[pin] = arg2;
      teensy_data.arg3[pin] = arg3;
      teensy_data.val[pin][0] = val;
      // See: https://www.pjrc.com/teensy/td_pulse.html
      // You can't have the chicken and egg at the same time ...
      //analogWriteFrequency(pin, 375000); // Teensy 3.0 pin 3 also changes to 375 kHz
      //analogWriteResolution(12);  // analogWrite value 0 to 4095, or 4096 for high
      if (DEBUG > 0) {
        Serial.printf("INIT: Teensy pin %i initialized as Motor Output\n", pin);
        Serial.printf("      with direction pins %i %i \n", arg1, arg2);
        if (arg3 != INITVAL) Serial.printf("      and current sensing pin %i \n", arg3);
      }
      teensy_write(pin, val);
    } else {
      if (DEBUG > 0) {
        Serial.printf("Init ERROR: Wrong Teensy PinMode for pin %i \n", pin);
      }
    }
  } else {
    if (DEBUG > 0) {
      Serial.printf("Init ERROR: Teensy pin %i out of range\n", pin);
    }
  }
}

void teensy_write(int8_t pin, int16_t val) {
  if (val != INITVAL) {
    if ((pin >= 0) && (pin < MAX_PINS)) {
      if (teensy_data.pinmode[pin] == PINMODE_OUTPUT) {
        if (DEBUG > 0) {
          Serial.printf("RECV: Teensy Digital Output pin %i has value %i \n", pin, val);
        }
        teensy_data.val[pin][0] = val;
        if ((val == 0) || (val == 1)) {
          digitalWrite(pin, val);
        }
      } else if (teensy_data.pinmode[pin] == PINMODE_PWM) {
        if (DEBUG > 0) {
          Serial.printf("RECV: Teensy PWM Output pin %i has value %i \n", pin, val);
        }
        teensy_data.val[pin][0] = val;
        analogWrite(pin, val);
      } else if (teensy_data.pinmode[pin] == PINMODE_MOTOR) {
        /* L298N Driver using Pins EN with PWM and IN1/IN2 for Direction */
        if (DEBUG > 0) {
          Serial.printf("RECV: Teensy Motor Output pin %i has value %i \n", pin, val);
        }
        teensy_data.val[pin][0] = val;
        if (val == 1000) {
          /* BRAKE MOTOR */
          digitalWrite(pin, 1);
          digitalWrite(teensy_data.arg1[pin], 0);
          digitalWrite(teensy_data.arg2[pin], 0);
        } else {
          if (val > 0) {
            /* TURN CW */
            analogWrite(pin, val);
            digitalWrite(teensy_data.arg1[pin], 1);
            digitalWrite(teensy_data.arg2[pin], 0);
          } else {
            /* TURN CCW */
            analogWrite(pin, -val);
            digitalWrite(teensy_data.arg1[pin], 0);
            digitalWrite(teensy_data.arg2[pin], 1);
          }
        }
      } else if (teensy_data.pinmode[pin] == PINMODE_SERVO) {
        if ((teensy_data.arg1[pin] < teensy_data.num_servo) && (teensy_data.arg1[pin] >= 0)) {
          if (DEBUG > 0) {
            Serial.printf("RECV: Teensy Servo Output pin %i has value %i \n", pin, val);
          }
          servo[teensy_data.arg1[pin]].write(val);
        }
      } else {
        if (DEBUG > 0) {
          Serial.printf("RECV ERROR: Teensy Pin %i not set for Output, Servo or PWM \n", pin);
        }
      }
    } else {
      if (DEBUG > 0) {
        Serial.printf("RECV ERROR: Teensy Pin %i out of range \n", pin);
      }
    }
  }
}

void teensy_read() {
  int ret;
  int16_t val;
  int h;
  for (int8_t pin = 0; pin < MAX_PINS; pin++) {
    if (teensy_data.pinmode[pin] == PINMODE_INPUT) {
      if (digitalRead(pin)) {
        teensy_data.val[pin][0] = 0; /* If a pin is "HIGH", the switch is open */
      } else {
        teensy_data.val[pin][0] = 1; /* If a pin is "LOW", the switch is connected to ground, thus closed */
      }
      if (teensy_data.val[pin][0] != teensy_data.val_save[pin]) {
        if (DEBUG > 0) {
          Serial.printf("READ: Teensy Pin %i New Digital Value %i\n", pin, teensy_data.val[pin][0]);
        }
        ret = udp_send(TEENSY_TYPE, 0, pin, teensy_data.val[pin][0]);
        if (ret == SENDMSGLEN) {
          teensy_data.val_save[pin] = teensy_data.val[pin][0];
        }
      }
    } else if (teensy_data.pinmode[pin] == PINMODE_INTERRUPT) {
      if (!digitalRead(pin)) {
        if (teensy_data.arg1[pin] == MCP23017_TYPE) {
          mcp23017_read(teensy_data.arg2[pin]);
        } else if (teensy_data.arg1[pin] == INITVAL) {
          /* Interrupt signaling for not yet initialized board */
        } else {
          if (DEBUG > 0) {
            Serial.printf("READ ERROR: Teensy Interrupt on Pin %i for unknown device. Currently only implemented for MCP23017 \n", pin);
          }
        }
        if (DEBUG > 0) {
          Serial.printf("READ: Teensy Interrupt on Pin %i \n", pin);
        }
      }
    } else if (teensy_data.pinmode[pin] == PINMODE_ANALOGINPUT) {
      val = analogRead(pin);

      /* Median Filter input noise of potentiometers */
      int16_t temparr[MAX_HIST];
      int16_t noise = 5;

      memcpy(temparr, teensy_data.val[pin], sizeof(teensy_data.val[pin][0]) * MAX_HIST);
      quicksort(temparr, 0, MAX_HIST - 1);
      int16_t median = temparr[MAX_HIST / 2];

      //Serial.printf("%i %i\n", median, teensy_data.val_save[pin]);

      if (median != INITVAL) {
        /* only send current value if it is outside median and noise */
        if ((val < (median - noise)) || (val > (median + noise)) || (teensy_data.val_save[pin] == INITVAL)) {
          /* only send current value if it is not the same as in last send */
          if (val != teensy_data.val_save[pin]) {
            if (DEBUG > 0) {
              Serial.printf("READ: Teensy Pin %i New Analog Value %i median: %i save: %i \n", pin, val,
                            teensy_data.val_save[pin], median);
            }

            /* send current value */
            ret = udp_send(TEENSY_TYPE, 0, pin, val);

            /* save current value for later comparison */
            teensy_data.val_save[pin] = val;
          }
        }
      }

      /* Shift History of analog inputs and update current value */
      for (h = MAX_HIST - 2; h >= 0; h--) {
        teensy_data.val[pin][h + 1] = teensy_data.val[pin][h];
      }
      teensy_data.val[pin][0] = val;
    }
  }
}