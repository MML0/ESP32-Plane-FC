#include "MPU6050_Module.h"

#include "PIDManager.h"
#include "Mixer.h"
#include "Telemetry.h"


MPU6050_Module imu(14);

PIDManager pid;

struct StickInput {
    int16_t x;  // -255 to +255 (left/right)
    int16_t y;  // -255 to +255 (forward/back)
};
StickInput stick = {0, 0};  // x = roll, y = pitch

// Map a value from one range to another
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void setup() {
    Serial.begin(921600);
    imu.begin(4, 5);
}

void loop() {
    // Roll in ANGLE mode: map stick to desired roll angle (-45° to +45°)
    float rollSetpoint = mapFloat(stick.x, -255, 255, -45.0, 45.0);

    // Pitch in ACRO mode: map stick to desired pitch rate (-200 deg/s to +200 deg/s)
    float pitchRateSetpoint = mapFloat(stick.y, -255, 255, -200.0, 200.0);

    if (imu.update()){
      // Get IMU data
      // float yaw   = imu.getYaw();
      // float pitch = imu.getPitch();
      // float roll  = imu.getRoll();

      float yaw   = imu.getYawUnwrapped();
      float pitch = imu.getPitchUnwrapped();
      float roll  = imu.getRollUnwrapped();

      float ax = imu.getAccX(), ay = imu.getAccY(), az = imu.getAccZ();
      float lax = imu.getLinAccX(), lay = imu.getLinAccY(), laz = imu.getLinAccZ();
      float gx = imu.getGyroX(), gy = imu.getGyroY(), gz = imu.getGyroZ();

      // Compute PID outputs
      float rollOutput  = pid.computeRoll(roll, gy, rollSetpoint);
      float pitchOutput = pid.computePitch(pitch, gx, pitchRateSetpoint);
      // Mix elevators
      ElevatorOutput elev = Mixer::mix(pitchOutput, rollOutput);

      // Telemetry
      Telemetry::print(yaw, pitch, roll, ax, ay, az, lax, lay, laz, gx, gy, gz);

      // Send to servos (pseudo)
      // leftServo.write(elev.left);
      // rightServo.write(elev.right);
    }

}
