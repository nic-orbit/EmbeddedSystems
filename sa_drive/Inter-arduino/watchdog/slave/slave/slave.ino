#include <SoftwareSerial.h>

// Serial Communication between arduinos at pin 2, 3
SoftwareSerial SUART(2, 3); // SRX = 2, STX = 3

unsigned long lastPingTime = 0;  // Time of the last received ping
const unsigned long watchdogTimeout = 3000;  // Watchdog timeout (3 seconds)

void setup() {
  Serial.begin(9600);  // Start the serial communication
  SUART.begin(9600);   // Start serial communication between arduinos
  lastPingTime = millis();
}

void loop() {
  if (SUART.available() > 0) {
    String message = SUART.readStringUntil('\n');  // Read the incoming message
    Serial.print("Received message: ");
    Serial.println(message);

    message.trim();  // Remove any leading or trailing whitespace/newline characters

    if (message == "ping") {
      SUART.println("pong");  // Respond with "pong" if the message is "ping"
      Serial.println("Answered with: pong");
      lastPingTime = millis();  // Update the last ping time
    }
  }

  if (millis() - lastPingTime >= watchdogTimeout) {
    // Master has stopped working, take appropriate action
    Serial.println("No ping from master, taking over.");
    // Add logic to take over control here
    lastPingTime = millis();  // Reset the timer to avoid continuous printing
  }

  delay(100);  // Optional delay to help with timing issues
}
