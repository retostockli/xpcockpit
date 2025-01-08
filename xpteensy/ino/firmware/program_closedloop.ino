int program_closedloop(int prog) {

  int ret = 0;

  int16_t precision = pow(2, ANALOGINPUT_NBITS) / 50;  // servo precision: set to 1%

  if ((prog >= 0) && (prog < MAX_PROG)) {

    int8_t active = program_data[prog].val8[0];
    int8_t pot_pin = program_data[prog].val8[1];
    int8_t mot_pin = program_data[prog].val8[2];

    int16_t servo_request = program_data[prog].val16[0];
    //int16_t servo_min = program_data[prog].val16[1];
    //int16_t servo_max = program_data[prog].val16[2];

    if ((teensy_data.pinmode[pot_pin] != PINMODE_ANALOGINPUTMEDIAN) && (teensy_data.pinmode[pot_pin] != PINMODE_ANALOGINPUTMEAN)) {
      if (DEBUG > 0) Serial.printf("Program %i Closed Loop Motor: Pin %i not defined as Anlog Input\n", prog, pot_pin);
    }

    if (teensy_data.pinmode[mot_pin] != PINMODE_MOTOR) {
      if (DEBUG > 0) Serial.printf("Program %i Closed Loop Motor: Pin %i not defined as Motor Output\n", prog, mot_pin);
    }


    /* closed loop active? */
    if ((active == 1) && (servo_request != INITVAL)) {

      int16_t servo_actual = analogRead(pot_pin);

      bool forward = false;
      bool backward = false;

      uint8_t speed = 255;

      if (servo_request > (servo_actual + precision)) forward = true;
      if (servo_request < (servo_actual - precision)) backward = true;


      if (program_data[prog].val16_save[0] != servo_request) {
        if (DEBUG > 0) {
          Serial.printf("Program %i New Closed Loop Motor Servo Value: %i\n", prog, servo_request);
        }
        program_data[prog].val16_save[0] = servo_request;
      }

      if (forward) {
        /* move motor forward */
        if (DEBUG > 0) {
          Serial.printf("Program %i New Closed Loop Motor Servo run FORWARD with Speed %i \n", prog, speed);
          Serial.printf("Servo REQ: %i Servo ACT: %i \n", servo_request, servo_actual);
        }
        analogWrite(mot_pin, speed);
        digitalWrite(teensy_data.arg1[mot_pin], 1);
        digitalWrite(teensy_data.arg2[mot_pin], 0);
      } else if (backward) {
        /* move motor backward */
        if (DEBUG > 0) {
          Serial.printf("Program %i New Closed Loop Motor Servo run BACKWARD with Speed %i \n", prog, speed);
          Serial.printf("Servo REQ: %i Servo ACT: %i \n", servo_request, servo_actual);
        }
        analogWrite(mot_pin, speed);
        digitalWrite(teensy_data.arg1[mot_pin], 0);
        digitalWrite(teensy_data.arg2[mot_pin], 1);
      } else {
        /* hard stop */
        //digitalWrite(mot_pin, 1);
        analogWrite(mot_pin, 255);
        digitalWrite(teensy_data.arg1[mot_pin], 0);
        digitalWrite(teensy_data.arg2[mot_pin], 0);
      }

    } else {
      /* disable closed loop and have the motor in free running mode (EN=0, IN1=0, IN2=0) */
      //digitalWrite(mot_pin, 0);
      analogWrite(mot_pin, 0);
      digitalWrite(teensy_data.arg1[mot_pin], 0);
      digitalWrite(teensy_data.arg2[mot_pin], 1);
    }
  } else {
    ret = -1;
  }

  return ret;
}