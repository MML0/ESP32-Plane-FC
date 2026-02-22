// Actuators.h

#pragma once
#include <Servo.h>

class Actuators {
public:
    void begin();

    void setElevators(float pitch, float roll);
    void setMotors(float throttle, float yaw);

private:
    Servo leftServo;
    Servo rightServo;

    int leftMotorPWM = D4;
    int rightMotorPWM = D3;

    float constrainFloat(float v, float minv, float maxv);
};
