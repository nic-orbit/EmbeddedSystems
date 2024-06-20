#include <Stepper.h>
#include <SoftwareSerial.h>

// Slave code
// Slave sends heartbeat signal to Master every second
// If Slave stops sending heartbeat, Master takes over control

SoftwareSerial SUART(2, 3);  // RX, TX for Software Serial

void setup() {
  Serial.begin(9600);  // Start Serial communication for debugging
  SUART.begin(9600);   // Initialize software serial communication
}

void loop() {
  // Send heartbeat signal to the slave
  SUART.println("heartbeat");
  Serial.println("Slave: Sent heartbeat");
  
  delay(1000);  // Send heartbeat every second
}
