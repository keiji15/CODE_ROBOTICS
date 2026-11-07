// ========================================
// HC-05 BLUETOOTH SUMOBOT
// Arduino Nano ATmega328P
//
// HC-05 TXD -> Arduino Nano RX0 (D0)
// HC-05 RXD -> Arduino Nano TX1 (D1)
// ========================================

// ========================================
// TB6612FNG MOTOR DRIVER PINS
// ========================================

#define PWMA 5
#define PWMB 6

#define AIN1 7
#define AIN2 8

#define BIN1 9
#define BIN2 10

#define STBY 11


// ========================================
// MOTOR SPEED
// ========================================
// Range: 0-255

int motorSpeed = 200;

char command;


// ========================================
// FAILSAFE
// ========================================

unsigned long lastCommandTime = 0;
const unsigned long timeout = 1000;


// ========================================
// SETUP
// ========================================

void setup() {

  // Motor pins
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  // Enable TB6612FNG
  digitalWrite(STBY, HIGH);

  // Stop motors at startup
  stopMotors();

  // HC-05 hardware serial
  Serial.begin(9600);

  delay(500);

  Serial.println("================================");
  Serial.println("       SUMOBOT HC-05 READY");
  Serial.println("================================");
  Serial.println("Bluetooth: HC-05");
  Serial.println("Baud Rate: 9600");
  Serial.println("Commands: F B L R S");
  Serial.println("--------------------------------");
  Serial.println("F = Forward");
  Serial.println("B = Backward");
  Serial.println("L = Left");
  Serial.println("R = Right");
  Serial.println("S = Stop");
  Serial.println("================================");
}


// ========================================
// MAIN LOOP
// ========================================

void loop() {

  // Check for Bluetooth data
  if (Serial.available() > 0) {

    command = Serial.read();

    // Convert lowercase to uppercase
    if (command >= 'a' && command <= 'z') {
      command -= 32;
    }

    // Display received command
    Serial.print("Received: ");
    Serial.println(command);


    // ====================================
    // COMMAND PROCESSING
    // ====================================

    switch (command) {

      case 'F':

        forward();

        lastCommandTime = millis();

        Serial.println("FORWARD");

        break;


      case 'B':

        backward();

        lastCommandTime = millis();

        Serial.println("BACKWARD");

        break;


      case 'L':

        left();

        lastCommandTime = millis();

        Serial.println("LEFT");

        break;


      case 'R':

        right();

        lastCommandTime = millis();

        Serial.println("RIGHT");

        break;


      case 'S':

        stopMotors();

        lastCommandTime = 0;

        Serial.println("STOP");

        break;


      default:

        Serial.println("Unknown command");

        break;
    }
  }


  // ====================================
  // FAILSAFE
  // ====================================

  if (lastCommandTime != 0 &&
      millis() - lastCommandTime > timeout) {

    stopMotors();

    lastCommandTime = 0;

    Serial.println("FAILSAFE: Motors stopped");
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

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, motorSpeed);
  analogWrite(PWMB, motorSpeed);
}


// ========================================
// RIGHT
// ========================================

void right() {

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

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
