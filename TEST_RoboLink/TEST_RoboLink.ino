#include <WiFi.h>
#include <RoboLink.h>

RoboLinkWiFi robolink;

// =====================================================
// L298N #1 - RIGHT MOTOR GROUP (SERIES)
// Channel A: OUT1 / OUT2
// =====================================================
#define RIGHT_EN   25
#define RIGHT_IN1  26
#define RIGHT_IN2  27

// =====================================================
// L298N #2 - LEFT MOTOR GROUP (SERIES)
// Channel B: OUT3 / OUT4
// =====================================================
#define LEFT_EN    14
#define LEFT_IN1   12
#define LEFT_IN2   13

// =====================================================
// L298N #3 - VERTICAL MOTOR
// Channel B: OUT3 / OUT4
// =====================================================
#define VERTICAL_EN   32
#define VERTICAL_IN1  33
#define VERTICAL_IN2  18

// =====================================================
// L298N #3 - CLAW MOTOR
// Channel A: OUT1 / OUT2
// =====================================================
#define CLAW_EN   22
#define CLAW_IN1  21
#define CLAW_IN2  19

// =====================================================
// ROBO LINK SETTINGS
// =====================================================
const int INPUT_MIN = 600;
const int INPUT_MAX = 1400;
const int CENTER = 1000;
const int DEADZONE = 80;

const unsigned long ARM_DELAY_MS = 1500;

// =====================================================
// STARTUP / ARMING STATE
// =====================================================
bool motorsArmed = false;
unsigned long neutralSince = 0;

// =====================================================
// VALIDATE JOYSTICK VALUES
// =====================================================
bool validAxis(int value) {
  return value >= INPUT_MIN && value <= INPUT_MAX;
}

bool axisCentered(int value) {
  return value >= CENTER - DEADZONE &&
         value <= CENTER + DEADZONE;
}

// =====================================================
// MOTOR DRIVER CONTROL
// speed: -255 = reverse, 0 = stop, +255 = forward
// =====================================================
void setMotor(int en, int in1, int in2, int speed) {
  speed = constrain(speed, -255, 255);

  if (speed == 0) {
    analogWrite(en, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    return;
  }

  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }

  analogWrite(en, abs(speed));
}

// =====================================================
// STOP FUNCTIONS
// =====================================================
void stopRight() {
  setMotor(RIGHT_EN, RIGHT_IN1, RIGHT_IN2, 0);
}

void stopLeft() {
  setMotor(LEFT_EN, LEFT_IN1, LEFT_IN2, 0);
}

void stopVertical() {
  setMotor(VERTICAL_EN, VERTICAL_IN1, VERTICAL_IN2, 0);
}

void stopClaw() {
  setMotor(CLAW_EN, CLAW_IN1, CLAW_IN2, 0);
}

void stopAllMotors() {
  stopRight();
  stopLeft();
  stopVertical();
  stopClaw();
}

// =====================================================
// CONVERT JOYSTICK TO SPEED
// Invalid readings must never command a motor.
// =====================================================
int joystickToSpeed(int value) {
  if (!validAxis(value)) {
    return 0;
  }

  if (axisCentered(value)) {
    return 0;
  }

  if (value > CENTER + DEADZONE) {
    return constrain(
      (int)map(value, CENTER + DEADZONE,
               INPUT_MAX, 0, 255),
      0, 255
    );
  }

  return -constrain(
    (int)map(value, INPUT_MIN,
             CENTER - DEADZONE, 255, 0),
    0, 255
  );
}

// =====================================================
// MAIN MOVEMENT
// =====================================================
void controlMovement(int steerValue, int throttleValue) {
  int steer = joystickToSpeed(steerValue);
  int throttle = joystickToSpeed(throttleValue);

  int leftSpeed = constrain(throttle + steer, -255, 255);
  int rightSpeed = constrain(throttle - steer, -255, 255);

  setMotor(RIGHT_EN, RIGHT_IN1, RIGHT_IN2, rightSpeed);
  setMotor(LEFT_EN, LEFT_IN1, LEFT_IN2, leftSpeed);
}

// =====================================================
// VERTICAL MOTOR
// =====================================================
void controlVertical(int value) {
  int speed = joystickToSpeed(value);

  setMotor(
    VERTICAL_EN,
    VERTICAL_IN1,
    VERTICAL_IN2,
    speed
  );
}

// =====================================================
// CLAW MOTOR
// Buttons: ON = 100, OFF = 0
// =====================================================
void controlClaw(int openButton, int closeButton) {
  if (openButton == 100 && closeButton == 100) {
    stopClaw();
  } else if (openButton == 100) {
    setMotor(CLAW_EN, CLAW_IN1, CLAW_IN2, 255);
  } else if (closeButton == 100) {
    setMotor(CLAW_EN, CLAW_IN1, CLAW_IN2, -255);
  } else {
    stopClaw();
  }
}

// =====================================================
// INITIALIZE OUTPUTS TO LOW
// =====================================================
void initializeMotorPins() {
  // Set the output latches LOW before enabling outputs.
  digitalWrite(RIGHT_EN, LOW);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);

  digitalWrite(LEFT_EN, LOW);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(VERTICAL_EN, LOW);
  digitalWrite(VERTICAL_IN1, LOW);
  digitalWrite(VERTICAL_IN2, LOW);

  digitalWrite(CLAW_EN, LOW);
  digitalWrite(CLAW_IN1, LOW);
  digitalWrite(CLAW_IN2, LOW);

  pinMode(RIGHT_EN, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  pinMode(LEFT_EN, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  pinMode(VERTICAL_EN, OUTPUT);
  pinMode(VERTICAL_IN1, OUTPUT);
  pinMode(VERTICAL_IN2, OUTPUT);

  pinMode(CLAW_EN, OUTPUT);
  pinMode(CLAW_IN1, OUTPUT);
  pinMode(CLAW_IN2, OUTPUT);

  stopAllMotors();
}

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);

  initializeMotorPins();
  delay(500);

  // RoboLink's communication timeout, where supported.
  robolink.setTimeoutMs(500);
  robolink.beginAP("SMARTDRAIN", "12345678");

  stopAllMotors();

  Serial.println("SMARTDRAIN STARTED");
  Serial.println("MOTORS DISARMED");
  Serial.println("Connect RoboLink and center all joysticks.");
}

// =====================================================
// MAIN LOOP
// =====================================================
void loop() {
  robolink.update();

  int steer = robolink.get("steer");
  int throttle = robolink.get("throttle");
  int vertical = robolink.get("vertical");

  int clawOpen = robolink.get("claw_open");
  int clawClose = robolink.get("claw_close");

  // ---------------------------------------------------
  // REQUIRE VALID JOYSTICK DATA BEFORE ARMING
  // ---------------------------------------------------
  bool axesValid =
      validAxis(steer) &&
      validAxis(throttle) &&
      validAxis(vertical);

  bool controlsCentered =
      axesValid &&
      axisCentered(steer) &&
      axisCentered(throttle) &&
      axisCentered(vertical) &&
      clawOpen == 0 &&
      clawClose == 0;

  if (!motorsArmed) {
    stopAllMotors();

    if (!controlsCentered) {
      neutralSince = 0;
    } else {
      if (neutralSince == 0) {
        neutralSince = millis();
      }

      if (millis() - neutralSince >= ARM_DELAY_MS) {
        motorsArmed = true;
        Serial.println("MOTORS ARMED");
      }
    }

    delay(20);
    return;
  }

  // ---------------------------------------------------
  // INVALID AXIS DATA: STOP AND DISARM
  // ---------------------------------------------------
  if (!axesValid) {
    stopAllMotors();
    motorsArmed = false;
    neutralSince = 0;

    Serial.println("INVALID CONTROL DATA - DISARMED");
    delay(20);
    return;
  }

  // ---------------------------------------------------
  // NORMAL MOTOR CONTROL
  // ---------------------------------------------------
  controlMovement(steer, throttle);
  controlVertical(vertical);
  controlClaw(clawOpen, clawClose);

  delay(20);
}
