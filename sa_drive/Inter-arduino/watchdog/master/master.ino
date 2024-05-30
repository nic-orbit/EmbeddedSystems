#include <SoftwareSerial.h>

// Serial Communication between arduinos at pin 2, 3
SoftwareSerial SUART(2, 3); // SRX = 2, STX = 3

unsigned long lastPingTime = 0;  // Time of the last sent ping
const unsigned long pingInterval = 2000;  // Interval between pings (2 seconds)
const unsigned long watchdogTimeout = 3000;  // Watchdog timeout (3 seconds)

void setup() {
  Serial.begin(9600); // Start the serial communication
  SUART.begin(9600);  // Start serial communication between the arduinos
  lastPingTime = millis();
}

void loop() {
  if (millis() - lastPingTime >= pingInterval) {
    SUART.println("ping");  // Send "ping" to the slave
    Serial.println("Sent ping to slave");
    lastPingTime = millis();  // Update the last ping time
  }

  unsigned long startTime = millis();  // Record the start time
  bool responseReceived = false;

  // Wait for a response for up to 1 second
  while (millis() - startTime < 1000) {
    if (SUART.available() > 0) {
      String response = SUART.readStringUntil('\n');  // Read the response from the slave
      response.trim();  // Remove any leading or trailing whitespace

      if (response == "pong") {
        Serial.println("Slave is operational!");
      } else {
        Serial.print("Unexpected response: ");
        Serial.println(response);
      }
      responseReceived = true;
      break;
    }
  }

  if (!responseReceived) {
    Serial.println("No response from slave");
  }

  delay(100);  // Short delay to allow other operations
}
