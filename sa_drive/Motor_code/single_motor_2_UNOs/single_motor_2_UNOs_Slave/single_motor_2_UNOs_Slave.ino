#include <Stepper.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

bool DEBUG = false; // use this to switch on debug prints into the serial monitor, 
              // this will break the python interface communication

// Slave code
// Slave sends heartbeat signal to Master every second
// If Slave stops sending heartbeat, Master takes over control

int stepsPerRevolution = 2048*3/3.5;
// int stepsPerRevolution_rotary = 20;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);


// PIN allocations
int SUART_IN = 12;
int SUART_OUT = 13;

// Rotary encoder pins
// #define encoder0PinA 2  // CLK Output A (attachInterrupt)
// #define encoder0PinB 3  // DT Output B

// volatile long encoder0Pos = 0; // Position counter
// const int degreesPerStep = 18; // Encoder resolution: 18 degrees per step

int angleIn = 0;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

bool newData = false;

int dataNumber = 0;

// int encoderPosCount = 0;
// int pinALast;
// int aVal;
// bool bCW;

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

  // // Set up the rotary encoder pins
  // pinMode(encoder0PinA, INPUT); // Use internal pull-up resistor
  // pinMode(encoder0PinB, INPUT); // Use internal pull-up resistor

  // Attach interrupt to the CLK pin (pin 2) with CHANGE mode
  // attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);
  
  Serial.println("start");


}

void loop() 
{
  recvWithEndMarker();
  if (newData) {
    
    angleIn = atoi(receivedChars);
    // Serial.println(receivedChars);
    // Serial.println(angleIn);
    moveSM_to_angle(angleIn);
    delay(10);
    }
    newData = false;
    delay(100);
  }
  
  


void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    if (Serial.available() > 0) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

// Timer interrupt function to check SUART
void checkSUART() {
  // Serial.println(SUART.available());
  if (SUART.available() > 0) {
    int request = SUART.read();
    // request.trim();
    if (DEBUG){
      Serial.print("  [M]: ");
      Serial.print(request);
      Serial.println();
    }
    if (request == 40) {
      sendPing();
    }
  }
}

// Function to send ping response
void sendPing() {
  SUART.write(41);
  if (DEBUG){
    Serial.print("  [S]: ");
    Serial.println(41);
  }
}

void moveSM_to_angle(long angle) 
{
  long steps = map(angle, 0, 360, 0, stepsPerRevolution);
  // long initialEncoderPos = encoder0Pos;

  // attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);

  if (angle >= 0) {
    myStepper.setSpeed(100);
  } else {
    myStepper.setSpeed(-100);
  }

  for (long i = 0; i < abs(steps); ++i) 
  {
    myStepper.step(steps > 0 ? 1 : -1);
    // if (!takeDigitalReadings()) {
    //   Serial.println("Motor not working");
    //   return;
    // }
    delay(10);  // Short delay to allow the motor to step smoothly

  // // Check if encoder position matches the expected steps
  //   long expectedPos = (i + 1);
  //   Serial.print("Encoder0pos: ");
  //   Serial.println(encoder0Pos);
  //     if (abs(encoder0Pos - (expectedPos/102)) > 5) { // Tolerance to be calculated
  //       Serial.println(expectedPos);
  //       Serial.println("Encoder mismatch detected! Motor malfunctioning.");
  //       return;
}
  // }
  // detachInterrupt(digitalPinToInterrupt(encoder0PinA));
  
  // Serial.print("Motor moved to angle: ");
  // Serial.println(angle);
  // Serial.print("Target steps: ");
  // Serial.println(steps);
  // Serial.print("Actual steps: ");
  // Serial.println((encoder0Pos - initialEncoderPos)*stepsPerRevolution_rotary/stepsPerRevolution);
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
