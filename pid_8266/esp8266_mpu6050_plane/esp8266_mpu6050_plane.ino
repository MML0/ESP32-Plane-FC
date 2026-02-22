#include "MPU6050_Module.h"
#include "PIDManager.h"
#include "Mixer.h"
#include "Telemetry.h"
#include "RadioReceiver.h"
#include "PIDTuner.h"
#include "PIDStorage.h"
#include "Actuators.h"

MPU6050_Module imu(D6);
RadioReceiver radio;
PIDManager pid;
Actuators actuators;

float throttle = 0;
float yawOutput = 0;

struct StickInput {
    int16_t x;  // -255 to +255 (left/right)
    int16_t y;  // -255 to +255 (forward/back)
    float throttle;
    float yaw;
};
StickInput stick = {0, 0, 0, 0};  // x = roll, y = pitch

// Map a value from one range to another
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void logPIDValues() {
    Serial.println("=== PID Values ===");

    Serial.printf("Roll Angle:   Kp=%.3f Ki=%.3f Kd=%.3f\n",
                  pid.rollAnglePID.getKp(),
                  pid.rollAnglePID.getKi(),
                  pid.rollAnglePID.getKd());

    Serial.printf("Pitch Angle:  Kp=%.3f Ki=%.3f Kd=%.3f\n",
                  pid.pitchAnglePID.getKp(),
                  pid.pitchAnglePID.getKi(),
                  pid.pitchAnglePID.getKd());

    Serial.printf("Roll Rate:    Kp=%.3f Ki=%.3f Kd=%.3f\n",
                  pid.rollRatePID.getKp(),
                  pid.rollRatePID.getKi(),
                  pid.rollRatePID.getKd());

    Serial.printf("Pitch Rate:   Kp=%.3f Ki=%.3f Kd=%.3f\n",
                  pid.pitchRatePID.getKp(),
                  pid.pitchRatePID.getKi(),
                  pid.pitchRatePID.getKd());

    Serial.println("==================");
}


void setup() {
    Serial.begin(921600);

    imu.begin(D5, D7);
    radio.begin();
    actuators.begin();

    PIDStorage::begin();
    PIDStorage::load(pid);
    PIDTuner::attach(&pid);

    logPIDValues();
}


void loop() {
    // ---------- RADIO ----------
    if (radio.hasNewControl()) {
        int16_t r,p,y;
        uint16_t t;

        radio.getControl(r,p,y,t);

        stick.x = map(r, 1000, 2000, -255, 255);
        stick.y = map(p, 1000, 2000, -255, 255);

        // stick.throttle = map(t, 1000, 2000, 0, 255);
        stick.throttle = map(t, 1000, 2000, -30, 255);
        stick.yaw = map(y, 1000, 2000, -200, 200);
    }
    
    if (imu.update()){
      // Get IMU data
      float yaw   = imu.getYaw();
      float pitch = imu.getPitch();
      float roll  = imu.getRoll();

      // float yaw   = imu.getYawUnwrapped();
      // float pitch = imu.getPitchUnwrapped();
      // float roll  = imu.getRollUnwrapped();

      float ax = imu.getAccX(), ay = imu.getAccY(), az = imu.getAccZ();
      float lax = imu.getLinAccX(), lay = imu.getLinAccY(), laz = imu.getLinAccZ();
      float gx = imu.getGyroX(), gy = imu.getGyroY(), gz = imu.getGyroZ();

      // ---------- SETPOINTS ----------
      float rollSetpoint = mapFloat(stick.x, -255, 255, -40.0, 40.0);
      float pitchRateSetpoint = mapFloat(stick.y, -255, 255, -40.0, 40.0);

      // Compute PID outputs

      float rollOutput  = pid.computeRoll(roll , gx , rollSetpoint);
      float pitchOutput = pid.computePitch(pitch , -gy , pitchRateSetpoint);

      // Telemetry
      Telemetry::print(yaw, pitch, roll, ax, ay, az, lax, lay, laz, gx, gy, gz);

      // ---------- ACTUATORS ----------
      actuators.setElevators(pitchOutput, rollOutput);
      actuators.setMotors(stick.throttle, stick.yaw);

    }

}
