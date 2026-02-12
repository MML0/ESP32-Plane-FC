# ESP8266 MPU6050 Plane Flight Controller

This project is a flight controller for a plane using the ESP8266 , ESP32 microcontroller and the MPU6050 sensor module. It includes PID control and telemetry features.

## Repository

Original inspiration and reference: [ESP32-Plane-FC](https://github.com/MML0/ESP32-Plane-FC.git)

## Files

- `esp8266_mpu6050_plane.ino`: Main Arduino sketch file.
- `MPU6050_Module.cpp` / `MPU6050_Module.h`: MPU6050 sensor module interface.
- `PID.cpp` / `PID.h`: PID controller implementation.
- `PIDManager.h`: PID management header.
- `Mixer.h`: Mixer header for control mixing.
- `Telemetry.h`: Telemetry header for data transmission.

## Features

- MPU6050 sensor integration for orientation and motion sensing.
- PID control loops for flight stabilization.
- Telemetry support for monitoring flight data.

## Usage

Compile and upload the `esp8266_mpu6050_plane.ino` sketch to an ESP8266 board using the Arduino IDE or compatible environment.

## License

MIT
