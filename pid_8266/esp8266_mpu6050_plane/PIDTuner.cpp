// PIDTuner.cpp
#include "PIDTuner.h"
#include "PIDStorage.h"
#include <Arduino.h>

PIDManager* PIDTuner::pid = nullptr;

void PIDTuner::attach(PIDManager *manager) {
    pid = manager;
}

void PIDTuner::apply(uint8_t axis, uint8_t level,
                     float kp, float ki, float kd)
{
    if (!pid) return;

    if (axis == 0) { // ROLL
        if (level == 0)
            pid->rollAnglePID.setTunings(kp, ki, kd);
        else
            pid->rollRatePID.setTunings(kp, ki, kd);
    } else if (axis == 1) { // PITCH
        if (level == 0)
            pid->pitchAnglePID.setTunings(kp, ki, kd);
        else
            pid->pitchRatePID.setTunings(kp, ki, kd);
    }

    Serial.printf("PID Updated A:%d L:%d Kp:%.3f Ki:%.3f Kd:%.3f\n",
                  axis, level, kp, ki, kd);

    // Save updated values to EEPROM
    PIDStorage::save(*pid);
}
