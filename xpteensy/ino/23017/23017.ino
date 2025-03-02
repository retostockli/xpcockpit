// Blinks an LED attached to a MCP23XXX pin.

// ok to include only the one needed
// both included here to make things simple for example
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>
#include <Wire.h>
#include "mylib.h"

#define TEE_INT_PIN 4  // Teensy pin for catching interrupt
#define TEE_LED_PIN 0  // Teensy pin where LED is connected
#define MCP_LED_PIN 7  // MCP23XXX pin LED is attached to

//I2C Wires:
//red: 3.3V or 5.0V (make sure you have a level shifter of the signals if using 5.0V)
//black: ground
//yellow: SCL
//blue: SDA

// uncomment appropriate line
//Adafruit_MCP23X08 mcp;
Adafruit_MCP23X17 mcp;
extern TwoWire Wire1;  // Wire, Wire1, Wire2

uint8_t obits[2];
uint8_t nbits[2];

// LOOP TIME CAN BE MAX 2 ms for not missing encoder steps

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("MCP23xxx Blink Test!");

  // Start I2C bus
  Wire1.begin();

  // begin_I2C: first argument Hex of address, second argument: wire instance
  // I2C Address can be changed with inputs D0,D1,D2 from 0x20 - 0x27
  if (!mcp.begin_I2C(0x20, &Wire)) {
    Serial.println("Error.");
    while (1)
      ;
  }

  //  Set speed of I2C bus
  Wire.setClock(400000);

  pinMode(TEE_INT_PIN, INPUT_PULLUP);
  // configure MCP23017 for interrupt: 1st argument: A OR B, 2nd argument: open drain, 3rd argument: interrupt = LOW
  mcp.setupInterrupts(true, true, LOW);

  for (int i = 0; i < 6; i++) {
    mcp.pinMode(i, INPUT_PULLUP);  // set all pins on board #1 as inputs
    mcp.setupInterruptPin(i, CHANGE);
  }
  // read MCP23017 asap to init for use with interrupt
  mcp.readGPIOAB();

  // configure pina for output
  mcp.pinMode(MCP_LED_PIN, OUTPUT);
  pinMode(TEE_LED_PIN, OUTPUT);

  Serial.println("Looping...");
}

void loop() {
  unsigned long StartTime = micros();
  if (!digitalRead(TEE_INT_PIN)) {
    uint16_t b = mcp.readGPIOAB();
    //uint8_t b = mcp.readGPIOA();
    obits[0] = nbits[0];
    obits[1] = nbits[1];
    nbits[0] = (b >> 2) & 1;
    nbits[1] = (b >> 3) & 1;
    //nbits[0] = 0 != (b & 4);
    //nbits[1] = 0 != (b & 8);
    Serial.print(nbits[0]);
    Serial.println(nbits[1]);
    //Serial.println(b);
    int updown = encoder(obits, nbits);
    if (updown == 1) Serial.println("UPUP");
    if (updown == -1) Serial.println("DOWN");
  } else {
  }
  unsigned long CurrentTime = micros();
  unsigned long ElapsedTime = CurrentTime - StartTime;

  if (ElapsedTime > 5) Serial.printf("Time: %i \n",ElapsedTime);

  mcp.digitalWrite(MCP_LED_PIN, HIGH);
  delay(2000);
  mcp.digitalWrite(MCP_LED_PIN, LOW);
  delay(500);
  
}