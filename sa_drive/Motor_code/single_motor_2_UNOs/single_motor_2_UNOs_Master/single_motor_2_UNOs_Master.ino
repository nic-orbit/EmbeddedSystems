#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <Stepper.h>


bool DEBUG = true; // use this to switch on debug prints into the serial monitor, 
              // this will break the python interface communication

int stepsPerRevolution = 2048*3;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
int angleIn = 0;

// PIN allocations for Software Serial
int SUART_IN = 2;
int SUART_OUT = 3;

SoftwareSerial SUART(SUART_IN, SUART_OUT);  // RX, TX for Software Serial

volatile bool receivedPing = false;
volatile bool waitingForPing = false;
volatile unsigned long lastPingTime = 0;
volatile bool SlaveStatus = true;

int RequestPingTimer = 1; // seconds

void setup() {
  delay(1000);
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
    if (DEBUG){
      Serial.println("Master received ping from slave");
    }
    receivedPing = false;  // Reset flag
    waitingForPing = false;  // Reset waiting flag
  }

  // Check if we are waiting for a ping and if the timeout has occurred
  if (waitingForPing && (millis() - lastPingTime > 2000)) {
    if (DEBUG){
      Serial.println("Master: Slave not responding, declaring slave as not working");
    }
    SlaveStatus = false;
    receivedPing = false;
    waitingForPing = false;  // Reset waiting flag
  }

  // Check for response from slave
  if (true) {
    String response = SUART.readStringUntil('\n');
    response.trim();
    if (DEBUG){
      Serial.print("  [S]: "); 
      Serial.print(response);
      Serial.println();
    }
    if (response == "ping") {
      receivedPing = true;
      SlaveStatus = true;
    }
  }
  delay(100);
}

// This function is called whenever data is received on the serial port
void serialEvent() {
  while (Serial.available() > 0) {
    angleIn = Serial.parseInt();
    moveSM_to_angle(angleIn);
  }
}

void moveSM_to_angle(long angle) {

    if (angle >= 0) {
    myStepper.setSpeed(10);
  } else {
    myStepper.setSpeed(-10);
    angle = angle*(-1);
  }

  long steps = map(angle, 0, 360, 0, stepsPerRevolution);

  for (long i = 0; i < abs(steps); ++i) {
    myStepper.step(steps > 0 ? 1 : -1);
    // if (!takeDigitalReadings()) {
    //   Serial.println("Motor not working");
    //   return;
    // }
    delay(10);  // Short delay to allow the motor to step smoothly
  }
  Serial.print("Motor moved to angle: ");
  Serial.println(angle);
  
  Serial.println(steps);
  delay(1000);
}

// Timer interrupt function to request ping
void requestPing() {
  if (waitingForPing == false){
    SUART.println("requestPing");
    if (DEBUG){
      Serial.println("  [M]: requestPing");
    }
    lastPingTime = millis();  // Record the time of the request
  }
  waitingForPing = true;  // Set waiting flag
  printSlaveStatus(SlaveStatus);
}

void printSlaveStatus(bool status) {
  if (status && DEBUG){
    Serial.println("Slave IS OPERATIONAL");
  }
  else if (DEBUG) {
    Serial.println("Slave NOT RESPONDING!!!");
  }
}
