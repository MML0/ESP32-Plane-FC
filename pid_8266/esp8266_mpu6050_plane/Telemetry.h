// Telemetry.h
#pragma once
#include <Arduino.h>

class Telemetry {
public:
    static void print(float yaw, float pitch, float roll,
                      float ax, float ay, float az,
                      float lax, float lay, float laz,
                      float gx, float gy, float gz)
    {
        // Serial.print(yaw); Serial.print(",");
        Serial.print(pitch); Serial.print(",");
        Serial.print(roll); Serial.print(",");
        // Serial.print(ax); Serial.print(",");
        // Serial.print(ay); Serial.print(",");
        // Serial.print(az); Serial.print(",");
        // Serial.print(lax); Serial.print(",");
        // Serial.print(lay); Serial.print(",");
        // Serial.print(laz); Serial.print(",");
        Serial.print(gx); Serial.print(",");
        Serial.println(gy); 
        // Serial.print(gy); Serial.print(",");
        // Serial.println(gz);

    }
};
