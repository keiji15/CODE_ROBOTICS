// ========================================
// HC-06 BLUETOOTH SUMOBOT
// Arduino Nano ATmega328P
//
// HC-06 TXD -> Nano RX0 / D0
// HC-06 RXD -> Nano TX1 / D1
// HC-06 RXD should use a voltage divider
// ========================================

// TB6612FNG motor driver pins
#define PWMA 5
#define PWMB 6
#define AIN1 7
#define AIN2 8
#define BIN1 9
#define BIN2 10
#define STBY 11

// Motor speed: 0 to 255
int motorSpeed = 200;

char command;

// Failsafe
unsigned long lastCommandTime = 0;
const unsigned long timeout = 1000;

void setup() {

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  // Enable TB6612FNG
  digitalWrite(STBY, HIGH);

  stopMotors();

  // HC-06 uses Nano hardware serial
  Serial.begin(9600);
}

void loop() {

  if (Serial.available()) {

    command = Serial.read();

    // Convert lowercase to uppercase
    if (command >= 'a' && command <= 'z') {
      command -= 32;
    }

    switch (command) {

      case 'F':
        forward();
        lastCommandTime = millis();
        break;

      case 'B':
        backward();
        lastCommandTime = millis();
        break;

      case 'L':
        left();
        lastCommandTime = millis();
        break;

      case 'R':
        right();
        lastCommandTime = millis();
        break;

      case 'S':
        stopMotors();
        lastCommandTime = 0;
        break;
    }
  }

  // Failsafe stop
  if (lastCommandTime != 0 &&
      millis() - lastCommandTime > timeout) {

    stopMotors();
    lastCommandTime = 0;
  }
}

// ========================================
// FORWARD
// ========================================

void forward() {

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

// ========================================
// BACKWARD
// ========================================

void backward() {

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

// ========================================
// LEFT
// ========================================

void left() {

  // Left motor backward
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  // Right motor forward
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

// ========================================
// RIGHT
// ========================================

void right() {

  // Left motor forward
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  // Right motor backward
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

// ========================================
// STOP
// ========================================

void stopMotors() {

  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}
