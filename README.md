# ESP8266 Differential Thrust Plane Flight Controller

A lightweight, low-cost, DIY flight controller for fixed-wing aircraft based on the **ESP8266** and **MPU6050** IMU.

Unlike traditional RC airplanes, this project **does not use any servos or control surfaces**. Instead, roll control is achieved entirely through **differential thrust** by independently controlling two coreless DC motors.

The project demonstrates that a stable fixed-wing aircraft can be built using inexpensive, widely available modules and a custom handwritten cascaded PID controller.

## Features

- ✈️ DIY and low-cost hardware
- 📡 ESP8266 flight controller
- 📶 Long-range **ESP-NOW** wireless communication (~200 m)
- 🧭 MPU6050 6-axis IMU
- ⚖️ Differential thrust flight control (no servos required)
- 🔄 Custom handwritten cascaded PID controller
- 🔋 Powered by a single-cell  Li-Ion/LiPo battery

## Hardware

- ESP8266 (Aircraft Flight Controller)
- ESP32 (Ground Controller)
- MPU6050 IMU
- 2 × Coreless DC motors
- 2 × MOSFET motor drivers
- 1S (3.7 V) Li-Ion/LiPo battery

## Communication

```
Phone Joystick
      │
   Wi-Fi (UDP)
      │
     ESP32
      │
   ESP-NOW
      │
    ESP8266
      │
 Differential Thrust
```

## Flight Control

The aircraft is stabilized using a custom cascaded PID controller.

The pilot sends only:

- Throttle
- Desired roll angle

The controller continuously reads the MPU6050, estimates aircraft attitude, and independently adjusts the left and right motor speeds to achieve the requested roll angle.

No ailerons, rudder, elevator servos, or traditional control surfaces are used.

## Repository Structure

- `esp8266_mpu6050_plane.ino` — Main flight controller
- `MPU6050_Module.*` — IMU driver
- `PID.*` — PID implementation
- `PIDManager.h` — PID management
- `Mixer.h` — Differential thrust mixer
- `Telemetry.h` — Telemetry interface

## Project Goals

- Build a lightweight DIY flight controller
- Minimize hardware cost
- Eliminate servos using differential thrust
- Demonstrate stable autonomous attitude control on an ESP8266
- Provide an educational open-source flight controller

## License

MIT License