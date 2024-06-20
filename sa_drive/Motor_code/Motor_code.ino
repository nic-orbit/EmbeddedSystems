// Include the Arduino Stepper.h library:
#include "Stepper.h"
#include <Arduino.h>

// Define number of steps per rotation:
const int stepsPerRevolution = 2048;

// Wiring:
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver

// Create stepper object called 'myStepper', note the pin order:
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

// Define the digital pins for LED monitoring
const int LED1Pin = 4;  // Digital pin for LED1
const int LED2Pin = 5;  // Digital pin for LED2
const int LED3Pin = 6;  // Digital pin for LED3
const int LED4Pin = 7;  // Digital pin for LED4

int angleIn;

void setup() {
  // Set the speed to 5 rpm:
  myStepper.setSpeed(1);
  
  // Begin Serial communication at a baud rate of 9600:
  Serial.begin(9600);

  // Set LED monitoring pins to inputs
  pinMode(LED1Pin, INPUT);
  pinMode(LED2Pin, INPUT);
  pinMode(LED3Pin, INPUT);
  pinMode(LED4Pin, INPUT);
}

void loop() {
  Serial.println("Enter the angle (0-360): ");
  while (!Serial.available()) {
    delay(10);
  }
  angleIn = Serial.parseInt();
  moveSM_to_angle(angleIn);
  delay(500);
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
    if (!takeDigitalReadings()) {
      Serial.println("Motor not working");
      return;
    }
    delay(10);  // Short delay to allow the motor to step smoothly
  }
  Serial.print("Motor moved to angle: ");
  Serial.println(angle);
  
  Serial.println(steps);
  delay(5000);
}

bool takeDigitalReadings() {
  int led1State = digitalRead(LED1Pin);
  int led2State = digitalRead(LED2Pin);
  int led3State = digitalRead(LED3Pin);
  int led4State = digitalRead(LED4Pin);

  // Print the digital states
  Serial.print(led1State);
  Serial.print(" ");
  Serial.print(led2State);
  Serial.print(" ");
  Serial.print(led3State);
  Serial.print(" ");
  Serial.println(led4State);

  // Check for valid combinations
  if ((led1State == HIGH && led2State == LOW && led3State == HIGH && led4State == LOW) ||//1010
      (led1State == LOW && led2State == HIGH && led3State == HIGH && led4State == LOW) ||//0110
      (led1State == LOW && led2State == HIGH && led3State == LOW && led4State == HIGH) ||//0101
      (led1State == HIGH && led2State == LOW && led3State == LOW && led4State == HIGH)) //1001
  {
    // Valid combination
    return true;
  } else {
    // Invalid combination
    return false;
  }
}
