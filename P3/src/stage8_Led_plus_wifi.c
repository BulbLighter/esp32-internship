// PROJECT 3 — Stage 8: WiFi Extension
// Combines: LDR filtering (Stage 5) + threshold switch (Stage 6) + WiFi POST

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// === WiFi credentials — EDIT THESE ===
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverURL = "http://YOUR_PC_IP:5000/light";  // Use your PC's IP from Project 2

// === Pin setup ===
const int LDR_PIN = 35;   // ADC1 — safe with WiFi
const int LED_PIN = 22;
const int NUM_SAMPLES = 16;

// === Hysteresis thresholds (from Stage 6) ===
const int DARK_THRESHOLD = 150;
const int BRIGHT_THRESHOLD = 350;

bool ledState = false;
unsigned long lastPostTime = 0;
const unsigned long POST_INTERVAL = 5000;  // 5 seconds

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  analogSetAttenuation(ADC_11db);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
  // Auto-reconnect WiFi if dropped (from Project 2 lesson)
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.begin(ssid, password);
    delay(1000);
    return;
  }
  
  // Get filtered ADC reading (Stage 5B averaging)
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(LDR_PIN);
    delayMicroseconds(100);
  }
  int averaged = sum / NUM_SAMPLES;
  float voltage = (averaged / 4095.0) * 3.3;
  
  // Hysteresis logic (Stage 6)
  if (averaged < DARK_THRESHOLD && ledState == false) {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("  >>> LED ON (dark detected) <<<");
  } 
  else if (averaged > BRIGHT_THRESHOLD && ledState == true) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("  >>> LED OFF (bright detected) <<<");
  }
  
  Serial.print("Averaged ADC: ");
  Serial.print(averaged);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V | LED: ");
  Serial.println(ledState ? "ON" : "OFF");
  
  // POST to server every 5 seconds (non-blocking, millis()-based)
  if (millis() - lastPostTime >= POST_INTERVAL) {
    lastPostTime = millis();
    sendLightData(averaged, voltage);
  }
  
  delay(200);
}

void sendLightData(int raw, float volts) {
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<200> doc;
  doc["light_raw"] = raw;
  doc["light_volts"] = volts;
  doc["ts"] = millis();
  
  String jsonStr;
  serializeJson(doc, jsonStr);
  
  int httpCode = http.POST(jsonStr);
  
  if (httpCode > 0) {
    Serial.print("[POST] Success, code: ");
    Serial.println(httpCode);
  } else {
    Serial.print("[POST] Failed, error: ");
    Serial.println(http.errorToString(httpCode));
  }
  
  http.end();
}

// ....
// Connected! IP address: 10.166.247.18

// Averaged ADC: 1588 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1577 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1583 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1584 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1575 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1580 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1591 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1584 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1589 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1586 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1582 | Voltage: 1.27V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1585 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1517 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1511 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1585 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1574 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1577 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1517 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1517 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1576 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1589 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1578 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1582 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1583 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1587 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1578 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1577 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1588 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1583 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1513 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1584 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1515 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1577 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1520 | Voltage: 1.22V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1517 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1586 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1517 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1577 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1506 | Voltage: 1.21V | LED: OFF
// Averaged ADC: 1575 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1583 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1579 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1587 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1586 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1541 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1514 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1565 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1583 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1584 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1588 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1579 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1583 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1603 | Voltage: 1.29V | LED: OFF
// Averaged ADC: 1596 | Voltage: 1.29V | LED: OFF
// Averaged ADC: 1591 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1589 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1598 | Voltage: 1.29V | LED: OFF
// Averaged ADC: 1598 | Voltage: 1.29V | LED: OFF
// Averaged ADC: 1601 | Voltage: 1.29V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1608 | Voltage: 1.30V | LED: OFF
// Averaged ADC: 1604 | Voltage: 1.29V | LED: OFF
// Averaged ADC: 1618 | Voltage: 1.30V | LED: OFF
// Averaged ADC: 1610 | Voltage: 1.30V | LED: OFF
// Averaged ADC: 1619 | Voltage: 1.30V | LED: OFF
// Averaged ADC: 1539 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1540 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1528 | Voltage: 1.23V | LED: OFF
// Averaged ADC: 1549 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1557 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1553 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1569 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1559 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1554 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1552 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1567 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1559 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1562 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1567 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1559 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1570 | Voltage: 1.27V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1558 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1554 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1565 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1553 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1565 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1508 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1496 | Voltage: 1.21V | LED: OFF
// Averaged ADC: 1558 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1562 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1557 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1573 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1561 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1559 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1560 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1561 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1572 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1566 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1568 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1551 | Voltage: 1.25V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1560 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1557 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1522 | Voltage: 1.23V | LED: OFF
// Averaged ADC: 1496 | Voltage: 1.21V | LED: OFF
// Averaged ADC: 1493 | Voltage: 1.20V | LED: OFF
// Averaged ADC: 1567 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1567 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1561 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1554 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1560 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1552 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1562 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1554 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1561 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1574 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1565 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1570 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1560 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1571 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1583 | Voltage: 1.28V | LED: OFF
// Averaged ADC: 1563 | Voltage: 1.26V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1566 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1568 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1578 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1555 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1551 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1511 | Voltage: 1.22V | LED: OFF
// Averaged ADC: 1548 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1555 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1551 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1555 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1560 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1562 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1558 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1554 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1566 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1570 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1561 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1548 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1555 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1556 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1557 | Voltage: 1.25V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1567 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1559 | Voltage: 1.26V | LED: OFF
//   >>> LED ON (dark detected) <<<
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// [POST] Success, code: 200
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// [POST] Success, code: 200
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
// [POST] Success, code: 200
// Averaged ADC: 0 | Voltage: 0.00V | LED: ON
//   >>> LED OFF (bright detected) <<<
// Averaged ADC: 1497 | Voltage: 1.21V | LED: OFF
// Averaged ADC: 1568 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1575 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1580 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1580 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1574 | Voltage: 1.27V | LED: OFF
// Averaged ADC: 1507 | Voltage: 1.21V | LED: OFF
// Averaged ADC: 1491 | Voltage: 1.20V | LED: OFF
// Averaged ADC: 1549 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1557 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1548 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1564 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1551 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1546 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1548 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1548 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1555 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1548 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1560 | Voltage: 1.26V | LED: OFF
// Averaged ADC: 1547 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1540 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1540 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1542 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1537 | Voltage: 1.24V | LED: OFF
// [POST] Success, code: 200
// Averaged ADC: 1545 | Voltage: 1.25V | LED: OFF
// Averaged ADC: 1539 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1541 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1541 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1542 | Voltage: 1.24V | LED: OFF
// Averaged ADC: 1491 | Voltage: 1.20V | LED: OFF
// Averaged ADC: 1493 | Voltage: 1.20V | LED: OFF
