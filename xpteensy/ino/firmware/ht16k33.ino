#include "7segfont.h"

HT16K33 ht16k33[MAX_DEV];

void ht16k33_init(int8_t dev, int8_t wirenum, uint8_t address) {

  if ((dev >= 0) && (dev < MAX_DEV)) {
    // Connect board to I2C bus if not already connected
    if (ht16k33_data[dev].connected == 0) {
      if (wirenum == 0) {
        ht16k33[dev].begin(address, &Wire);
      } else if (wirenum == 1) {
        ht16k33[dev].begin(address, &Wire1);
      } else if (wirenum == 2) {
        ht16k33[dev].begin(address, &Wire2);
      } else {
        if (DEBUG > 0) {
          Serial.printf("INIT: HT16K33 Dev %i Wire Number %i Out of Range. Wire Number can only be 0, 1 or 2 \n", dev, wirenum);
        }
      }

      /* Load Font Table */
      ht16k33[dev].define7segFont((uint8_t*)&fontTable);

      /* Clear all Displays on Init */
      ht16k33[dev].clearAll();

      if (DEBUG > 0) {
        Serial.printf("INIT: HT16K33 Device Number %i connected to I2C bus %i with address 0x%02x \n", dev, wirenum, address);
      }

      ht16k33_data[dev].connected = 1;
    }
  } else {
    if (DEBUG > 0) {
      Serial.printf("INIT: HT16K33 Device Number %i out of range \n", dev);
    }
  }
}

void ht16k33_write(int8_t dev, int8_t dig, int16_t val, int8_t dp, int8_t brightness) {

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
            Serial.printf("WRITE: HT16K33 Send Value %i to Digit %i Brightness %i \n", val, dig, brightness);
          }

        }  // value has changed
      } else {
        if (DEBUG > 0) {
          Serial.printf("WRITE: HT16K33 Device %i not connected \n", dev);
        }
      }
    } else {
      if (DEBUG > 0) {
        Serial.printf("WRITE: HT16K33 Device Number %i out of range \n", dev);
      }
    }
  } else {
    if (DEBUG > 0) {
      Serial.printf("WRITE: HT16K33 Device %i Digit %i out of range \n", dev, dig);
    }
  }
}
