// --- Include Libraries ---
#include <IRremote.h>

// --- Pin Definitions ---
// Motor Pins
#define MOTOR_LEFT_PWM     5
#define MOTOR_LEFT_DIR     4
#define MOTOR_RIGHT_PWM    6
#define MOTOR_RIGHT_DIR    7

// Line Sensor Pins (Analog)
#define LINE_LEFT_PIN      A0
#define LINE_RIGHT_PIN     A1
#define LINE_BACK_PIN      A2

// IR Sensors (Digital)
#define IR1_PIN            8
#define IR2_PIN            9
#define IR3_PIN            10
#define IR4_PIN            11
#define IR5_PIN            12

// DIP Switch (Tactic Selector)
#define DIP_0              A3
#define DIP_1              A4
#define DIP_2              A5

// RGB LEDs
#define RGB1_PIN           3
#define RGB2_PIN           2

// IR Remote Receiver
#define IR_RECV_PIN        13

// Constants
#define LINE_THRESHOLD     500     // Adjust based on surface
#define IR_DETECTED        LOW     // Assuming active LOW output
#define MAX_SPEED          255

// Global Objects
IRrecv irrecv(IR_RECV_PIN);
decode_results results;

// --- Setup ---
void setup() {
  Serial.begin(9600);

  // Motor Pins
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);

  // IR Sensors
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(IR3_PIN, INPUT);
  pinMode(IR4_PIN, INPUT);
  pinMode(IR5_PIN, INPUT);

  // DIP Switch
  pinMode(DIP_0, INPUT_PULLUP);
  pinMode(DIP_1, INPUT_PULLUP);
  pinMode(DIP_2, INPUT_PULLUP);

  // RGB LEDs
  pinMode(RGB1_PIN, OUTPUT);
  pinMode(RGB2_PIN, OUTPUT);

  // IR Remote
  irrecv.enableIRIn();

  // Wait for remote signal to start
  waitForStart();
}

// --- Main Loop ---
void loop() {
  byte tactic = readDipSwitch();

  switch (tactic) {
    case 0: tacticLeftZigZag(); break;
    case 1: tacticRightZigZag(); break;
    case 2: tacticRightSemiCircle(); break;
    case 3: tacticLeftSemiCircle(); break;
    case 4: tacticTurn180(); break;
    case 5: tacticRetreat(); break;
    case 6: tacticForwardAttack(); break;
    default: stopMotors(); break;
  }
}

// --- Functions ---

void waitForStart() {
  digitalWrite(RGB1_PIN, HIGH);
  digitalWrite(RGB2_PIN, HIGH);

  Serial.println("Waiting for remote start...");
  while (true) {
    if (irrecv.decode(&results)) {
      Serial.println("Started!");
      irrecv.resume();
      break;
    }
    delay(100);
  }

  digitalWrite(RGB1_PIN, LOW);
  digitalWrite(RGB2_PIN, LOW);
}

byte readDipSwitch() {
  byte val = 0;
  val |= digitalRead(DIP_0) == LOW ? 1 : 0;
  val |= digitalRead(DIP_1) == LOW ? 2 : 0;
  val |= digitalRead(DIP_2) == LOW ? 4 : 0;
  return val;
}

// Motor Control
void setMotorLeft(int speed) {
  digitalWrite(MOTOR_LEFT_DIR, speed >= 0 ? HIGH : LOW);
  analogWrite(MOTOR_LEFT_PWM, constrain(abs(speed), 0, 255));
}

void setMotorRight(int speed) {
  digitalWrite(MOTOR_RIGHT_DIR, speed >= 0 ? HIGH : LOW);
  analogWrite(MOTOR_RIGHT_PWM, constrain(abs(speed), 0, 255));
}

void stopMotors() {
  setMotorLeft(0);
  setMotorRight(0);
}

// Sensor Checks
bool lineDetectedFront() {
  return analogRead(LINE_LEFT_PIN) > LINE_THRESHOLD || analogRead(LINE_RIGHT_PIN) > LINE_THRESHOLD;
}

bool lineDetectedBack() {
  return analogRead(LINE_BACK_PIN) > LINE_THRESHOLD;
}

bool opponentDetected() {
  return digitalRead(IR1_PIN) == IR_DETECTED ||
         digitalRead(IR2_PIN) == IR_DETECTED ||
         digitalRead(IR3_PIN) == IR_DETECTED ||
         digitalRead(IR4_PIN) == IR_DETECTED ||
         digitalRead(IR5_PIN) == IR_DETECTED;
}

// Tactics

void tacticLeftZigZag() {
  while (true) {
    if (lineDetectedFront()) retreat();
    if (opponentDetected()) attack();
    setMotorLeft(200);
    setMotorRight(100);
    delay(500);
    setMotorLeft(100);
    setMotorRight(200);
    delay(500);
  }
}

void tacticRightZigZag() {
  while (true) {
    if (lineDetectedFront()) retreat();
    if (opponentDetected()) attack();
    setMotorLeft(100);
    setMotorRight(200);
    delay(500);
    setMotorLeft(200);
    setMotorRight(100);
    delay(500);
  }
}

void tacticRightSemiCircle() {
  while (true) {
    if (lineDetectedFront()) retreat();
    if (opponentDetected()) attack();
    setMotorLeft(200);
    setMotorRight(0);
    delay(700);
  }
}

void tacticLeftSemiCircle() {
  while (true) {
    if (lineDetectedFront()) retreat();
    if (opponentDetected()) attack();
    setMotorLeft(0);
    setMotorRight(200);
    delay(700);
  }
}

void tacticTurn180() {
  setMotorLeft(200);
  setMotorRight(-200);
  delay(600);
}

void tacticRetreat() {
  setMotorLeft(-200);
  setMotorRight(-200);
  delay(500);
  stopMotors();
}

void tacticForwardAttack() {
  while (true) {
    if (lineDetectedFront()) retreat();
    if (opponentDetected()) attack();
    setMotorLeft(255);
    setMotorRight(255);
  }
}

void attack() {
  digitalWrite(RGB1_PIN, HIGH);
  digitalWrite(RGB2_PIN, HIGH);
  setMotorLeft(255);
  setMotorRight(255);
}

void retreat() {
  digitalWrite(RGB1_PIN, LOW);
  digitalWrite(RGB2_PIN, LOW);
  tacticRetreat();
}
