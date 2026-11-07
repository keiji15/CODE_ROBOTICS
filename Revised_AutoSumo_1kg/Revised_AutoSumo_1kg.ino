
/*
  AUTONOMOUS SUMOBOT
  Arduino Nano + L298N

  MOTOR CONNECTIONS:
  Left motor:
    ENA -> D6
    IN1 -> D7
    IN2 -> D8

  Right motor:
    ENB -> D5
    IN3 -> D9
    IN4 -> D10

  SENSORS:
    Left opponent  -> A1
    Front opponent -> A5
    Right opponent -> A3
    Line sensor    -> A0

  Serial Monitor: 9600 baud
*/

// ---------------- PIN DEFINITIONS ----------------

#define ENA 6
#define ENB 5

#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10

#define LEFT_SENSOR  A1
#define FRONT_SENSOR A5
#define RIGHT_SENSOR A3
#define LINE_SENSOR  A0

// ---------------- SENSOR THRESHOLDS ----------------

// Opponent sensors read LOW when an opponent is detected
const int LEFT_THRESHOLD  = 300;
const int FRONT_THRESHOLD = 300;
const int RIGHT_THRESHOLD = 400;

// Boundary is expected to read BELOW this value
const int LINE_THRESHOLD = 750;

// ---------------- MOTOR SPEEDS ----------------

// PWM range: 0-255
const int SEARCH_SPEED  = 120;  // Half-speed searching
const int ATTACK_SPEED  = 255;  // Full PWM attack
const int TURN_SPEED    = 240;  // Normal turning
const int REVERSE_SPEED = 255;  // Full PWM reverse

// ---------------- TIMINGS ----------------

const unsigned long START_DELAY = 5000;

// Front attack cycle
const unsigned long FRONT_ATTACK_TIME  = 5000;
const unsigned long FRONT_REVERSE_TIME = 200;
const unsigned long FRONT_TURN_TIME    = 250;

// Side opponent behavior
const unsigned long SIDE_TURN_TIME   = 180;
const unsigned long SIDE_ATTACK_TIME  = 1000;

// Boundary recovery sequence
const unsigned long ESCAPE_REVERSE_TIME = 500;
const unsigned long ESCAPE_TURN_TIME    = 500;
const unsigned long ESCAPE_FORWARD_TIME = 500;

// ---------------- MOTOR CONTROL ----------------

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward(int speedValue) {
  // Both motors move forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void reverseMotors(int speedValue) {
  // Both motors move backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void pivotLeft(int speedValue) {
  // Left motor backward, right motor forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void pivotRight(int speedValue) {
  // Left motor forward, right motor backward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

// ---------------- SENSOR DETECTION ----------------

bool opponentLeft() {
  return analogRead(LEFT_SENSOR) < LEFT_THRESHOLD;
}

bool opponentFront() {
  return analogRead(FRONT_SENSOR) < FRONT_THRESHOLD;
}

bool opponentRight() {
  return analogRead(RIGHT_SENSOR) < RIGHT_THRESHOLD;
}

// Confirm boundary using 3 quick readings.
// Boundary must read below LINE_THRESHOLD.
bool confirmedLineDetected() {
  int lowCount = 0;

  for (int i = 0; i < 3; i++) {
    if (analogRead(LINE_SENSOR) < LINE_THRESHOLD) {
      lowCount++;
    }
  }

  return lowCount >= 2;
}

// ---------------- BOUNDARY RECOVERY ----------------

bool checkAndEscapeBoundary() {
  if (!confirmedLineDetected()) {
    return false;
  }

  Serial.println("BOUNDARY DETECTED!");

  // STEP 1: Reverse at full PWM for 500 ms
  Serial.println("REVERSE - FULL PWM");
  reverseMotors(255);
  delay(ESCAPE_REVERSE_TIME);

  // STEP 2: Rotate right at full PWM for 500 ms
  // Left motor forward, right motor backward
  Serial.println("ROTATE RIGHT - FULL PWM");
  pivotRight(255);
  delay(ESCAPE_TURN_TIME);

  // STEP 3: Move forward at full PWM for 500 ms
  Serial.println("FORWARD - FULL PWM");
  forward(255);
  delay(ESCAPE_FORWARD_TIME);

  Serial.println("RESUMING NORMAL OPERATION");

  // Main loop will check all sensors again
  return true;
}

// ---------------- MOVEMENT WITH BOUNDARY CHECK ----------------

bool runMovementWithBoundaryCheck(
  void (*movement)(int),
  int speedValue,
  unsigned long duration
) {
  unsigned long startTime = millis();

  while (millis() - startTime < duration) {
    // Boundary has priority during every timed movement
    if (checkAndEscapeBoundary()) {
      return false;
    }

    movement(speedValue);
    delay(5);
  }

  return true;
}

// ---------------- FRONT ATTACK CYCLE ----------------

void frontAttackSequence() {
  while (true) {
    Serial.println("FRONT ATTACK - 5 SECONDS");

    // Attack forward for 5 seconds
    if (!runMovementWithBoundaryCheck(
          forward, ATTACK_SPEED, FRONT_ATTACK_TIME)) {
      return;
    }

    // Brief reverse
    Serial.println("FRONT ATTACK - BRIEF REVERSE");

    if (!runMovementWithBoundaryCheck(
          reverseMotors, REVERSE_SPEED, FRONT_REVERSE_TIME)) {
      return;
    }

    // Brief right pivot
    Serial.println("FRONT ATTACK - PIVOT RIGHT");

    if (!runMovementWithBoundaryCheck(
          pivotRight, TURN_SPEED, FRONT_TURN_TIME)) {
      return;
    }

    // Repeat until boundary detection interrupts the sequence.
  }
}

// ---------------- SETUP ----------------

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LEFT_SENSOR, INPUT);
  pinMode(FRONT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(LINE_SENSOR, INPUT);

  stopMotors();

  Serial.println("SUMOBOT READY");
  Serial.println("STARTING IN 5 SECONDS");

  delay(START_DELAY);

  Serial.println("AUTONOMOUS MODE STARTED");
}

// ---------------- MAIN LOOP ----------------

void loop() {
  int leftValue  = analogRead(LEFT_SENSOR);
  int frontValue = analogRead(FRONT_SENSOR);
  int rightValue = analogRead(RIGHT_SENSOR);
  int lineValue  = analogRead(LINE_SENSOR);

  // Serial Monitor diagnostics
  Serial.print("L:");
  Serial.print(leftValue);

  Serial.print(" F:");
  Serial.print(frontValue);

  Serial.print(" R:");
  Serial.print(rightValue);

  Serial.print(" LINE:");
  Serial.println(lineValue);

  // Highest priority: boundary detection
  if (checkAndEscapeBoundary()) {
    return;
  }

  // FRONT opponent: repeat 5-second attack cycle
  if (frontValue < FRONT_THRESHOLD) {
    frontAttackSequence();
    return;
  }

  // RIGHT opponent: pivot right, then attack
  if (rightValue < RIGHT_THRESHOLD) {
    Serial.println("OPPONENT RIGHT");

    if (!runMovementWithBoundaryCheck(
          pivotRight, TURN_SPEED, SIDE_TURN_TIME)) {
      return;
    }

    runMovementWithBoundaryCheck(
      forward, ATTACK_SPEED, SIDE_ATTACK_TIME);

    return;
  }

  // LEFT opponent: pivot left, then attack
  if (leftValue < LEFT_THRESHOLD) {
    Serial.println("OPPONENT LEFT");

    if (!runMovementWithBoundaryCheck(
          pivotLeft, TURN_SPEED, SIDE_TURN_TIME)) {
      return;
    }

    runMovementWithBoundaryCheck(
      forward, ATTACK_SPEED, SIDE_ATTACK_TIME);

    return;
  }

  // No opponent detected: search forward at half speed
  Serial.println("NO ENEMY - HALF SPEED SEARCH");
  forward(SEARCH_SPEED);
}
