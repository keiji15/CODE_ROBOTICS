// ========================================
// HC-06 BLUETOOTH SERIAL MONITOR TEST
// Arduino Nano ATmega328P
//
// HC-06 TXD -> Nano RX0 / D0
// HC-06 RXD -> Nano TX1 / D1
// ========================================

void setup() {
  Serial.begin(9600);

  delay(1000);

  Serial.println("================================");
  Serial.println("     HC-06 BLUETOOTH TEST");
  Serial.println("================================");
  Serial.println("Waiting for Bluetooth data...");
  Serial.println();
}

void loop() {

  // Receive data from HC-06
  if (Serial.available()) {

    char command = Serial.read();

    // Display received character
    Serial.print("Received: ");
    Serial.print(command);

    // Display command meaning
    Serial.print("  ->  ");

    switch (command) {

      case 'F':
      case 'f':
        Serial.println("FORWARD");
        break;

      case 'B':
      case 'b':
        Serial.println("BACKWARD");
        break;

      case 'L':
      case 'l':
        Serial.println("LEFT");
        break;

      case 'R':
      case 'r':
        Serial.println("RIGHT");
        break;

      case 'S':
      case 's':
        Serial.println("STOP");
        break;

      default:
        Serial.println("UNKNOWN COMMAND");
        break;
    }
  }
}
