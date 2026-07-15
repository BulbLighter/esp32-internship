#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(2000); // extra long delay to be safe
}

void loop() {
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  delay(2000); // print repeatedly instead of once
}

