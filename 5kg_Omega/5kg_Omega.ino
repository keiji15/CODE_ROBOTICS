#include <IRremote.h>

// Motor Pins (adjust based on your wiring)
#define LEFT_MOTOR_PWM 5
#define LEFT_MOTOR_DIR 4
#define RIGHT_MOTOR_PWM 6
#define RIGHT_MOTOR_DIR 7

// Line Sensors (Analog)
#define LINE_LEFT A0
#define LINE_CENTER A1
#define LINE_RIGHT A2

// IR Sensors (Digital)
#define IR1 8
#define IR2 9
#define IR3 10
#define IR4 11
#define IR5 12

// DIP Switches (Tactic Selection)
#define DIP1 A3
#define DIP2 A4
#define DIP3 A5
`
// RGB (Example common-anode RGB LED)
#define RGB1_R 2
#define RGB1_G 3
#define RGB1_B 13

// IR Receiver
#define IR_RECV_PIN 0
IRrecv irrecv(IR_RECV_PIN);
decode_results results;

bool robotStarted = false;

void setup() {
  // Motors
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);

  // Sensors
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
  pinMode(LINE_LEFT, INPUT);
  pinMode(LINE_CENTER, INPUT);
  pinMode(LINE_RIGHT, INPUT);

  // DIP Switches
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);
  pinMode(DIP3, INPUT_PULLUP);

  // RGB
  pinMode(RGB1_R, OUTPUT);
  pinMode(RGB1_G, OUTPUT);
  pinMode(RGB1_B, OUTPUT);

  // Remote Start
  irrecv.enableIRIn();

  // Startup RGB effect
  startupRGB();
}

void loop() {
  if (!robotStarted) {
    if (irrecv.decode(&results)) {
      if (results.value == 0xA90) { // Sony power button
        robotStarted = true;
        lightRGB(0, 255, 0); // Green
      }
      irrecv.resume();
    }
    return;
  }

  int tactic = getTactic();
  switch (tactic) {
    case 0: forwardAttack(); break;
    case 1: retreat(); break;
    case 2: leftZigzag(); break;
    case 3: rightZigzag(); break;
    case 4: leftSemiCircle(); break;
    case 5: rightSemiCircle(); break;
    case 6: turn180(); break;
    default: stopMotors(); break;
  }
}

// ---------------- Tactics (Simplified Placeholder) ----------------

void forwardAttack() {
  if (digitalRead(IR3) == LOW) {
    lightRGB(255, 0, 0); // Red
    moveForward(255);
  } else {
    stopMotors();
  }
}

void retreat() {
  moveBackward(255);
  delay(500);
  stopMotors();
}

void leftZigzag() {
  moveForward(255);
  delay(300);
  turnLeft(255);
  delay(200);
}

void rightZigzag() {
  moveForward(255);
  delay(300);
  turnRight(255);
  delay(200);
}

void leftSemiCircle() {
  turnLeft(255);
  delay(600);
}

void rightSemiCircle() {
  turnRight(255);
  delay(600);
}

void turn180() {
  turnRight(255);
  delay(1000);
}

// ---------------- Movement ----------------

void moveForward(int speed) {
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTOR_PWM, speed);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(RIGHT_MOTOR_PWM, speed);
}

void moveBackward(int speed) {
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTOR_PWM, speed);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(RIGHT_MOTOR_PWM, speed);
}

void turnLeft(int speed) {
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTOR_PWM, speed);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(RIGHT_MOTOR_PWM, speed);
}

void turnRight(int speed) {
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTOR_PWM, speed);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(RIGHT_MOTOR_PWM, speed);
}

void stopMotors() {
  analogWrite(LEFT_MOTOR_PWM, 0);
  analogWrite(RIGHT_MOTOR_PWM, 0);
}

// ---------------- DIP Switch Reader ----------------

int getTactic() {
  int value = (!digitalRead(DIP1) << 0) |
              (!digitalRead(DIP2) << 1) |
              (!digitalRead(DIP3) << 2);
  return value;
}

// ---------------- RGB Effects ----------------

void lightRGB(int r, int g, int b) {
  analogWrite(RGB1_R, 255 - r);
  analogWrite(RGB1_G, 255 - g);
  analogWrite(RGB1_B, 255 - b);
}

void startupRGB() {
  for (int i = 0; i < 255; i += 5) {
    lightRGB(i, 0, 255 - i);
    delay(20);
  }
}
