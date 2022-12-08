#include <ZumoShield.h>

#include<Wire.h>
#include<ZumoShield.h>
#include <arduino-timer.h>
#define LED 13


#define turnSpeed 275
#define forwardSpeed 200
#define forwardDuration 4000 //change duration
#define turnLeftDuration 307
#define turnRightDuration 300


int trigPin = 10;
int echoPin = 9;
long duration, cm;

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

void goForward(){
  motors.setSpeeds(forwardSpeed, forwardSpeed);
  delay(forwardDuration);
  stop();
}

// go back w negative speed
void goBackward(){
  motors.setSpeeds(-forwardSpeed, -forwardSpeed);
  delay(duration);
  stop();
}



void stop(){
  motors.setSpeeds(0,0);
  delay(300);
}
void turnLeft(){
  motors.setSpeeds(-turnSpeed, turnSpeed);
  delay(turnLeftDuration);
  stop();
}

void turnRight(){
  motors.setSpeeds(turnSpeed, -turnSpeed);
  delay(turnRightDuration);
  stop();
}

void setup() {
  // put your setup code here, to run once:

  pinMode(LED, HIGH);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:


  goForward();
  //do everything


}

void getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = (duration/2) * 0.0343;

  Serial.print("in,");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  delay(230);

}