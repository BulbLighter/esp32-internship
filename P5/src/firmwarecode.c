#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ─── CONFIG ───────────────────────────────────────────────
#define WIFI_SSID     "wifi name"
#define WIFI_PASS     "pw"
#define SERVER_URL    "http://[PC_IP]:5000/readings"
#define DEVICE_ID     "node-01"
#define FW_VERSION    "1.0.0"

#define DHT_PIN       4
#define DHT_TYPE      DHT22
#define LDR_PIN       32
#define POST_INTERVAL 15000   // 15 seconds

// ─── STATE MACHINE ────────────────────────────────────────
enum State { CONNECTING, ONLINE, OFFLINE_RETRY };
State currentState = CONNECTING;

// ─── GLOBALS ──────────────────────────────────────────────
DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastPostTime   = 0;
unsigned long lastRetryTime  = 0;
unsigned long successCount   = 0;
unsigned long failCount      = 0;
unsigned long readErrorCount = 0;

// ─── SENSOR READ ──────────────────────────────────────────
struct SensorData {
  float temp;
  float hum;
  int   lightRaw;
  bool  valid;
};

SensorData readSensors() {
  SensorData data;
  data.temp     = dht.readTemperature();
  data.hum      = dht.readHumidity();
  int sum = 0;
  for (int i = 0; i < 16; i++) {
  sum += analogRead(LDR_PIN);
  delay(5);
  }
  data.lightRaw = sum / 16;
  data.valid    = (!isnan(data.temp) && !isnan(data.hum));
  if (!data.valid) readErrorCount++;
  return data;
}

// ─── BUILD PAYLOAD ────────────────────────────────────────
String buildPayload(SensorData &d) {
  StaticJsonDocument<256> doc;
  doc["device_id"]       = DEVICE_ID;
  doc["fw_version"]      = FW_VERSION;
  doc["uptime_s"]        = millis() / 1000;
  doc["rssi"]            = WiFi.RSSI();
  doc["temp"]            = d.temp;
  doc["hum"]             = d.hum;
  doc["light_raw"]       = d.lightRaw;
  doc["light_lux_approx"] = map(d.lightRaw, 0, 4095, 0, 1000);
  doc["ts"]              = millis() / 1000;
  String out;
  serializeJson(doc, out);
  return out;
}

// ─── TRANSMIT ─────────────────────────────────────────────
bool transmit(String &payload) {
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(payload);
  http.end();
  return (code == 200);
}

// ─── SETUP ────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  dht.begin();
  analogSetAttenuation(ADC_11db);
  Serial.println("[BOOT] Starting node-01");
}

// ─── LOOP ─────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  switch (currentState) {

    case CONNECTING:
      Serial.println("[WIFI] Connecting...");
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println();
      Serial.print("[WIFI] Connected. IP: ");
      Serial.println(WiFi.localIP());
      currentState = ONLINE;
      break;

    case ONLINE:
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WIFI] Lost connection. Entering OFFLINE_RETRY.");
        currentState = OFFLINE_RETRY;
        break;
      }
      if (now - lastPostTime >= POST_INTERVAL) {
        lastPostTime = now;
        SensorData d = readSensors();
        if (!d.valid) {
          Serial.printf("[SENSOR] Read error #%lu. Skipping POST.\n", readErrorCount);
          break;
        }
        Serial.printf("[SENSOR] T=%.1f C  H=%.1f%%  Light=%d\n",
                      d.temp, d.hum, d.lightRaw);
        String payload = buildPayload(d);
        if (transmit(payload)) {
          successCount++;
          Serial.printf("[POST] OK #%lu\n", successCount);
        } else {
          failCount++;
          Serial.printf("[POST] FAIL #%lu\n", failCount);
          currentState = OFFLINE_RETRY;
        }
      }
      break;

    case OFFLINE_RETRY:
      if (now - lastRetryTime >= 5000) {
        lastRetryTime = now;
        Serial.println("[RETRY] Attempting reconnect...");
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        unsigned long waitStart = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - waitStart < 10000) 
        {
              delay(500);
        }
        if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[RETRY] Reconnected.");
        currentState = ONLINE;
        } 
        else {
        Serial.println("[RETRY] Still offline. Will retry in 5s.");
        }
      }
      break;
  }
}