#ifndef MPU6050_MODULE_H
#define MPU6050_MODULE_H

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

class MPU6050_Module {
public:
    MPU6050_Module(uint8_t interruptPin);

    bool begin(uint8_t sda, uint8_t scl);
    void calibrateGyro(uint16_t samples = 100);
    bool update();

    // getters
    float getYaw();
    float getPitch();
    float getRoll();

    float getAccX();
    float getAccY();
    float getAccZ();

    float getLinAccX();
    float getLinAccY();
    float getLinAccZ();

    float getYawUnwrapped();
    float getPitchUnwrapped();
    float getRollUnwrapped();

    float getGyroX();
    float getGyroY();
    float getGyroZ();

private:
    MPU6050 mpu;

    uint8_t _interruptPin;
    bool dmpReady = false;
    uint16_t packetSize;

    uint8_t fifoBuffer[64];

    Quaternion q;
    VectorFloat gravity;
    VectorInt16 aa, aaReal;

    float ypr[3];


    // Unwrap
    float yaw_prev, pitch_prev, roll_prev;
    float yaw_total, pitch_total, roll_total;


    int16_t gx_raw, gy_raw, gz_raw;
    float gx, gy, gz;
    float ax, ay, az;
    float lax, lay, laz;
    
    float gx_bias = 0, gy_bias = 0, gz_bias = 0;

    volatile bool mpuInterrupt = false;

    static void IRAM_ATTR isrWrapper();
    static MPU6050_Module* instance;
    
    float unwrapAngle(float current, float &prev, float &total);
};

#endif
