/*
  smoothed_pot_control v1.0

  Controls two servos by reading the voltage across two potentiometers, and
  optionally smooth the outputs by calculating a sliding average.

  created 03 Apr 2025
  by Oskar Meszaros
  zID z5636164
  email o.meszaros@student.unsw.edu.au
*/
#include <Servo.h>

/* Define I/O pins */
const int POT1 = A0;
const int POT2 = A1;
const int LEDPIN1 = 2; // LED1 in use
const int LEDPIN2 = 3; // LED2 in use
const int SWITCH2 = 4;
const int SWITCH1 = 5;
const int SERVO1 = 9;
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

int pot_to_servo(int value) {
  /* Translate potentiometer value to servo position */
  return (value/1023.0) * 180;
}

int smooth_value(int value, int readings[]) {
  /* Update the reading buffer and return average */
  int total = 0;
  
  for (int i = 0; i < smoothingSteps-1; i++) {
    readings[i] = readings[i+1];
    total += readings[i];
  }

  readings[smoothingSteps-1] = value;
  total += value;
  
  return (int)(total/smoothingSteps);
}

void calibrate(int cycles, int gap, int pot_pin, int readings[]) {
  /*
  Propogates the buffer with values to prevent the servo moving prior to having the correction position.
  Fills buffer cycles times.
  */
  for (int cycle = 0; cycle < cycles; cycle++){
    for (int i = 0; i < smoothingSteps; i++) {
      int pot_value = analogRead(pot_pin);
      smooth_value(pot_value, readings);
      delay(gap);
    }
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

  /* Calibrate potentiometer smoothing */
  digitalWrite(LEDPIN1, HIGH);
  calibrate(8, 10, POT1, pot1_readings);
  calibrate(8, 10, POT2, pot2_readings);
  digitalWrite(LEDPIN1, LOW);
  delay(100);
  digitalWrite(LEDPIN1, HIGH);
  delay(100);
  digitalWrite(LEDPIN1, LOW);

  /* Begin Serial */
  Serial.begin(9600);
}

void loop() {
  /* Read all inputs */
  pot1_old = pot1_value;
  pot2_old = pot2_value;
  pot1_value = analogRead(POT1);
  pot2_value = analogRead(POT2);

  switch1_value = digitalRead(SWITCH1);
  switch2_value = digitalRead(SWITCH2);

  /* Smooth potentiometer input. Still add to buffer if not. */
  if (switch1_value == HIGH) {
    pot1_value = smooth_value(pot1_value, pot1_readings);
  } else {
    smooth_value(pot1_value, pot1_readings);
  }

  if (switch2_value == HIGH) {
    pot2_value = smooth_value(pot2_value, pot2_readings);
  } else {
    smooth_value(pot2_value, pot2_readings);
  }

  /* Indicator light*/
  int delta1 = pot1_old - pot1_value;
  int delta2 = pot2_old - pot2_value;

  if ((delta1 < -threshold) or (delta1 > threshold)) {
    moving = true;
  }
  if ((delta2 < -threshold) or (delta2 > threshold)) {
    moving = true;
  }

  if (moving) {
    moving = false;
    digitalWrite(LEDPIN1, HIGH);
    digitalWrite(LEDPIN2, LOW);
  } else {
    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN2, HIGH);
  }

  /* Update servos */
  servo1_pos = pot_to_servo(pot1_value);
  servo2_pos = pot_to_servo(pot2_value);

  servo1.write(servo1_pos);
  servo2.write(servo2_pos);

  delay(10);
}
