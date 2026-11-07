#include <Servo.h>
//////////////// Motor Outputs////////////////////
#define LeftMotorPwm 3      // LEFT Motor Pwm Output
#define RightMotorPwm 6     // RIGHT Motor Pwm Output
#define LeftMotorEnable 7   // LEFT Motor Enable
#define RightMotorEnable 9  // Right Motor Enable

Servo Rainbow; 

int pos = 0;
int starting = 0;

int switch1 = 0;
int switch2 = 1;

int BackTime = A6;
int TurnTime = A7;

/// Sensor Inputs ///
int LSens = 8;    // LEFT Sensor
int LFSens = 11;  // LEFT Diagonal Sensor
int MSens = 10;   // MIDDLE Sensor
int RFSens = A5;  // RIGHT Diagonal Sensor
int RSens = 12;   // RIGHT Sensor

/// Edge Sensor Inputs ///
int LeftLine = A0;         // LEFT Line Sensor
int RightLine = A1;        // RIGHT Line Sensor
int BackLine = A2;         // Back Line Sensor
int LineBlackValue = 650;  //
int LineWhiteValue = 500;  //


/// Start Module ///
int Start = 5;
int x = 0;
/// User Led ///
int ULed = 13;
int i = 0;
/// Buzzer ///
int Buzzer = 2;
unsigned long breaktime;
int BackTimeVal, TurnTime45;

unsigned long sayac = 0, counter = 0, mod = 0;
int LastSensorValue = 0;

//490 hz standart, we change it to 3.9 Kz (x8)

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }
}


void GoBackward() {
  digitalWrite(RightMotorEnable, HIGH);
  digitalWrite(LeftMotorEnable, HIGH);
  analogWrite(RightMotorPwm, 2);
  analogWrite(LeftMotorPwm, 2);
}
void GoForward() {
  digitalWrite(RightMotorEnable, HIGH);
  digitalWrite(LeftMotorEnable, HIGH);
  analogWrite(RightMotorPwm, 252);
  analogWrite(LeftMotorPwm, 252);
}
void TurnRight() {
  digitalWrite(RightMotorEnable, HIGH);
  digitalWrite(LeftMotorEnable, HIGH);
  analogWrite(RightMotorPwm, 10);
  analogWrite(LeftMotorPwm, 245);
}
void TurnLeft() {
  digitalWrite(RightMotorEnable, HIGH);
  digitalWrite(LeftMotorEnable, HIGH);
  analogWrite(RightMotorPwm, 245);
  analogWrite(LeftMotorPwm, 10);
}
void Stop() {  // Free Wheeling
  digitalWrite(RightMotorEnable, LOW);
  digitalWrite(LeftMotorEnable, LOW);
  analogWrite(RightMotorPwm, 127);
  analogWrite(LeftMotorPwm, 127);
}

void Brake() {  // Hard Stop
  digitalWrite(RightMotorEnable, HIGH);
  digitalWrite(LeftMotorEnable, HIGH);
  analogWrite(RightMotorPwm, 128);
  analogWrite(LeftMotorPwm, 128);
}
//// This function below controls motor speed raw between -255 to +255
void MotorControl(int LeftSpeed, int RightSpeed) {
  //while (digitalRead(Start) == 0) {
     while (digitalRead(Start) == 0 || millis()-breaktime > 40000) { //after 4 second stop
    Stop();
    delay(100);
  }
  if (RightSpeed > 0) {
    RightSpeed = RightSpeed / 2 + 127;
    digitalWrite(RightMotorEnable, HIGH);
    analogWrite(RightMotorPwm, RightSpeed);
  } else if (RightSpeed < 0) {
    RightSpeed = abs(RightSpeed);
    RightSpeed = (127 - RightSpeed / 2);
    digitalWrite(RightMotorEnable, HIGH);
    analogWrite(RightMotorPwm, RightSpeed);
  } else {
    digitalWrite(RightMotorEnable, LOW);
    analogWrite(RightMotorPwm, 127);
  }
  if (LeftSpeed > 0) {
    LeftSpeed = LeftSpeed / 2 + 127;
    digitalWrite(LeftMotorEnable, HIGH);
    analogWrite(LeftMotorPwm, LeftSpeed);
  } else if (LeftSpeed < 0) {
    LeftSpeed = abs(LeftSpeed);
    LeftSpeed = (127 - LeftSpeed / 2);
    analogWrite(LeftMotorPwm, LeftSpeed);
    digitalWrite(LeftMotorEnable, HIGH);
  } else {
    digitalWrite(LeftMotorEnable, LOW);
    analogWrite(LeftMotorPwm, 127);
  }
}

void SensorDebug() {
  Serial.print("Opponent Sensors:");
  Serial.print(digitalRead(LSens));
  Serial.print(" - ");
  Serial.print(digitalRead(LFSens));
  Serial.print(" - ");
  Serial.print(digitalRead(MSens));
  Serial.print(" - ");
  Serial.print(digitalRead(RFSens));
  Serial.print(" - ");
  Serial.print(digitalRead(RSens));
  Serial.print("      ");
  Serial.print("Line Sensors:");
  Serial.print(analogRead(LeftLine));
  Serial.print(" - ");
  Serial.print(analogRead(RightLine));
  Serial.print(" - ");
  Serial.println(analogRead(BackLine));
  delay(250);
}

void LineControl() {
  if (analogRead(LeftLine) < LineWhiteValue && analogRead(RightLine) > LineBlackValue)  //Left Line Sensor Saw the Line
  {
    delay(8);
    if (analogRead(LeftLine) < LineWhiteValue)  //Left Line Sensor Saw the Line
    {
      GoBackward();
      delay(BackTimeVal);
      Brake();  // Brakes added for better manuevering control.
      delay(50);
      TurnRight();
      delay(TurnTime45 * 2);
      Brake();
      delay(50);
    }
  } else if (analogRead(LeftLine) > LineBlackValue && analogRead(RightLine) < LineWhiteValue)  //Right Line Sensor Saw the Line
  {
    delay(8);
    if (analogRead(RightLine) < LineWhiteValue)  //Right Line Sensor Saw the Line
    {
      GoBackward();
      delay(BackTimeVal);
      Brake();
      delay(50);
      TurnLeft();
      delay(TurnTime45 * 2);
      Brake();
      delay(50);
    }
  } else if (analogRead(LeftLine) < LineWhiteValue && analogRead(RightLine) < LineWhiteValue)  //Both Sensor Saw the Line
  {
    delay(8);
    if (analogRead(LeftLine) < LineWhiteValue && analogRead(RightLine) < LineWhiteValue)  //Both Sensor Saw the Line
    {
      GoBackward();
      delay(BackTimeVal);
      Brake();
      delay(50);
      TurnRight();
      delay(TurnTime45 * 2);
      Brake();
      delay(50);
    }
  }
}

void follow() {
  while (digitalRead(Start) == 1) {

    LineControl();
    if (digitalRead(MSens) == 1) {
      x++;
      LastSensorValue = 0;
      delay(1);
      if (x < 300) {
        GoForward();
        delay(1);
      } else if ((x == 300) && digitalRead(MSens) == 1) {
        Stop();
      }
    } else if (digitalRead(LSens) == 1 && digitalRead(MSens) == 0) {
      TurnLeft();
      delay(8);
      x = 0;
    }

    else if (digitalRead(RSens) == 1) {
      TurnRight();
      delay(8);
      x = 0;
    }

    else if (digitalRead(LFSens) == 1 && digitalRead(MSens) == 0) {
      TurnLeft();
      delay(16);
      x = 0;
    }


    else if (digitalRead(RFSens) == 1 && digitalRead(MSens) == 0) {
      TurnRight();
      delay(16);
      x = 0;
    } else {
      Stop();
    }
  }

  while (digitalRead(Start) == 0) {
    Stop();
    delay(1);
  }
}

///////////////////////////////////
void attack() {
  while (digitalRead(Start) == 0) {
    // while (digitalRead(Start) == 0 || millis()-breaktime > 32000) { //after 4 second stop
    Stop();
    delay(100);
  }
  if (analogRead(LeftLine) < LineWhiteValue || analogRead(RightLine) < LineWhiteValue) {
    LineControl();
  } else if (digitalRead(MSens) == HIGH) {
    delay(8);
    if (digitalRead(MSens) == HIGH) {
      GoForward();
      LastSensorValue = 3;
    }
  } else if (digitalRead(LFSens) == HIGH) {
    delay(8);
    if (digitalRead(LFSens) == HIGH) {
      TurnLeft();
      LastSensorValue = 2;
      delay(8);
    }
  } else if (digitalRead(RFSens) == HIGH) {
    delay(8);
    if (digitalRead(RFSens) == HIGH) {
      TurnRight();
      LastSensorValue = 4;
    }
  } else if (digitalRead(LSens) == HIGH) {
    delay(8);
    if (digitalRead(LSens) == HIGH) {
      TurnLeft();
      LastSensorValue = 1;
    }
  } else if (digitalRead(RSens) == HIGH) {
    delay(8);
    if (digitalRead(RSens) == HIGH) {
      TurnRight();
      LastSensorValue = 5;
    }
  } else {
    if (LastSensorValue == 3) {
      MotorControl(250, 250);
    } else if (LastSensorValue == 1) {
      MotorControl(-250, 250);
    } else if (LastSensorValue == 2) {
      MotorControl(-250, 250);
    } else if (LastSensorValue == 4) {
      MotorControl(250, -250);
    } else if (LastSensorValue == 5) {
      MotorControl(250, -250);
    }
  }
  //else{Stop();}
}

void setup() {
Rainbow.attach(A3);
  /// (Pin,Divisor) ///
  setPwmFrequency(RightMotorPwm, 8);
  setPwmFrequency(LeftMotorPwm, 8);

  /// OUTPUTS ///
  pinMode(LeftMotorPwm, OUTPUT);
  pinMode(RightMotorPwm, OUTPUT);
  pinMode(LeftMotorEnable, OUTPUT);
  pinMode(RightMotorEnable, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(ULed, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  /// INPUTS ///
  pinMode(LSens, INPUT);
  pinMode(LFSens, INPUT);
  pinMode(MSens, INPUT);
  pinMode(RFSens, INPUT);
  pinMode(RSens, INPUT);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(BackTime, INPUT);
  pinMode(TurnTime, INPUT);
  pinMode(LeftLine, INPUT);
  pinMode(RightLine, INPUT);
  pinMode(BackLine, INPUT);


  BackTimeVal = map(analogRead(BackTime), 0, 1023, 400, 2000);
  TurnTime45 = map(analogRead(TurnTime), 0, 1023, 100, 1200);

  /// Sensors Pullup ///
  digitalWrite(LSens, HIGH);
  digitalWrite(LFSens, HIGH);
  digitalWrite(MSens, HIGH);
  digitalWrite(RFSens, HIGH);
  digitalWrite(RSens, HIGH);

  pinMode(Start, INPUT);
  for (pos = 60; pos <= 140; pos += 1) {
    Rainbow.write(pos);
    delay(10);  // 1.25 ms Real.
  }
  for (pos = 140; pos >= 60; pos -= 1) {
    Rainbow.write(pos);
    delay(10);
  }
  //Serial.begin(9600); // This function Disables Dipswitch 0 and 1 Use careful.
}
void loop() {

  // Stop();
  //while(1){SensorDebug();}//For sensor debug

  while (digitalRead(Start) == 0) {
    Stop();
    delay(8);
    if (digitalRead(LSens) == HIGH) {
      tone(Buzzer, 300, 300);
      Rainbow.write(60);
      delay(20);
    } else if (digitalRead(LFSens) == HIGH) {
      tone(Buzzer, 600, 300);
      Rainbow.write(70);
      delay(20);
    } else if (digitalRead(MSens) == HIGH) {
      tone(Buzzer, 1000, 300);
      Rainbow.write(90);
      delay(20);
    } else if (digitalRead(RFSens) == HIGH) {
      tone(Buzzer, 90, 300);
      Rainbow.write(110);
      delay(20);
    } else if (digitalRead(RSens) == HIGH) {
      tone(Buzzer, 2300, 300);
      Rainbow.write(140);
      delay(20);
    } else {
      noTone(Buzzer);
    }
  }

  breaktime = millis();
  LastSensorValue = 3;
  noTone(Buzzer);
  //// Tactical Modes
  if (digitalRead(switch1) == 1 && digitalRead(switch2) == 0)  // Right ZigZag
  {
    MotorControl(250, -250);
    delay(TurnTime45);
    MotorControl(1, 1);
    delay(50);
    MotorControl(250, 250);
    delay(TurnTime45 * 3);
    MotorControl(1, 1);
    delay(50);
    MotorControl(-250, 250);
    delay(TurnTime45);
    MotorControl(1, 1);
    delay(50);
    MotorControl(250, 250);
    delay(TurnTime45 * 3);
    Stop();
  } else if (digitalRead(switch1) == 0 && digitalRead(switch2) == 1)  // Left ZigZag
  {
    MotorControl(-250, 250);
    delay(TurnTime45);
    MotorControl(1, 1);
    delay(50);
    MotorControl(250, 250);
    delay(TurnTime45 * 3);
    MotorControl(1, 1);
    delay(50);
    MotorControl(250, -250);
    delay(TurnTime45);
    MotorControl(1, 1);
    delay(50);
    MotorControl(250, 250);
    delay(TurnTime45 * 3);
    Stop();
  } else if (digitalRead(switch1) == 1 && digitalRead(switch2) == 1)  //Down Down Down
  {
    while (analogRead(BackLine) > LineWhiteValue) {
      MotorControl(-180, -180);
      delay(8);
    }
    GoForward();
    delay(80);  //10mS
    Stop();
    delay(4000);  // 4000 = ~0.5 second.

  } else if (digitalRead(switch1) == 0 && digitalRead(switch2) == 0)  // Direct Forward Attack
  {
   GoForward();
    delay(80);
    attack();
  }
  while (1) {
    attack();
  }
}
