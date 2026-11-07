#include <L298N.h>

// ===============================
// MOTOR DRIVER PINS
// ===============================
const unsigned int IN1 = 7;
const unsigned int IN2 = 8;
const unsigned int IN3 = 9;
const unsigned int IN4 = 10;

const unsigned int ENA = 6;   // Left motor PWM
const unsigned int ENB = 5;   // Right motor PWM

// ===============================
// MOTOR OBJECTS
// ===============================
L298N motor1(IN1, IN2, ENA);   // Left motor
L298N motor2(IN3, IN4, ENB);   // Right motor

// ===============================
// MOTOR SPEEDS
// ===============================
const int moveSpeed   = 100;
const int attackSpeed = 150;
const int turnSpeed   = 120;
const int escapeSpeed = 130;

// ===============================
// OPPONENT SENSOR PINS
// ===============================
const int sensorFrontPin = A5;
const int sensorLeftPin  = A1;
const int sensorRightPin = A3;

// ===============================
// LINE SENSOR
// Located underneath the scoop
// ===============================
const int lineSensorPin = A0;

// ===============================
// SENSOR VALUES
// ===============================
int sensorFront = 0;
int sensorLeftF = 0;
int sensorRightF = 0;
int lineSensor = 0;

// ===============================
// OPPONENT DETECTION THRESHOLD
// ===============================
const int distance = 500;

// ===============================
// LINE SENSOR THRESHOLD
// ===============================
// If the line sensor reading is BELOW
// this value, the robot considers it
// a boundary.
//
// Adjust this value after testing.
const int lineThreshold = 500;

// ===============================
// MODES
// ===============================
int mode = 0;

const int searchMode     = 0;
const int attackMode     = 1;
const int turnLeftModeF  = 4;
const int turnRightModeF = 5;

// ===============================
// SETUP
// ===============================
void setup() {

  motor1.setSpeed(moveSpeed);
  motor2.setSpeed(moveSpeed);

  // 5-second starting delay
  delay(5000);
}

// ===============================
// MAIN LOOP
// ===============================
void loop() {

  // =================================
  // READ ALL SENSORS
  // =================================
  sensorFront = analogRead(sensorFrontPin);
  sensorLeftF = analogRead(sensorLeftPin);
  sensorRightF = analogRead(sensorRightPin);
  lineSensor  = analogRead(lineSensorPin);


  // =================================
  // ARENA BOUNDARY DETECTION
  // HIGHEST PRIORITY
  // =================================
  if (lineSensor < lineThreshold) {

    // Stop first
    motor1.stop();
    motor2.stop();

    delay(50);

    // Reverse away from the boundary
    motor1.setSpeed(escapeSpeed);
    motor2.setSpeed(escapeSpeed);

    motor1.backward();
    motor2.backward();

    delay(300);

    // Stop briefly
    motor1.stop();
    motor2.stop();

    delay(50);

    // Turn away from the edge
    motor1.setSpeed(turnSpeed);
    motor2.setSpeed(turnSpeed);

    motor1.forward();
    motor2.backward();

    delay(300);

    // Return to searching
    mode = searchMode;

    return;
  }


  // =================================
  // FRONT OPPONENT DETECTION
  // =================================
  if (sensorFront < distance) {

    mode = attackMode;
  }

  // =================================
  // LEFT OPPONENT DETECTION
  // =================================
  else if (sensorLeftF < distance) {

    mode = turnLeftModeF;
  }

  // =================================
  // RIGHT OPPONENT DETECTION
  // =================================
  else if (sensorRightF < distance) {

    mode = turnRightModeF;
  }

  // =================================
  // NOTHING DETECTED
  // =================================
  else {

    mode = searchMode;
  }


  // =================================
  // SEARCH MODE
  // =================================
  if (mode == searchMode) {

    motor1.setSpeed(moveSpeed);
    motor2.setSpeed(moveSpeed);

    motor1.forward();
    motor2.forward();
  }


  // =================================
  // ATTACK MODE
  // =================================
  else if (mode == attackMode) {

    motor1.setSpeed(attackSpeed);
    motor2.setSpeed(attackSpeed);

    motor1.forward();
    motor2.forward();
  }


  // =================================
  // TURN LEFT
  // =================================
  else if (mode == turnLeftModeF) {

    motor1.setSpeed(turnSpeed);
    motor2.setSpeed(turnSpeed);

    // Left motor backward
    // Right motor forward
    motor1.backward();
    motor2.forward();

    delay(200);

    mode = searchMode;
  }


  // =================================
  // TURN RIGHT
  // =================================
  else if (mode == turnRightModeF) {

    motor1.setSpeed(turnSpeed);
    motor2.setSpeed(turnSpeed);

    // Left motor forward
    // Right motor backward
    motor1.forward();
    motor2.backward();

    delay(200);

    mode = searchMode;
  }
}
