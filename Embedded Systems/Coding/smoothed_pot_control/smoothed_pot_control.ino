#include <Servo.h>

/* Define I/O pins */
const int POT1 = A0;
const int POT2 = A1;
const int LED5 = 2; // LED1, LED3 in use
const int LED6 = 3; // LED2, LED4 in use
const int SWITCH1 = 4;
const int SWITCH2 = 5;
const int SERVO1 = 6;
const int SERVO2 = 7;

/* Define servos*/
Servo servo1;
Servo servo2;

/* Define program variables*/
int servo1_pos = 0;
int servo2_pos = 0;
int pot1_value = 0;
int pot2_value = 0;
int switch1_value = LOW; // LOW = Smoothing off, HIGH = Smoothing on
int switch2_value = LOW;

const int smoothingSteps = 16;
int pot1_readings[smoothingSteps] = {0};
int pot2_readings[smoothingSteps] = {0};
int pot1_running_total = 0;
int pot2_running_total = 0;

/* Translate potentiometer value to servo position */
int pot_to_servo(int value) {
  return (value/1023.0) * 180;
}

int smooth_value(int value, int readings[]) {
  /* Remove the oldest reading, append new reading, and return average */
  int old = readings[0];
  int total = 0;
  
  for (int i = 0; i < smoothingSteps-1, i++;) {
    readings[i] = readings[i+1];
  }
  readings[smoothingSteps-1] = value;

  for (int i = 0; i < smoothingSteps, i++;) {
    total += readings[i];
  }
  
  return (int)(total/smoothingSteps);
}

void setup() {
  /* Configure pins*/
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(SWITCH1, INPUT);
  pinMode(SWITCH2, INPUT);

  /* Attach Servos */
  servo1.attach(SERVO1);
  servo2.attach(SERVO2);

  /* Begin Serial */
  Serial.begin(9600);
}

void loop() {
  pot1_value = analogRead(POT1);
  pot2_value = analogRead(POT2);

  switch1_value = digitalRead(SWITCH1);
  switch2_value = digitalRead(SWITCH2);

  if (switch1_value == HIGH) {
    pot1_value = smooth_value(pot1_value, pot1_readings);
  }
  if (switch2_value == HIGH) {
    pot2_value = smooth_value(pot2_value, pot2_readings);
  }

  servo1_pos = pot_to_servo(pot1_value);
  servo2_pos = pot_to_servo(pot2_value);

  servo1.write(servo1_pos);
  servo2.write(servo2_pos);

  for (int i = 0; i < smoothingSteps; i++) {
    Serial.print(pot1_readings[i]);
    Serial.print("\t");
  }
  Serial.println();

  // Serial.println(pot1_readings);
  // Serial.print("\t");
  // Serial.println(pot2_value);

  delay(10);
}
