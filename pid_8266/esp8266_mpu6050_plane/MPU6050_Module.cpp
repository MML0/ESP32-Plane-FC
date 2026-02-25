#include "MPU6050_Module.h"

MPU6050_Module* MPU6050_Module::instance = nullptr;

MPU6050_Module::MPU6050_Module(uint8_t interruptPin) {
    _interruptPin = interruptPin;
    instance = this;
    yaw_prev = pitch_prev = roll_prev = 0;
    yaw_total = pitch_total = roll_total = 0;

}

void IRAM_ATTR MPU6050_Module::isrWrapper() {
    if (instance) instance->mpuInterrupt = true;
}

bool MPU6050_Module::begin(uint8_t sda, uint8_t scl) {
    Wire.begin(sda, scl);
    Wire.setClock(400000);

    mpu.initialize();
    mpu.dmpInitialize();
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

    // calibrateGyro();

    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(_interruptPin), isrWrapper, RISING);

    packetSize = mpu.dmpGetFIFOPacketSize();
    dmpReady = true;

    return true;
}

void MPU6050_Module::calibrateGyro(uint16_t samples) {
    long sx=0, sy=0, sz=0;
    delay(1);

    for(int i=0;i<samples;i++){
        mpu.getRotation(&gx_raw,&gy_raw,&gz_raw);
        sx+=gx_raw; sy+=gy_raw; sz+=gz_raw;
        delay(1);
    }

    gx_bias = (sx/(float)samples)/16.4;
    gy_bias = (sy/(float)samples)/16.4;
    gz_bias = (sz/(float)samples)/16.4;
}


float MPU6050_Module::unwrapAngle(float current, float &prev, float &total) {
    float delta = current - prev;

    if (delta > 200)       delta -= 360;
    else if (delta < -200) delta += 360;
    else if (delta > 150)  delta -= 180;
    else if (delta < -150) delta += 180;
    
    total += delta;
    prev = current;
    return total;
}

bool MPU6050_Module::update() {
    if (!dmpReady || !mpuInterrupt) return false;
    mpuInterrupt = false;

    uint16_t fifoCount = mpu.getFIFOCount();
    if (fifoCount >= 1024) {
        mpu.resetFIFO();
        return false;
    }

    if (fifoCount < packetSize) return false;

    mpu.getFIFOBytes(fifoBuffer, packetSize);

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    mpu.getRotation(&gx_raw, &gy_raw, &gz_raw);
    gx = gx_raw/16.4 - gx_bias;
    gy = gy_raw/16.4 - gy_bias;
    gz = gz_raw/16.4 - gz_bias;

    // ----- Acceleration -----
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);

    ax  = (aa.x / 16384.0) * 9.80665;
    ay  = (aa.y / 16384.0) * 9.80665;
    az  = (aa.z / 16384.0) * 9.80665;

    lax = (aaReal.x / 16384.0) * 9.80665;
    lay = (aaReal.y / 16384.0) * 9.80665;
    laz = (aaReal.z / 16384.0) * 9.80665;

    // Unwrap angles
    unwrapAngle(ypr[0]*180/M_PI, yaw_prev, yaw_total);
    unwrapAngle(ypr[1]*180/M_PI, pitch_prev, pitch_total);
    unwrapAngle(ypr[2]*180/M_PI, roll_prev, roll_total);



    return true;
}

// Getters
float MPU6050_Module::getYaw()   { return ypr[0]*180/M_PI; }
float MPU6050_Module::getPitch() { return ypr[1]*180/M_PI; }
float MPU6050_Module::getRoll()  { return ypr[2]*180/M_PI; }

float MPU6050_Module::getGyroX() { return gx; }
float MPU6050_Module::getGyroY() { return gy; }
float MPU6050_Module::getGyroZ() { return gz; }

float MPU6050_Module::getAccX() { return ax; }
float MPU6050_Module::getAccY() { return ay; }
float MPU6050_Module::getAccZ() { return az; }

float MPU6050_Module::getLinAccX() { return lax; }
float MPU6050_Module::getLinAccY() { return lay; }
float MPU6050_Module::getLinAccZ() { return laz; }

// Getters unwrapped
float MPU6050_Module::getYawUnwrapped()   { return yaw_total; }
float MPU6050_Module::getPitchUnwrapped() { return pitch_total; }
float MPU6050_Module::getRollUnwrapped()  { return roll_total; }

