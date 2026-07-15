#include <esp_now.h>
#include <WiFi.h>

typedef struct {
  int counter;
  float distance_cm;
} Message;

Message incomingData;

void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingBytes, int len) {
  memcpy(&incomingData, incomingBytes, sizeof(incomingData));
  Serial.print("Received boot#: ");
  Serial.print(incomingData.counter);
  Serial.print("  Distance: ");
  Serial.print(incomingData.distance_cm);
  Serial.println(" cm");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
}

void loop() {}


/*
Received boot#: 2  Distance: 85.49 cm
Received boot#: 3  Distance: 85.08 cm
Received boot#: 4  Distance: 85.49 cm
Received boot#: 5  Distance: 85.49 cm
Received boot#: 6  Distance: 85.06 cm
Received boot#: 7  Distance: 85.49 cm
Received boot#: 8  Distance: 85.48 cm
Received boot#: 9  Distance: 85.49 cm
Received boot#: 10  Distance: 85.06 cm
Received boot#: 11  Distance: 85.48 cm
Received boot#: 12  Distance: 85.49 cm
Received boot#: 13  Distance: 35.86 cm
Received boot#: 14  Distance: 55.03 cm
Received boot#: 15  Distance: 85.48 cm
Received boot#: 16  Distance: 85.05 cm
*/