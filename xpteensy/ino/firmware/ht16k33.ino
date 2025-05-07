#include "7segfont.h"

HT16K33 ht16k33[MAX_DEV];

int16_t ht16k33_init(int8_t dev, int8_t wirenum, uint8_t address) {

  int16_t retval = 0;

  if ((dev >= 0) && (dev < MAX_DEV)) {
    // Connect board to I2C bus if not already connected
    if (ht16k33_data[dev].connected == 0) {
      int ret = 0;
      if (wirenum == 0) {
        ret = ht16k33[dev].begin(address, &Wire);
      } else if (wirenum == 1) {
        ret = ht16k33[dev].begin(address, &Wire1);
      } else if (wirenum == 2) {
        ret = ht16k33[dev].begin(address, &Wire2);
      } else {
        retval = ERROR_WIRE_RANGE;
        if (DEBUG > 0) {
          Serial.printf("INIT: HT16K33 Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
        }
      }

      /* reset wire speed since ht16k33 begin may reset it to 100 kHz */
      if (wirenum == 0) {
        Wire.setClock(WIRESPEED);
      } else if (wirenum == 1) {
        Wire1.setClock(WIRESPEED);
      } else if (wirenum == 2) {
        Wire2.setClock(WIRESPEED);
      }

      if (ret != 0) {
        retval = ERROR_INIT;
        if (DEBUG > 0) {
          Serial.printf("INIT: HT16K33 Device %i could not be connected to I2C bus %i with address 0x%02x \n", dev, wirenum, address);
        }
      } else {      
        /* Load Font Table */
        ht16k33[dev].define7segFont((uint8_t*)&fontTable);

        /* Clear all Displays on Init */
        ht16k33[dev].clearAll();

        if (DEBUG > 0) {
          Serial.printf("INIT: HT16K33 Device Number %i connected to I2C bus %i with address 0x%02x \n", dev, wirenum, address);
        }

        ht16k33_data[dev].connected = 1;
      }
    }
  } else {
    retval = ERROR_DEV_RANGE;
    if (DEBUG > 0) {
      Serial.printf("INIT: HT16K33 Device Number %i out of range \n", dev);
    }
  }

  return retval;

}

int16_t ht16k33_write(int8_t dev, int8_t dig, int16_t val, int8_t dp, int8_t brightness) {

  int16_t retval =0;

  if ((dig >= 0) && (dig < HT16K33_MAX_DIG)) {
    if ((dev >= 0) && (dev < MAX_DEV)) {
      if (ht16k33_data[dev].connected == 1) {
        if (val != ht16k33_data[dev].val[dig]) {

          ht16k33[dev].setBrightness(brightness);
          if (dp == 1) {
            ht16k33[dev].set7Seg(dig, val, true);
          } else {
            ht16k33[dev].set7Seg(dig, val, false);
          }
          ht16k33[dev].sendLed();

          if (DEBUG > 0) {
            Serial.printf("WRITE: HT16K33 Device %i Send Value %i to Digit %i Brightness %i \n", dev, val, dig, brightness);
          }

        }  // value has changed
      } else {
        retval = ERROR_NOT_CONNECTED;
        if (DEBUG > 0) {
          Serial.printf("WRITE: HT16K33 Device %i not connected \n", dev);
        }
      }
    } else {
      retval = ERROR_DEV_RANGE;
      if (DEBUG > 0) {
        Serial.printf("WRITE: HT16K33 Device Number %i out of range \n", dev);
      }
    }
  } else {
    retval = ERROR_PIN_RANGE;
    if (DEBUG > 0) {
      Serial.printf("WRITE: HT16K33 Device %i Digit %i out of range \n", dev, dig);
    }
  }

  return retval;
}
