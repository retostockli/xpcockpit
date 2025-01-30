/****************************************************************
 * Simple test of simple ht16k33 library turning on and off LEDs
 */

#include <Wire.h>
#include "ht16k33.h"
#include "7segfont.h"

// Define the class
HT16K33 HT;

/****************************************************************/
void setup() {
  Serial.begin(57600);
  Serial.println(F("ht16k33 light test v0.01"));
  Serial.println();


  Wire.begin();

  // initialize everything, 0x70 is the first i2c address (0x70 - 0x77 by jumper selection)
  HT.begin(0x70);

  HT.define7segFont((uint8_t*)&fontTable);
}

/****************************************************************/
void loop() {
  uint8_t led;
  uint8_t dig;
  uint8_t val;

  HT.clearAll();
  delay(1000);

  HT.setBrightness(15);

  for (dig = 8; dig < 16; dig++) {
    for (val = 0; val < 16; val++) {
      Serial.printf("Send Value %i to Digit %i \n", val, dig);
      HT.set7Seg(dig, val, true);
      HT.sendLed();
      delay(500);
    }
  }


  /*
  Serial.println(F("Turn on all LEDs"));
  // first light up all LEDs
  for (led = 0; led < 128; led++) {
    HT.setLedNow(led);
    delay(10);
  }  // for led

  Serial.println(F("Clear all LEDs"));
  //Next clear them
  for (led = 0; led < 128; led++) {
    HT.clearLedNow(led);
    delay(10);
  }  // for led


  //Now do one by one, slowly, and print out which one
  for (led = 0; led < 128; led++) {
    HT.setLedNow(led);
    Serial.print(F("Led no "));
    Serial.print(led, DEC);
    Serial.print(F(": On"));
    delay(1000);
    HT.clearLedNow(led);
    Serial.println(F(" Off"));
  }  // for led

  */
}  // loop
