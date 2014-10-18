// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
AF_DCMotor motor(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  // turn on motor
  motor.setSpeed(127);
 
  motor.run(RELEASE);
}

void loop() {
  int i;
  
  motor.run(FORWARD);
  for (i=0; i<127; i++) {
  motor.setSpeed(i);
  delay(35);  
  }
  delay(400);
  
  for (i=127; i!=0; i--) {
  motor.setSpeed(i); 
  delay (35); 
  }
  delay(400);
  
  motor.run(BACKWARD);
  for (i=0; i<127; i++) {
  motor.setSpeed(i);
  delay(35);  
  }
  delay(400);
  
  for (i=127; i!=0; i--) {
  motor.setSpeed(i); 
  delay (35); 
  }
  delay(400);
}
