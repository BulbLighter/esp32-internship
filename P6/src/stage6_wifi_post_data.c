#include <WiFi.h>
#include <HTTPClient.h>

#define BUTTON_PIN 21

const char* ssid = "wifi name";
const char* password = "pw";
const char* serverURL = "http://[PC_IP]:5000/pulses"; 

volatile unsigned long pulseCount = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

unsigned long countAtWindowStart = 0;
unsigned long windowStartTime = 0;
float lastRate = 0;

unsigned long lastPostTime = 0;

void IRAM_ATTR buttonISR() {
  portENTER_CRITICAL_ISR(&mux);
  pulseCount++;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  windowStartTime = millis();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());
}

float computeRate() {
  unsigned long elapsed = millis() - windowStartTime;
  if (elapsed >= 60000) {
    portENTER_CRITICAL(&mux);
    unsigned long count = pulseCount;
    portEXIT_CRITICAL(&mux);
    lastRate = (count - countAtWindowStart) * 60000.0 / elapsed;
    countAtWindowStart = count;
    windowStartTime = millis();
  }
  return lastRate;
}

void postData(unsigned long count, float rate) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping POST");
    return;
  }

  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"count_total\":" + String(count) +
                    ",\"rate_per_min\":" + String(rate, 2) + "}";

  int httpCode = http.POST(payload);
  Serial.print("POST -> ");
  Serial.print(payload);
  Serial.print(" | Response: ");
  Serial.println(httpCode);

  http.end();
}

void loop() {
  float rate = computeRate(); // updates only every 60s internally

  if (millis() - lastPostTime >= 30000) {
    portENTER_CRITICAL(&mux);
    unsigned long safeCount = pulseCount;
    portEXIT_CRITICAL(&mux);

    postData(safeCount, rate);
    lastPostTime = millis();
  }
}

// ...
// Connected. IP: 172.22.15.18
// POST -> {"count_total":282,"rate_per_min":0.00} | Response: 200
// POST -> {"count_total":614,"rate_per_min":611.00} | Response: 200
// POST -> {"count_total":945,"rate_per_min":611.00} | Response: 200
// POST -> {"count_total":1278,"rate_per_min":661.00} | Response: 200
// POST -> {"count_total":1613,"rate_per_min":661.00} | Response: 200
// POST -> {"count_total":1950,"rate_per_min":666.00} | Response: 200
// POST -> {"count_total":2286,"rate_per_min":666.00} | Response: 200
// POST -> {"count_total":2624,"rate_per_min":667.00} | Response: 200
// POST -> {"count_total":2963,"rate_per_min":667.00} | Response: 200
// POST -> {"count_total":3301,"rate_per_min":668.00} | Response: 200
// POST -> {"count_total":3632,"rate_per_min":668.00} | Response: 200
// POST -> {"count_total":3966,"rate_per_min":666.00} | Response: 200
// POST -> {"count_total":4282,"rate_per_min":666.00} | Response: 200
// POST -> {"count_total":4587,"rate_per_min":618.00} | Response: 200
// POST -> {"count_total":4921,"rate_per_min":618.00} | Response: 200
// POST -> {"count_total":5257,"rate_per_min":668.00} | Response: 200
// POST -> {"count_total":5589,"rate_per_min":668.00} | Response: 200
// POST -> {"count_total":5923,"rate_per_min":661.00} | Response: 200
// POST -> {"count_total":6265,"rate_per_min":661.00} | Response: 200
// POST -> {"count_total":6606,"rate_per_min":676.00} | Response: 200
// POST -> {"count_total":6941,"rate_per_min":676.00} | Response: 200
// POST -> {"count_total":7273,"rate_per_min":665.00} | Response: 200
// POST -> {"count_total":7605,"rate_per_min":665.00} | Response: 200
