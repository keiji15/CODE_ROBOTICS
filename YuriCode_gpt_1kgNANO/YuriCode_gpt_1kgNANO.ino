#include <xmotionV3.h>

// Motor pins
const int ENA = 3;  // PWM - Left motor speed
const int IN1 = 4;
const int IN2 = 5;

const int ENB = 9;  // PWM - Right motor speed
const int IN3 = 6;
const int IN4 = 7;

// Sensor pins
const int MR45_LEFT = 10;
const int MR45_CENTER = 11;
const int MR45_RIGHT = 12;

const int LINE_SENSOR = A0;

// Thresholds
const int LINE_THRESHOLD = 600; // Adjust this based on your line sensor

// Motor speed
const int speed = 180; // Adjust based on your motor power

// One-time startup flag
bool started = false;

void setup() {
  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Wait 5 seconds on first loop
  if (!started) {
    Serial.println("Starting in 5 seconds...");
    delay(5000); // 5 second delay
    started = true;
  }

  int lineValue = analogRead(LINE_SENSOR);
  bool enemyLeft = digitalRead(MR45_LEFT) == LOW;
  bool enemyCenter = digitalRead(MR45_CENTER) == LOW;
  bool enemyRight = digitalRead(MR45_RIGHT) == LOW;

  // Avoid falling off
  if (lineValue < LINE_THRESHOLD) {
    reverse();
    delay(300);
    turnRight();
    delay(400);
    stopMotors();
    return;
  }

  // Enemy detection
  if (enemyCenter) {
    forward();
  } else if (enemyLeft) {
    turnLeft();
  } else if (enemyRight) {
    turnRight();
  } else {
    // No enemy detected - rotate to search
    rotateLeft();
  }
}

// Movement functions

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, SPEED);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED);
}

void reverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, SPEED);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, SPEED);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, SPEED / 2);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED / 2);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, SPEED / 2);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, SPEED / 2);
}

void rotateLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, SPEED);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED);
}
