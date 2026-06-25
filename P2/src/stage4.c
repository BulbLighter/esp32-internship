#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// ---- WiFi credentials ----
const char* WIFI_SSID = "WIFI_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";

// ---- Server details ----
const char* SERVER_URL = "http://[PC_IP]:5000/readings";

// ---- DHT setup ----
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---- Timing ----
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 10000; // 10 seconds

unsigned long lastWifiCheckTime = 0;
const unsigned long WIFI_CHECK_INTERVAL = 5000; // check + flush every 5s

unsigned int errorCount = 0;     // sensor NaN failures
unsigned int bufferedCount = 0;  // readings that needed buffering
unsigned int droppedCount = 0;   // readings lost to buffer overflow

// ---- Buffer for failed sends ----
struct Reading {
  float temp;
  float hum;
  unsigned long ts;
};

#define BUFFER_SIZE 360 // ~1 hour at 10s interval
Reading buffer[BUFFER_SIZE];
int bufferHead = 0;   // index of oldest entry
int bufferCount = 0;  // number of valid entries currently stored

void bufferAdd(Reading r) {
  if (bufferCount < BUFFER_SIZE) {
    int insertIndex = (bufferHead + bufferCount) % BUFFER_SIZE;
    buffer[insertIndex] = r;
    bufferCount++;
  } else {
    // Buffer full: overwrite oldest entry
    buffer[bufferHead] = r;
    bufferHead = (bufferHead + 1) % BUFFER_SIZE;
    droppedCount++;
  }
  bufferedCount++;
  Serial.print("Buffered reading. Buffer: ");
  Serial.print(bufferCount);
  Serial.print("/");
  Serial.println(BUFFER_SIZE);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    Serial.print(".");
    delay(500);
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
  Serial.println("DHT22 Stage 5 - Buffer + Send on Reconnect");

  dht.begin();
  connectToWiFi();
}

// Attempts to POST one reading. Returns true on success (HTTP 200).
bool sendReading(float temp, float hum, unsigned long ts) {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<128> doc;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["ts"] = ts;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  int httpResponseCode = http.POST(jsonPayload);
  bool success = false;

  if (httpResponseCode == 200) {
    String response = http.getString();
    Serial.print("POST sent. HTTP code: ");
    Serial.print(httpResponseCode);
    Serial.print(" Response: ");
    Serial.println(response);
    success = true;
  } else if (httpResponseCode > 0) {
    Serial.print("POST returned non-200 code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("POST failed. Error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
  return success;
}

// Attempts to drain the buffer, oldest first, while WiFi is connected.
// Stops at the first failure (keeps remaining buffered for next attempt).
void flushBuffer() {
  if (bufferCount == 0) return;
  if (WiFi.status() != WL_CONNECTED) return;

  Serial.print("Attempting to flush buffer (");
  Serial.print(bufferCount);
  Serial.println(" pending)...");

  while (bufferCount > 0) {
    Reading r = buffer[bufferHead];
    bool ok = sendReading(r.temp, r.hum, r.ts);

    if (ok) {
      bufferHead = (bufferHead + 1) % BUFFER_SIZE;
      bufferCount--;
      Serial.print("Flushed buffered reading. Remaining: ");
      Serial.println(bufferCount);
    } else {
      Serial.println("Flush attempt failed, will retry next cycle.");
      break;
    }
  }
}

void loop() {
  unsigned long now = millis();

  // WiFi health check + buffer flush, every 5s
  if (now - lastWifiCheckTime >= WIFI_CHECK_INTERVAL) {
    lastWifiCheckTime = now;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, attempting reconnect...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    } else {
      flushBuffer();
    }
  }

  // Sensor read + send, every 10s
  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;

    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature();

    if (isnan(humidity) || isnan(tempC)) {
      Serial.println("Sensor read failed (NaN) - skipping entirely (not bufferable).");
      errorCount++;
      return;
    }

    Serial.print("Read OK - Temp: ");
    Serial.print(tempC);
    Serial.print("C, Hum: ");
    Serial.print(humidity);
    Serial.println("%");

    bool sent = sendReading(tempC, humidity, now);

    if (!sent) {
      Reading r = { tempC, humidity, now };
      bufferAdd(r);
    }
  }
}


// WiFi connected. IP: 10.212.130.18
// Read OK - Temp: 33.20C, Hum: 61.10%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }
// Read OK - Temp: 33.20C, Hum: 61.10%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }
// Read OK - Temp: 33.30C, Hum: 61.10%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }
// Read OK - Temp: 33.30C, Hum: 61.00%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }
// Read OK - Temp: 33.20C, Hum: 61.10%
// POST sent. HTTP code: 200 Response: {
//   "status": "ok"
// }



//Failure mode (WiFi disconnected)
// ..
// WiFi connected. IP: 10.212.130.18
// Read OK - Temp: 33.90C, Hum: 57.70%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 57.70%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.00%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.00%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// WiFi disconnected, attempting reconnect...
// Read OK - Temp: 33.90C, Hum: 58.30%
// Buffered reading. Buffer: 1/360
// WiFi disconnected, attempting reconnect...
// WiFi disconnected, attempting reconnect...
// Read OK - Temp: 33.90C, Hum: 58.10%
// Buffered reading. Buffer: 2/360
// WiFi disconnected, attempting reconnect...
// Attempting to flush buffer (2 pending)...
// POST failed. Error: connection refused
// Flush attempt failed, will retry next cycle.
// Read OK - Temp: 33.90C, Hum: 58.00%
// POST failed. Error: connection refused
// Buffered reading. Buffer: 3/360
// Attempting to flush buffer (3 pending)...
// POST failed. Error: connection refused
// Flush attempt failed, will retry next cycle.
// Read OK - Temp: 33.90C, Hum: 57.90%
// POST failed. Error: connection refused
// Buffered reading. Buffer: 4/360
// Attempting to flush buffer (4 pending)...
// POST failed. Error: connection refused
// Flush attempt failed, will retry next cycle.
// Read OK - Temp: 33.90C, Hum: 57.90%
// POST failed. Error: connection refused
// Buffered reading. Buffer: 5/360
// Attempting to flush buffer (5 pending)...
// POST failed. Error: connection refused
// Flush attempt failed, will retry next cycle.
// Read OK - Temp: 33.90C, Hum: 57.80%
// POST failed. Error: connection refused
// Buffered reading. Buffer: 6/360
// Attempting to flush buffer (6 pending)...
// POST failed. Error: connection refused
// Flush attempt failed, will retry next cycle.
// Read OK - Temp: 33.80C, Hum: 57.80%
// POST failed. Error: connection refused
// Buffered reading. Buffer: 7/360
// Attempting to flush buffer (7 pending)...
// POST failed. Error: connection refused
// Flush attempt failed, will retry next cycle.
// Read OK - Temp: 33.90C, Hum: 57.80%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Attempting to flush buffer (7 pending)...
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 6
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 5
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 4
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 3
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 2
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 1
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 0
// Read OK - Temp: 33.90C, Hum: 57.90%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.00%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.00%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 57.90%
// POST failed. Error: connection refused
// Buffered reading. Buffer: 1/360
// Attempting to flush buffer (1 pending)...
// POST failed. Error: connection refused
// Flush attempt failed, will retry next cycle.
// Attempting to flush buffer (1 pending)...
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Flushed buffered reading. Remaining: 0
// Read OK - Temp: 33.90C, Hum: 57.90%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.80C, Hum: 58.10%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.00%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.80C, Hum: 58.20%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.10%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.20%
// POST sent. HTTP code: 200 Response: {"status":"ok"}

// Read OK - Temp: 33.90C, Hum: 58.30%
// POST sent. HTTP code: 200 Response: {"status":"ok"}
