// ESP32_UDP_to_ESP_NOW.ino
#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_now.h>
#include "esp_wifi.h"

// ===== CONFIG =====
#define WIFI_CHANNEL 1
#define LOCAL_UDP_PORT 50000

WiFiUDP udp;

// Broadcast to all ESP-NOW devices
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// ---------------- PID PACKET ----------------
typedef struct __attribute__((packed)) {
    uint8_t cmdType;
    uint8_t axis;
    uint8_t level;
    float kp;
    float ki;
    float kd;
} PIDPacket;

// ---------------- CONTROL PACKET ----------------
typedef struct __attribute__((packed)) {
    uint8_t cmdType;  // CMD_CONTROL = 0
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    uint16_t throttle;
} ControlPacket;

PIDPacket pidPacket;
ControlPacket ctrlPacket;
esp_now_peer_info_t peerInfo;

// ---------------- ESP-NOW CALLBACK ----------------
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("ESP-NOW send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

// ---------------- WIFI / AP ----------------
void setupWiFi() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("ESP32_AP_esp_now_bridge", "", WIFI_CHANNEL);
    Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
    Serial.print("Channel: "); Serial.println(WiFi.channel());
}

// ---------------- ESP-NOW ----------------
void setupESPNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = WIFI_CHANNEL;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    esp_now_register_send_cb(onSent);
    Serial.println("ESP-NOW Ready");
}

// ---------------- SETUP ----------------
void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupESPNow();
    udp.begin(LOCAL_UDP_PORT);
    Serial.println("UDP → ESP-NOW bridge ready");
}

// ---------------- LOOP ----------------
void loop() {
    int packetSize = udp.parsePacket();
    if(packetSize <= 0) return;

    // read full packet into buffer
    char buf[64];      // big enough for control or PID
    int len = udp.read((uint8_t*)buf, sizeof(buf)-1);
    buf[len] = 0;      // null-terminate

    // ---- PID Packet ----
    if(len == sizeof(PIDPacket)) {
        PIDPacket pidPacket;
        memcpy(&pidPacket, buf, sizeof(PIDPacket));

        Serial.printf("Received PID: axis=%d level=%d Kp=%.3f Ki=%.3f Kd=%.3f\n",
                      pidPacket.axis, pidPacket.level,
                      pidPacket.kp, pidPacket.ki, pidPacket.kd);

        if(esp_now_send(broadcastAddress, (uint8_t*)&pidPacket, sizeof(PIDPacket)) != ESP_OK)
            Serial.println("ESP-NOW send failed!");
    }
    // ---- Control Packet (16-byte ASCII) ----
    else if(len == 16) {
        ControlPacket ctrlPacket;
        char temp[5]; temp[4] = 0;

        memcpy(temp, buf, 4);     ctrlPacket.yaw    = atoi(temp);
        memcpy(temp, buf+4, 4);   ctrlPacket.throttle = atoi(temp);
        memcpy(temp, buf+8, 4);   ctrlPacket.roll      = atoi(temp);
        memcpy(temp, buf+12, 4);  ctrlPacket.pitch    = atoi(temp);

        ctrlPacket.cmdType = 0;  // control

        // Serial.printf("Received Control: roll=%d pitch=%d yaw=%d throttle=%d\n",
        //               ctrlPacket.roll, ctrlPacket.pitch,
        //               ctrlPacket.yaw, ctrlPacket.throttle);

        if(esp_now_send(broadcastAddress, (uint8_t*)&ctrlPacket, sizeof(ctrlPacket)) != ESP_OK)
            Serial.println("ESP-NOW send failed!");
    }
    else {
        Serial.printf("UDP unknown packet (%d bytes): %.*s\n", packetSize, len, buf);
    }
}

