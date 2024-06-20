#include <Stepper.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

// Slave code
// Slave sends heartbeat signal to Master every second
// If Slave stops sending heartbeat, Master takes over control

int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


// PIN allocations
int SUART_IN = 2;
int SUART_OUT = 3;

int angleIn = 0;

SoftwareSerial SUART(SUART_IN, SUART_OUT);  // RX, TX for Software Serial



void setup() {
  Serial.begin(9600);  // Start Serial communication for debugging
  SUART.begin(9600);   // Initialize software serial communication
  pinMode(SUART_IN, INPUT);
  pinMode(SUART_OUT, OUTPUT);

  // Initialize Timer1 to trigger every 100ms for checking SUART
  Timer1.initialize(100000);  // Set timer for 100ms (100,000 microseconds)
  Timer1.attachInterrupt(checkSUART);  // Attach the checkSUART function to the timer interrupt
}

void loop() {
  // nothing to do here...
}

// This function is called whenever data is received on the serial port
void serialEvent() {
  while (Serial.available()) {
    angleIn = Serial.parseInt();
    moveSM_to_angle(angleIn);
  }
}

// Timer interrupt function to check SUART
void checkSUART() {
  if (SUART.available()) {
    String request = SUART.readStringUntil('\n');
    request.trim();
    if (request == "requestPing") {
      sendPing();
    }
  }
}

// Function to send ping response
void sendPing() {
  SUART.println("ping");
  Serial.println("Slave: Sent ping response to master");
}

void moveSM_to_angle(long angle) {
  long steps = map(angle, 0, 360, 0, stepsPerRevolution);

  if (angle >= 0) {
    myStepper.setSpeed(10);
  } else {
    myStepper.setSpeed(-10);
  }

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
