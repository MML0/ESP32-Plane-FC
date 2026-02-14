// PIDTuner.h
#pragma once
#include "PIDManager.h"
#include <stdint.h>

class PIDTuner {
public:
    static void attach(PIDManager *manager);
    static void apply(uint8_t axis, uint8_t level,
                      float kp, float ki, float kd);

private:
    static PIDManager *pid;
};
