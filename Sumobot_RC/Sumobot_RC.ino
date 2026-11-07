#include <L298N.h>

/*
 * Sumobot RC Control via Bluetooth (HC-05)
 * Components: Arduino Nano, HC-05, L298N, 2x DC Motors
 * LEFT motor = Motor A (ENA, IN1, IN2)
 * RIGHT motor = Motor B (ENB, IN3, IN4)
 */

// Motor Driver Pins (L298N)
#define ENA 6    // Enable pin for Left Motor (PWM)
#define ENB 5    // Enable pin for Right Motor (PWM)
#define IN1 7    // Left Motor direction pin 1
#define IN2 8    // Left Motor direction pin 2
#define IN3 9    // Right Motor direction pin 1
#define IN4 10   // Right Motor direction pin 2

char command; // incoming Bluetooth command
int motorSpeed = 200; // Default speed (0–255)

void setup() {
  // Set motor pins as output
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Start serial for Bluetooth (HC-05 defaults to 9600 baud)
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    command = Serial.read();  // Read Bluetooth command
    stopMotors();             // Stop first, then act

    switch (command) {
      case 'F': forward(); break;   // Forward
      case 'B': backward(); break;  // Backward
      case 'L': left(); break;      // Turn Left
      case 'R': right(); break;     // Turn Right
      case 'S': stopMotors(); break; // Stop
    }
  }
}

// --------- Motor Control Functions ---------
void forward() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backward() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  // Stop both motors by disabling direction only
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
