#include <L298N.h>

// Motor driver pins
const unsigned int IN1 = 7;
const unsigned int IN2 = 8;
const unsigned int IN3 = 9;
const unsigned int IN4 = 10;
const unsigned int ENA = 6;  // must be PWM pin
const unsigned int ENB = 5;  // must be PWM pin

// Initialize motors
L298N motor1(IN1, IN2, ENA);  // Left motor
L298N motor2(IN3, IN4, ENB);  // Right motor

// Sensor variables
int sensorIlalim;
int sensorLeftF;
int sensorRightF;
int sensorFront;

// Thresholds
const int line = 500;       // floor line detection threshold
const int distance = 300;   // obstacle detection threshold

// Speeds (0–255)
const int moveSpeed = 100;
const int attackSpeed = 150;
const int turnSpeed = 120;
const int turnSpeedStop = 0;

// Modes
int mode = 0;
const int searchMode = 0;
const int attackMode = 1;
const int turnLeftModeF = 4;
const int turnRightModeF = 5;

void setup() {
  motor1.setSpeed(moveSpeed);
  motor2.setSpeed(moveSpeed);
}

void loop() {
  // Read sensors (use valid analog pins!)
  sensorIlalim = analogRead(A6);
  sensorLeftF  = analogRead(A1);
  sensorRightF = analogRead(A3);
  sensorFront   = analogRead(A5);

  // Stay inside the ring
  if (line && sensorIlalim) {
    // --- SUMO LOGIC ---
    if (mode == searchMode) {
      motor1.forward();
      motor2.forward();
      
      if (sensorLeftF < distance)  mode = turnLeftModeF;
      if (sensorRightF < distance) mode = turnRightModeF;
      if (sensorFront < distance)  mode = attackMode;
      if (sensorIlalim < distance)   mode = turnLeftModeF;

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
  else if (line && sensorIlalim) {
    // Back up and turn right
    motor1.backward();
    motor2.backward();
    delay(300); 
  }
}

void checkSensors() {
  if (sensorFront < distance) {
    motor1.setSpeed(attackSpeed);
    motor2.setSpeed(attackSpeed);
    mode = attackMode;
  }
}
