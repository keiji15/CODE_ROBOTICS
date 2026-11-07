/*
  LINE SENSOR TEST
  Arduino Nano
  Line Sensor Analog Output -> A0
  Serial Monitor -> 9600 baud

  Normal floor: usually low readings
  White line: usually high readings
*/

#define LINE_SENSOR A0

const int LINE_THRESHOLD = 450;

void setup() {
  Serial.begin(9600);
  pinMode(LINE_SENSOR, INPUT);

  Serial.println("LINE SENSOR TEST STARTED");
}

void loop() {
  long total = 0;

  // Read the sensor five times
  for (int i = 0; i < 5; i++) {
    total += analogRead(LINE_SENSOR);
    delay(3);
  }

  int averageValue = total / 5;

  Serial.print("LINE AVERAGE: ");
  Serial.print(averageValue);

  if (averageValue > LINE_THRESHOLD) {
    Serial.println(" - WHITE LINE DETECTED");
  } else {
    Serial.println(" - NORMAL FLOOR");
  }

  delay(250);
}
