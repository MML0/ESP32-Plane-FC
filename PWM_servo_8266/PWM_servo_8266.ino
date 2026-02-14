#include <Servo.h>
Servo myServo;

void setup() {
  myServo.attach(D4); // attach servo to GPIO2/D4
}

void loop() {
  myServo.write(0);
  delay(1000);
  myServo.write(180);
  delay(1000);
}
