// ========================================
// HC-05 SERIAL MONITOR TEST
// Arduino Nano ATmega328P
//
// HC-05 TXD -> Nano RX0 / D0
// HC-05 RXD -> Nano TX1 / D1
// ========================================

void setup() {

  // Start hardware serial communication
  Serial.begin(9600);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("   HC-05 Bluetooth Serial Test");
  Serial.println("================================");
  Serial.println("Baud Rate: 9600");
  Serial.println("HC-05 TXD -> Nano D0 / RX");
  Serial.println("HC-05 RXD -> Nano D1 / TX");
  Serial.println("--------------------------------");
  Serial.println("Waiting for Bluetooth data...");
  Serial.println();
}

void loop() {

  // Check if HC-05 received data
  if (Serial.available() > 0) {

    char data = Serial.read();

    Serial.print("HC-05 Received: ");
    Serial.println(data);
  }
}
