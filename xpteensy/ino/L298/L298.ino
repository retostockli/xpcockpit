#define POT_PIN 14
#define DIR_PIN 30
#define BRAKE_PIN 31
#define EN_PIN 33
#define IN1_PIN 34
#define IN2_PIN 35

int16_t pot_save;
int16_t dir_save;
int16_t brake_save;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("Starting Setup ...\n");

  pinMode(POT_PIN, INPUT);
  pinMode(DIR_PIN, INPUT_PULLUP);
  pinMode(BRAKE_PIN, INPUT_PULLUP);
  pinMode(EN_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int16_t brake = digitalRead(BRAKE_PIN);
  if (brake != brake_save) {
    Serial.printf("New BRAKE Value: %i \n", brake);
    brake_save = brake;
  }

  int16_t dir = digitalRead(DIR_PIN);
  if (dir != dir_save) {
    Serial.printf("New DIRECTION Value: %i \n", dir);
    dir_save = dir;
  }

  int16_t pot = analogRead(POT_PIN);
  if ((pot < (pot_save - 2)) || (pot > (pot_save + 2))) {
    Serial.printf("New Potentiometer Value: %i \n", pot);
    pot_save = pot;
  }

  if (brake == 0) {
    if (dir == 0) {
      digitalWrite(IN1_PIN, 1);
      digitalWrite(IN2_PIN, 0);
      analogWrite(EN_PIN, pot / 4);
    } else {
      digitalWrite(IN1_PIN, 0);
      digitalWrite(IN2_PIN, 1);
      analogWrite(EN_PIN, pot / 4);
    }
  } else {
    digitalWrite(IN1_PIN, 0);
    digitalWrite(IN2_PIN, 0);
    digitalWrite(EN_PIN, 1);
  }

  delay(10);
}
