// ========================================
// HC-06 SERIAL MONITOR TEST
// HC-06 TXD -> Nano RX0 / D0
// HC-06 RXD -> Nano TX1 / D1
// ========================================

void setup() {
  Serial.begin(9600);

  Serial.println("================================");
  Serial.println("HC-06 Bluetooth Serial Test");
  Serial.println("Baud Rate: 9600");
  Serial.println("Waiting for Bluetooth data...");
  Serial.println("================================");
}

void loop() {

  // Bluetooth -> Serial Monitor
  if (Serial.available()) {

    char data = Serial.read();

    Serial.print("HC-06 Received: ");
    Serial.println(data);
  }
}
