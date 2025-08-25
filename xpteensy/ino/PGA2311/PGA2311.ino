#include <SPI.h>

// mixer pins
#define SWITCH  0
#define LED 1
#define POT 14

#define SCLK 13  // Pin 6 of PGA2311
#define MISO 12  // Pin 7 of PGA2311, Optional, since no output from IC
#define MOSI 11  // Pin 3 of PGA2311
#define CS   10  // Pin 2 of PGA2311

/* NOTES FOR THE PGA2311
   Pin 8 MUTE --> HIGH for unmute
   Pin 1 ZCEN --> HIGH for enable zero crossing (advised)

   Have a 10uF and 0.1 uF to Digital and Analog Power inputs towards ground close to the Chip

*/

void setup() {
    // set up pins
    pinMode(LED, OUTPUT);   
    digitalWrite(LED, HIGH);

    // set up SPI
    SPI.setMOSI(MOSI); 
    SPI.setMISO(MISO);
    SPI.setSCK(SCLK);
    pinMode(CS, OUTPUT);
    SPI.begin();
        
    Serial.begin(9600);
    Serial.println("READY!");

    digitalWrite(LED, LOW);
}

uint8_t val_save = 0;

void loop() {

    uint8_t val = analogRead(POT) / 4;

    if ((val > (val_save+2)) || (val < (val_save-2))) {
        
        digitalWrite(LED, HIGH);

        val_save = val;
        Serial.printf("New Analog Input Value %i \n",val);

        uint8_t right = val;
        uint8_t left = val;

        digitalWrite(LED, HIGH);
        //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
        digitalWrite(CS, LOW);     //CS = on
        //delayMicroseconds(10);
        SPI.transfer(right);
        SPI.transfer(left);
        digitalWrite(CS, HIGH);    //CS = off
        //delayMicroseconds(10);
        //SPI.endTransaction(); 
        //delay(100);
        digitalWrite(LED, LOW);
 
    }
 
}







