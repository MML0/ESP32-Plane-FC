// RadioReceiver.h  (ESP8266 VERSION)
#pragma once

#include <ESP8266WiFi.h>

extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

enum CommandType {
    CMD_CONTROL = 0,
    CMD_PID_TUNE = 1
};

struct RadioPacket {
    uint8_t cmdType;

    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    uint16_t throttle;

    uint8_t axis;     // 0=roll 1=pitch
    uint8_t level;    // 0=anglePID 1=ratePID
    float kp;
    float ki;
    float kd;
};
// Control packet
struct __attribute__((packed)) ControlPacket {
    uint8_t cmdType;   // CMD_CONTROL = 0
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    uint16_t throttle;
};

// PID packet
struct __attribute__((packed)) PIDPacket {
    uint8_t cmdType;   // CMD_PID_TUNE = 1
    uint8_t axis;      // 0 = roll, 1 = pitch
    uint8_t level;     // 0 = angle PID, 1 = rate PID
    float kp;
    float ki;
    float kd;
};


class RadioReceiver {
public:
    void begin();
    bool hasNewControl();
    void getControl(int16_t &r, int16_t &p, int16_t &y, uint16_t &t);

private:
    static void onRecv(uint8_t *mac, uint8_t *data, uint8_t len);
    static void handleControl(const ControlPacket &p);

    static volatile bool newControl;

    static int16_t roll;
    static int16_t pitch;
    static int16_t yaw;
    static uint16_t throttle;
};
