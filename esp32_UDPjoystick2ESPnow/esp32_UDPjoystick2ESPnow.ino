#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_now.h>

// ===== SELECT MODE =====
// 1 = AP mode
// 2 = Station mode
#define WIFI_MODE_SELECT 1
// =======================

WiFiUDP udp;
const unsigned int localPort = 50000;

char incomingPacket[32];

// Broadcast MAC
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

typedef struct {
  uint16_t ch1;
  uint16_t ch2;
  uint16_t ch3;
  uint16_t ch4;
} ControlPacket;

ControlPacket packet;

// ESP-NOW peer info
esp_now_peer_info_t peerInfo;

void setupWiFi() {

#if WIFI_MODE_SELECT == 1
  // ===== AP MODE =====
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAPConfig(IPAddress(192,168,43,66),
                    IPAddress(192,168,43,1),
                    IPAddress(255,255,255,0));

  WiFi.softAP("base_ap");
  Serial.println("Running in AP mode");
  Serial.println(WiFi.softAPIP());

#elif WIFI_MODE_SELECT == 2
  // ===== STATION MODE =====
  WiFi.mode(WIFI_STA);

  IPAddress local_IP(192,168,43,66);
  IPAddress gateway(192,168,43,1);
  IPAddress subnet(255,255,255,0);

  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin("2.", "12345678an");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");
  Serial.println(WiFi.localIP());

#endif

  // Force channel 1
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}

void setupESPNow() {

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW Ready");
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  setupESPNow();

  udp.begin(localPort);
  Serial.println("UDP → ESP-NOW bridge ready");
}

void loop() {

  int packetSize = udp.parsePacket();
  if (packetSize) {

    int len = udp.read(incomingPacket, sizeof(incomingPacket)-1);
    if (len > 0) incomingPacket[len] = 0;

    if (len == 16) {

      packet.ch1 = (incomingPacket[0]-'0')*1000 +
                   (incomingPacket[1]-'0')*100 +
                   (incomingPacket[2]-'0')*10 +
                   (incomingPacket[3]-'0');

      packet.ch2 = (incomingPacket[4]-'0')*1000 +
                   (incomingPacket[5]-'0')*100 +
                   (incomingPacket[6]-'0')*10 +
                   (incomingPacket[7]-'0');

      packet.ch3 = (incomingPacket[8]-'0')*1000 +
                   (incomingPacket[9]-'0')*100 +
                   (incomingPacket[10]-'0')*10 +
                   (incomingPacket[11]-'0');

      packet.ch4 = (incomingPacket[12]-'0')*1000 +
                   (incomingPacket[13]-'0')*100 +
                   (incomingPacket[14]-'0')*10 +
                   (incomingPacket[15]-'0');

      esp_now_send(broadcastAddress, (uint8_t*)&packet, sizeof(packet));

      Serial.printf("Sent: %d %d %d %d\n",
                    packet.ch1,
                    packet.ch2,
                    packet.ch3,
                    packet.ch4);
    }
  }
}
