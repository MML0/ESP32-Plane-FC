// Actuators.cpp

#include "Actuators.h"

void Actuators::begin() {
    leftServo.attach(D1);
    rightServo.attach(D2);

    pinMode(leftMotorPWM, OUTPUT);
    pinMode(rightMotorPWM, OUTPUT);

    digitalWrite(leftMotorPWM, LOW);
    digitalWrite(rightMotorPWM, LOW);

    analogWrite(leftMotorPWM, 0);
    analogWrite(rightMotorPWM, 0);

    analogWriteFreq(10000);   // 10kHz for motors
}

float Actuators::constrainFloat(float v, float minv, float maxv) {
    if (v < minv) return minv;
    if (v > maxv) return maxv;
    return v;
}

void Actuators::setElevators(float pitch, float roll) {

    float left  = pitch + roll;
    float right = pitch - roll;
    int max_angle = 60;
    left  = constrainFloat(left,  -max_angle, max_angle);
    right = constrainFloat(right, -max_angle, max_angle);

    // left servo is mirrored
    int leftAngle  = map(left,  max_angle, -max_angle,  0, 180);
    int rightAngle = map(right, -max_angle, max_angle,  0, 180);

    leftServo.write(leftAngle);
    rightServo.write(rightAngle);
}

void Actuators::setMotors(float throttle, float yaw) {

    float left  = throttle + yaw;
    float right = throttle - yaw;

    left  = constrainFloat(left,  0, 255);
    right = constrainFloat(right, 0, 255);

    analogWrite(leftMotorPWM,  left);
    analogWrite(rightMotorPWM, right);
}
