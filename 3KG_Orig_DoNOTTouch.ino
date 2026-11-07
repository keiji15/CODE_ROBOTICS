#include <L298N.h>

// Motor driver pins
const unsigned int IN1 = 2;
const unsigned int IN2 = 3;
const unsigned int IN3 = 4;
const unsigned int IN4 = 5;
const unsigned int ENA = 6;  // must be PWM pin
const unsigned int ENB = 9;  // must be PWM pin

// Initialize motors
L298N motor1(IN1, IN2, ENA);  // Left motor
L298N motor2(IN3, IN4, ENB);  // Right motor

// Sensor variables
int sensorIlalimLeft;
int sensorIlalimRight;
int sensorLeftF;
int sensorRightF;
int sensorLeft;
int sensorRight;
int sensorFront;
int sensorBack;

// Thresholds
const int line = 500;       // floor line detection threshold
const int distance = 300;   // obstacle detection threshold

// Speeds (0–255)
const int moveSpeed = 120;
const int attackSpeed = 200;
const int turnSpeed = 150;
const int turnSpeedStop = 0;

// Modes
int mode = 0;
const int searchMode = 0;
const int attackMode = 1;
const int turnLeftMode = 2;
const int turnRightMode = 3;
const int turnLeftModeF = 4;
const int turnRightModeF = 5;

void setup() {
  motor1.setSpeed(moveSpeed);
  motor2.setSpeed(moveSpeed);
}

void loop() {
  // Read sensors (use valid analog pins!)
  sensorIlalimLeft  = analogRead(A6); // bottom left sensor
  sensorIlalimRight = analogRead(A7); // bottom right sensor
  sensorLeft   = analogRead(A0);
  sensorLeftF  = analogRead(A1);
  sensorFront  = analogRead(A2);
  sensorRightF = analogRead(A3);
  sensorRight  = analogRead(A4);
  sensorBack   = analogRead(A5);

  // Stay inside the ring
  if (sensorIlalimLeft < line && sensorIlalimRight < line) {
    // --- SUMO LOGIC ---
    if (mode == searchMode) {
      motor1.forward();
      motor2.forward();

      if (sensorLeft < distance)   mode = turnLeftMode;
      if (sensorRight < distance)  mode = turnRightMode;
      if (sensorLeftF < distance)  mode = turnLeftModeF;
      if (sensorRightF < distance) mode = turnRightModeF;
      if (sensorFront < distance)  mode = attackMode;
      if (sensorBack < distance)   mode = turnLeftMode;

    } else if (mode == attackMode) {
      motor1.setSpeed(attackSpeed);
      motor2.setSpeed(attackSpeed);
      motor1.forward();
      motor2.forward();

      if (sensorFront > distance) {
        motor1.setSpeed(moveSpeed);
        motor2.setSpeed(moveSpeed);
        mode = searchMode;
      }

    } else if (mode == turnLeftMode) {
      motor1.backward();
      motor2.forward();
      checkSensors();

    } else if (mode == turnRightMode) {
      motor1.forward();
      motor2.backward();
      checkSensors();

    } else if (mode == turnLeftModeF) {
      motor1.stop();
      motor2.forward();
      checkSensors();

    } else if (mode == turnRightModeF) {
      motor1.forward();
      motor2.stop();
      checkSensors();
    }
  }
  else if (sensorIlalimLeft > line && sensorIlalimRight < line) {
    // Back up and turn right
    motor1.backward();
    motor2.backward();
    delay(300);
    motor1.forward();
    motor2.backward();
    delay(500);

  } else if (sensorIlalimLeft < line && sensorIlalimRight > line) {
    // Back up and turn left
    motor1.backward();
    motor2.backward();
    delay(300);
    motor1.backward();
    motor2.forward();
    delay(500);
  }
}

void checkSensors() {
  if (sensorFront < distance) {
    motor1.setSpeed(attackSpeed);
    motor2.setSpeed(attackSpeed);
    mode = attackMode;
  }
}
