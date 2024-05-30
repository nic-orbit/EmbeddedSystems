#include<SoftwareSerial.h>
//Serial Communication between arduinos at pin 0, 1
SoftwareSerial SUART(2, 3); //SRX = 0, STX = 1

void setup() {
  Serial.begin(9600); // Start the serial communication
  SUART.begin(9600);  //Start serial communication between the arduinos
}

void loop() {
  SUART.println("ping");  // Send "ping" to the slave
  delay(100);  // Short delay to allow transmission

  unsigned long startTime = millis();  // Record the start time
  bool responseReceived = false;

  // Wait for a response for up to 1 second
  while (millis() - startTime < 1000) {
    if (SUART.available() > 0) {
      String response = SUART.readStringUntil('\n');  // Read the response from the slave
      response.trim();  // Remove any leading or trailing whitespace

      if (response == "pong") {
        Serial.println("Slave is operational!!!!");
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

  delay(2000);  // Wait for 2 seconds before sending the next ping
}
