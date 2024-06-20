#include <SoftwareSerial.h>
#include <Stepper.h>

// Master code
// Master monitors heartbeat of Slave and takes over control if Slave stops working

SoftwareSerial SUART(2, 3);  // RX, TX for Software Serial

unsigned long lastHeartbeatTime = 0;
const unsigned long heartbeatTimeout = 2000;  // Timeout period in milliseconds

void setup() {
  Serial.begin(9600);  // Start Serial communication for debugging
  SUART.begin(9600);   // Initialize software serial communication
}

void loop() {
  // Check for heartbeat signal

  if (SUART.available()) {
    String message = SUART.readStringUntil('\n');
    Serial.print("Received message: ");
    Serial.println(message);
    message.trim();
    if (message == "heartbeat") {
      lastHeartbeatTime = millis();  // Reset the heartbeat timer
      Serial.println("Master: Received heartbeat");
    }
  }

  if (millis() - lastHeartbeatTime > heartbeatTimeout) {
    // Master has stopped working, print a message
    Serial.println("Master: Slave not working, taking over.");
    // Add logic to take over control here
    lastHeartbeatTime = millis();  // Reset the timer to avoid continuous printing
  }
}
