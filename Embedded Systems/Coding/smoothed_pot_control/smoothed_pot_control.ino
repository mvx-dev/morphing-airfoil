#include <Servo.h>

/* Define I/O pins */
const int POT1 = A0;
const int POT2 = A1;
const int LEDPIN1 = 2; // LED1 in use
const int LEDPIN2 = 3; // LED2 in use
const int SWITCH2 = 4;
const int SWITCH1 = 5;
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
int pot1_old = 0;
int pot2_old = 0;
int switch1_value = LOW; // LOW = Smoothing off, HIGH = Smoothing on
int switch2_value = LOW;

int threshold = 4;
bool moving = false;

const int smoothingSteps = 48;
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
  int total = 0;
  
  for (int i = 0; i < smoothingSteps-1; i++) {
    readings[i] = readings[i+1];
    total += readings[i];
  }

  readings[smoothingSteps-1] = value;
  total += value;
  
  return (int)(total/smoothingSteps);
}

void calibrate(int cycles) {
  /*
  Propogates the buffer with values to prevent the servo moving prior to 
  */
  for (int i = 0; i < cycles; i++){
    pot1_value = analogRead(POT1);
    pot2_value = analogRead(POT2);
  }
}

void setup() {
  /* Configure pins*/
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(SWITCH1, INPUT);
  pinMode(SWITCH2, INPUT);

  /* Attach Servos */
  servo1.attach(SERVO1);
  servo2.attach(SERVO2);

  /* Begin Serial */
  Serial.begin(9600);
}

void loop() {
  pot1_old = pot1_value;
  pot2_old = pot2_value;
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

  int delta1 = pot1_old - pot1_value;
  int delta2 = pot2_old - pot2_value;

  if ((delta1 < -threshold) or (delta1 > threshold)) {
    moving = true;
  }
  if ((delta2 < -threshold) or (delta1 > threshold)) {
    moving = true;
  }

  if (moving) {
    moving = false;
    digitalWrite(LEDPIN1, HIGH);
  } else {
    digitalWrite(LEDPIN1, LOW);
  }

  servo1_pos = pot_to_servo(pot1_value);
  servo2_pos = pot_to_servo(pot2_value);

  servo1.write(servo1_pos);
  servo2.write(servo2_pos);

  Serial.print(pot1_value);
  Serial.print("\t");
  Serial.println(pot2_value);

  delay(10);
}
