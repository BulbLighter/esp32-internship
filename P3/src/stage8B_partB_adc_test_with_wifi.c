// Stage 8B Part B — ADC2 test, WiFi ON
#include <WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const int ADC2_TEST_PIN = 25;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("=== ADC2 Test (GPIO 25) — WiFi ON ===");
}

void loop() {
  int raw = analogRead(ADC2_TEST_PIN);
  Serial.print("GPIO 25 raw (WiFi ON): ");
  Serial.println(raw);
  delay(500);
}

// part B

// ...
// === ADC2 Test (GPIO 25) — WiFi ON ===
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0
// GPIO 25 raw (WiFi ON): 0