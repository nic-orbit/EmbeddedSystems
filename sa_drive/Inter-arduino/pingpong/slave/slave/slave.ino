#include<SoftwareSerial.h>

SoftwareSerial SUART(2, 3); //SRX = 0, STX = 1

void setup() {
  Serial.begin(9600);  // Start the serial communication
  SUART.begin(9600);   // Start serial communication between arduinos
}

void loop() {
  if (SUART.available() > 0) {
    String message = SUART.readStringUntil('\n');  // Read the incoming message
    Serial.println("Received message: ");
    Serial.println(message);

    message.trim();  // Remove any leading or trailing whitespace/newline characters

    if (message == "ping") {
      SUART.println("pong");  // Respond with "pong" if the message is "ping"
      Serial.println("answered with: \n pong");
    }
  }
  delay(100);  // Optional delay to help with timing issues
}
