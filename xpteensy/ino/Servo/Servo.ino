#include <Servo.h>

#define POT_PIN 14
#define SERVO_PIN 28
#define SERVO_MINANGLE 0       /* minimum angle of servos (deg) */
#define SERVO_MAXANGLE 180     /* maximum angle of servos (deg)*/
#define SERVO_MINPULSE 600     /* minimum pulse width of servos (us) default 544 */
#define SERVO_MAXPULSE 2600    /* maximum pulse width of servos (us) default 2400 */

int16_t pot_save;
Servo servo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("Starting Setup ...\n");

  pinMode(POT_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  servo.attach(SERVO_PIN, SERVO_MINPULSE, SERVO_MAXPULSE);

}

void loop() {
  
  int16_t pot = analogRead(POT_PIN);
  if ((pot < (pot_save - 2)) || (pot > (pot_save + 2))) {
    Serial.printf("New Potentiometer Value: %i \n", pot);
    pot_save = pot;

    int16_t val = pot * SERVO_MAXANGLE / 1024;

    servo.write(val);

  }

 
  delay(10);
}
