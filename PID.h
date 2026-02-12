#ifndef SIMPLE_PID_H
#define SIMPLE_PID_H

class PID {
  public:
    PID(float kp, float ki, float kd, float outMin, float outMax);

    void setTunings(float kp, float ki, float kd);
    void setOutputLimits(float min, float max);
    void setSetpoint(float sp);

    float compute(float input);

  private:
    float _kp, _ki, _kd;
    float _setpoint;

    float _integral;
    float _lastError;

    float _outMin, _outMax;

    unsigned long _lastTime;
};

#endif
