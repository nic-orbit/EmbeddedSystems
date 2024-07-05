#include <Stepper.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

bool DEBUG = true; // use this to switch on debug prints into the serial monitor, 
              // this will break the python interface communication

// Slave code
// Slave sends heartbeat signal to Master every second
// If Slave stops sending heartbeat, Master takes over control

int stepsPerRevolution = 2048*3/3.5;
int stepsPerRevolution_rotary = 20;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


// PIN allocations
int SUART_IN = 12;
int SUART_OUT = 13;

// Rotary encoder pins
#define encoder0PinA 2  // CLK Output A (attachInterrupt)
#define encoder0PinB 3  // DT Output B

volatile long encoder0Pos = 0; // Position counter
const int degreesPerStep = 18; // Encoder resolution: 18 degrees per step

int angleIn = 0;

int encoderPosCount = 0;
int pinALast;
int aVal;
bool bCW;

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
  pinMode(encoder0PinA, INPUT); // Use internal pull-up resistor
  pinMode(encoder0PinB, INPUT); // Use internal pull-up resistor

  // Attach interrupt to the CLK pin (pin 2) with CHANGE mode
  // attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);
  
  Serial.println("start");


}

void loop() {
  // // nothing to do here. 
  //Serial.println(Serial.available());
  
  aVal = digitalRead(encoder0PinA);
  if (aVal != pinALast){ // Means the knob is rotating
  // if the knob is rotating, we need to determine direction
  // We do that by reading pin B.
    if (digitalRead(encoder0PinB) != aVal) { // Means pin A Changed first - We're Rotating Clockwise
      encoderPosCount --;
      bCW = true;
    } else {// Otherwise B changed first and we're moving CCW
      bCW = false;
      encoderPosCount++;
    }
    Serial.print ("Rotated: ");
    if (bCW){
      Serial.println ("counterclockwise");
    }else{
      Serial.println("clockwise");
    }
    Serial.print("Encoder Position: ");
    Serial.println(encoderPosCount);
  }
  pinALast = aVal;
  
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

  // attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);

  if (angle >= 0) {
    myStepper.setSpeed(100);
  } else {
    myStepper.setSpeed(-100);
  }

  for (long i = 0; i < abs(steps); ++i) {
    myStepper.step(steps > 0 ? 1 : -1);
    // if (!takeDigitalReadings()) {
    //   Serial.println("Motor not working");
    //   return;
    // }
    delay(10);  // Short delay to allow the motor to step smoothly

  // Check if encoder position matches the expected steps
    long expectedPos = (i + 1);
    Serial.print("Encoder0pos: ");
    Serial.println(encoder0Pos);
      if (abs(encoder0Pos - (expectedPos/102)) > 5) { // Tolerance to be calculated
        Serial.println(expectedPos);
        Serial.println("Encoder mismatch detected! Motor malfunctioning.");
        return;
}
  }
  // detachInterrupt(digitalPinToInterrupt(encoder0PinA));
  
  Serial.print("Motor moved to angle: ");
  Serial.println(angle);
  Serial.print("Target steps: ");
  Serial.println(steps);
  Serial.print("Actual steps: ");
  Serial.println((encoder0Pos - initialEncoderPos)*stepsPerRevolution_rotary/stepsPerRevolution);
  delay(1000);
}

void doEncoder() {
  // Serial.println("entered doEncoder function");
  // // Read the state of the encoder pins
  // int stateA = digitalRead(encoder0PinA);
  // int stateB = digitalRead(encoder0PinB);

  // // Determine the direction of rotation
  // if (stateA == stateB) {
  //   encoder0Pos++;
  // } 
  // else {
  //   encoder0Pos--;
  // }
  // Serial.println("Encoder Position: ");
  // Serial.println(encoder0Pos);
  
}
