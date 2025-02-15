// Create an instance of the Servo library
Servo servo[MAX_SERVO];

void teensy_init(int8_t pin, int8_t pinmode, int16_t val, int8_t arg1, int8_t arg2, int8_t arg3, uint8_t arg4, uint8_t arg5, int16_t arg6) {

  int i;
  bool found;

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
    } else if ((pinmode == PINMODE_ANALOGINPUTMEDIAN) || (pinmode == PINMODE_ANALOGINPUTMEAN)) {
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
      /* Specify the pin number where the servo's control signal is connected, and the servo's minimum and maximum control pulse width, in microseconds. */

      /* check first if this servo is already connected */
      found = false;
      for (i = 0; i < teensy_data.num_servo; i++) {
        if ((servo[i].attached()) && (teensy_data.arg1[pin] == i)) {
          if (DEBUG > 0) {
            Serial.printf("INIT: Teensy pin %i already has servo %i defined. No initialization needed.\n", pin, i);
          }
          found = true;
        }
      }
      if (!found) {
        servo[teensy_data.num_servo].attach(pin, SERVO_MINPULSE, SERVO_MAXPULSE);
        teensy_data.arg1[pin] = teensy_data.num_servo;
        /* store servo instance number for this pin */
        teensy_data.num_servo++;
        if (DEBUG > 0) {
          Serial.printf("INIT: Teensy pin %i initialized as Servo Output\n", pin);
        }
      }
      teensy_data.val[pin][0] = val;
      teensy_write(pin, val);
    } else if (pinmode == PINMODE_MOTOR) {
      pinMode(pin, OUTPUT);  // EN PWM Output PIN
      if (arg1 != INITVAL) pinMode(arg1, OUTPUT);
      if (arg2 != INITVAL) pinMode(arg2, OUTPUT);
      if (arg3 != INITVAL) pinMode(arg3, INPUT);
      teensy_data.arg1[pin] = arg1;  // IN1 Output PIN
      teensy_data.arg2[pin] = arg2;  // IN2 Output PIN
      teensy_data.arg3[pin] = arg3;  // Current Sense Analog Input PIN
      teensy_data.arg4[pin] = arg4;  // Motor Minium Speed (0-255B)
      teensy_data.arg5[pin] = arg5;  // Motor Maximum Speed (0-255B)
      teensy_data.arg6[pin] = arg6;  // Motor maximum current (0..1023)
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
      /* ALWAYS STOP MOTOR AT INIT */
      digitalWrite(pin, 0);
      digitalWrite(teensy_data.arg1[pin], 0);
      digitalWrite(teensy_data.arg2[pin], 0);
      /* And then start it with a value if not missing */
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
  float dt;

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
    } else if ((teensy_data.pinmode[pin] == PINMODE_ANALOGINPUTMEDIAN) || (teensy_data.pinmode[pin] == PINMODE_ANALOGINPUTMEAN)) {
      val = analogRead(pin);

      /* Shift History of analog inputs and update current value */
      for (h = MAX_HIST - 2; h >= 0; h--) {
        teensy_data.val[pin][h + 1] = teensy_data.val[pin][h];
      }
      teensy_data.val[pin][0] = val;

      if (teensy_data.pinmode[pin] == PINMODE_ANALOGINPUTMEAN) {
        /* Mean Filter input noise of potentiometers */
        int16_t noise = 6;

        int16_t meanval = calcmean(teensy_data.val[pin], 0, MAX_HIST - 1);
        //Serial.printf("%i %i\n", meanval, teensy_data.val_save[pin]);

        if (meanval != INITVAL) {
          /* only send current value if it is outside mean and noise */
          if ((teensy_data.val_save[pin] < (meanval - noise)) || (teensy_data.val_save[pin] > (meanval + noise)) || (teensy_data.val_save[pin] == INITVAL)) {
            if (meanval != teensy_data.val_save[pin]) {

              // time difference in [ms]
              dt = (float(current_time.tv_sec - teensy_data.val_time[pin].tv_sec) + float(current_time.tv_usec - teensy_data.val_time[pin].tv_usec) / 1000000.0) * 1000.0;

              /* do not send new analog inputs more than 20 times a second */
              if (dt > 50) {

                if (DEBUG > 0) {
                  Serial.printf("READ: Teensy Pin %i New Analog Value %i mean: %i save: %i \n", pin, val,
                                meanval, teensy_data.val_save[pin]);
                }

                /* send current input value */
                ret = udp_send(TEENSY_TYPE, 0, pin, val);

                /* save new mean value for later comparison */
                teensy_data.val_save[pin] = meanval;

                teensy_data.val_time[pin].tv_sec = current_time.tv_sec;
                teensy_data.val_time[pin].tv_usec = current_time.tv_usec;

              } else {
                if (DEBUG > 0) {
                  Serial.printf("READ: Teensy Pin %i New Analog Value %i WAITING TO SEND \n", pin, val);
                }
              }
            }
          }
        }

      } else {
        /* Median Filter input noise of potentiometers */
        /* Can have a time lag since the median filter needs to be filled with new values first */
        int16_t temparr[MAX_HIST];
        int16_t noise = 5;

        memcpy(temparr, teensy_data.val[pin], sizeof(teensy_data.val[pin][0]) * MAX_HIST);
        quicksort(temparr, 0, MAX_HIST - 1);
        int16_t median = temparr[MAX_HIST / 2];

        //Serial.printf("%i %i %i\n", val, median, teensy_data.val_save[pin]);

        if (median != INITVAL) {
          /* only send current value if it is outside median and noise */
          if ((median < (teensy_data.val_save[pin] - noise)) || (median > (teensy_data.val_save[pin] + noise)) || (teensy_data.val_save[pin] == INITVAL)) {

            // time difference in [ms]
            dt = (float(current_time.tv_sec - teensy_data.val_time[pin].tv_sec) + float(current_time.tv_usec - teensy_data.val_time[pin].tv_usec) / 1000000.0) * 1000.0;

            /* do not send new analog inputs more than 20 times a second */
            if (dt > 50) {

              if (DEBUG > 0) {
                Serial.printf("READ: Teensy Pin %i New Analog Value %i median: %i median save: %i \n", pin, val,
                              median, teensy_data.val_save[pin]);
              }

              /* send current value */
              ret = udp_send(TEENSY_TYPE, 0, pin, val);

              /* save new median value for later comparison */
              teensy_data.val_save[pin] = median;

              teensy_data.val_time[pin].tv_sec = current_time.tv_sec;
              teensy_data.val_time[pin].tv_usec = current_time.tv_usec;
            }
          }
        }
      }
    }
  }
}