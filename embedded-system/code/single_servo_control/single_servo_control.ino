#include <Servo.h>


Servo servo;  // create servo object to control a servo

// Define pins
const int POT_IN = A0;
// const int SERVO_IN = A1;
const int SERVO_OUT = 10;

// Program variables
int servo_pos = 0;
int pot_value = 0;
int servo_value = 0;

float potToServo(int pot) {
  float ratio = pot / 1023.0;
  return ratio * 180.0;
}

void setup() {
  servo.attach(SERVO_OUT);  // attaches the servo on pin 9 to the servo object
  servo.write(0);  // Home it to 0
  delay(50);

  // Configure serial
  Serial.begin(9600);
}

void loop() {
  pot_value = analogRead(POT_IN);
  servo_pos = potToServo(pot_value);
  // servo_value = analogRead(SERVO_IN);
  Serial.print(pot_value);
  Serial.print("\t");
  // Serial.print(servo_value);
  // Serial.print("\t");
  Serial.println(servo_pos);

  servo.write(servo_pos);

  delay(10);
}