#include "PID.h"
#include <Arduino.h>

PID::PID(float kp, float ki, float kd, float outMin, float outMax) {
  _kp = kp;
  _ki = ki;
  _kd = kd;

  _outMin = outMin;
  _outMax = outMax;

  _integral = 0;
  _lastError = 0;
  _lastTime = millis();
}

void PID::setTunings(float kp, float ki, float kd) {
  _kp = kp;
  _ki = ki;
  _kd = kd;
}

void PID::setOutputLimits(float min, float max) {
  _outMin = min;
  _outMax = max;
}

void PID::setSetpoint(float sp) {
  _setpoint = sp;
}

float PID::compute(float input) {
  unsigned long now = millis();
  float dt = (now - _lastTime) / 1000.0;
  if (dt <= 0) return 0;

  float error = _setpoint - input;

  _integral += error * dt;

  float derivative = (error - _lastError) / dt;

  float output = (_kp * error) +
                 (_ki * _integral) +
                 (_kd * derivative);

  // Anti-windup clamp
  if (output > _outMax) output = _outMax;
  if (output < _outMin) output = _outMin;

  _lastError = error;
  _lastTime = now;

  return output;
}
