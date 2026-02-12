#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

MPU6050 mpu;

#define INTERRUPT_PIN 14
#define CALIB_SAMPLES 500

bool dmpReady = false;
uint16_t packetSize;
uint8_t fifoBuffer[64];

Quaternion q;
VectorFloat gravity;
VectorInt16 aa, aaReal;
float ypr[3];

int16_t gx_raw, gy_raw, gz_raw;
float gx, gy, gz;

float gx_bias=0, gy_bias=0, gz_bias=0;

volatile bool mpuInterrupt = false;
void IRAM_ATTR dmpDataReady() { mpuInterrupt = true; }

void calibrateGyro() {
  long sx=0, sy=0, sz=0;
  delay(200);
  for(int i=0;i<CALIB_SAMPLES;i++){
    mpu.getRotation(&gx_raw,&gy_raw,&gz_raw);
    sx+=gx_raw; sy+=gy_raw; sz+=gz_raw;
    delay(1);
  }
  gx_bias = (sx/(float)CALIB_SAMPLES)/16.4;
  gy_bias = (sy/(float)CALIB_SAMPLES)/16.4;
  gz_bias = (sz/(float)CALIB_SAMPLES)/16.4;
}

void setup() {
  Serial.begin(921600);
  Wire.begin(4,5);
  Wire.setClock(400000);

  mpu.initialize();
  mpu.dmpInitialize();
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

  calibrateGyro();

  mpu.setDMPEnabled(true);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);

  packetSize = mpu.dmpGetFIFOPacketSize();
  dmpReady = true;
}

void loop() {
  if (!dmpReady || !mpuInterrupt) return;
  mpuInterrupt = false;

  uint16_t fifoCount = mpu.getFIFOCount();
  if (fifoCount >= 1024) { mpu.resetFIFO(); return; }
  if (fifoCount < packetSize) return;

  mpu.getFIFOBytes(fifoBuffer, packetSize);

  // ----- Orientation -----
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  float yaw   = ypr[0]*180/M_PI;
  float pitch = ypr[1]*180/M_PI;
  float roll  = ypr[2]*180/M_PI;

  // ----- Acceleration -----
  mpu.dmpGetAccel(&aa, fifoBuffer);
  mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);

  float ax = (aa.x / 16384.0) * 9.80665;
  float ay = (aa.y / 16384.0) * 9.80665;
  float az = (aa.z / 16384.0) * 9.80665;

  float lax = (aaReal.x / 16384.0) * 9.80665;
  float lay = (aaReal.y / 16384.0) * 9.80665;
  float laz = (aaReal.z / 16384.0) * 9.80665;

  // ----- Gyro -----
  mpu.getRotation(&gx_raw, &gy_raw, &gz_raw);
  gx = gx_raw/16.4 - gx_bias;
  gy = gy_raw/16.4 - gy_bias;
  gz = gz_raw/16.4 - gz_bias;

  // ----- TELEMETRY CSV -----
  Serial.print(yaw); Serial.print(",");
  Serial.print(pitch); Serial.print(",");
  Serial.print(roll); Serial.print(",");

  // Serial.print(ax); Serial.print(",");
  // Serial.print(ay); Serial.print(",");
  // Serial.print(az); Serial.print(",");

  // Serial.print(lax); Serial.print(",");
  // Serial.print(lay); Serial.print(",");
  // Serial.print(laz); Serial.print(",");

  Serial.print(gx); Serial.print(",");
  Serial.print(gy); Serial.print(",");
  Serial.println(gz);
}
