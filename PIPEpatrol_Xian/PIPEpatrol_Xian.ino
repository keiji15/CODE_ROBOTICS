#include <WiFi.h>
#include <RoboLink.h>

RoboLinkWiFi robolink;

// =====================================================
// RIGHT MOTOR - L298N #1 - CHANNEL A
// =====================================================
#define RIGHT_EN   25
#define RIGHT_IN1  26
#define RIGHT_IN2  27

// =====================================================
// LEFT MOTOR - L298N #2 - CHANNEL B
// =====================================================
#define LEFT_EN    14
#define LEFT_IN1   12
#define LEFT_IN2   13

// =====================================================
// VERTICAL MOTOR - L298N #3 - CHANNEL B
// =====================================================
#define VERTICAL_EN   32
#define VERTICAL_IN1  33
#define VERTICAL_IN2  18

// =====================================================
// CLAW MOTOR - L298N #3 - CHANNEL A
// =====================================================
#define CLAW_EN   22
#define CLAW_IN1  21
#define CLAW_IN2  19

// =====================================================
// ROBO LINK SETTINGS
// =====================================================
const int CENTER = 1000;
const int DEADZONE = 80;

// =====================================================
// SETUP
// =====================================================
void setup() {

  Serial.begin(115200);

  // ===================================================
  // CONFIGURE ALL MOTOR PINS
  // ===================================================

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

  // ===================================================
  // IMMEDIATELY FORCE ALL MOTOR OUTPUTS OFF
  // ===================================================

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

  // ===================================================
  // ALLOW MOTOR DRIVERS TO SETTLE
  // ===================================================

  delay(1000);

  // ===================================================
  // START ROBOLINK
  // ===================================================

  robolink.beginAP("SMARTDRAIN", "12345678");

  Serial.println();
  Serial.println("================================");
  Serial.println("       SMARTDRAIN READY");
  Serial.println("================================");
  Serial.println("All motors initialized OFF.");
  Serial.println("RoboLink Wi-Fi started.");
  Serial.println();
}

// =====================================================
// MAIN LOOP
// =====================================================
void loop() {

  robolink.update();

  // ===================================================
  // MAIN MOVEMENT JOYSTICK
  // ===================================================

  int steer = robolink.get("steer");
  int throttle = robolink.get("throttle");

  controlMovement(steer, throttle);

  // ===================================================
  // VERTICAL JOYSTICK
  // X = vertical_x (ignored)
  // Y = vertical
  // ===================================================

  int vertical = robolink.get("vertical");

  controlVertical(vertical);

  // ===================================================
  // CLAW BUTTONS
  // ===================================================

  int clawOpen = robolink.get("claw_open");
  int clawClose = robolink.get("claw_close");

  controlClaw(clawOpen, clawClose);

  // ===================================================
  // SERIAL DEBUG
  // ===================================================

  Serial.print("Steer: ");
  Serial.print(steer);

  Serial.print(" | Throttle: ");
  Serial.print(throttle);

  Serial.print(" | Vertical: ");
  Serial.print(vertical);

  Serial.print(" | Claw Open: ");
  Serial.print(clawOpen);

  Serial.print(" | Claw Close: ");
  Serial.println(clawClose);

  delay(20);
}

// =====================================================
// MOVEMENT CONTROL
// =====================================================
void controlMovement(int steer, int throttle) {

  // ===================================================
  // FORWARD
  // ===================================================

  if (throttle > CENTER + DEADZONE) {

    int speed = (int)map(
      throttle,
      CENTER + DEADZONE,
      1400,
      0,
      255
    );

    speed = constrain(speed, 0, 255);

    // -------------------------
    // STRAIGHT FORWARD
    // -------------------------

    if (steer >= CENTER - DEADZONE &&
        steer <= CENTER + DEADZONE) {

      rightForward(speed);
      leftForward(speed);
    }

    // -------------------------
    // TURN RIGHT
    // -------------------------

    else if (steer > CENTER + DEADZONE) {

      int turn = (int)map(
        steer,
        CENTER + DEADZONE,
        1400,
        0,
        speed
      );

      turn = constrain(turn, 0, speed);

      rightForward(speed - turn);
      leftForward(speed);
    }

    // -------------------------
    // TURN LEFT
    // -------------------------

    else {

      int turn = (int)map(
        steer,
        600,
        CENTER - DEADZONE,
        speed,
        0
      );

      turn = constrain(turn, 0, speed);

      rightForward(speed);
      leftForward(speed - turn);
    }
  }

  // ===================================================
  // BACKWARD
  // ===================================================

  else if (throttle < CENTER - DEADZONE) {

    int speed = (int)map(
      throttle,
      600,
      CENTER - DEADZONE,
      255,
      0
    );

    speed = constrain(speed, 0, 255);

    // -------------------------
    // STRAIGHT BACKWARD
    // -------------------------

    if (steer >= CENTER - DEADZONE &&
        steer <= CENTER + DEADZONE) {

      rightBackward(speed);
      leftBackward(speed);
    }

    // -------------------------
    // TURN RIGHT
    // -------------------------

    else if (steer > CENTER + DEADZONE) {

      int turn = (int)map(
        steer,
        CENTER + DEADZONE,
        1400,
        0,
        speed
      );

      turn = constrain(turn, 0, speed);

      rightBackward(speed - turn);
      leftBackward(speed);
    }

    // -------------------------
    // TURN LEFT
    // -------------------------

    else {

      int turn = (int)map(
        steer,
        600,
        CENTER - DEADZONE,
        speed,
        0
      );

      turn = constrain(turn, 0, speed);

      rightBackward(speed);
      leftBackward(speed - turn);
    }
  }

  // ===================================================
  // STOP
  // ===================================================

  else {

    stopRight();
    stopLeft();
  }
}

// =====================================================
// RIGHT MOTOR
// =====================================================
void rightForward(int speed) {

  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);

  analogWrite(RIGHT_EN, speed);
}

void rightBackward(int speed) {

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);

  analogWrite(RIGHT_EN, speed);
}

void stopRight() {

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);

  analogWrite(RIGHT_EN, 0);
}

// =====================================================
// LEFT MOTOR
// =====================================================
void leftForward(int speed) {

  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);

  analogWrite(LEFT_EN, speed);
}

void leftBackward(int speed) {

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);

  analogWrite(LEFT_EN, speed);
}

void stopLeft() {

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);

  analogWrite(LEFT_EN, 0);
}

// =====================================================
// VERTICAL MOTOR
// =====================================================
void controlVertical(int vertical) {

  // ===================================================
  // CENTER = STOP
  // 920 - 1080
  // ===================================================

  if (vertical >= CENTER - DEADZONE &&
      vertical <= CENTER + DEADZONE) {

    stopVertical();
    return;
  }

  // ===================================================
  // UP
  // ===================================================

  if (vertical > CENTER + DEADZONE) {

    int speed = (int)map(
      vertical,
      CENTER + DEADZONE,
      1400,
      0,
      255
    );

    speed = constrain(speed, 0, 255);

    digitalWrite(VERTICAL_IN1, HIGH);
    digitalWrite(VERTICAL_IN2, LOW);

    analogWrite(VERTICAL_EN, speed);
  }

  // ===================================================
  // DOWN
  // ===================================================

  else {

    int speed = (int)map(
      vertical,
      600,
      CENTER - DEADZONE,
      255,
      0
    );

    speed = constrain(speed, 0, 255);

    digitalWrite(VERTICAL_IN1, LOW);
    digitalWrite(VERTICAL_IN2, HIGH);

    analogWrite(VERTICAL_EN, speed);
  }
}

void stopVertical() {

  digitalWrite(VERTICAL_IN1, LOW);
  digitalWrite(VERTICAL_IN2, LOW);

  analogWrite(VERTICAL_EN, 0);
}

// =====================================================
// CLAW MOTOR
// =====================================================
void controlClaw(int openButton, int closeButton) {

  // ===================================================
  // BOTH BUTTONS = STOP
  // ===================================================

  if (openButton == 100 &&
      closeButton == 100) {

    stopClaw();
    return;
  }

  // ===================================================
  // OPEN
  // ===================================================

  if (openButton == 100) {

    digitalWrite(CLAW_IN1, HIGH);
    digitalWrite(CLAW_IN2, LOW);

    analogWrite(CLAW_EN, 255);
  }

  // ===================================================
  // CLOSE
  // ===================================================

  else if (closeButton == 100) {

    digitalWrite(CLAW_IN1, LOW);
    digitalWrite(CLAW_IN2, HIGH);

    analogWrite(CLAW_EN, 255);
  }

  // ===================================================
  // STOP
  // ===================================================

  else {

    stopClaw();
  }
}

void stopClaw() {

  digitalWrite(CLAW_IN1, LOW);
  digitalWrite(CLAW_IN2, LOW);

  analogWrite(CLAW_EN, 0);
}
