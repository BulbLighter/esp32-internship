#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// ---- WiFi credentials ----
const char* WIFI_SSID = "WIFI_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";

// ---- Server details ----
const char* SERVER_URL = "http://[PC_IP]:5000/readings"; // <-- PC's IP

// ---- DHT setup ----
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---- Timing ----
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 10000; // 10 seconds

unsigned long lastWifiCheckTime = 0;
const unsigned long WIFI_CHECK_INTERVAL = 5000; // check connection every 5s

unsigned int errorCount = 0;

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    Serial.print(".");
    delay(500); // acceptable here: one-time blocking during initial connect only
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed (will retry in main loop).");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("DHT22 Stage 4 - WiFi + HTTP POST");

  dht.begin();
  connectToWiFi();
}

void sendReading(float temp, float hum, unsigned long ts) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping POST.");
    errorCount++;
    return;
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  // Build JSON payload
  StaticJsonDocument<128> doc;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["ts"] = ts;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("POST sent. HTTP code: ");
    Serial.print(httpResponseCode);
    Serial.print(" Response: ");
    Serial.println(response);
  } else {
    Serial.print("POST failed. Error: ");
    Serial.println(http.errorToString(httpResponseCode));
    errorCount++;
  }

  http.end();
}

void loop() {
  unsigned long now = millis();

  // Non-blocking WiFi reconnect check
  if (now - lastWifiCheckTime >= WIFI_CHECK_INTERVAL) {
    lastWifiCheckTime = now;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, attempting reconnect...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  }

  // Sensor read + POST every READ_INTERVAL
  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;

    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();

    if (isnan(humidity) || isnan(tempC)) {
      Serial.println("Sensor read failed (NaN) - skipping POST.");
      errorCount++;
      Serial.print("Total error count: ");
      Serial.println(errorCount);
      return;
    }

    Serial.print("Read OK - Temp: ");
    Serial.print(tempC);
    Serial.print("C, Hum: ");
    Serial.print(humidity);
    Serial.println("%");

    sendReading(tempC, humidity, now);
  }
}


// WiFi connected. IP: 10.212.130.18
// Read OK - Temp: 33.60C, Hum: 61.60%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }
// Read OK - Temp: 33.50C, Hum: 61.60%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }
// Read OK - Temp: 33.40C, Hum: 61.70%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }
// Read OK - Temp: 33.40C, Hum: 61.70%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"