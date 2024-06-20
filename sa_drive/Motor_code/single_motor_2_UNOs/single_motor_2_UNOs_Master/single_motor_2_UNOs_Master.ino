#include <SoftwareSerial.h>
#include <TimerOne.h>

// PIN allocations for Software Serial
int SUART_IN = 2;
int SUART_OUT = 3;

SoftwareSerial SUART(SUART_IN, SUART_OUT);  // RX, TX for Software Serial

volatile bool receivedPing = false;
volatile bool waitingForPing = false;
volatile unsigned long lastPingTime = 0;
volatile bool SlaveOperational = true;

int RequestPingTimer = 1; // seconds

void setup() {
  Serial.begin(9600);  // Start Serial communication for debugging
  SUART.begin(9600);   // Initialize software serial communication
  pinMode(SUART_IN, INPUT);
  pinMode(SUART_OUT, OUTPUT);

  // Initialize Timer1 to trigger every second
  Timer1.initialize(RequestPingTimer*1000000);  // Set timer for 1 second (1,000,000 microseconds)
  Timer1.attachInterrupt(requestPing);  // Attach the requestPing function to the timer interrupt
}

void loop() {
  // printSlaveStatus(SlaveOperational);

  if (receivedPing) {
    Serial.println("Master: Received ping from slave");
    receivedPing = false;  // Reset flag
    waitingForPing = false;  // Reset waiting flag
  }

  // Check if we are waiting for a ping and if the timeout has occurred
  if (waitingForPing && (millis() - lastPingTime > 2000)) {
    Serial.println("Master: Slave not responding, declaring slave as not working");
    SlaveOperational = false;
    receivedPing = false;
    waitingForPing = false;  // Reset waiting flag
  }

  // Check for response from slave
  if (SUART.available()) {
    String response = SUART.readStringUntil('\n');
    response.trim();
    if (response == "ping") {
      receivedPing = true;
    }
  }
  delay(100);
}

// Timer interrupt function to request ping
void requestPing() {
  SUART.println("requestPing");
  Serial.println("Master: Sent ping request to slave");
  if (waitingForPing == false){
    lastPingTime = millis();  // Record the time of the request
  }
  waitingForPing = true;  // Set waiting flag
}

void printSlaveStatus(bool status) {
  if (status){
    Serial.println("Slave is operational");
  }
  else {
    Serial.println("Slave NOT WORKINGGG!!!");
  }
}
