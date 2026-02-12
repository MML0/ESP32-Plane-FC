#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu;

// DMP control/status
bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

// Orientation data
Quaternion q;
VectorFloat gravity;
float ypr[3];

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  devStatus = mpu.dmpInitialize();

  // Gyro offsets (rough defaults, OK for testing)
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    packetSize = mpu.dmpGetFIFOPacketSize();
    dmpReady = true;
    Serial.println("DMP ready!");
  } else {
    Serial.print("DMP init failed (code ");
    Serial.print(devStatus);
    Serial.println(")");
  }
}

void loop() {
  if (!dmpReady) return;

  fifoCount = mpu.getFIFOCount();

  if (fifoCount == 1024) {
    mpu.resetFIFO();
    Serial.println("FIFO overflow!");
    return;
  }

  if (fifoCount >= packetSize) {
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // Get quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);

    // Get gravity vector
    mpu.dmpGetGravity(&gravity, &q);

    // Get yaw pitch roll
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    float yaw   = ypr[0] * 180 / PI;
    float pitch = ypr[1] * 180 / PI;
    float roll  = ypr[2] * 180 / PI;

    Serial.print("Yaw: ");   Serial.print(yaw, 2);
    Serial.print(" | Pitch: "); Serial.print(pitch, 2);
    Serial.print(" | Roll: ");  Serial.println(roll, 2);
  }
}
