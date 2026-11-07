#include <SoftwareSerial.h>

// HC-06 Bluetooth
// HC-06 TXD -> Nano D2
// HC-06 RXD <- Nano D3 (through voltage divider)
SoftwareSerial BT(2, 3); // RX, TX

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

// Failsafe: stop if no repeated command is received
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

  BT.begin(9600);
  Serial.begin(9600); // USB debugging
}

void loop() {
  if (BT.available()) {
    command = BT.read();

    // Convert lowercase commands to uppercase
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

  // Stop if Bluetooth commands stop arriving
  if (lastCommandTime != 0 &&
      millis() - lastCommandTime > timeout) {
    stopMotors();
    lastCommandTime = 0;
  }
}

// Left motor: A channel
// Right motor: B channel

void forward() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

void backward() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

void left() {
  // Left motor backward, right motor forward
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

void right() {
  // Left motor forward, right motor backward
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}

void stopMotors() {
  // Coast to a stop by disabling PWM
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}
