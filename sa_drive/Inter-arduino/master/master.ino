void setup() {
  Serial.begin(9600);  // Start the serial communication
}

void loop() {
  Serial.println("ping");  // Send "ping" to the slave
  delay(100);  // Short delay to allow transmission

  unsigned long startTime = millis();  // Record the start time
  bool responseReceived = false;

  // Wait for a response for up to 1 second
  while (millis() - startTime < 1000) {
    if (Serial.available() > 0) {
      String response = Serial.readStringUntil('\n');  // Read the response from the slave
      if (response == "pong") {
        Serial.println("Slave is operational");
      } else {
        Serial.println("Unexpected response: " + response);
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