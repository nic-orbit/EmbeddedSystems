#include <Stepper.h>

const int stepsPerRevolution = 2048;
bool M1_work = true;
bool M2_work = true;

Stepper myStepper1(stepsPerRevolution, 8, 10, 9, 11);
Stepper myStepper2(stepsPerRevolution, 1, 2, 3, 4);

// Function declarations
void moveSM_to_angle(int angle, Stepper SM);
void SM_work();

void setup() {
  myStepper1.setSpeed(5);
  myStepper2.setSpeed(5);
  Serial.begin(9600);
}

void loop() {
  int a0 = analogRead(A0);
  Serial.println(a0);
  int a5 = analogRead(A5);
  Serial.println(a5);
  delay(100);
  if (Serial.available() > 0) {
    int angle = Serial.parseInt();
    moveSM_to_angle(angle, myStepper1);
    SM_work();
    delay(300);
    if(!M1_work){
      Serial.println("M1 fail");
      moveSM_to_angle(angle, myStepper2);
      SM_work();
      delay(300);
    }
  }
}

void moveSM_to_angle(int angle, Stepper SM){
  int steps = (angle/360.0) * stepsPerRevolution ;
  if (angle >= 0){ 
    SM.setSpeed(5);
  } else {
    SM.setSpeed(-5);
  }
  SM.step(steps);
  Serial.println("Success");
}

void SM_work(){
  int V_p1 = analogRead(A0) ;
  int V_p2 = analogRead(A1) ;
  float V = (V_p1 + V_p2)/2.0 ;
  if (V < 700){
    Serial.println("work") ;
  } else{
    M1_work = false ;
  }
}
