// ========================================
// HC-05 BLUETOOTH 4-WHEEL MECANUM ROBOT
// Arduino Nano ATmega328P
//
// HC-05 TXD -> Nano D0 / RX
// HC-05 RXD -> Nano D1 / TX
//
// TWO L298N MOTOR DRIVERS
//
// L298N #1
// OUT1/OUT2 -> FRONT LEFT MOTOR
// OUT3/OUT4 -> FRONT RIGHT MOTOR
//
// L298N #2
// OUT1/OUT2 -> REAR LEFT MOTOR
// OUT3/OUT4 -> REAR RIGHT MOTOR
// ========================================


// ========================================
// L298N #1
// FRONT MOTORS
// ========================================

// FRONT LEFT
#define FL_EN   5
#define FL_IN1  2
#define FL_IN2  4

// FRONT RIGHT
#define FR_EN   6
#define FR_IN1  7
#define FR_IN2  8


// ========================================
// L298N #2
// REAR MOTORS
// ========================================

// REAR LEFT
#define RL_EN   9
#define RL_IN1  12
#define RL_IN2  13

// REAR RIGHT
#define RR_EN   10
#define RR_IN1  A0
#define RR_IN2  A1


// ========================================
// MOTOR SPEED
// ========================================

int motorSpeed = 150;

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

  // Front Left
  pinMode(FL_EN, OUTPUT);
  pinMode(FL_IN1, OUTPUT);
  pinMode(FL_IN2, OUTPUT);

  // Front Right
  pinMode(FR_EN, OUTPUT);
  pinMode(FR_IN1, OUTPUT);
  pinMode(FR_IN2, OUTPUT);

  // Rear Left
  pinMode(RL_EN, OUTPUT);
  pinMode(RL_IN1, OUTPUT);
  pinMode(RL_IN2, OUTPUT);

  // Rear Right
  pinMode(RR_EN, OUTPUT);
  pinMode(RR_IN1, OUTPUT);
  pinMode(RR_IN2, OUTPUT);


  // Stop all motors
  stopMotors();


  // HC-05
  Serial.begin(9600);

  delay(500);


  Serial.println("================================");
  Serial.println("     HC-05 MECANUM ROBOT");
  Serial.println("================================");
  Serial.println("Bluetooth: HC-05");
  Serial.println("Baud Rate: 9600");
  Serial.println("--------------------------------");
  Serial.println("F = Forward");
  Serial.println("B = Backward");
  Serial.println("L = Strafe Left");
  Serial.println("R = Strafe Right");
  Serial.println("Q = Rotate Left");
  Serial.println("E = Rotate Right");
  Serial.println("S = Stop");
  Serial.println("================================");
}


// ========================================
// MAIN LOOP
// ========================================

void loop() {

  if (Serial.available() > 0) {

    command = Serial.read();


    // Convert lowercase to uppercase
    if (command >= 'a' && command <= 'z') {
      command -= 32;
    }


    // Ignore newline/carriage return
    if (command == '\n' || command == '\r') {
      return;
    }


    Serial.print("Received: ");
    Serial.println(command);


    switch (command) {

      // ==================================
      // FORWARD
      // ==================================

      case 'F':

        forward();

        lastCommandTime = millis();

        Serial.println("FORWARD");

        break;


      // ==================================
      // BACKWARD
      // ==================================

      case 'B':

        backward();

        lastCommandTime = millis();

        Serial.println("BACKWARD");

        break;


      // ==================================
      // STRAFE LEFT
      // ==================================

      case 'L':

        strafeLeft();

        lastCommandTime = millis();

        Serial.println("STRAFE LEFT");

        break;


      // ==================================
      // STRAFE RIGHT
      // ==================================

      case 'R':

        strafeRight();

        lastCommandTime = millis();

        Serial.println("STRAFE RIGHT");

        break;


      // ==================================
      // ROTATE LEFT
      // ==================================

      case 'Q':

        rotateLeft();

        lastCommandTime = millis();

        Serial.println("ROTATE LEFT");

        break;


      // ==================================
      // ROTATE RIGHT
      // ==================================

      case 'E':

        rotateRight();

        lastCommandTime = millis();

        Serial.println("ROTATE RIGHT");

        break;


      // ==================================
      // STOP
      // ==================================

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
// GENERIC MOTOR CONTROL
// ========================================
//
// Positive value  = forward
// Negative value  = backward
// Zero             = stop
//
// ========================================

void setMotor(int en, int in1, int in2, int speedValue) {

  speedValue = constrain(speedValue, -255, 255);


  if (speedValue > 0) {

    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    analogWrite(en, speedValue);
  }


  else if (speedValue < 0) {

    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    analogWrite(en, -speedValue);
  }


  else {

    analogWrite(en, 0);

    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}


// ========================================
// FORWARD
// ========================================

void forward() {

  setMotor(FL_EN, FL_IN1, FL_IN2, motorSpeed);

  setMotor(FR_EN, FR_IN1, FR_IN2, motorSpeed);

  setMotor(RL_EN, RL_IN1, RL_IN2, motorSpeed);

  setMotor(RR_EN, RR_IN1, RR_IN2, motorSpeed);
}


// ========================================
// BACKWARD
// ========================================

void backward() {

  setMotor(FL_EN, FL_IN1, FL_IN2, -motorSpeed);

  setMotor(FR_EN, FR_IN1, FR_IN2, -motorSpeed);

  setMotor(RL_EN, RL_IN1, RL_IN2, -motorSpeed);

  setMotor(RR_EN, RR_IN1, RR_IN2, -motorSpeed);
}


// ========================================
// STRAFE LEFT
// ========================================
//
// FL = BACKWARD
// FR = FORWARD
// RL = FORWARD
// RR = BACKWARD
//
// ========================================

void strafeLeft() {

  setMotor(FL_EN, FL_IN1, FL_IN2, -motorSpeed);

  setMotor(FR_EN, FR_IN1, FR_IN2, motorSpeed);

  setMotor(RL_EN, RL_IN1, RL_IN2, motorSpeed);

  setMotor(RR_EN, RR_IN1, RR_IN2, -motorSpeed);
}


// ========================================
// STRAFE RIGHT
// ========================================
//
// FL = FORWARD
// FR = BACKWARD
// RL = BACKWARD
// RR = FORWARD
//
// ========================================

void strafeRight() {

  setMotor(FL_EN, FL_IN1, FL_IN2, motorSpeed);

  setMotor(FR_EN, FR_IN1, FR_IN2, -motorSpeed);

  setMotor(RL_EN, RL_IN1, RL_IN2, -motorSpeed);

  setMotor(RR_EN, RR_IN1, RR_IN2, motorSpeed);
}


// ========================================
// ROTATE LEFT
// ========================================
//
// LEFT  = BACKWARD
// RIGHT = FORWARD
//
// ========================================

void rotateLeft() {

  setMotor(FL_EN, FL_IN1, FL_IN2, -motorSpeed);

  setMotor(FR_EN, FR_IN1, FR_IN2, motorSpeed);

  setMotor(RL_EN, RL_IN1, RL_IN2, -motorSpeed);

  setMotor(RR_EN, RR_IN1, RR_IN2, motorSpeed);
}


// ========================================
// ROTATE RIGHT
// ========================================
//
// LEFT  = FORWARD
// RIGHT = BACKWARD
//
// ========================================

void rotateRight() {

  setMotor(FL_EN, FL_IN1, FL_IN2, motorSpeed);

  setMotor(FR_EN, FR_IN1, FR_IN2, -motorSpeed);

  setMotor(RL_EN, RL_IN1, RL_IN2, motorSpeed);

  setMotor(RR_EN, RR_IN1, RR_IN2, -motorSpeed);
}


// ========================================
// STOP ALL MOTORS
// ========================================

void stopMotors() {

  setMotor(FL_EN, FL_IN1, FL_IN2, 0);

  setMotor(FR_EN, FR_IN1, FR_IN2, 0);

  setMotor(RL_EN, RL_IN1, RL_IN2, 0);

  setMotor(RR_EN, RR_IN1, RR_IN2, 0);
}
