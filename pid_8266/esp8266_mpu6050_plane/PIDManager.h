// PIDManager.h
#pragma once
#include "PID.h"
#include <math.h>

class PIDManager {
public:
    PIDManager()
        : pitchAnglePID(4.0, 0.0, 0.0, -200, 200),
          rollAnglePID(4.0, 0.0, 0.0, -200, 200),
          pitchRatePID(0.15, 0.0, 0.002, -500, 500),
          rollRatePID(0.15, 0.0, 0.002, -500, 500) {}

    float computePitch(float pitch, float gyroX, float externalRateSetpoint = NAN) {
        if (isnan(externalRateSetpoint)) {  // Angle mode by default
            pitchAnglePID.setSetpoint(0);   // level
            float desiredRate = pitchAnglePID.compute(pitch);
            pitchRatePID.setSetpoint(desiredRate);
        } else {
            pitchAnglePID.setSetpoint(externalRateSetpoint);   // level
            float desiredRate = pitchAnglePID.compute(pitch);
            pitchRatePID.setSetpoint(desiredRate);
            // pitchRatePID.setSetpoint(externalRateSetpoint);  // ACRO mode
        }
        return pitchRatePID.compute(gyroX);
    }

    float computeRoll(float roll, float gyroY, float externalAngleSetpoint = NAN) {
        if (isnan(externalAngleSetpoint)) {  // Rate mode by default
            rollAnglePID.setSetpoint(0);  // level
            float desiredRate = rollAnglePID.compute(roll);
            rollRatePID.setSetpoint(desiredRate);
        } else {
            rollAnglePID.setSetpoint(externalAngleSetpoint);  // ANGLE mode
            float desiredRate = rollAnglePID.compute(roll);
            rollRatePID.setSetpoint(desiredRate);
        }
        return rollRatePID.compute(gyroY);
    }

public:
    PID pitchAnglePID, rollAnglePID;
    PID pitchRatePID, rollRatePID;
};
