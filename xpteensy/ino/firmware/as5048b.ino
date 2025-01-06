#include "ams_as5048b.h"

#define AS5048B_POLL_TIME 20 /* number of milliseconds to poll the board */

//AMS_AS5048B as5048b(0x40);
AMS_AS5048B as5048b[MAX_DEV];

unsigned long as5048b_time;

void as5048b_init(int8_t dev, int8_t nangle, int8_t type, int8_t wirenum, uint8_t address, int16_t val) {

  if ((dev >= 0) && (dev < MAX_DEV)) {
    // Connect board to I2C bus if not already connected
    if (as5048b_data[dev].connected == 0) {

      // TODO: IMPLEMENT FOR WIRE 1 and 2 if needed
      if (wirenum == 0) {
        as5048b[dev] = AMS_AS5048B(address);
        as5048b_data[dev].connected = 1;
        as5048b_data[dev].wire = wirenum;
        as5048b_data[dev].address = address;
      } else if (wirenum == 1) {
        Serial.printf("INIT: as5048b Device %i Wire 1 NOT SUPPORTED YET \n", dev);
      } else if (wirenum == 2) {
        Serial.printf("INIT: as5048b Device %i Wire 2NOT SUPPORTED YET \n", dev);
      } else {
        if (DEBUG > 0) {
          Serial.printf("INIT: as5048b Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
        }
      }
    }

    /* Only initialize connected devices */
    if (as5048b_data[dev].connected == 1) {

      as5048b_data[dev].val_save = INITVAL;  // reset pin state so that it will be sent to client after read
      as5048b_data[dev].type = type;
      as5048b_data[dev].nangle = nangle;

      as5048b[dev].begin();
      //consider the current position as zero
      as5048b[dev].setZeroReg();

      if (DEBUG > 0) {
        if (type == 0) {
          Serial.printf("INIT: as5048b Device %i initialized as Direction Count Reporting \n", dev);
        } else {
          Serial.printf("INIT: as5048b Device %i initialized as Angle Reporting \n", dev);
        }
      }

    } else {
      if (DEBUG > 0) {
        Serial.printf("INIT: as5048b Device Number %i out of range \n", dev);
      }
    }
  }

  /* reset last time */
  as5048b_time = micros();
}

void as5048b_read() {

  int ret = 0;
  unsigned long time = micros();
  if (time > (as5048b_time + AS5048B_POLL_TIME * 1000)) {

    as5048b_time = time;

    for (int8_t dev = 0; dev < MAX_DEV; dev++) {
      if (as5048b_data[dev].connected == 1) {

        bool up = false;
        bool dn = false;
        int16_t angle = as5048b[dev].angleR(3, true) + 5; // read angle and add some in order not to stick to 359 and 0
        angle = angle % 360;  // set 360 degrees to 0 degrees
        int16_t angle_save = angle;
        int16_t angle_previous = as5048b_data[dev].val;

        /* check if we just crossed the 360 degree border and adjust angle for comparison */
        /* we only compare positive angles in order to get each nangle step */
        if (as5048b_data[dev].val != INITVAL) {
          if (angle > (as5048b_data[dev].val + 300)) angle_previous += 360;
          if (angle < (as5048b_data[dev].val - 300)) angle += 360;

          if ((angle * as5048b_data[dev].nangle / 360) > (angle_previous * as5048b_data[dev].nangle / 360)) up = true;
          if ((angle * as5048b_data[dev].nangle / 360) < (angle_previous * as5048b_data[dev].nangle / 360)) dn = true;
        }

        if ((as5048b_data[dev].val == INITVAL) || up || dn) {

          if (as5048b_data[dev].type == 1) {
            /* send current angle value */
            ret = udp_send(AS5048B_TYPE, dev, 0, angle_save);
          } else {
            /* send up / down value */
            if (up) {
              ret = udp_send(AS5048B_TYPE, dev, 0, 1);
            } else {
              ret = udp_send(AS5048B_TYPE, dev, 0, -1);
            }
          }
          if (DEBUG > 0) {
            Serial.printf("READ: as5048b Device Number %i Angle %i \n", dev, angle_save);
            //Serial.printf("%i %i %i %i\n", angle, angle_save, angle_previous, as5048b_data[dev].val);
            //Serial.printf("%i %i\n", angle * as5048b_data[dev].nangle / 360, angle_previous * as5048b_data[dev].nangle / 360);
          }

          /* update stored angle upon successful send (not used right now) */
          if (ret == SENDMSGLEN) {
            as5048b_data[dev].val_save = as5048b_data[dev].val;
          }
        }  // angle changed enough so we can send update

        /* store new angle for later comparison */
        as5048b_data[dev].val = angle_save;

      }  // connected
    }    // loop through devices
  }      // time to poll devices
}