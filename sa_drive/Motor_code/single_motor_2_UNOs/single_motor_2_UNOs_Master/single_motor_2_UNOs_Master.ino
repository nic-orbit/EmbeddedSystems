#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <Stepper.h>


bool DEBUG = false; // use this to switch on debug prints into the serial monitor, 
              // this will break the python interface communication

int stepsPerRevolution = 2048*3/3.5;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
int angleIn = 0;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

bool newData = false;

int dataNumber = 0;  

// PIN allocations for Software Serial
int SUART_IN = 12;
int SUART_OUT = 13;
// PIN allocation for Slave reset
int SLAVE_RESET_PIN = 7;

SoftwareSerial SUART(SUART_IN, SUART_OUT);  // RX, TX for Software Serial

const int pinA = 2;
const int pinB = 3;

volatile int encoderPosition = 0;
volatile bool motor_not_working = false;

volatile bool receivedPing = false;
volatile bool waitingForPing = false;
volatile unsigned long lastPingTime = 0;
volatile bool SlaveStatus = true;

int RequestPingTimer = 1; // seconds
int SlaveTimeoutTime = 3; //seconds

void setup() {
  pinMode(SLAVE_RESET_PIN, INPUT);   // configure pin to reset Slave as input by default
  digitalWrite(SLAVE_RESET_PIN, LOW);  // if Slave should be reset, change pin mode to output

  delay(1000);
  Serial.begin(9600);  // Start Serial communication for debugging
  SUART.begin(9600);   // Initialize software serial communication
  pinMode(SUART_IN, INPUT);
  pinMode(SUART_OUT, OUTPUT);

  // Initialize Timer1 to trigger every second
  Timer1.initialize(RequestPingTimer*1000000);  // Set timer for 1 second (1,000,000 microseconds)
  Timer1.attachInterrupt(requestPing);  // Attach the requestPing function to the timer interrupt

  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);

  attachInterrupt(digitalPinToInterrupt(pinA), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), updateEncoder, CHANGE);

}

void loop() 
{
  // printSlaveStatus(SlaveOperational);
  recvWithEndMarker();
  if (newData) {
    float previousAngle = encoderPosition*4.5; // get angle from rotary encoder with * 360/80 = 4.5
    angleIn = atoi(receivedChars);
    // Serial.println(receivedChars);
    // Serial.println(angleIn);
    moveSM_to_angle(angleIn);
    delay(10);
    if (SlaveStatus){
      // wait for slave to execute
      delay(21000);
    }
    float finalAngle = encoderPosition*4.5; 
    float measuredAngle = previousAngle - finalAngle;

    bool angle_correct = abs(angleIn - measuredAngle) < 36;

    if(DEBUG){
    Serial.print("Commanded Angle: ");
    Serial.println(angleIn);
    Serial.print("Measured Angle: ");
    Serial.println(measuredAngle);
    }

    if (!angle_correct) {
      Serial.println("Motor fail");
      motor_not_working = true;
    }
    newData = false;
  }
    
  if (receivedPing) {
    if (DEBUG){
      Serial.println("Master received ping from slave");
    }
    receivedPing = false;  // Reset flag
    waitingForPing = false;  // Reset waiting flag
  }

  // Check if we are waiting for a ping and if the timeout has occurred
  if ((millis() - lastPingTime > SlaveTimeoutTime*1000)) {
    if (DEBUG && SlaveStatus){
      Serial.println("Master: Slave not responding, declaring slave as not working");
    }

    SlaveStatus = false;
    receivedPing = false;
    waitingForPing = false;  // Reset waiting flag
  }

  // Check for response from slave
  if (SUART.available() > 0) {
    int response = SUART.read();
    // response.trim();
    if (DEBUG && response != 0){
      Serial.print("  [S]: "); 
      Serial.print(response);
      Serial.println();
    }
    if (response == 41) {
      receivedPing = true;
      SlaveStatus = true;
      waitingForPing = false; // we received ping and are not waiting anymore
    }
  }
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

// This function is called whenever data is received on the serial port
// void serialEvent() {
  
//   delay(10);
//   if (Serial.available() > 0) {
//     Serial.println("while loop");
//     angleIn = Serial.parseInt();
//     moveSM_to_angle(angleIn);
//     // break;
//   }
//   delay(10);
//   float finalAngle = encoderPosition*4.5; 
//   float measuredAngle = previousAngle - finalAngle;

//   bool angle_correct = abs(angleIn - measuredAngle) < 5;

//   Serial.print("Commanded Angle: ");
//   Serial.println(angleIn);
//   Serial.print("Measured Angle: ");
//   Serial.println(measuredAngle);

//   if (!angle_correct && DEBUG) {
//     Serial.println("Mootooor fucky wucky :(");
//     motor_not_working = true;
//   }
//   else if (DEBUG) {
//     Serial.println("All goooood!");
//   }
// }

void moveSM_to_angle(long angle) {
  if (SlaveStatus) {
    // do nothing
    if (DEBUG){
      Serial.println("Slave does the work!!!");
    }
  }
  else {
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
    delay(10);
  }

  }
  
  
  
}


// Timer interrupt function to request ping
void requestPing() {
  if (waitingForPing == false){
    SUART.write(40); // 0b0101 = "requestPing"
    String request_out = "40";
    if (DEBUG){
      Serial.print("  [M]: ");
      Serial.println(request_out);
    }
    lastPingTime = millis();  // Record the time of the request
  }
  waitingForPing = true;  // Set waiting flag
  printSlaveStatus(SlaveStatus);
}

void printSlaveStatus(bool status) {
  if (status){
    Serial.println("Slave IS OPERATIONAL");
  }
  else {
    Serial.println("Slave NOT RESPONDING!!!");
  }
}

 void serialFlush(){
  while(SUART.available() > 0) {
    char t = SUART.read();
  }
} 

void updateEncoder() {
  static int lastEncoded = 0;
  int MSB = digitalRead(pinA);
  int LSB = digitalRead(pinB);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderPosition++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderPosition--;
  
  lastEncoded = encoded;
}