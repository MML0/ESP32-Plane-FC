// RadioReceiver.cpp  (ESP8266 VERSION)
#include "RadioReceiver.h"
#include "PIDTuner.h"
#include <Arduino.h>

volatile bool RadioReceiver::newControl = false;

int16_t RadioReceiver::roll = 0;
int16_t RadioReceiver::pitch = 0;
int16_t RadioReceiver::yaw = 0;
uint16_t RadioReceiver::throttle = 1000;

void RadioReceiver::begin() {

    WiFi.mode(WIFI_STA);
    wifi_set_channel(1);   // IMPORTANT for ESP-NOW

    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);  // allows send + receive

    esp_now_register_recv_cb(onRecv);

    Serial.println("ESP8266 Radio Receiver Ready");
}

void RadioReceiver::onRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
    // Serial.printf("Received ESP-NOW packet, len=%d\n", len);

    if(len == sizeof(ControlPacket)) {
        ControlPacket packet;
        memcpy(&packet, data, len);
        handleControl(packet);  // map fields to roll/pitch/yaw/throttle
    }
    else if(len == sizeof(PIDPacket)) {
        PIDPacket pidPacket;
        memcpy(&pidPacket, data, len);
        PIDTuner::apply(pidPacket.axis,
                        pidPacket.level,
                        pidPacket.kp,
                        pidPacket.ki,
                        pidPacket.kd);
    }
    else {
        Serial.println("Unknown packet type");
    }
}


void RadioReceiver::handleControl(const ControlPacket &p) {

    roll     = p.roll;
    pitch    = p.pitch;
    yaw      = p.yaw;
    throttle = p.throttle;

    newControl = true;
    // Log received values
    Serial.printf("Control packet applied -> roll=%d pitch=%d yaw=%d throttle=%d\n",
                  roll, pitch, yaw, throttle);
}

bool RadioReceiver::hasNewControl() {

    if (newControl) {
        newControl = false;
        return true;
    }

    return false;
}

void RadioReceiver::getControl(int16_t &r, int16_t &p, int16_t &y, uint16_t &t) {
    r = roll;
    p = pitch;
    y = yaw;
    t = throttle;
}
