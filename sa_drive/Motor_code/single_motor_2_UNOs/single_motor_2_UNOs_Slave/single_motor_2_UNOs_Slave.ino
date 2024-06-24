#include <Stepper.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

bool DEBUG = true; // use this to switch on debug prints into the serial monitor, 
              // this will break the python interface communication

// Slave code
// Slave sends heartbeat signal to Master every second
// If Slave stops sending heartbeat, Master takes over control

int stepsPerRevolution = 2048*3;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


// PIN allocations
int SUART_IN = 2;
int SUART_OUT = 3;

// Rotary encoder pins
#define encoder0PinA 12  // CLK Output A (attachInterrupt)
#define encoder0PinB 13  // DT Output B

volatile long encoder0Pos = 0; // Position counter
const int degreesPerStep = 18; // Encoder resolution: 18 degrees per step

int angleIn = 0;

SoftwareSerial SUART(SUART_IN, SUART_OUT);  // RX, TX for Software Serial



void setup() {
  delay(500);
  Serial.begin(9600);  // Start Serial communication for debugging
  SUART.begin(9600);   // Initialize software serial communication
  pinMode(SUART_IN, INPUT);
  pinMode(SUART_OUT, OUTPUT);

  // Initialize Timer1 to trigger every 100ms for checking SUART
  Timer1.initialize(100000);  // Set timer for 100ms (100,000 microseconds)
  Timer1.attachInterrupt(checkSUART);  // Attach the checkSUART function to the timer interrupt

  // Set up the rotary encoder pins
  pinMode(encoder0PinA, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(encoder0PinB, INPUT_PULLUP); // Use internal pull-up resistor

  // Attach interrupt to the CLK pin (pin 2) with CHANGE mode
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);

  Serial.println("start");


}

void loop() {
  // nothing to do here. 
  Serial.println(Serial.available());
  delay(100);
}

// This function is called whenever data is received on the serial port
void serialEvent() {
  while (Serial.available() > 0) {
    angleIn = Serial.parseInt();
    moveSM_to_angle(angleIn);
  }
}

// Timer interrupt function to check SUART
void checkSUART() {
  if (SUART.available() > 0) {
    String request = SUART.readStringUntil('\n');
    request.trim();
    Serial.print("  [M]: ");
    Serial.print(request);
    Serial.println();
    if (request == "requestPing") {
      sendPing();
    }
  }
}

// Function to send ping response
void sendPing() {
  SUART.println("ping");
  Serial.println("  [S]: ping");
}

void moveSM_to_angle(long angle) {
  long steps = map(angle, 0, 360, 0, stepsPerRevolution);
  long initialEncoderPos = encoder0Pos;

  if (angle >= 0) {
    myStepper.setSpeed(10);
  } else {
    myStepper.setSpeed(-10);
    steps = -steps;
  }

  for (long i = 0; i < abs(steps); ++i) {
    myStepper.step(steps > 0 ? 1 : -1);
    // if (!takeDigitalReadings()) {
    //   Serial.println("Motor not working");
    //   return;
    // }
    delay(10);  // Short delay to allow the motor to step smoothly

  // Check if encoder position matches the expected steps
    long expectedPos = initialEncoderPos + (i + 1) * (targetSteps > 0 ? 1 : -1);
      if (abs(encoder0Pos - expectedPos) > 5) { // Tolerance to be calculated
        Serial.println("Encoder mismatch detected! Motor malfunctioning.");
        return;
}
  }
  Serial.print("Motor moved to angle: ");
  Serial.println(angle);
  Serial.print("Target steps: ");
  Serial.println(steps);
  Serial.print("Actual steps: ");
  Serial.println(encoder0Pos - initialEncoderPos);
  delay(1000);
}

void doEncoder() {
  Serial.println("entered doEncoder function");
  // Read the state of the encoder pins
  int stateA = digitalRead(encoder0PinA);
  int stateB = digitalRead(encoder0PinB);

  // Determine the direction of rotation
  if (stateA == stateB) {
    encoder0Pos++;
  } 
  else {
    encoder0Pos--;
  }
  Serial.println("Encoder Position: ");
  Serial.println(encoder0Pos);
}
