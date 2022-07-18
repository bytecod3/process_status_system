#include <ESP8266WiFi.h>
#include <Servo.h>

#include "credentials.h"
#define SERVO_DELAY 50

// servo pin
Servo servo_motor;
int servo_motor_pin = 16;
int angle;
String servo_angle = "0";

int servo_angle_to_set;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

   // attach servo motor pin
  servo_motor.attach(servo_motor_pin);
  servo_motor.write(56);

  
}

void loop() {
  // put your main code here, to run repeatedly:


  

}
