#include <SoftwareSerial.h>

SoftwareSerial SUART(2, 3);  // RX, TX for Software Serial

void setup() {
  Serial.begin(9600);  // Start Serial communication for debugging
  SUART.begin(9600);   // Initialize software serial communication
}

void loop() {
  // Send heartbeat signal to the slave
  SUART.println("heartbeat");
  Serial.println("Master: Sent heartbeat");
  
  delay(1000);  // Send heartbeat every second
}
