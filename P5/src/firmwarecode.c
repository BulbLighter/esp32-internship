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

/*
.
[WIFI] Connected. IP: 10.224.224.18
[SENSOR] T=31.2 C  H=74.2%  Light=33
[POST] OK #1
[SENSOR] T=31.2 C  H=74.3%  Light=33
[POST] OK #2
[SENSOR] T=31.3 C  H=74.3%  Light=82
[POST] OK #3
[SENSOR] T=31.3 C  H=74.2%  Light=68
[POST] OK #4
[SENSOR] T=31.3 C  H=74.2%  Light=78
[POST] OK #5
[SENSOR] T=31.3 C  H=74.3%  Light=213
[POST] OK #6
[SENSOR] T=31.2 C  H=74.3%  Light=270
[POST] OK #7
[SENSOR] T=31.3 C  H=74.2%  Light=70
[POST] OK #8
[SENSOR] T=31.3 C  H=74.2%  Light=72
[POST] OK #9
[SENSOR] T=31.3 C  H=74.3%  Light=75
[POST] OK #10
[SENSOR] T=31.3 C  H=74.3%  Light=78
[POST] OK #11
[SENSOR] T=31.2 C  H=74.3%  Light=69
[POST] OK #12
[SENSOR] T=31.2 C  H=74.5%  Light=83
[POST] OK #13
[SENSOR] T=31.2 C  H=74.5%  Light=81
[POST] OK #14
[SENSOR] T=31.3 C  H=74.4%  Light=78
[POST] OK #15
[SENSOR] T=31.2 C  H=74.5%  Light=71
[POST] OK #16
[SENSOR] T=31.2 C  H=74.5%  Light=12
[POST] OK #17
[SENSOR] T=31.2 C  H=74.6%  Light=16
[POST] OK #18
[SENSOR] T=31.2 C  H=74.5%  Light=66
[POST] OK #19
[SENSOR] T=31.2 C  H=74.5%  Light=37
[POST] OK #20
[SENSOR] T=31.2 C  H=74.4%  Light=14
[POST] OK #21
[SENSOR] T=31.2 C  H=74.5%  Light=32
[POST] OK #22
[SENSOR] T=31.2 C  H=74.5%  Light=10
[POST] OK #23
[SENSOR] T=31.2 C  H=74.4%  Light=3
[POST] OK #24
[SENSOR] T=31.3 C  H=74.5%  Light=2
[POST] OK #25
[SENSOR] T=31.3 C  H=74.5%  Light=12
[POST] OK #26
[SENSOR] T=31.3 C  H=74.6%  Light=76
[POST] OK #27
[SENSOR] T=31.4 C  H=74.4%  Light=79
[POST] OK #28
[SENSOR] T=31.4 C  H=74.4%  Light=77
[POST] OK #29
[SENSOR] T=31.4 C  H=74.4%  Light=7
[POST] OK #30
[SENSOR] T=31.4 C  H=74.3%  Light=8
[POST] OK #31
[SENSOR] T=31.4 C  H=74.4%  Light=3
[POST] OK #32
[SENSOR] T=31.4 C  H=74.3%  Light=22
[POST] OK #33
[SENSOR] T=31.4 C  H=74.3%  Light=67
[POST] OK #34
[SENSOR] T=31.4 C  H=74.4%  Light=225
[POST] OK #35
[SENSOR] T=31.4 C  H=74.5%  Light=275
[POST] OK #36
[SENSOR] T=31.4 C  H=74.3%  Light=285
[POST] OK #37
[SENSOR] T=31.4 C  H=74.4%  Light=275
[POST] OK #38
[SENSOR] T=31.5 C  H=74.4%  Light=267
[POST] OK #39
[SENSOR] T=31.4 C  H=74.4%  Light=80
[POST] OK #40
[SENSOR] T=31.4 C  H=74.4%  Light=80
[POST] OK #41
[SENSOR] T=31.4 C  H=74.5%  Light=74
[POST] OK #42
[SENSOR] T=31.4 C  H=74.5%  Light=81
[POST] OK #43
[SENSOR] T=31.4 C  H=74.6%  Light=72
[POST] OK #44
[SENSOR] T=31.3 C  H=74.7%  Light=77
[POST] OK #45
[SENSOR] T=31.3 C  H=74.7%  Light=76
[POST] OK #46
[SENSOR] T=31.2 C  H=74.8%  Light=77
[POST] OK #47
[SENSOR] T=31.3 C  H=74.8%  Light=76
[POST] OK #48
[SENSOR] T=31.2 C  H=74.8%  Light=77
[POST] OK #49
[SENSOR] T=31.2 C  H=74.9%  Light=81
[POST] OK #50
[SENSOR] T=31.1 C  H=75.0%  Light=78
[POST] OK #51
[SENSOR] T=31.1 C  H=75.1%  Light=76
[POST] OK #52
[SENSOR] T=31.2 C  H=75.0%  Light=73
[POST] OK #53
[SENSOR] T=31.3 C  H=75.1%  Light=77
[POST] OK #54
[SENSOR] T=31.3 C  H=74.8%  Light=82
[POST] OK #55
[SENSOR] T=31.3 C  H=74.8%  Light=79
[POST] OK #56
[SENSOR] T=31.3 C  H=74.7%  Light=75
[POST] OK #57
[SENSOR] T=31.3 C  H=74.7%  Light=75
[POST] OK #58
[SENSOR] T=31.3 C  H=74.8%  Light=75
[POST] OK #59
[SENSOR] T=31.3 C  H=75.1%  Light=73
[POST] OK #60
[SENSOR] T=31.3 C  H=75.0%  Light=74
[POST] OK #61
[SENSOR] T=31.2 C  H=75.0%  Light=81
[POST] OK #62
[SENSOR] T=31.2 C  H=75.1%  Light=74
[POST] OK #63
[SENSOR] T=31.2 C  H=75.2%  Light=77
[POST] OK #64
[SENSOR] T=31.2 C  H=75.1%  Light=77
[POST] OK #65
[SENSOR] T=31.1 C  H=75.3%  Light=73
[POST] OK #66
[SENSOR] T=31.1 C  H=75.4%  Light=81
[POST] OK #67
[SENSOR] T=31.0 C  H=75.5%  Light=77
[POST] OK #68
[SENSOR] T=31.0 C  H=75.6%  Light=71
[POST] OK #69
[SENSOR] T=31.1 C  H=75.6%  Light=76
[POST] OK #70
[SENSOR] T=31.1 C  H=75.5%  Light=76
[POST] OK #71
[SENSOR] T=31.1 C  H=75.6%  Light=68
[POST] OK #72
[SENSOR] T=31.1 C  H=75.5%  Light=74
[POST] OK #73
[SENSOR] T=31.1 C  H=75.5%  Light=74
[POST] OK #74
[SENSOR] T=31.0 C  H=75.5%  Light=77
[POST] OK #75
[SENSOR] T=31.1 C  H=75.6%  Light=81
[POST] OK #76
[SENSOR] T=31.0 C  H=75.7%  Light=78
[POST] OK #77
[SENSOR] T=31.0 C  H=75.6%  Light=73
[POST] OK #78
[SENSOR] T=31.0 C  H=75.5%  Light=73
[POST] OK #79
[SENSOR] T=30.9 C  H=75.6%  Light=71
[POST] OK #80
[SENSOR] T=30.9 C  H=75.7%  Light=70
[POST] OK #81
[SENSOR] T=30.8 C  H=75.8%  Light=81
[POST] OK #82
[SENSOR] T=30.7 C  H=75.7%  Light=76
[POST] OK #83
[SENSOR] T=30.7 C  H=75.9%  Light=78
[POST] OK #84
[SENSOR] T=30.8 C  H=76.1%  Light=75
[POST] OK #85
[SENSOR] T=30.8 C  H=75.9%  Light=45
[POST] OK #86
[SENSOR] T=30.9 C  H=75.8%  Light=54
[POST] OK #87
[SENSOR] T=30.9 C  H=75.6%  Light=78
[POST] OK #88
[SENSOR] T=30.9 C  H=75.8%  Light=75
[POST] OK #89
[SENSOR] T=31.0 C  H=75.5%  Light=73
[POST] OK #90
[SENSOR] T=31.0 C  H=75.5%  Light=83
[POST] OK #91
[SENSOR] T=30.9 C  H=75.4%  Light=80
[POST] OK #92
[SENSOR] T=30.9 C  H=75.4%  Light=76
[POST] OK #93
[SENSOR] T=30.9 C  H=75.4%  Light=254
[POST] OK #94
[SENSOR] T=30.9 C  H=75.6%  Light=243
[POST] OK #95
[SENSOR] T=30.9 C  H=75.5%  Light=244
[POST] OK #96
[SENSOR] T=30.8 C  H=75.2%  Light=242
[POST] OK #97
[SENSOR] T=30.9 C  H=75.4%  Light=241
[POST] OK #98
[SENSOR] T=31.0 C  H=75.4%  Light=243
[POST] OK #99
[SENSOR] T=31.0 C  H=75.3%  Light=235
[POST] OK #100
[SENSOR] T=31.0 C  H=75.2%  Light=241
[POST] OK #101
[SENSOR] T=31.0 C  H=75.1%  Light=229
[POST] OK #102
[SENSOR] T=31.0 C  H=75.1%  Light=64
[POST] OK #103
[SENSOR] T=31.0 C  H=75.0%  Light=63
[POST] OK #104
[SENSOR] T=30.9 C  H=74.9%  Light=77
[POST] OK #105
[SENSOR] T=31.0 C  H=75.1%  Light=73
[POST] OK #106
[SENSOR] T=30.9 C  H=75.1%  Light=73
[POST] OK #107
[SENSOR] T=30.9 C  H=75.1%  Light=70
[POST] OK #108
[SENSOR] T=30.9 C  H=74.9%  Light=71
[POST] OK #109
[SENSOR] T=30.9 C  H=74.9%  Light=68
[POST] OK #110
[SENSOR] T=30.9 C  H=75.1%  Light=66
[POST] OK #111
[SENSOR] T=30.9 C  H=75.0%  Light=6
[POST] OK #112
[SENSOR] T=30.9 C  H=75.2%  Light=74
[POST] OK #113
[SENSOR] T=30.9 C  H=75.2%  Light=68
[POST] OK #114
[SENSOR] T=30.9 C  H=74.9%  Light=73
[POST] OK #115
[SENSOR] T=30.9 C  H=75.1%  Light=68
[POST] OK #116
[SENSOR] T=30.9 C  H=75.0%  Light=80
[POST] OK #117
[SENSOR] T=30.9 C  H=74.9%  Light=68
[POST] OK #118
[SENSOR] T=30.9 C  H=74.8%  Light=68
[POST] OK #119
[SENSOR] T=30.8 C  H=74.8%  Light=66
[POST] OK #120
[SENSOR] T=30.9 C  H=75.0%  Light=73
[POST] OK #121
[SENSOR] T=30.8 C  H=74.9%  Light=71
[POST] OK #122
[SENSOR] T=30.9 C  H=74.9%  Light=72
[POST] OK #123
[SENSOR] T=30.9 C  H=74.7%  Light=74
[POST] OK #124
[SENSOR] T=30.8 C  H=74.8%  Light=30
[POST] OK #125
[SENSOR] T=30.8 C  H=74.8%  Light=72
[POST] OK #126
[SENSOR] T=30.9 C  H=74.8%  Light=76
[POST] OK #127
[SENSOR] T=30.8 C  H=74.8%  Light=72
[POST] OK #128
[SENSOR] T=30.9 C  H=74.8%  Light=76
[POST] OK #129
[SENSOR] T=30.9 C  H=74.6%  Light=75
[POST] OK #130
[SENSOR] T=30.8 C  H=74.5%  Light=78
[POST] OK #131
[SENSOR] T=30.8 C  H=74.7%  Light=72
[POST] OK #132
[SENSOR] T=30.8 C  H=74.6%  Light=78
[POST] OK #133
[SENSOR] T=30.8 C  H=74.6%  Light=75
[POST] OK #134
[SENSOR] T=30.8 C  H=74.7%  Light=75
[POST] OK #135
[SENSOR] T=30.8 C  H=74.7%  Light=76
[POST] OK #136
[SENSOR] T=30.8 C  H=74.7%  Light=72
[POST] OK #137
[SENSOR] T=30.7 C  H=74.7%  Light=70
[POST] OK #138
[SENSOR] T=30.8 C  H=74.7%  Light=74
[POST] OK #139
[SENSOR] T=30.7 C  H=74.8%  Light=72
[POST] OK #140
[SENSOR] T=30.7 C  H=74.8%  Light=75
[POST] OK #141
[SENSOR] T=30.7 C  H=74.9%  Light=74
[POST] OK #142
[SENSOR] T=30.7 C  H=74.7%  Light=74
[POST] OK #143
[SENSOR] T=30.7 C  H=74.7%  Light=72
[POST] OK #144
[SENSOR] T=30.8 C  H=74.6%  Light=75
[POST] OK #145
[SENSOR] T=30.7 C  H=74.6%  Light=75
[POST] OK #146
[SENSOR] T=30.8 C  H=74.6%  Light=73
[POST] OK #147
[SENSOR] T=30.8 C  H=74.7%  Light=72
[POST] OK #148
[SENSOR] T=30.7 C  H=74.7%  Light=66
[POST] OK #149
[SENSOR] T=30.7 C  H=74.6%  Light=77
[POST] OK #150
[SENSOR] T=30.7 C  H=74.7%  Light=66
[POST] OK #151
[SENSOR] T=30.7 C  H=74.7%  Light=72
[POST] OK #152
[SENSOR] T=30.7 C  H=74.7%  Light=71
[POST] OK #153
[SENSOR] T=30.7 C  H=74.7%  Light=75
[POST] OK #154
[SENSOR] T=30.7 C  H=74.7%  Light=73
[POST] OK #155
[SENSOR] T=30.6 C  H=74.8%  Light=75
[POST] OK #156
[SENSOR] T=30.6 C  H=74.9%  Light=74
[POST] OK #157
[SENSOR] T=30.6 C  H=75.0%  Light=69
[POST] OK #158
[SENSOR] T=30.6 C  H=75.0%  Light=76
[POST] OK #159
[SENSOR] T=30.6 C  H=75.0%  Light=74
[POST] OK #160
[SENSOR] T=30.7 C  H=74.8%  Light=72
[POST] OK #161
[SENSOR] T=30.6 C  H=75.0%  Light=69
[POST] OK #162
[SENSOR] T=30.6 C  H=75.0%  Light=73
[POST] OK #163
[SENSOR] T=30.6 C  H=75.1%  Light=74
[POST] OK #164
[SENSOR] T=30.6 C  H=75.1%  Light=80
[POST] OK #165
[SENSOR] T=30.6 C  H=75.2%  Light=80
[POST] OK #166
[SENSOR] T=30.6 C  H=75.1%  Light=77
[POST] OK #167
[SENSOR] T=30.6 C  H=75.3%  Light=71
[POST] OK #168
[SENSOR] T=30.6 C  H=75.2%  Light=72
[POST] OK #169
[SENSOR] T=30.6 C  H=75.2%  Light=79
[POST] OK #170
[SENSOR] T=30.6 C  H=75.2%  Light=70
[POST] OK #171
[SENSOR] T=30.6 C  H=75.2%  Light=78
[POST] OK #172
[SENSOR] T=30.6 C  H=75.2%  Light=71
[POST] OK #173
[SENSOR] T=30.7 C  H=75.1%  Light=70
[POST] OK #174
[SENSOR] T=30.7 C  H=75.0%  Light=71
[POST] OK #175
[SENSOR] T=30.7 C  H=74.9%  Light=71
[POST] OK #176
[SENSOR] T=30.7 C  H=74.9%  Light=76
[POST] OK #177
[SENSOR] T=30.7 C  H=75.0%  Light=70
[POST] OK #178
[SENSOR] T=30.7 C  H=75.0%  Light=76
[POST] OK #179
[SENSOR] T=30.7 C  H=74.9%  Light=76
[POST] OK #180
[SENSOR] T=30.7 C  H=74.9%  Light=66
[POST] OK #181
[SENSOR] T=30.7 C  H=75.1%  Light=74
[POST] OK #182
[SENSOR] T=30.8 C  H=74.9%  Light=70
[POST] OK #183
[SENSOR] T=30.8 C  H=74.8%  Light=73
[POST] OK #184
[SENSOR] T=30.8 C  H=74.7%  Light=67
[POST] OK #185
[SENSOR] T=30.9 C  H=74.7%  Light=71
[POST] OK #186
[SENSOR] T=30.8 C  H=74.8%  Light=77
[POST] OK #187
[SENSOR] T=30.8 C  H=74.8%  Light=73
[POST] OK #188
[SENSOR] T=30.8 C  H=74.8%  Light=74
[POST] OK #189
[SENSOR] T=30.7 C  H=75.0%  Light=74
[POST] OK #190
[SENSOR] T=30.7 C  H=75.0%  Light=73
[POST] OK #191
[SENSOR] T=30.7 C  H=75.1%  Light=76
[POST] OK #192
[SENSOR] T=30.7 C  H=75.1%  Light=74
[POST] OK #193
[SENSOR] T=30.7 C  H=75.1%  Light=73
[POST] OK #194
[SENSOR] T=30.7 C  H=75.1%  Light=74
[POST] OK #195
[SENSOR] T=30.7 C  H=75.1%  Light=68
[POST] OK #196
[SENSOR] T=30.7 C  H=75.2%  Light=75
[POST] OK #197
[SENSOR] T=30.6 C  H=75.3%  Light=75
[POST] OK #198
[SENSOR] T=30.6 C  H=75.3%  Light=75
[POST] OK #199
[SENSOR] T=30.6 C  H=75.4%  Light=73
[POST] OK #200
[SENSOR] T=30.6 C  H=75.5%  Light=77
[POST] OK #201
[SENSOR] T=30.5 C  H=75.4%  Light=68
[POST] OK #202
[SENSOR] T=30.5 C  H=75.5%  Light=72
[POST] OK #203
[SENSOR] T=30.5 C  H=75.5%  Light=75
[POST] OK #204
[SENSOR] T=30.5 C  H=75.5%  Light=76
[POST] OK #205
[SENSOR] T=30.5 C  H=75.5%  Light=71
[POST] OK #206
[SENSOR] T=30.5 C  H=75.4%  Light=76
[POST] OK #207
[SENSOR] T=30.5 C  H=75.5%  Light=59
[POST] OK #208
[SENSOR] T=30.5 C  H=75.5%  Light=61
[POST] OK #209
[SENSOR] T=30.4 C  H=75.5%  Light=63
[POST] FAIL #1
[RETRY] Attempting reconnect...
E (3150767) wifi:sta is connecting, return error
[RETRY] Reconnected.
[SENSOR] T=30.4 C  H=75.6%  Light=62
[POST] OK #210
[SENSOR] T=30.4 C  H=75.5%  Light=60
[POST] OK #211
[SENSOR] T=30.5 C  H=75.6%  Light=73
[POST] OK #212
[SENSOR] T=30.4 C  H=75.4%  Light=76
[POST] OK #213
[SENSOR] T=30.4 C  H=75.4%  Light=63
[POST] OK #214
[SENSOR] T=30.4 C  H=75.5%  Light=78
[POST] OK #215
[SENSOR] T=30.4 C  H=75.5%  Light=77
[POST] OK #216
[SENSOR] T=30.4 C  H=75.5%  Light=75
[POST] OK #217
[SENSOR] T=30.4 C  H=75.5%  Light=64
[POST] OK #218
[SENSOR] T=30.4 C  H=75.5%  Light=74
[POST] OK #219
[SENSOR] T=30.4 C  H=75.5%  Light=72
[POST] OK #220
[SENSOR] T=30.4 C  H=75.4%  Light=79
[POST] OK #221
[SENSOR] T=30.3 C  H=75.3%  Light=78
[POST] OK #222
[SENSOR] T=30.3 C  H=75.2%  Light=77
[POST] OK #223
[SENSOR] T=30.3 C  H=75.3%  Light=78
[POST] OK #224
[SENSOR] T=30.3 C  H=75.3%  Light=77
[POST] OK #225
[SENSOR] T=30.3 C  H=75.3%  Light=76
[POST] OK #226
[SENSOR] T=30.2 C  H=75.2%  Light=79
[POST] OK #227
[SENSOR] T=30.2 C  H=75.3%  Light=74
[POST] OK #228
[SENSOR] T=30.2 C  H=75.4%  Light=76
[POST] OK #229
[SENSOR] T=30.2 C  H=75.4%  Light=75
[POST] OK #230
[SENSOR] T=30.2 C  H=75.4%  Light=74
[POST] OK #231
[SENSOR] T=30.2 C  H=75.4%  Light=75
[POST] OK #232
[SENSOR] T=30.2 C  H=75.5%  Light=75
[POST] OK #233
[SENSOR] T=30.2 C  H=75.5%  Light=80
[POST] OK #234
[SENSOR] T=30.2 C  H=75.5%  Light=74
[POST] OK #235
[SENSOR] T=30.1 C  H=75.4%  Light=77
[POST] OK #236
[SENSOR] T=30.1 C  H=75.4%  Light=74
[POST] OK #237
[SENSOR] T=30.1 C  H=75.5%  Light=69
[POST] OK #238
[SENSOR] T=30.1 C  H=75.5%  Light=74
[POST] OK #239
[SENSOR] T=30.1 C  H=75.5%  Light=76
[POST] OK #240
[SENSOR] T=30.1 C  H=75.4%  Light=76
[POST] OK #241
[SENSOR] T=30.1 C  H=75.4%  Light=75
[POST] OK #242
[SENSOR] T=30.1 C  H=75.5%  Light=79
[POST] OK #243
[SENSOR] T=30.1 C  H=75.4%  Light=74
[POST] OK #244
[SENSOR] T=30.1 C  H=75.5%  Light=70
[POST] OK #245
[SENSOR] T=30.1 C  H=75.5%  Light=76
[POST] OK #246
[SENSOR] T=30.1 C  H=75.6%  Light=63
[POST] OK #247
[SENSOR] T=30.1 C  H=75.5%  Light=79
[POST] OK #248
[SENSOR] T=30.1 C  H=75.6%  Light=77
[POST] OK #249
[SENSOR] T=30.1 C  H=75.5%  Light=76
[POST] OK #250
[SENSOR] T=30.1 C  H=75.4%  Light=68
[POST] OK #251
[SENSOR] T=30.1 C  H=75.5%  Light=78
[POST] OK #252
[SENSOR] T=30.1 C  H=75.7%  Light=72
[POST] OK #253
[SENSOR] T=30.1 C  H=75.6%  Light=73
[POST] OK #254
[SENSOR] T=30.1 C  H=75.6%  Light=77
[POST] OK #255
[SENSOR] T=30.1 C  H=75.6%  Light=87
[POST] OK #256
[SENSOR] T=30.1 C  H=75.7%  Light=50
[POST] OK #257
[SENSOR] T=30.1 C  H=75.6%  Light=72
[POST] OK #258
[SENSOR] T=30.1 C  H=75.7%  Light=71
[POST] OK #259
[SENSOR] T=30.1 C  H=75.7%  Light=76
[POST] OK #260
[SENSOR] T=30.1 C  H=75.8%  Light=87
[POST] OK #261
[SENSOR] T=30.1 C  H=75.7%  Light=78
[POST] OK #262
[SENSOR] T=30.1 C  H=75.7%  Light=72
[POST] OK #263
[SENSOR] T=30.1 C  H=75.7%  Light=76
[POST] OK #264
[SENSOR] T=30.1 C  H=75.7%  Light=74
[POST] OK #265
[SENSOR] T=30.1 C  H=75.7%  Light=72
[POST] OK #266
[SENSOR] T=30.1 C  H=75.7%  Light=74
[POST] OK #267
[SENSOR] T=30.1 C  H=75.6%  Light=78
[POST] OK #268
[SENSOR] T=30.1 C  H=75.5%  Light=73
[POST] OK #269
[SENSOR] T=30.1 C  H=75.6%  Light=76
[POST] OK #270
[SENSOR] T=30.1 C  H=75.6%  Light=74
[POST] OK #271
[SENSOR] T=30.1 C  H=75.5%  Light=78
[POST] OK #272
[SENSOR] T=30.1 C  H=75.5%  Light=72
[POST] OK #273
[SENSOR] T=30.0 C  H=75.5%  Light=233
[POST] OK #274
[SENSOR] T=30.0 C  H=74.8%  Light=205
[POST] OK #275
[SENSOR] T=29.9 C  H=75.0%  Light=208
[POST] OK #276
[SENSOR] T=29.9 C  H=75.0%  Light=213
[POST] OK #277
[SENSOR] T=30.0 C  H=75.0%  Light=204
[POST] OK #278
[SENSOR] T=30.0 C  H=75.1%  Light=197
[POST] OK #279
[SENSOR] T=30.0 C  H=75.4%  Light=196
[POST] OK #280
[SENSOR] T=30.0 C  H=74.7%  Light=206
[POST] OK #281
[SENSOR] T=30.0 C  H=74.7%  Light=206
[POST] OK #282
[SENSOR] T=30.0 C  H=74.9%  Light=198
[POST] OK #283
[SENSOR] T=30.0 C  H=74.9%  Light=204
[POST] OK #284
[SENSOR] T=29.9 C  H=75.0%  Light=202
[POST] OK #285
[SENSOR] T=29.9 C  H=75.1%  Light=201
[POST] OK #286
[SENSOR] T=29.9 C  H=75.1%  Light=198
[POST] OK #287
[SENSOR] T=29.9 C  H=75.2%  Light=197
[POST] OK #288
[SENSOR] T=29.9 C  H=75.2%  Light=201
[POST] OK #289
[SENSOR] T=30.0 C  H=75.2%  Light=198
[POST] OK #290
[SENSOR] T=30.0 C  H=75.2%  Light=195
[POST] OK #291
[SENSOR] T=30.0 C  H=75.2%  Light=190
[POST] OK #292
[SENSOR] T=30.0 C  H=75.2%  Light=200
[POST] OK #293
[SENSOR] T=30.0 C  H=75.1%  Light=204
[POST] OK #294
[SENSOR] T=30.0 C  H=75.7%  Light=198
[POST] OK #295
[SENSOR] T=30.1 C  H=75.5%  Light=190
[POST] OK #296
[SENSOR] T=30.1 C  H=75.4%  Light=198
[POST] OK #297
[SENSOR] T=30.1 C  H=75.4%  Light=194
[POST] OK #298
[SENSOR] T=30.0 C  H=75.5%  Light=201
[POST] OK #299
[SENSOR] T=30.1 C  H=75.6%  Light=193
[POST] OK #300
[SENSOR] T=30.1 C  H=75.6%  Light=196
[POST] OK #301
[SENSOR] T=30.1 C  H=75.5%  Light=197
[POST] OK #302
[SENSOR] T=30.1 C  H=75.6%  Light=196
[POST] OK #303
[SENSOR] T=30.1 C  H=75.6%  Light=194
[POST] OK #304
[SENSOR] T=30.1 C  H=75.6%  Light=184
[POST] OK #305
[SENSOR] T=30.1 C  H=75.5%  Light=190
[POST] OK #306
[SENSOR] T=30.2 C  H=75.6%  Light=211
[POST] OK #307
[SENSOR] T=30.1 C  H=75.4%  Light=193
[POST] OK #308
[SENSOR] T=30.1 C  H=75.4%  Light=195
[POST] OK #309
[SENSOR] T=30.1 C  H=75.4%  Light=189
[POST] OK #310
[SENSOR] T=30.1 C  H=75.5%  Light=195
[POST] OK #311
[SENSOR] T=30.1 C  H=75.4%  Light=188
[POST] OK #312
[SENSOR] T=30.1 C  H=75.4%  Light=191
[POST] OK #313
[SENSOR] T=30.1 C  H=75.4%  Light=205
[POST] OK #314
[SENSOR] T=30.1 C  H=75.4%  Light=194
[POST] OK #315
[SENSOR] T=30.1 C  H=75.5%  Light=188
[POST] OK #316
[SENSOR] T=30.1 C  H=75.4%  Light=194
[POST] OK #317
[SENSOR] T=30.1 C  H=75.4%  Light=183
[POST] OK #318
[SENSOR] T=30.1 C  H=75.4%  Light=196
[POST] OK #319
[SENSOR] T=30.1 C  H=75.5%  Light=196
[POST] OK #320
[SENSOR] T=30.0 C  H=75.5%  Light=194
[POST] OK #321
[SENSOR] T=30.0 C  H=75.5%  Light=197
[POST] OK #322
[SENSOR] T=30.0 C  H=75.0%  Light=200
[POST] OK #323
[SENSOR] T=30.0 C  H=75.5%  Light=194
[POST] OK #324
[SENSOR] T=30.0 C  H=74.9%  Light=179
[POST] OK #325
[SENSOR] T=30.0 C  H=74.9%  Light=197
[POST] OK #326
[SENSOR] T=30.0 C  H=75.0%  Light=175
[POST] OK #327
[SENSOR] T=29.9 C  H=75.0%  Light=193
[POST] OK #328
[SENSOR] T=30.0 C  H=75.0%  Light=180
[POST] OK #329
[SENSOR] T=30.0 C  H=74.9%  Light=194
[POST] OK #330
[SENSOR] T=30.0 C  H=74.9%  Light=194
[POST] OK #331
[SENSOR] T=30.0 C  H=74.9%  Light=191
[POST] OK #332
[SENSOR] T=30.0 C  H=74.9%  Light=195
[POST] OK #333
[SENSOR] T=30.0 C  H=74.9%  Light=188
[POST] OK #334
[SENSOR] T=30.0 C  H=74.9%  Light=201
[POST] OK #335
[SENSOR] T=30.0 C  H=74.8%  Light=194
[POST] OK #336
[SENSOR] T=30.0 C  H=74.8%  Light=192
[POST] OK #337
[SENSOR] T=30.0 C  H=74.9%  Light=187
[POST] OK #338
[SENSOR] T=30.0 C  H=74.9%  Light=190
[POST] OK #339
[SENSOR] T=30.0 C  H=74.9%  Light=196
[POST] OK #340
[SENSOR] T=29.9 C  H=74.9%  Light=194
[POST] OK #341
[SENSOR] T=30.0 C  H=74.9%  Light=176
[POST] OK #342
[SENSOR] T=30.0 C  H=74.9%  Light=189
[POST] OK #343
[SENSOR] T=30.0 C  H=74.8%  Light=198
[POST] OK #344
[SENSOR] T=30.0 C  H=74.8%  Light=199
[POST] OK #345
[SENSOR] T=30.0 C  H=74.8%  Light=195
[POST] OK #346
[SENSOR] T=30.0 C  H=74.8%  Light=195
[POST] OK #347
[SENSOR] T=30.1 C  H=75.3%  Light=197
[POST] OK #348
[SENSOR] T=30.1 C  H=75.3%  Light=201
[POST] OK #349
[SENSOR] T=30.1 C  H=75.2%  Light=190
[POST] OK #350
[SENSOR] T=30.1 C  H=75.2%  Light=186
[POST] OK #351
[SENSOR] T=30.1 C  H=75.2%  Light=194
[POST] OK #352
[SENSOR] T=30.1 C  H=75.2%  Light=195
[POST] OK #353
[SENSOR] T=30.1 C  H=75.2%  Light=194
[POST] OK #354
[SENSOR] T=30.2 C  H=75.1%  Light=196
[POST] OK #355
[SENSOR] T=30.1 C  H=75.1%  Light=195
[POST] OK #356
[SENSOR] T=30.2 C  H=75.1%  Light=178
[POST] OK #357
[SENSOR] T=30.2 C  H=75.0%  Light=179
[POST] OK #358
[SENSOR] T=30.3 C  H=75.0%  Light=194
[POST] OK #359
[SENSOR] T=30.3 C  H=74.8%  Light=194
[POST] OK #360
[SENSOR] T=30.3 C  H=74.9%  Light=193
[POST] OK #361
[SENSOR] T=30.3 C  H=74.8%  Light=191
[POST] OK #362
[SENSOR] T=30.4 C  H=74.8%  Light=193
[POST] OK #363
[SENSOR] T=30.4 C  H=74.6%  Light=192
[POST] OK #364
[SENSOR] T=30.5 C  H=74.5%  Light=189
[POST] OK #365
[SENSOR] T=30.5 C  H=74.4%  Light=188
[POST] OK #366
[SENSOR] T=30.6 C  H=74.3%  Light=192
[POST] OK #367
[SENSOR] T=30.6 C  H=74.2%  Light=189
[POST] OK #368
[SENSOR] T=30.7 C  H=74.2%  Light=192
[POST] OK #369
[SENSOR] T=30.6 C  H=74.3%  Light=190
[POST] OK #370
[SENSOR] T=30.6 C  H=74.4%  Light=193
[POST] OK #371
[SENSOR] T=30.6 C  H=74.5%  Light=188
[POST] OK #372
[SENSOR] T=30.6 C  H=74.5%  Light=187
[POST] OK #373
[SENSOR] T=30.6 C  H=74.5%  Light=190
[POST] OK #374
[SENSOR] T=30.6 C  H=74.6%  Light=192
[POST] OK #375
[SENSOR] T=30.6 C  H=74.6%  Light=184
[POST] OK #376
[SENSOR] T=30.7 C  H=74.5%  Light=192
[POST] OK #377
[SENSOR] T=30.7 C  H=74.5%  Light=190
[POST] OK #378
[SENSOR] T=30.7 C  H=74.5%  Light=193
[POST] OK #379
[SENSOR] T=30.6 C  H=74.7%  Light=193
[POST] OK #380
[SENSOR] T=30.6 C  H=74.8%  Light=265
[POST] OK #381
[SENSOR] T=30.7 C  H=74.9%  Light=154
[POST] OK #382
[SENSOR] T=30.7 C  H=74.8%  Light=188
[POST] OK #383
[SENSOR] T=30.8 C  H=74.7%  Light=193
[POST] OK #384
[SENSOR] T=30.7 C  H=74.6%  Light=188
[POST] OK #385
[SENSOR] T=30.7 C  H=74.7%  Light=185
[POST] OK #386
[SENSOR] T=30.7 C  H=74.7%  Light=190
[POST] OK #387
[SENSOR] T=30.7 C  H=74.7%  Light=162
[POST] OK #388
[SENSOR] T=30.7 C  H=74.8%  Light=193
[POST] OK #389
[SENSOR] T=30.7 C  H=74.8%  Light=191
[POST] OK #390
[SENSOR] T=30.7 C  H=74.8%  Light=130
[POST] OK #391
[SENSOR] T=30.7 C  H=74.8%  Light=130
[POST] OK #392
[SENSOR] T=30.7 C  H=74.8%  Light=143
[POST] OK #393
[SENSOR] T=30.8 C  H=74.7%  Light=190
[POST] OK #394
[SENSOR] T=30.7 C  H=74.7%  Light=74
[POST] OK #395
[SENSOR] T=30.8 C  H=74.6%  Light=61
[POST] OK #396
[SENSOR] T=30.8 C  H=74.7%  Light=75
[POST] OK #397
[SENSOR] T=30.8 C  H=74.7%  Light=63
[POST] OK #398
[SENSOR] T=30.8 C  H=74.7%  Light=74
[POST] OK #399
[SENSOR] T=30.8 C  H=74.6%  Light=78
[POST] OK #400
[SENSOR] T=30.8 C  H=74.7%  Light=80
[POST] OK #401
[SENSOR] T=30.8 C  H=74.6%  Light=76
[POST] OK #402
[SENSOR] T=30.9 C  H=74.5%  Light=71
[POST] OK #403
[SENSOR] T=30.9 C  H=74.6%  Light=81
[POST] OK #404
[SENSOR] T=30.9 C  H=74.5%  Light=76
[POST] OK #405
[SENSOR] T=30.9 C  H=74.5%  Light=75
[POST] OK #406
[SENSOR] T=30.9 C  H=74.5%  Light=76
[POST] OK #407
[SENSOR] T=30.9 C  H=74.5%  Light=79
[POST] OK #408
[SENSOR] T=30.9 C  H=74.4%  Light=74
[POST] OK #409
[SENSOR] T=31.0 C  H=74.4%  Light=69
[POST] OK #410
[SENSOR] T=31.0 C  H=74.4%  Light=74
[POST] OK #411
[SENSOR] T=31.0 C  H=74.3%  Light=84
[POST] OK #412
[SENSOR] T=31.0 C  H=74.3%  Light=78
[POST] OK #413
[SENSOR] T=31.0 C  H=74.3%  Light=76
[POST] OK #414
[SENSOR] T=31.0 C  H=74.3%  Light=72
[POST] OK #415
[SENSOR] T=31.0 C  H=74.3%  Light=67
[POST] OK #416
[SENSOR] T=31.0 C  H=74.4%  Light=71
[POST] OK #417
[SENSOR] T=31.0 C  H=74.3%  Light=69
[POST] OK #418
[SENSOR] T=31.1 C  H=74.3%  Light=76
[POST] OK #419
[SENSOR] T=31.0 C  H=74.3%  Light=78
[POST] OK #420
[SENSOR] T=31.1 C  H=74.3%  Light=74
[POST] OK #421
[SENSOR] T=31.1 C  H=74.3%  Light=61
[POST] OK #422
[SENSOR] T=31.1 C  H=74.2%  Light=72
[POST] OK #423
[SENSOR] T=31.1 C  H=74.1%  Light=62
[POST] OK #424
[SENSOR] T=31.2 C  H=74.1%  Light=74
[POST] OK #425
[SENSOR] T=31.2 C  H=74.0%  Light=78
[POST] OK #426
[SENSOR] T=31.2 C  H=74.0%  Light=72
[POST] OK #427
[SENSOR] T=31.2 C  H=74.0%  Light=78
[POST] OK #428
[SENSOR] T=31.2 C  H=74.0%  Light=74
[POST] OK #429
[SENSOR] T=31.2 C  H=73.9%  Light=78
[POST] OK #430
[SENSOR] T=31.2 C  H=73.9%  Light=78
[POST] OK #431
[SENSOR] T=31.2 C  H=73.9%  Light=74
[POST] OK #432
[SENSOR] T=31.2 C  H=73.9%  Light=80
[POST] OK #433
[SENSOR] T=31.2 C  H=73.9%  Light=76
[POST] OK #434
[SENSOR] T=31.3 C  H=73.9%  Light=76
[POST] OK #435
[SENSOR] T=31.3 C  H=73.8%  Light=75
[POST] OK #436
[SENSOR] T=31.3 C  H=73.8%  Light=76
[POST] OK #437
[SENSOR] T=31.3 C  H=73.8%  Light=81
[POST] OK #438
[SENSOR] T=31.3 C  H=73.8%  Light=76
[POST] OK #439
[SENSOR] T=31.3 C  H=73.7%  Light=77
[POST] OK #440
[SENSOR] T=31.3 C  H=73.7%  Light=0
[POST] OK #441
[SENSOR] T=31.3 C  H=73.7%  Light=73
[POST] OK #442
[SENSOR] T=31.3 C  H=73.7%  Light=75
[POST] OK #443
[SENSOR] T=31.4 C  H=73.8%  Light=85
[POST] OK #444
[SENSOR] T=31.3 C  H=73.7%  Light=60
[POST] OK #445
[SENSOR] T=31.4 C  H=73.7%  Light=75
[POST] OK #446
[SENSOR] T=31.4 C  H=73.7%  Light=77
[POST] OK #447
[SENSOR] T=31.4 C  H=73.6%  Light=76
[POST] OK #448
[SENSOR] T=31.4 C  H=73.7%  Light=70
[POST] OK #449
[SENSOR] T=31.4 C  H=73.7%  Light=78
[POST] OK #450
[SENSOR] T=31.3 C  H=73.7%  Light=57
[POST] OK #451
[SENSOR] T=31.3 C  H=73.7%  Light=74
[POST] OK #452
[SENSOR] T=31.4 C  H=73.8%  Light=79
[POST] OK #453
[SENSOR] T=31.4 C  H=73.7%  Light=77
[POST] OK #454
[SENSOR] T=31.4 C  H=73.7%  Light=72
[POST] OK #455
[SENSOR] T=31.4 C  H=73.7%  Light=64
[POST] OK #456
[SENSOR] T=31.4 C  H=73.7%  Light=76
[POST] OK #457
[SENSOR] T=31.4 C  H=73.6%  Light=77
[POST] OK #458
[SENSOR] T=31.4 C  H=73.7%  Light=69
[POST] OK #459
[SENSOR] T=31.4 C  H=73.7%  Light=74
[POST] OK #460
[SENSOR] T=31.4 C  H=73.6%  Light=79
[POST] OK #461
[SENSOR] T=31.4 C  H=73.7%  Light=77
[POST] OK #462
[SENSOR] T=31.4 C  H=73.7%  Light=74
[POST] OK #463
[SENSOR] T=31.4 C  H=73.7%  Light=78
[POST] OK #464
[SENSOR] T=31.4 C  H=73.6%  Light=81
[POST] OK #465
[SENSOR] T=31.4 C  H=73.7%  Light=84
[POST] OK #466
[SENSOR] T=31.4 C  H=73.7%  Light=75
[POST] OK #467
[SENSOR] T=31.4 C  H=73.7%  Light=78
[POST] OK #468
[SENSOR] T=31.4 C  H=73.8%  Light=79
[POST] OK #469
[SENSOR] T=31.4 C  H=73.7%  Light=4
[POST] OK #470
[SENSOR] T=31.4 C  H=73.8%  Light=6
[POST] OK #471
[SENSOR] T=31.4 C  H=73.8%  Light=74
[POST] OK #472
[SENSOR] T=31.4 C  H=73.8%  Light=68
[POST] OK #473
[SENSOR] T=31.4 C  H=73.7%  Light=80
[POST] OK #474
[SENSOR] T=31.4 C  H=73.7%  Light=76
[POST] OK #475
[SENSOR] T=31.4 C  H=73.7%  Light=75
[POST] OK #476
[SENSOR] T=31.4 C  H=73.7%  Light=79
[POST] OK #477
[SENSOR] T=31.4 C  H=73.6%  Light=77
[POST] OK #478
[SENSOR] T=31.4 C  H=73.7%  Light=79
[POST] OK #479
[SENSOR] T=31.4 C  H=73.6%  Light=77
[POST] OK #480
[SENSOR] T=31.4 C  H=73.6%  Light=73
[POST] OK #481
[SENSOR] T=31.4 C  H=73.6%  Light=75
[POST] OK #482
[SENSOR] T=31.4 C  H=73.6%  Light=78
[POST] OK #483
[SENSOR] T=31.4 C  H=73.6%  Light=71
[POST] OK #484
[SENSOR] T=31.4 C  H=73.7%  Light=69
[POST] OK #485
[SENSOR] T=31.4 C  H=73.7%  Light=74
[POST] OK #486
[SENSOR] T=31.4 C  H=73.8%  Light=77
[POST] OK #487
[SENSOR] T=31.4 C  H=73.8%  Light=84
[POST] OK #488
[SENSOR] T=31.4 C  H=73.7%  Light=73
[POST] OK #489
[SENSOR] T=31.4 C  H=73.7%  Light=76
[POST] OK #490
[SENSOR] T=31.4 C  H=73.8%  Light=81
[POST] OK #491
[SENSOR] T=31.4 C  H=73.7%  Light=76
[POST] OK #492
[SENSOR] T=31.4 C  H=73.6%  Light=76
[POST] OK #493
[SENSOR] T=31.4 C  H=73.7%  Light=79
[POST] OK #494
[SENSOR] T=31.4 C  H=73.7%  Light=74
[POST] OK #495
[SENSOR] T=31.4 C  H=73.7%  Light=82
[POST] OK #496
[SENSOR] T=31.4 C  H=73.7%  Light=76
[POST] OK #497
[SENSOR] T=31.5 C  H=73.7%  Light=83
[POST] OK #498
[SENSOR] T=31.4 C  H=73.7%  Light=84
[POST] OK #499
[SENSOR] T=31.4 C  H=73.7%  Light=78
[POST] OK #500
[SENSOR] T=31.4 C  H=73.6%  Light=59
[POST] OK #501
[SENSOR] T=31.4 C  H=73.6%  Light=77
[POST] OK #502
[SENSOR] T=31.5 C  H=73.6%  Light=73
[POST] OK #503
[SENSOR] T=31.5 C  H=73.5%  Light=77
[POST] OK #504
[SENSOR] T=31.5 C  H=73.5%  Light=74
[POST] OK #505
[SENSOR] T=31.5 C  H=73.5%  Light=80
[POST] OK #506
[SENSOR] T=31.5 C  H=73.4%  Light=75
[POST] OK #507
[SENSOR] T=31.5 C  H=73.5%  Light=76
[POST] OK #508
[SENSOR] T=31.5 C  H=73.5%  Light=75
[POST] OK #509
[SENSOR] T=31.4 C  H=73.6%  Light=75
[POST] OK #510
[SENSOR] T=31.5 C  H=73.6%  Light=73
[POST] OK #511
[SENSOR] T=31.5 C  H=73.6%  Light=75
[POST] OK #512
[SENSOR] T=31.5 C  H=73.5%  Light=81
[POST] OK #513
[SENSOR] T=31.5 C  H=73.5%  Light=74
[POST] OK #514
[SENSOR] T=31.5 C  H=73.4%  Light=73
[POST] OK #515
[SENSOR] T=31.5 C  H=73.4%  Light=77
[POST] OK #516
[SENSOR] T=31.5 C  H=73.4%  Light=76
[POST] OK #517
[SENSOR] T=31.5 C  H=73.5%  Light=77
[POST] OK #518
[SENSOR] T=31.5 C  H=73.4%  Light=77
[POST] OK #519
[SENSOR] T=31.5 C  H=73.4%  Light=86
[POST] OK #520
[SENSOR] T=31.5 C  H=73.4%  Light=76
[POST] OK #521
[SENSOR] T=31.5 C  H=73.5%  Light=74
[POST] OK #522
[SENSOR] T=31.5 C  H=73.4%  Light=75
[POST] OK #523
[SENSOR] T=31.5 C  H=73.4%  Light=73
[POST] OK #524
[SENSOR] T=31.5 C  H=73.4%  Light=77
[POST] OK #525
[SENSOR] T=31.5 C  H=73.4%  Light=77
[POST] OK #526
[SENSOR] T=31.5 C  H=73.4%  Light=72
[POST] OK #527
[SENSOR] T=31.5 C  H=73.4%  Light=80
[POST] OK #528
[SENSOR] T=31.5 C  H=73.3%  Light=81
[POST] OK #529
[SENSOR] T=31.5 C  H=73.3%  Light=76
[POST] OK #530
[SENSOR] T=31.5 C  H=73.3%  Light=71
[POST] OK #531
[SENSOR] T=31.5 C  H=73.3%  Light=80
[POST] OK #532
[SENSOR] T=31.5 C  H=73.4%  Light=76
[POST] OK #533
[SENSOR] T=31.5 C  H=73.3%  Light=80
[POST] OK #534
[SENSOR] T=31.5 C  H=73.3%  Light=75
[POST] OK #535
[SENSOR] T=31.5 C  H=73.3%  Light=77
[POST] OK #536
[SENSOR] T=31.5 C  H=73.2%  Light=78
[POST] OK #537
[SENSOR] T=31.5 C  H=73.2%  Light=81
[POST] OK #538
[SENSOR] T=31.6 C  H=73.2%  Light=77
[POST] OK #539
[SENSOR] T=31.6 C  H=73.2%  Light=72
[POST] OK #540
[SENSOR] T=31.6 C  H=73.2%  Light=77
[POST] OK #541
[SENSOR] T=31.6 C  H=73.2%  Light=79
[POST] OK #542
[SENSOR] T=31.6 C  H=73.2%  Light=77
[POST] OK #543
[SENSOR] T=31.6 C  H=73.2%  Light=76
[POST] OK #544
[SENSOR] T=31.6 C  H=73.2%  Light=74
[POST] OK #545
[SENSOR] T=31.6 C  H=73.1%  Light=77
[POST] OK #546
[SENSOR] T=31.6 C  H=73.1%  Light=65
[POST] OK #547
[SENSOR] T=31.6 C  H=73.2%  Light=77
[POST] OK #548
[SENSOR] T=31.6 C  H=73.2%  Light=71
[POST] OK #549
[SENSOR] T=31.6 C  H=73.2%  Light=78
[POST] OK #550
[SENSOR] T=31.6 C  H=73.3%  Light=77
[POST] OK #551
[SENSOR] T=31.6 C  H=73.2%  Light=76
[POST] OK #552
[SENSOR] T=31.6 C  H=73.3%  Light=42
[POST] OK #553
[SENSOR] T=31.5 C  H=73.3%  Light=58
[POST] OK #554
[SENSOR] T=31.6 C  H=73.3%  Light=75
[POST] OK #555
[SENSOR] T=31.6 C  H=73.4%  Light=72
[POST] OK #556
[SENSOR] T=31.6 C  H=73.4%  Light=77
[POST] OK #557
[SENSOR] T=31.6 C  H=73.4%  Light=78
[POST] OK #558
[SENSOR] T=31.6 C  H=73.4%  Light=73
[POST] OK #559
[SENSOR] T=31.5 C  H=73.4%  Light=71
[POST] OK #560
[SENSOR] T=31.6 C  H=73.5%  Light=75
[POST] OK #561
[SENSOR] T=31.5 C  H=73.5%  Light=78
[POST] OK #562
[SENSOR] T=31.5 C  H=73.5%  Light=88
[POST] OK #563
[SENSOR] T=31.5 C  H=73.6%  Light=78
[POST] OK #564
[SENSOR] T=31.5 C  H=73.5%  Light=78
[POST] OK #565
[SENSOR] T=31.5 C  H=73.6%  Light=78
[POST] OK #566
[SENSOR] T=31.5 C  H=73.6%  Light=70
[POST] OK #567
[SENSOR] T=31.5 C  H=73.7%  Light=76
[POST] OK #568
[SENSOR] T=31.5 C  H=73.7%  Light=79
[POST] OK #569
[SENSOR] T=31.5 C  H=73.8%  Light=70
[POST] OK #570
[SENSOR] T=31.5 C  H=73.8%  Light=78
[POST] OK #571
[SENSOR] T=31.4 C  H=73.9%  Light=76
[POST] OK #572
[SENSOR] T=31.4 C  H=73.9%  Light=57
[POST] OK #573
[SENSOR] T=31.4 C  H=73.9%  Light=76
[POST] OK #574
[SENSOR] T=31.4 C  H=73.9%  Light=76
[POST] OK #575
[SENSOR] T=31.4 C  H=73.9%  Light=75
[POST] OK #576
[SENSOR] T=31.4 C  H=74.0%  Light=69
[POST] OK #577
[SENSOR] T=31.4 C  H=74.0%  Light=83
[POST] OK #578
[SENSOR] T=31.4 C  H=73.9%  Light=77
[POST] OK #579
[SENSOR] T=31.4 C  H=73.8%  Light=76
[POST] OK #580
[SENSOR] T=31.4 C  H=73.9%  Light=77
[POST] OK #581
[SENSOR] T=31.4 C  H=73.9%  Light=77
[POST] OK #582
[SENSOR] T=31.4 C  H=73.9%  Light=79
[POST] OK #583
[SENSOR] T=31.4 C  H=73.9%  Light=74
[POST] OK #584
[SENSOR] T=31.3 C  H=73.9%  Light=54
[POST] OK #585
[SENSOR] T=31.3 C  H=74.0%  Light=65
[POST] OK #586
[SENSOR] T=31.4 C  H=73.9%  Light=78
[POST] OK #587
[SENSOR] T=31.3 C  H=74.0%  Light=85
[POST] OK #588
[SENSOR] T=31.2 C  H=74.3%  Light=82
[POST] OK #589
[SENSOR] T=31.2 C  H=74.3%  Light=76
[POST] OK #590
[SENSOR] T=31.2 C  H=74.4%  Light=75
[POST] OK #591
[SENSOR] T=31.2 C  H=74.5%  Light=67
[POST] OK #592
[SENSOR] T=31.2 C  H=74.6%  Light=75
[POST] OK #593
[SENSOR] T=31.2 C  H=74.6%  Light=77
[POST] OK #594
[SENSOR] T=31.2 C  H=74.5%  Light=77
[POST] OK #595
[SENSOR] T=31.2 C  H=74.5%  Light=75
[POST] OK #596
[SENSOR] T=31.2 C  H=74.6%  Light=62
[POST] OK #597
[SENSOR] T=31.3 C  H=74.6%  Light=71
[POST] OK #598
[SENSOR] T=31.3 C  H=74.6%  Light=82
[POST] OK #599
[SENSOR] T=31.3 C  H=74.6%  Light=69
[POST] OK #600
[SENSOR] T=31.3 C  H=74.5%  Light=81
[POST] OK #601
[SENSOR] T=31.3 C  H=74.4%  Light=76
[POST] OK #602
[SENSOR] T=31.3 C  H=74.5%  Light=85
[POST] OK #603
[SENSOR] T=31.3 C  H=74.5%  Light=68
[POST] OK #604
[SENSOR] T=31.3 C  H=74.5%  Light=81
[POST] OK #605
[SENSOR] T=31.3 C  H=74.5%  Light=79
[POST] OK #606
[SENSOR] T=31.3 C  H=74.5%  Light=65
[POST] OK #607
[WIFI] Lost connection. Entering OFFLINE_RETRY.
[RETRY] Attempting reconnect...
E (9132916) wifi:sta is connecting, return error
[RETRY] Reconnected.
[SENSOR] T=31.2 C  H=74.4%  Light=69
[POST] OK #608
[SENSOR] T=31.3 C  H=74.5%  Light=73
[POST] OK #609
[SENSOR] T=31.3 C  H=74.4%  Light=58
[POST] FAIL #2
[RETRY] Attempting reconnect...
[RETRY] Reconnected.
[SENSOR] T=31.2 C  H=74.4%  Light=62
[POST] OK #610
[SENSOR] T=31.3 C  H=74.5%  Light=67
[POST] OK #611
[SENSOR] T=31.2 C  H=74.5%  Light=68
[POST] OK #612
[SENSOR] T=31.2 C  H=74.4%  Light=82
[POST] OK #613
[SENSOR] T=31.1 C  H=74.5%  Light=81
[POST] OK #614
[SENSOR] T=31.1 C  H=74.4%  Light=84
[POST] OK #615
[SENSOR] T=31.1 C  H=74.5%  Light=79
[POST] OK #616
[SENSOR] T=31.1 C  H=74.5%  Light=81
[POST] OK #617
[SENSOR] T=31.1 C  H=74.5%  Light=85
[POST] OK #618
[SENSOR] T=31.0 C  H=74.4%  Light=85
[POST] OK #619
[SENSOR] T=31.0 C  H=74.6%  Light=72
[POST] OK #620
[SENSOR] T=31.0 C  H=74.6%  Light=78
[POST] OK #621
[SENSOR] T=31.0 C  H=74.7%  Light=78
[POST] OK #622
[SENSOR] T=31.0 C  H=74.8%  Light=82
[POST] OK #623
[SENSOR] T=31.0 C  H=74.7%  Light=76
[POST] OK #624
[SENSOR] T=31.0 C  H=74.7%  Light=75
[POST] OK #625
[SENSOR] T=30.9 C  H=74.7%  Light=82
[POST] OK #626
[SENSOR] T=30.9 C  H=74.8%  Light=80
[POST] OK #627
[SENSOR] T=30.9 C  H=74.8%  Light=28
[POST] OK #628
[SENSOR] T=30.9 C  H=74.8%  Light=76
[POST] OK #629
[SENSOR] T=30.9 C  H=74.9%  Light=79
[POST] OK #630
[SENSOR] T=30.9 C  H=74.9%  Light=78
[POST] OK #631
[SENSOR] T=30.8 C  H=74.9%  Light=81
[POST] OK #632
[SENSOR] T=30.8 C  H=75.0%  Light=38
[POST] OK #633
[SENSOR] T=30.8 C  H=75.0%  Light=69
[POST] OK #634
[SENSOR] T=30.8 C  H=75.1%  Light=80
[POST] OK #635
[SENSOR] T=30.8 C  H=75.1%  Light=70
[POST] OK #636
[SENSOR] T=30.7 C  H=75.2%  Light=80
[POST] OK #637
[SENSOR] T=30.7 C  H=75.2%  Light=88
[POST] OK #638
[SENSOR] T=30.7 C  H=75.2%  Light=81
[POST] OK #639
[SENSOR] T=30.7 C  H=75.2%  Light=76
[POST] OK #640
[SENSOR] T=30.8 C  H=75.3%  Light=78
[POST] OK #641
[SENSOR] T=30.8 C  H=75.2%  Light=81
[POST] OK #642
[SENSOR] T=30.7 C  H=75.2%  Light=84
[POST] OK #643
[SENSOR] T=30.7 C  H=75.3%  Light=47
[POST] OK #644
[SENSOR] T=30.7 C  H=75.3%  Light=79
[POST] OK #645
[SENSOR] T=30.7 C  H=75.2%  Light=84
[POST] OK #646
[SENSOR] T=30.7 C  H=75.2%  Light=95
[POST] OK #647
[SENSOR] T=30.7 C  H=75.3%  Light=50
[POST] OK #648
[SENSOR] T=30.7 C  H=75.3%  Light=82
[POST] OK #649
[SENSOR] T=30.8 C  H=75.2%  Light=77
[POST] OK #650
[SENSOR] T=30.8 C  H=75.2%  Light=37
[POST] OK #651
[SENSOR] T=30.7 C  H=75.3%  Light=33
[POST] OK #652
[SENSOR] T=30.8 C  H=75.3%  Light=83
[POST] OK #653
[SENSOR] T=30.7 C  H=75.3%  Light=79
[POST] OK #654
[SENSOR] T=30.7 C  H=75.3%  Light=75
[POST] OK #655
[SENSOR] T=30.7 C  H=75.3%  Light=77
[POST] OK #656
[SENSOR] T=30.7 C  H=75.4%  Light=18
[POST] OK #657
[SENSOR] T=30.7 C  H=75.5%  Light=21
[POST] OK #658
[SENSOR] T=30.7 C  H=75.6%  Light=81
[POST] OK #659
[SENSOR] T=30.8 C  H=75.4%  Light=95
[POST] OK #660
[SENSOR] T=30.9 C  H=75.2%  Light=71
[POST] OK #661
[SENSOR] T=30.9 C  H=75.1%  Light=83
[POST] OK #662
[SENSOR] T=31.0 C  H=74.9%  Light=81
[POST] OK #663
[SENSOR] T=31.0 C  H=74.8%  Light=81
[POST] OK #664
[SENSOR] T=31.0 C  H=74.7%  Light=82
[POST] OK #665
[SENSOR] T=31.0 C  H=74.6%  Light=91
[POST] OK #666
[SENSOR] T=31.1 C  H=74.5%  Light=87
[POST] OK #667
[SENSOR] T=31.1 C  H=74.5%  Light=81
[POST] OK #668
[SENSOR] T=31.1 C  H=74.5%  Light=80
[POST] OK #669
[SENSOR] T=31.1 C  H=74.5%  Light=82
[POST] OK #670
[SENSOR] T=31.2 C  H=74.5%  Light=82
[POST] OK #671
[SENSOR] T=31.2 C  H=74.5%  Light=84
[POST] OK #672
[SENSOR] T=31.2 C  H=74.4%  Light=84
[POST] OK #673
[SENSOR] T=31.1 C  H=74.5%  Light=80
[POST] OK #674
[SENSOR] T=31.2 C  H=74.5%  Light=80
[POST] OK #675
[SENSOR] T=31.2 C  H=74.5%  Light=88
[POST] OK #676
[SENSOR] T=31.2 C  H=74.4%  Light=84
[POST] OK #677
[SENSOR] T=31.2 C  H=74.5%  Light=81
[POST] OK #678
[SENSOR] T=31.2 C  H=74.5%  Light=86
[POST] OK #679
[SENSOR] T=31.2 C  H=74.4%  Light=77
[POST] OK #680
[SENSOR] T=31.2 C  H=74.5%  Light=78
[POST] OK #681
[SENSOR] T=31.2 C  H=74.5%  Light=82
[POST] OK #682
[SENSOR] T=31.2 C  H=74.5%  Light=27
[POST] OK #683
[SENSOR] T=31.2 C  H=74.5%  Light=45
[POST] OK #684
[SENSOR] T=31.2 C  H=74.5%  Light=68
[POST] OK #685
[SENSOR] T=31.2 C  H=74.5%  Light=90
[POST] OK #686
[SENSOR] T=31.2 C  H=74.5%  Light=95
[POST] OK #687
[SENSOR] T=31.3 C  H=74.5%  Light=97
[POST] OK #688
[SENSOR] T=31.2 C  H=74.5%  Light=95
[POST] OK #689
[SENSOR] T=31.3 C  H=74.5%  Light=103
[POST] OK #690
[SENSOR] T=31.3 C  H=74.5%  Light=101
[POST] OK #691
[SENSOR] T=31.2 C  H=74.3%  Light=99
[POST] OK #692
[SENSOR] T=31.2 C  H=74.2%  Light=96
[POST] OK #693
[SENSOR] T=31.3 C  H=74.2%  Light=88
[POST] OK #694
[SENSOR] T=31.3 C  H=74.1%  Light=103
[POST] OK #695
[SENSOR] T=31.3 C  H=74.1%  Light=72
[POST] OK #696
[SENSOR] T=31.3 C  H=74.2%  Light=110
[POST] OK #697
[SENSOR] T=31.3 C  H=74.1%  Light=75
[POST] OK #698
[SENSOR] T=31.3 C  H=74.2%  Light=59
[POST] OK #699
[SENSOR] T=31.3 C  H=74.2%  Light=86
[POST] OK #700
[SENSOR] T=31.4 C  H=74.2%  Light=96
[POST] OK #701
[SENSOR] T=31.3 C  H=74.1%  Light=98
[POST] OK #702
[SENSOR] T=31.4 C  H=74.1%  Light=81
[POST] OK #703
[SENSOR] T=31.3 C  H=74.0%  Light=100
[POST] OK #704
[SENSOR] T=31.4 C  H=74.1%  Light=99
[POST] OK #705
[SENSOR] T=31.3 C  H=74.0%  Light=93
[POST] OK #706
[SENSOR] T=31.4 C  H=74.0%  Light=108
[POST] OK #707
[SENSOR] T=31.3 C  H=74.0%  Light=91
[POST] OK #708
[SENSOR] T=31.3 C  H=74.1%  Light=104
[POST] OK #709
[SENSOR] T=31.4 C  H=74.1%  Light=101
[POST] OK #710
[SENSOR] T=31.4 C  H=74.1%  Light=86
[POST] OK #711
[SENSOR] T=31.4 C  H=74.1%  Light=90
[POST] OK #712
[SENSOR] T=31.4 C  H=74.0%  Light=98
[POST] OK #713
[SENSOR] T=31.4 C  H=74.0%  Light=92
[POST] OK #714
[SENSOR] T=31.4 C  H=74.0%  Light=87
[POST] OK #715
[SENSOR] T=31.4 C  H=74.0%  Light=100
[POST] OK #716
[SENSOR] T=31.4 C  H=74.0%  Light=99
[POST] OK #717
[SENSOR] T=31.4 C  H=74.1%  Light=92
[POST] OK #718
[SENSOR] T=31.4 C  H=74.1%  Light=100
[POST] OK #719
[SENSOR] T=31.4 C  H=74.1%  Light=93
[POST] OK #720
[SENSOR] T=31.4 C  H=74.0%  Light=90
[POST] OK #721
[SENSOR] T=31.4 C  H=74.0%  Light=103
[POST] OK #722
[SENSOR] T=31.4 C  H=74.1%  Light=100
[POST] OK #723
[SENSOR] T=31.4 C  H=74.1%  Light=101
[POST] OK #724
[SENSOR] T=31.4 C  H=74.1%  Light=87
[POST] OK #725
[SENSOR] T=31.4 C  H=74.2%  Light=96
[POST] OK #726
[SENSOR] T=31.4 C  H=74.2%  Light=95
[POST] OK #727
[SENSOR] T=31.3 C  H=74.1%  Light=99
[POST] OK #728
[SENSOR] T=31.4 C  H=74.2%  Light=89
[POST] OK #729
[SENSOR] T=31.4 C  H=74.2%  Light=95
[POST] OK #730
[SENSOR] T=31.3 C  H=74.2%  Light=97
[POST] OK #731
[SENSOR] T=31.3 C  H=74.3%  Light=100
[POST] OK #732
[SENSOR] T=31.4 C  H=74.2%  Light=99
[POST] OK #733
[SENSOR] T=31.4 C  H=74.2%  Light=98
[POST] OK #734
[SENSOR] T=31.4 C  H=74.3%  Light=101
[POST] OK #735
[SENSOR] T=31.4 C  H=74.3%  Light=100
[POST] OK #736
[SENSOR] T=31.4 C  H=74.3%  Light=92
[POST] OK #737
[SENSOR] T=31.4 C  H=74.4%  Light=99
[POST] OK #738
[SENSOR] T=31.4 C  H=74.3%  Light=94
[POST] OK #739
[SENSOR] T=31.4 C  H=74.2%  Light=95
[POST] OK #740
[SENSOR] T=31.4 C  H=74.3%  Light=99
[POST] OK #741
[SENSOR] T=31.4 C  H=74.3%  Light=102
[POST] OK #742
[SENSOR] T=31.4 C  H=74.4%  Light=104
[POST] OK #743
[SENSOR] T=31.4 C  H=74.4%  Light=96
[POST] OK #744
[SENSOR] T=31.4 C  H=74.4%  Light=103
[POST] OK #745
[SENSOR] T=31.4 C  H=74.4%  Light=101
[POST] OK #746
[SENSOR] T=31.4 C  H=74.3%  Light=101
[POST] OK #747
[SENSOR] T=31.4 C  H=74.4%  Light=93
[POST] OK #748
[SENSOR] T=31.4 C  H=74.5%  Light=90
[POST] OK #749
[SENSOR] T=31.4 C  H=74.4%  Light=103
[POST] OK #750
[SENSOR] T=31.4 C  H=74.4%  Light=102
[POST] OK #751
[SENSOR] T=31.4 C  H=74.4%  Light=93
[POST] OK #752
[SENSOR] T=31.4 C  H=74.5%  Light=102
[POST] OK #753
[SENSOR] T=31.3 C  H=74.4%  Light=100
[POST] OK #754
[SENSOR] T=31.4 C  H=74.4%  Light=106
[POST] OK #755
[SENSOR] T=31.4 C  H=74.5%  Light=102
[POST] OK #756
[SENSOR] T=31.4 C  H=74.4%  Light=98
[POST] OK #757
[SENSOR] T=31.4 C  H=74.5%  Light=89
[POST] OK #758
[SENSOR] T=31.4 C  H=74.4%  Light=89
[POST] OK #759
[SENSOR] T=31.3 C  H=74.4%  Light=88
[POST] FAIL #3
[RETRY] Attempting reconnect...
[RETRY] Reconnected.
[SENSOR] T=31.4 C  H=74.4%  Light=73
[POST] OK #760
[SENSOR] T=31.4 C  H=74.4%  Light=92
[POST] OK #761
[SENSOR] T=31.4 C  H=74.4%  Light=100
[POST] OK #762
[SENSOR] T=31.4 C  H=74.4%  Light=101
[POST] OK #763
[SENSOR] T=31.4 C  H=74.4%  Light=39
[POST] OK #764
[SENSOR] T=31.4 C  H=74.4%  Light=99
[POST] OK #765
[SENSOR] T=31.4 C  H=74.4%  Light=107
[POST] OK #766
[SENSOR] T=31.4 C  H=74.4%  Light=95
[POST] OK #767
[SENSOR] T=31.4 C  H=74.3%  Light=102
[POST] OK #768
[SENSOR] T=31.4 C  H=74.4%  Light=96
[POST] OK #769
[SENSOR] T=31.3 C  H=74.4%  Light=100
[POST] OK #770
[SENSOR] T=31.3 C  H=74.4%  Light=104
[POST] OK #771
[SENSOR] T=31.4 C  H=74.4%  Light=99
[POST] OK #772
[SENSOR] T=31.3 C  H=74.3%  Light=97
[POST] OK #773
[SENSOR] T=31.4 C  H=74.4%  Light=101
[POST] OK #774
[SENSOR] T=31.4 C  H=74.4%  Light=100
[POST] OK #775
[SENSOR] T=31.4 C  H=74.4%  Light=96
[POST] OK #776
[SENSOR] T=31.4 C  H=74.4%  Light=95
[POST] OK #777
[SENSOR] T=31.4 C  H=74.4%  Light=92
[POST] OK #778
[SENSOR] T=31.4 C  H=74.4%  Light=86
[POST] OK #779
[SENSOR] T=31.3 C  H=74.5%  Light=77
[POST] OK #780
[SENSOR] T=31.2 C  H=74.8%  Light=94
[POST] OK #781
[SENSOR] T=31.1 C  H=75.0%  Light=85
[POST] OK #782
[SENSOR] T=31.1 C  H=75.4%  Light=95
[POST] OK #783
[SENSOR] T=31.1 C  H=75.4%  Light=91
[POST] OK #784
[SENSOR] T=31.0 C  H=75.6%  Light=87
[POST] OK #785
[SENSOR] T=31.0 C  H=75.6%  Light=94
[POST] OK #786
[SENSOR] T=31.0 C  H=75.6%  Light=86
[POST] OK #787
[SENSOR] T=31.0 C  H=75.6%  Light=78
[POST] OK #788
[SENSOR] T=31.0 C  H=75.6%  Light=88
[POST] OK #789
[SENSOR] T=31.0 C  H=75.5%  Light=90
[POST] OK #790
[SENSOR] T=31.1 C  H=75.5%  Light=83
[POST] OK #791
[SENSOR] T=31.1 C  H=75.5%  Light=92
[POST] OK #792
[SENSOR] T=31.1 C  H=75.4%  Light=95
[POST] OK #793
[SENSOR] T=31.0 C  H=75.6%  Light=96
[POST] OK #794
[SENSOR] T=31.1 C  H=75.6%  Light=83
[POST] OK #795
[SENSOR] T=31.1 C  H=75.5%  Light=90
[POST] OK #796
[SENSOR] T=31.1 C  H=75.4%  Light=96
[POST] OK #797
[SENSOR] T=31.1 C  H=75.4%  Light=91
[POST] OK #798
[SENSOR] T=31.0 C  H=75.4%  Light=92
[POST] OK #799
[SENSOR] T=31.0 C  H=75.5%  Light=90
[POST] OK #800
[SENSOR] T=31.0 C  H=75.6%  Light=81
[POST] OK #801
[SENSOR] T=31.0 C  H=75.6%  Light=92
[POST] OK #802
[SENSOR] T=31.0 C  H=75.7%  Light=90
[POST] OK #803
[SENSOR] T=30.9 C  H=75.6%  Light=86
[POST] OK #804
[SENSOR] T=30.9 C  H=75.6%  Light=94
[POST] OK #805
[SENSOR] T=30.9 C  H=75.6%  Light=86
[POST] OK #806
[SENSOR] T=30.8 C  H=75.7%  Light=97
[POST] OK #807
[SENSOR] T=30.8 C  H=75.9%  Light=93
[POST] OK #808
[SENSOR] T=30.8 C  H=75.9%  Light=93
[POST] OK #809
[SENSOR] T=30.8 C  H=75.9%  Light=95
[POST] OK #810
[SENSOR] T=30.8 C  H=75.9%  Light=95
[POST] OK #811
[SENSOR] T=30.8 C  H=75.9%  Light=90
[POST] OK #812
[SENSOR] T=30.8 C  H=75.9%  Light=82
[POST] OK #813
[SENSOR] T=30.8 C  H=76.1%  Light=86
[POST] OK #814
[SENSOR] T=30.8 C  H=76.1%  Light=79
[POST] OK #815
[SENSOR] T=30.8 C  H=76.2%  Light=93
[POST] OK #816
[SENSOR] T=30.7 C  H=76.2%  Light=94
[POST] OK #817
[SENSOR] T=30.7 C  H=76.3%  Light=95
[POST] OK #818
[SENSOR] T=30.8 C  H=76.2%  Light=90
[POST] OK #819
[SENSOR] T=30.8 C  H=76.4%  Light=98
[POST] OK #820
[SENSOR] T=30.8 C  H=76.2%  Light=93
[POST] OK #821
[SENSOR] T=30.8 C  H=76.1%  Light=76
[POST] OK #822
[SENSOR] T=30.8 C  H=76.1%  Light=99
[POST] OK #823
[SENSOR] T=30.9 C  H=76.1%  Light=92
[POST] OK #824
[SENSOR] T=30.9 C  H=76.0%  Light=85
[POST] OK #825
[SENSOR] T=30.9 C  H=76.0%  Light=90
[POST] OK #826
[SENSOR] T=30.9 C  H=75.9%  Light=91
[POST] OK #827
[SENSOR] T=31.0 C  H=75.8%  Light=89
[POST] OK #828
[SENSOR] T=31.0 C  H=75.6%  Light=96
[POST] OK #829
[SENSOR] T=31.1 C  H=75.7%  Light=95
[POST] OK #830
[SENSOR] T=31.1 C  H=75.7%  Light=81
[POST] OK #831
[SENSOR] T=31.1 C  H=75.6%  Light=90
[POST] OK #832
[SENSOR] T=31.1 C  H=75.6%  Light=80
[POST] OK #833
[SENSOR] T=31.1 C  H=75.5%  Light=84
[POST] OK #834
[SENSOR] T=31.2 C  H=75.6%  Light=98
[POST] OK #835
[SENSOR] T=31.2 C  H=75.5%  Light=92
[POST] OK #836
[SENSOR] T=31.2 C  H=75.6%  Light=95
[POST] OK #837
[SENSOR] T=31.2 C  H=75.5%  Light=91
[POST] OK #838
[SENSOR] T=31.2 C  H=75.5%  Light=94
[POST] OK #839
[SENSOR] T=31.2 C  H=75.6%  Light=93
[POST] OK #840
[SENSOR] T=31.2 C  H=75.6%  Light=89
[POST] OK #841
[SENSOR] T=31.2 C  H=75.6%  Light=92
[POST] OK #842
[SENSOR] T=31.3 C  H=75.6%  Light=105
[POST] OK #843
[SENSOR] T=31.3 C  H=75.6%  Light=93
[POST] OK #844
[SENSOR] T=31.3 C  H=75.4%  Light=78
[POST] OK #845
[SENSOR] T=31.3 C  H=75.4%  Light=87
[POST] OK #846
[SENSOR] T=31.3 C  H=75.4%  Light=93
[POST] OK #847
[SENSOR] T=31.3 C  H=75.4%  Light=88
[POST] OK #848
[SENSOR] T=31.3 C  H=75.4%  Light=89
[POST] OK #849
[SENSOR] T=31.3 C  H=75.4%  Light=85
[POST] OK #850
[SENSOR] T=31.3 C  H=75.4%  Light=96
[POST] OK #851
[SENSOR] T=31.2 C  H=75.5%  Light=88
[POST] FAIL #4
[RETRY] Attempting reconnect...
[RETRY] Reconnected.
[SENSOR] T=31.2 C  H=75.5%  Light=79
[POST] OK #852
[SENSOR] T=31.3 C  H=75.5%  Light=78
[POST] OK #853
[SENSOR] T=31.2 C  H=75.5%  Light=99
[POST] OK #854
[SENSOR] T=31.2 C  H=75.5%  Light=93
[POST] OK #855
[SENSOR] T=31.3 C  H=75.5%  Light=80
[POST] OK #856
[SENSOR] T=31.2 C  H=75.5%  Light=91
[POST] OK #857
[SENSOR] T=31.2 C  H=75.5%  Light=97
[POST] OK #858
[SENSOR] T=31.2 C  H=75.5%  Light=84
[POST] OK #859
[SENSOR] T=31.3 C  H=75.5%  Light=84
[POST] OK #860
[SENSOR] T=31.2 C  H=75.6%  Light=86
[POST] OK #861
[SENSOR] T=31.3 C  H=75.5%  Light=92
[POST] OK #862
[SENSOR] T=31.2 C  H=75.5%  Light=96
[POST] OK #863
[SENSOR] T=31.2 C  H=75.6%  Light=93
[POST] OK #864
[SENSOR] T=31.2 C  H=75.5%  Light=92
[POST] OK #865
[SENSOR] T=31.2 C  H=75.6%  Light=91
[POST] OK #866
[SENSOR] T=31.2 C  H=75.6%  Light=83
[POST] OK #867
[SENSOR] T=31.2 C  H=75.6%  Light=87
[POST] OK #868
[SENSOR] T=31.2 C  H=75.7%  Light=84
[POST] OK #869
[SENSOR] T=31.2 C  H=75.6%  Light=95
[POST] OK #870
[SENSOR] T=31.1 C  H=75.7%  Light=95
[POST] OK #871
[SENSOR] T=31.2 C  H=75.6%  Light=92
[POST] OK #872
[SENSOR] T=31.2 C  H=75.6%  Light=72
[POST] OK #873
[SENSOR] T=31.2 C  H=75.6%  Light=69
[POST] OK #874
[SENSOR] T=31.2 C  H=75.6%  Light=98
[POST] OK #875
[SENSOR] T=31.1 C  H=75.5%  Light=99
[POST] OK #876
[SENSOR] T=31.1 C  H=75.7%  Light=91
[POST] OK #877
[SENSOR] T=31.2 C  H=75.6%  Light=95
[POST] OK #878
[SENSOR] T=31.2 C  H=75.4%  Light=96
[POST] OK #879
[SENSOR] T=31.1 C  H=75.4%  Light=102
[POST] OK #880
[SENSOR] T=31.1 C  H=75.5%  Light=101
[POST] OK #881
[SENSOR] T=31.1 C  H=75.6%  Light=88
[POST] OK #882
[SENSOR] T=31.1 C  H=75.6%  Light=91
[POST] OK #883
[SENSOR] T=31.2 C  H=75.6%  Light=93
[POST] OK #884
[SENSOR] T=31.2 C  H=75.6%  Light=100
[POST] OK #885
[SENSOR] T=31.1 C  H=75.6%  Light=94
[POST] OK #886
[SENSOR] T=31.1 C  H=75.7%  Light=79
[POST] OK #887
[SENSOR] T=31.1 C  H=75.7%  Light=40
[POST] OK #888
[SENSOR] T=31.1 C  H=75.7%  Light=82
[POST] OK #889
[SENSOR] T=31.1 C  H=75.8%  Light=12
[POST] OK #890
[SENSOR] T=31.1 C  H=75.7%  Light=8
[POST] OK #891
[SENSOR] T=31.1 C  H=75.5%  Light=4
[POST] OK #892
[SENSOR] T=31.1 C  H=75.6%  Light=97
[POST] OK #893
[SENSOR] T=31.0 C  H=75.7%  Light=67
[POST] OK #894
[SENSOR] T=31.0 C  H=75.8%  Light=64
[POST] OK #895
[SENSOR] T=31.0 C  H=75.9%  Light=63
[POST] OK #896
[SENSOR] T=31.0 C  H=75.9%  Light=66
[POST] OK #897
[SENSOR] T=31.0 C  H=75.8%  Light=62
[POST] OK #898
[SENSOR] T=31.0 C  H=75.8%  Light=64
[POST] OK #899
[SENSOR] T=31.0 C  H=75.7%  Light=59
[POST] OK #900
[SENSOR] T=31.0 C  H=75.6%  Light=68
[POST] OK #901
[SENSOR] T=31.1 C  H=75.7%  Light=64
[POST] OK #902
[SENSOR] T=31.1 C  H=75.6%  Light=63
[POST] OK #903
[SENSOR] T=31.1 C  H=75.7%  Light=68
[POST] OK #904
[SENSOR] T=31.1 C  H=75.6%  Light=79
[POST] OK #905
[SENSOR] T=31.2 C  H=75.5%  Light=79
[POST] OK #906
[SENSOR] T=31.1 C  H=75.5%  Light=56
[POST] OK #907
[SENSOR] T=31.1 C  H=75.5%  Light=67
[POST] OK #908
[SENSOR] T=31.2 C  H=75.4%  Light=18
[POST] OK #909
[SENSOR] T=31.2 C  H=75.4%  Light=63
[POST] OK #910
[SENSOR] T=31.2 C  H=75.4%  Light=67
[POST] OK #911
[SENSOR] T=31.2 C  H=75.4%  Light=69
[POST] OK #912
[SENSOR] T=31.2 C  H=75.4%  Light=62
[POST] OK #913
[SENSOR] T=31.2 C  H=75.5%  Light=66
[POST] OK #914
[SENSOR] T=31.2 C  H=75.4%  Light=64
[POST] OK #915
[SENSOR] T=31.2 C  H=75.5%  Light=66
[POST] OK #916
[SENSOR] T=31.2 C  H=75.5%  Light=67
[POST] OK #917
[SENSOR] T=31.2 C  H=75.6%  Light=68
[POST] OK #918
[SENSOR] T=31.1 C  H=75.5%  Light=66
[POST] OK #919
[SENSOR] T=31.2 C  H=75.6%  Light=66
[POST] OK #920
[SENSOR] T=31.1 C  H=75.6%  Light=64
[POST] OK #921
[SENSOR] T=31.1 C  H=75.6%  Light=66
[POST] OK #922
[SENSOR] T=31.2 C  H=75.5%  Light=57
[POST] OK #923
[SENSOR] T=31.2 C  H=75.4%  Light=68
[POST] OK #924
[SENSOR] T=31.2 C  H=75.4%  Light=63
[POST] OK #925
[SENSOR] T=31.2 C  H=75.3%  Light=60
[POST] OK #926
[SENSOR] T=31.2 C  H=75.4%  Light=67
[POST] OK #927
[SENSOR] T=31.2 C  H=75.3%  Light=68
[POST] OK #928
[SENSOR] T=31.2 C  H=75.2%  Light=68
[POST] OK #929
[SENSOR] T=31.2 C  H=75.3%  Light=66
[POST] OK #930
[SENSOR] T=31.2 C  H=75.2%  Light=61
[POST] OK #931
[SENSOR] T=31.2 C  H=75.3%  Light=65
[POST] OK #932
[SENSOR] T=31.2 C  H=75.3%  Light=68
[POST] OK #933
[SENSOR] T=31.2 C  H=75.3%  Light=63
[POST] OK #934
[SENSOR] T=31.3 C  H=75.2%  Light=67
[POST] OK #935
[SENSOR] T=31.3 C  H=75.2%  Light=69
[POST] OK #936
[SENSOR] T=31.2 C  H=75.2%  Light=69
[POST] OK #937
[SENSOR] T=31.2 C  H=75.3%  Light=67
[POST] OK #938
[SENSOR] T=31.3 C  H=75.2%  Light=55
[POST] OK #939
[SENSOR] T=31.3 C  H=75.2%  Light=73
[POST] OK #940
[SENSOR] T=31.2 C  H=75.2%  Light=57
[POST] OK #941
[SENSOR] T=31.2 C  H=75.2%  Light=68
[POST] OK #942
[SENSOR] T=31.3 C  H=75.2%  Light=71
[POST] OK #943
[SENSOR] T=31.3 C  H=75.2%  Light=65
[POST] OK #944
[SENSOR] T=31.3 C  H=75.1%  Light=68
[POST] OK #945
[SENSOR] T=31.3 C  H=75.1%  Light=68
[POST] OK #946
[SENSOR] T=31.3 C  H=75.1%  Light=66
[POST] OK #947
[SENSOR] T=31.3 C  H=75.1%  Light=68
[POST] OK #948
[SENSOR] T=31.3 C  H=75.1%  Light=68
[POST] OK #949
[SENSOR] T=31.3 C  H=75.0%  Light=69
[POST] OK #950
[SENSOR] T=31.3 C  H=75.0%  Light=41
[POST] OK #951
[SENSOR] T=31.3 C  H=75.0%  Light=61
[POST] OK #952
[SENSOR] T=31.3 C  H=75.2%  Light=59
[POST] OK #953
[SENSOR] T=31.2 C  H=75.1%  Light=65
[POST] OK #954
[SENSOR] T=31.2 C  H=75.1%  Light=69
[POST] OK #955
[SENSOR] T=31.2 C  H=75.1%  Light=64
[POST] OK #956
[SENSOR] T=31.2 C  H=75.1%  Light=63
[POST] OK #957
[SENSOR] T=31.3 C  H=75.2%  Light=64
[POST] OK #958
[SENSOR] T=31.2 C  H=75.2%  Light=64
[POST] OK #959
[SENSOR] T=31.3 C  H=75.1%  Light=70
[POST] OK #960
[SENSOR] T=31.2 C  H=75.1%  Light=65
[POST] OK #961
[SENSOR] T=31.3 C  H=75.0%  Light=62
[POST] OK #962
[SENSOR] T=31.3 C  H=75.1%  Light=61
[POST] OK #963
[SENSOR] T=31.2 C  H=75.0%  Light=67
[POST] OK #964
[SENSOR] T=31.3 C  H=75.0%  Light=67
[POST] OK #965
[SENSOR] T=31.3 C  H=74.9%  Light=68
[POST] OK #966
[SENSOR] T=31.3 C  H=75.0%  Light=71
[POST] OK #967
[SENSOR] T=31.3 C  H=74.9%  Light=66
[POST] OK #968
[SENSOR] T=31.3 C  H=74.8%  Light=67
[POST] OK #969
[SENSOR] T=31.3 C  H=74.8%  Light=65
[POST] OK #970
[SENSOR] T=31.3 C  H=74.8%  Light=65
[POST] OK #971
[SENSOR] T=31.3 C  H=74.7%  Light=72
[POST] OK #972
[SENSOR] T=31.4 C  H=74.8%  Light=61
[POST] OK #973
[SENSOR] T=31.3 C  H=74.8%  Light=65
[POST] OK #974
[SENSOR] T=31.3 C  H=74.8%  Light=65
[POST] OK #975
[SENSOR] T=31.3 C  H=74.9%  Light=59
[POST] OK #976
[SENSOR] T=31.3 C  H=75.0%  Light=67
[POST] OK #977
[SENSOR] T=31.3 C  H=75.0%  Light=67
[POST] OK #978
[SENSOR] T=31.3 C  H=74.9%  Light=68
[POST] OK #979
[SENSOR] T=31.3 C  H=75.0%  Light=69
[POST] OK #980
[SENSOR] T=31.3 C  H=75.1%  Light=71
[POST] OK #981
[SENSOR] T=31.3 C  H=75.0%  Light=77
[POST] OK #982
[SENSOR] T=31.3 C  H=75.1%  Light=64
[POST] OK #983
[SENSOR] T=31.3 C  H=75.1%  Light=67
[POST] OK #984
[SENSOR] T=31.3 C  H=75.1%  Light=69
[POST] OK #985
[SENSOR] T=31.3 C  H=75.1%  Light=82
[POST] OK #986
[SENSOR] T=31.3 C  H=75.1%  Light=57
[POST] OK #987
[SENSOR] T=31.3 C  H=75.2%  Light=79
[POST] OK #988
[SENSOR] T=31.2 C  H=75.2%  Light=65
[POST] OK #989
[SENSOR] T=31.2 C  H=75.2%  Light=72
[POST] OK #990
[SENSOR] T=31.2 C  H=75.2%  Light=78
[POST] OK #991
[SENSOR] T=31.2 C  H=75.3%  Light=76
[POST] OK #992
[SENSOR] T=31.2 C  H=75.3%  Light=77
[POST] OK #993
[SENSOR] T=31.2 C  H=75.3%  Light=84
[POST] OK #994
[SENSOR] T=31.2 C  H=75.4%  Light=72
[POST] OK #995
[SENSOR] T=31.2 C  H=75.4%  Light=71
[POST] OK #996
[SENSOR] T=31.2 C  H=75.4%  Light=77
[POST] OK #997
[SENSOR] T=31.3 C  H=75.4%  Light=54
[POST] OK #998
[SENSOR] T=31.2 C  H=75.4%  Light=73
[POST] OK #999
[SENSOR] T=31.3 C  H=75.3%  Light=76
[POST] OK #1000
[SENSOR] T=31.3 C  H=75.2%  Light=73
[POST] OK #1001
[SENSOR] T=31.3 C  H=75.2%  Light=75
[POST] OK #1002
[SENSOR] T=31.3 C  H=75.2%  Light=75
[POST] OK #1003
[SENSOR] T=31.3 C  H=75.2%  Light=72
[POST] OK #1004
[SENSOR] T=31.3 C  H=75.1%  Light=67
[POST] OK #1005
[SENSOR] T=31.3 C  H=75.1%  Light=82
[POST] OK #1006
[SENSOR] T=31.3 C  H=75.3%  Light=75
[POST] OK #1007
[SENSOR] T=31.2 C  H=75.3%  Light=81
[POST] OK #1008
[SENSOR] T=31.3 C  H=75.2%  Light=75
[POST] OK #1009
[SENSOR] T=31.3 C  H=75.3%  Light=74
[POST] OK #1010
[SENSOR] T=31.3 C  H=75.2%  Light=41
[POST] OK #1011
[SENSOR] T=31.3 C  H=75.2%  Light=59
[POST] OK #1012
[SENSOR] T=31.3 C  H=75.1%  Light=80
[POST] OK #1013
[SENSOR] T=31.3 C  H=75.1%  Light=79
[POST] OK #1014
[SENSOR] T=31.3 C  H=75.1%  Light=78
[POST] OK #1015
[SENSOR] T=31.3 C  H=75.2%  Light=71
[POST] OK #1016
[SENSOR] T=31.3 C  H=75.2%  Light=75
[POST] OK #1017
[SENSOR] T=31.2 C  H=75.3%  Light=73
[POST] OK #1018
[SENSOR] T=31.2 C  H=75.3%  Light=75
[POST] OK #1019
[SENSOR] T=31.3 C  H=75.3%  Light=79
[POST] OK #1020
[SENSOR] T=31.3 C  H=75.3%  Light=78
[POST] OK #1021
[SENSOR] T=31.2 C  H=75.3%  Light=80
[POST] OK #1022
[SENSOR] T=31.2 C  H=75.5%  Light=75
[POST] OK #1023
[SENSOR] T=31.2 C  H=75.4%  Light=80
[POST] OK #1024
[SENSOR] T=31.2 C  H=75.4%  Light=74
[POST] OK #1025
[SENSOR] T=31.2 C  H=75.4%  Light=75
[POST] OK #1026
[SENSOR] T=31.2 C  H=75.4%  Light=74
[POST] OK #1027
[SENSOR] T=31.2 C  H=75.3%  Light=66
[POST] OK #1028
[SENSOR] T=31.2 C  H=75.3%  Light=73
[POST] OK #1029
[SENSOR] T=31.2 C  H=75.4%  Light=73
[POST] OK #1030
[SENSOR] T=31.3 C  H=75.3%  Light=79
[POST] OK #1031
[SENSOR] T=31.3 C  H=75.3%  Light=85
[POST] OK #1032
[SENSOR] T=31.3 C  H=75.2%  Light=74
[POST] OK #1033
[SENSOR] T=31.3 C  H=75.1%  Light=80
[POST] OK #1034
[SENSOR] T=31.3 C  H=75.3%  Light=0
[POST] OK #1035
[SENSOR] T=31.2 C  H=75.3%  Light=75
[POST] OK #1036
[SENSOR] T=31.2 C  H=75.2%  Light=80
[POST] OK #1037
[SENSOR] T=31.2 C  H=75.2%  Light=24
[POST] OK #1038
[SENSOR] T=31.2 C  H=75.3%  Light=3
[POST] OK #1039
[SENSOR] T=31.2 C  H=75.3%  Light=75
[POST] OK #1040
[SENSOR] T=31.2 C  H=75.5%  Light=70
[POST] OK #1041
[SENSOR] T=31.1 C  H=75.5%  Light=0
[POST] OK #1042
[SENSOR] T=31.2 C  H=75.5%  Light=0
[POST] OK #1043
[SENSOR] T=31.2 C  H=75.4%  Light=82
[POST] OK #1044
[SENSOR] T=31.2 C  H=75.3%  Light=71
[POST] OK #1045
[SENSOR] T=31.2 C  H=75.4%  Light=75
[POST] OK #1046
[SENSOR] T=31.2 C  H=75.5%  Light=74
[POST] OK #1047
[SENSOR] T=31.2 C  H=75.4%  Light=0
[POST] OK #1048
[SENSOR] T=31.2 C  H=75.5%  Light=76
[POST] OK #1049
[SENSOR] T=31.2 C  H=75.4%  Light=78
[POST] OK #1050
[SENSOR] T=31.2 C  H=75.5%  Light=74
[POST] OK #1051
[SENSOR] T=31.2 C  H=75.4%  Light=82
[POST] OK #1052
[SENSOR] T=31.2 C  H=75.4%  Light=73
[POST] OK #1053
[SENSOR] T=31.3 C  H=75.3%  Light=78
[POST] OK #1054
[SENSOR] T=31.3 C  H=75.2%  Light=74
[POST] OK #1055
[SENSOR] T=31.3 C  H=75.2%  Light=79
[POST] OK #1056
[SENSOR] T=31.3 C  H=75.1%  Light=80
[POST] OK #1057
[SENSOR] T=31.4 C  H=75.0%  Light=78
[POST] OK #1058
[SENSOR] T=31.4 C  H=74.9%  Light=74
[POST] OK #1059
[SENSOR] T=31.4 C  H=74.9%  Light=77
[POST] OK #1060
[SENSOR] T=31.4 C  H=74.8%  Light=75
[POST] OK #1061
[SENSOR] T=31.4 C  H=74.8%  Light=85
[POST] OK #1062
[SENSOR] T=31.4 C  H=74.8%  Light=80
[POST] OK #1063
[SENSOR] T=31.4 C  H=74.7%  Light=77
[POST] OK #1064
[SENSOR] T=31.4 C  H=74.8%  Light=74
[POST] OK #1065
[SENSOR] T=31.5 C  H=74.7%  Light=72
[POST] OK #1066
[SENSOR] T=31.4 C  H=74.7%  Light=72
[POST] OK #1067
[SENSOR] T=31.4 C  H=74.8%  Light=79
[POST] OK #1068
[SENSOR] T=31.4 C  H=74.8%  Light=74
[POST] OK #1069
[SENSOR] T=31.4 C  H=74.8%  Light=76
[POST] OK #1070
[SENSOR] T=31.4 C  H=74.8%  Light=74
[POST] OK #1071
[SENSOR] T=31.4 C  H=74.9%  Light=80
[POST] OK #1072
[SENSOR] T=31.4 C  H=74.9%  Light=78
[POST] OK #1073
[SENSOR] T=31.4 C  H=74.9%  Light=83
[POST] OK #1074
[SENSOR] T=31.4 C  H=74.9%  Light=9
[POST] OK #1075
[SENSOR] T=31.4 C  H=75.0%  Light=8
[POST] OK #1076
[SENSOR] T=31.4 C  H=74.9%  Light=73
[POST] OK #1077
[SENSOR] T=31.4 C  H=74.9%  Light=87
[POST] OK #1078
[SENSOR] T=31.4 C  H=74.9%  Light=76
[POST] OK #1079
[SENSOR] T=31.4 C  H=74.9%  Light=81
[POST] OK #1080
[SENSOR] T=31.4 C  H=74.8%  Light=76
[POST] OK #1081
[SENSOR] T=31.4 C  H=74.9%  Light=80
[POST] OK #1082
[SENSOR] T=31.4 C  H=74.8%  Light=79
[POST] OK #1083
[SENSOR] T=31.3 C  H=74.8%  Light=84
[POST] OK #1084
[SENSOR] T=31.4 C  H=75.0%  Light=79
[POST] OK #1085
[SENSOR] T=31.3 C  H=74.9%  Light=77
[POST] OK #1086
[SENSOR] T=31.3 C  H=75.1%  Light=83
[POST] OK #1087
[SENSOR] T=31.3 C  H=75.2%  Light=83
[POST] OK #1088
[SENSOR] T=31.3 C  H=75.1%  Light=79
[POST] OK #1089
[SENSOR] T=31.2 C  H=75.2%  Light=82
[POST] OK #1090
[SENSOR] T=31.2 C  H=75.2%  Light=77
[POST] OK #1091
[SENSOR] T=31.2 C  H=75.2%  Light=78
[POST] OK #1092
[SENSOR] T=31.2 C  H=75.3%  Light=81
[POST] OK #1093
[SENSOR] T=31.2 C  H=75.4%  Light=80
[POST] OK #1094
[SENSOR] T=31.1 C  H=75.4%  Light=80
[POST] OK #1095
[SENSOR] T=31.2 C  H=75.4%  Light=60
[POST] OK #1096
[SENSOR] T=31.2 C  H=75.4%  Light=83
[POST] OK #1097
[SENSOR] T=31.2 C  H=75.3%  Light=84
[POST] OK #1098
[SENSOR] T=31.2 C  H=75.3%  Light=59
[POST] OK #1099
[SENSOR] T=31.2 C  H=75.3%  Light=78
[POST] OK #1100
[SENSOR] T=31.2 C  H=75.3%  Light=85
[POST] OK #1101
[SENSOR] T=31.3 C  H=75.3%  Light=38
[POST] OK #1102
[SENSOR] T=31.3 C  H=75.2%  Light=75
[POST] OK #1103
[SENSOR] T=31.3 C  H=75.3%  Light=77
[POST] OK #1104
[SENSOR] T=31.3 C  H=75.3%  Light=79
[POST] OK #1105
[SENSOR] T=31.2 C  H=75.3%  Light=86
[POST] OK #1106
[SENSOR] T=31.3 C  H=75.3%  Light=74
[POST] OK #1107
[SENSOR] T=31.3 C  H=75.3%  Light=79
[POST] OK #1108
[SENSOR] T=31.3 C  H=75.2%  Light=83
[POST] OK #1109
[SENSOR] T=31.3 C  H=75.2%  Light=75
[POST] OK #1110
[SENSOR] T=31.3 C  H=75.2%  Light=88
[POST] OK #1111
[SENSOR] T=31.3 C  H=75.2%  Light=81
[POST] OK #1112
[SENSOR] T=31.3 C  H=75.1%  Light=79
[POST] OK #1113
[SENSOR] T=31.3 C  H=75.1%  Light=81
[POST] OK #1114
[SENSOR] T=31.4 C  H=75.1%  Light=96
[POST] OK #1115
[SENSOR] T=31.3 C  H=75.1%  Light=90
[POST] OK #1116
[SENSOR] T=31.4 C  H=75.1%  Light=92
[POST] OK #1117
[SENSOR] T=31.4 C  H=75.0%  Light=83
[POST] OK #1118
[SENSOR] T=31.4 C  H=75.0%  Light=90
[POST] OK #1119
[SENSOR] T=31.4 C  H=75.0%  Light=99
[POST] OK #1120
[SENSOR] T=31.4 C  H=75.0%  Light=95
[POST] OK #1121
[SENSOR] T=31.4 C  H=74.9%  Light=86
[POST] OK #1122
[SENSOR] T=31.4 C  H=74.9%  Light=104
[POST] OK #1123
[SENSOR] T=31.4 C  H=74.9%  Light=80
[POST] OK #1124
[SENSOR] T=31.4 C  H=75.0%  Light=97
[POST] OK #1125
[SENSOR] T=31.4 C  H=74.9%  Light=84
[POST] OK #1126
[SENSOR] T=31.4 C  H=74.9%  Light=97
[POST] OK #1127
[SENSOR] T=31.4 C  H=75.0%  Light=89
[POST] OK #1128
[SENSOR] T=31.4 C  H=75.0%  Light=47
[POST] OK #1129
[SENSOR] T=31.4 C  H=75.0%  Light=100
[POST] OK #1130
[SENSOR] T=31.4 C  H=75.0%  Light=99
[POST] OK #1131
[SENSOR] T=31.4 C  H=75.0%  Light=89
[POST] OK #1132
[SENSOR] T=31.4 C  H=75.0%  Light=88
[POST] OK #1133
[SENSOR] T=31.4 C  H=75.0%  Light=97
[POST] OK #1134
[SENSOR] T=31.4 C  H=75.0%  Light=105
[POST] OK #1135
[SENSOR] T=31.4 C  H=74.9%  Light=97
[POST] OK #1136
[SENSOR] T=31.4 C  H=74.8%  Light=100
[POST] OK #1137
[SENSOR] T=31.4 C  H=75.0%  Light=95
[POST] OK #1138
[SENSOR] T=31.4 C  H=74.8%  Light=101
[POST] OK #1139
[SENSOR] T=31.4 C  H=74.8%  Light=89
[POST] OK #1140
[SENSOR] T=31.4 C  H=74.8%  Light=100
[POST] OK #1141
[SENSOR] T=31.4 C  H=74.8%  Light=77
[POST] OK #1142
[SENSOR] T=31.4 C  H=74.7%  Light=95
[POST] OK #1143
[SENSOR] T=31.4 C  H=74.9%  Light=91
[POST] OK #1144
[SENSOR] T=31.4 C  H=74.9%  Light=90
[POST] OK #1145
[SENSOR] T=31.4 C  H=74.9%  Light=97
[POST] OK #1146
[SENSOR] T=31.4 C  H=74.8%  Light=89
[POST] OK #1147
[SENSOR] T=31.3 C  H=74.9%  Light=83
[POST] OK #1148
[SENSOR] T=31.3 C  H=74.9%  Light=80
[POST] OK #1149
[SENSOR] T=31.3 C  H=74.8%  Light=95
[POST] OK #1150
[SENSOR] T=31.3 C  H=74.8%  Light=90
[POST] OK #1151
[SENSOR] T=31.3 C  H=74.9%  Light=89
[POST] OK #1152
[SENSOR] T=31.3 C  H=74.8%  Light=89
[POST] OK #1153
[SENSOR] T=31.3 C  H=74.9%  Light=96
[POST] OK #1154
[SENSOR] T=31.2 C  H=75.1%  Light=99
[POST] OK #1155
[SENSOR] T=31.2 C  H=75.1%  Light=102
[POST] OK #1156
[SENSOR] T=31.2 C  H=75.2%  Light=101
[POST] OK #1157
[SENSOR] T=31.2 C  H=75.1%  Light=95
[POST] OK #1158
[SENSOR] T=31.1 C  H=75.2%  Light=98
[POST] OK #1159
[SENSOR] T=31.1 C  H=75.2%  Light=88
[POST] OK #1160
[SENSOR] T=31.1 C  H=75.3%  Light=100
[POST] OK #1161
[SENSOR] T=31.0 C  H=75.4%  Light=96
[POST] OK #1162
[SENSOR] T=31.0 C  H=75.5%  Light=88
[POST] OK #1163
[SENSOR] T=31.0 C  H=75.5%  Light=84
[POST] OK #1164
[SENSOR] T=31.0 C  H=75.5%  Light=93
[POST] OK #1165
[SENSOR] T=31.0 C  H=75.6%  Light=98
[POST] OK #1166
[SENSOR] T=31.0 C  H=75.6%  Light=93
[POST] OK #1167
[SENSOR] T=31.0 C  H=75.6%  Light=98
[POST] OK #1168
[SENSOR] T=31.0 C  H=75.6%  Light=94
[POST] OK #1169
[SENSOR] T=31.0 C  H=75.5%  Light=82
[POST] OK #1170
[SENSOR] T=31.1 C  H=75.5%  Light=88
[POST] OK #1171
[SENSOR] T=31.1 C  H=75.5%  Light=94
[POST] OK #1172
[SENSOR] T=31.1 C  H=75.4%  Light=98
[POST] OK #1173
[SENSOR] T=31.1 C  H=75.4%  Light=94
[POST] OK #1174
[SENSOR] T=31.1 C  H=75.5%  Light=88
[POST] OK #1175
[SENSOR] T=31.1 C  H=75.4%  Light=104
[POST] OK #1176
[SENSOR] T=31.1 C  H=75.4%  Light=91
[POST] OK #1177
[SENSOR] T=31.1 C  H=75.4%  Light=74
[POST] OK #1178
[SENSOR] T=31.1 C  H=75.4%  Light=76
[POST] OK #1179
[SENSOR] T=31.0 C  H=75.4%  Light=89
[POST] OK #1180
[SENSOR] T=31.1 C  H=75.4%  Light=99
[POST] OK #1181
[SENSOR] T=31.1 C  H=75.4%  Light=94
[POST] OK #1182
[SENSOR] T=31.0 C  H=75.5%  Light=92
[POST] OK #1183
[SENSOR] T=31.0 C  H=75.5%  Light=97
[POST] OK #1184
[SENSOR] T=31.0 C  H=75.5%  Light=97
[POST] OK #1185
[SENSOR] T=31.0 C  H=75.5%  Light=88
[POST] OK #1186
[SENSOR] T=31.0 C  H=75.6%  Light=97
[POST] OK #1187
[SENSOR] T=31.0 C  H=75.7%  Light=78
[POST] OK #1188
[SENSOR] T=30.9 C  H=75.9%  Light=93
[POST] OK #1189
[SENSOR] T=30.9 C  H=75.9%  Light=96
[POST] OK #1190
[SENSOR] T=30.9 C  H=75.8%  Light=102
[POST] OK #1191
[SENSOR] T=30.9 C  H=75.8%  Light=101
[POST] OK #1192
[SENSOR] T=30.9 C  H=75.8%  Light=96
[POST] OK #1193
[SENSOR] T=30.9 C  H=75.8%  Light=89
[POST] OK #1194
[SENSOR] T=31.0 C  H=75.7%  Light=69
[POST] OK #1195
[SENSOR] T=31.0 C  H=75.7%  Light=96
[POST] OK #1196
[SENSOR] T=31.0 C  H=75.6%  Light=94
[POST] OK #1197
[SENSOR] T=31.0 C  H=75.7%  Light=90
[POST] OK #1198
[SENSOR] T=31.0 C  H=75.5%  Light=84
[POST] OK #1199
[SENSOR] T=31.0 C  H=75.6%  Light=95
[POST] OK #1200
[SENSOR] T=31.0 C  H=75.5%  Light=94
[POST] OK #1201
[SENSOR] T=31.0 C  H=75.7%  Light=95
[POST] OK #1202
[SENSOR] T=31.0 C  H=75.6%  Light=91
[POST] OK #1203
[SENSOR] T=31.0 C  H=75.6%  Light=93
[POST] OK #1204
[SENSOR] T=31.1 C  H=75.6%  Light=92
[POST] OK #1205
[SENSOR] T=31.1 C  H=75.6%  Light=89
[POST] OK #1206
[SENSOR] T=31.0 C  H=75.5%  Light=86
[POST] OK #1207
[SENSOR] T=31.0 C  H=75.6%  Light=91
[POST] OK #1208
[SENSOR] T=31.0 C  H=75.5%  Light=93
[POST] OK #1209
[SENSOR] T=31.0 C  H=75.6%  Light=94
[POST] OK #1210
[SENSOR] T=31.0 C  H=75.6%  Light=89
[POST] OK #1211
[SENSOR] T=31.0 C  H=75.6%  Light=86
[POST] OK #1212
[SENSOR] T=31.0 C  H=75.6%  Light=81
[POST] OK #1213
[SENSOR] T=31.0 C  H=75.8%  Light=95
[POST] OK #1214
[SENSOR] T=30.9 C  H=75.8%  Light=106
[POST] OK #1215
[SENSOR] T=30.9 C  H=75.9%  Light=96
[POST] OK #1216
[SENSOR] T=30.8 C  H=76.0%  Light=94
[POST] OK #1217
[SENSOR] T=30.9 C  H=76.1%  Light=85
[POST] OK #1218
[SENSOR] T=30.9 C  H=76.0%  Light=95
[POST] OK #1219
[SENSOR] T=30.8 C  H=76.0%  Light=97
[POST] OK #1220
[SENSOR] T=30.9 C  H=76.0%  Light=96
[POST] OK #1221
[SENSOR] T=30.8 C  H=76.1%  Light=97
[POST] OK #1222
[SENSOR] T=30.9 C  H=76.1%  Light=87
[POST] OK #1223
[SENSOR] T=30.9 C  H=76.1%  Light=96
[POST] OK #1224
[SENSOR] T=30.9 C  H=76.1%  Light=86
[POST] OK #1225
[SENSOR] T=30.9 C  H=76.0%  Light=102
[POST] OK #1226
[SENSOR] T=30.9 C  H=76.0%  Light=95
[POST] OK #1227
[SENSOR] T=30.8 C  H=76.0%  Light=96
[POST] OK #1228
[SENSOR] T=30.9 C  H=76.0%  Light=99
[POST] OK #1229
[SENSOR] T=30.9 C  H=75.9%  Light=90
[POST] OK #1230
[SENSOR] T=30.9 C  H=75.9%  Light=77
[POST] OK #1231
[SENSOR] T=30.9 C  H=75.9%  Light=87
[POST] OK #1232
[SENSOR] T=30.8 C  H=76.0%  Light=88
[POST] OK #1233
[SENSOR] T=30.8 C  H=76.1%  Light=91
[POST] OK #1234
[SENSOR] T=30.8 C  H=76.2%  Light=92
[POST] OK #1235
[SENSOR] T=30.8 C  H=76.3%  Light=79
[POST] OK #1236
[SENSOR] T=30.8 C  H=76.3%  Light=72
[POST] OK #1237
[SENSOR] T=30.8 C  H=76.2%  Light=94
[POST] OK #1238
[SENSOR] T=30.8 C  H=76.2%  Light=87
[POST] OK #1239
[SENSOR] T=30.8 C  H=76.3%  Light=86
[POST] OK #1240
[SENSOR] T=30.8 C  H=76.2%  Light=92
[POST] OK #1241
[SENSOR] T=30.8 C  H=76.3%  Light=92
[POST] OK #1242
[SENSOR] T=30.8 C  H=76.3%  Light=98
[POST] OK #1243
[SENSOR] T=30.8 C  H=76.2%  Light=92
[POST] OK #1244
[SENSOR] T=30.8 C  H=76.2%  Light=98
[POST] OK #1245
[SENSOR] T=30.8 C  H=76.2%  Light=94
[POST] OK #1246
[SENSOR] T=30.8 C  H=76.1%  Light=91
[POST] OK #1247
[SENSOR] T=30.8 C  H=76.0%  Light=93
[POST] OK #1248
[SENSOR] T=30.8 C  H=76.0%  Light=95
[POST] OK #1249
[SENSOR] T=30.9 C  H=75.9%  Light=97
[POST] OK #1250
[SENSOR] T=30.9 C  H=75.9%  Light=84
[POST] OK #1251
[SENSOR] T=30.9 C  H=75.9%  Light=85
[POST] OK #1252
[SENSOR] T=30.9 C  H=76.1%  Light=83
[POST] OK #1253
[SENSOR] T=30.8 C  H=76.1%  Light=90
[POST] OK #1254
[SENSOR] T=30.8 C  H=76.0%  Light=99
[POST] OK #1255
[SENSOR] T=30.8 C  H=76.0%  Light=94
[POST] OK #1256
[SENSOR] T=30.8 C  H=76.1%  Light=101
[POST] OK #1257
[SENSOR] T=30.9 C  H=76.0%  Light=95
[POST] OK #1258
[SENSOR] T=30.9 C  H=75.9%  Light=59
[POST] OK #1259
[SENSOR] T=30.9 C  H=75.9%  Light=65
[POST] OK #1260
[SENSOR] T=31.0 C  H=75.8%  Light=0
[POST] OK #1261
[SENSOR] T=31.0 C  H=75.8%  Light=2
[POST] OK #1262
[SENSOR] T=31.0 C  H=75.8%  Light=16
[POST] OK #1263
[SENSOR] T=31.0 C  H=75.7%  Light=56
[POST] OK #1264
[SENSOR] T=31.1 C  H=75.7%  Light=75
[POST] OK #1265
[SENSOR] T=31.0 C  H=75.7%  Light=22
[POST] OK #1266
[SENSOR] T=31.1 C  H=75.6%  Light=36
[POST] OK #1267
[SENSOR] T=31.1 C  H=75.6%  Light=34
[POST] OK #1268
[SENSOR] T=31.1 C  H=75.7%  Light=0
[POST] OK #1269
[SENSOR] T=31.2 C  H=75.7%  Light=12
[POST] OK #1270
[SENSOR] T=31.1 C  H=75.6%  Light=62
[POST] OK #1271
[SENSOR] T=31.1 C  H=75.4%  Light=29
[POST] OK #1272
[SENSOR] T=31.2 C  H=75.5%  Light=68
[POST] OK #1273
[SENSOR] T=31.1 C  H=75.5%  Light=58
[POST] OK #1274
[SENSOR] T=31.1 C  H=75.4%  Light=56
[POST] OK #1275
[SENSOR] T=31.1 C  H=75.6%  Light=62
[POST] OK #1276
[SENSOR] T=31.2 C  H=75.6%  Light=32
[POST] OK #1277
[SENSOR] T=31.2 C  H=75.7%  Light=35
[POST] OK #1278
[SENSOR] T=31.1 C  H=75.7%  Light=66
[POST] OK #1279
[SENSOR] T=31.1 C  H=75.7%  Light=55
[POST] OK #1280
[SENSOR] T=31.1 C  H=75.8%  Light=44
[POST] OK #1281
[SENSOR] T=31.1 C  H=75.7%  Light=61
[POST] OK #1282
[SENSOR] T=31.0 C  H=75.8%  Light=38
[POST] OK #1283
[SENSOR] T=31.0 C  H=75.8%  Light=58
[POST] OK #1284
[SENSOR] T=31.0 C  H=75.9%  Light=54
[POST] OK #1285
����������������������������������������������������������������t=32
[POST] OK #1286
[SENSOR] T=31.0 C  H=76.0%  Light=50
[POST] OK #1287
[SENSOR] T=31.0 C  H=76.0%  Light=74
[POST] OK #1288
[SENSOR] T=31.0 C  H=76.0%  Light=85
[POST] OK #1289
[SENSOR] T=31.0 C  H=75.9%  Light=76
[POST] OK #1290
[SENSOR] T=31.0 C  H=76.0%  Light=70
[POST] OK #1291
[SENSOR] T=31.0 C  H=76.0%  Light=70
[POST] OK #1292
[SENSOR] T=31.0 C  H=76.0%  Light=71
[POST] OK #1293
[SENSOR] T=31.0 C  H=76.0%  Light=38
[POST] OK #1294
[SENSOR] T=31.0 C  H=76.0%  Light=52
[POST] OK #1295
[SENSOR] T=31.0 C  H=75.9%  Light=61
[POST] OK #1296
[SENSOR] T=31.0 C  H=75.9%  Light=81
[POST] OK #1297
[SENSOR] T=31.0 C  H=76.0%  Light=80
[POST] OK #1298
[SENSOR] T=31.0 C  H=76.1%  Light=69
[POST] OK #1299
[SENSOR] T=31.0 C  H=76.2%  Light=49
[POST] OK #1300
[SENSOR] T=31.0 C  H=76.2%  Light=28
[POST] OK #1301
[SENSOR] T=31.0 C  H=76.2%  Light=70
[POST] OK #1302
[SENSOR] T=31.0 C  H=76.3%  Light=75
[POST] OK #1303
[SENSOR] T=31.0 C  H=76.2%  Light=69
[POST] OK #1304
[SENSOR] T=31.0 C  H=76.3%  Light=70
[POST] OK #1305
[SENSOR] T=31.0 C  H=76.2%  Light=74
[POST] OK #1306
[SENSOR] T=31.0 C  H=76.3%  Light=79
[POST] OK #1307
[SENSOR] T=31.0 C  H=76.2%  Light=8
[POST] OK #1308
[SENSOR] T=31.0 C  H=76.2%  Light=73
[POST] OK #1309
[SENSOR] T=31.0 C  H=76.1%  Light=72
[POST] OK #1310
[SENSOR] T=31.0 C  H=76.2%  Light=20
[POST] OK #1311
[SENSOR] T=31.0 C  H=76.2%  Light=54
[POST] OK #1312
[SENSOR] T=31.1 C  H=76.2%  Light=62
[POST] OK #1313
[SENSOR] T=31.1 C  H=76.2%  Light=77
[POST] OK #1314
[SENSOR] T=31.1 C  H=76.1%  Light=15
[POST] OK #1315
[SENSOR] T=31.1 C  H=76.1%  Light=15
[POST] OK #1316
[SENSOR] T=31.1 C  H=76.0%  Light=21
[POST] OK #1317
[SENSOR] T=31.1 C  H=76.0%  Light=26
[POST] OK #1318
[SENSOR] T=31.1 C  H=76.0%  Light=27
[POST] OK #1319
[SENSOR] T=31.2 C  H=76.0%  Light=10
[POST] OK #1320
[SENSOR] T=31.1 C  H=76.0%  Light=17
[POST] OK #1321
[SENSOR] T=31.2 C  H=75.9%  Light=23
[POST] OK #1322
[SENSOR] T=31.2 C  H=76.0%  Light=31
[POST] OK #1323
[SENSOR] T=31.2 C  H=76.0%  Light=28
[POST] OK #1324
[SENSOR] T=31.2 C  H=76.0%  Light=79
[POST] OK #1325
[SENSOR] T=31.2 C  H=76.0%  Light=38
[POST] OK #1326
[SENSOR] T=31.1 C  H=76.0%  Light=21
[POST] OK #1327
[SENSOR] T=31.2 C  H=76.0%  Light=10
[POST] OK #1328
[SENSOR] T=31.2 C  H=75.9%  Light=64
[POST] OK #1329
[SENSOR] T=31.2 C  H=75.9%  Light=34
[POST] OK #1330
[SENSOR] T=31.2 C  H=76.0%  Light=77
[POST] OK #1331
[SENSOR] T=31.2 C  H=75.9%  Light=35
[POST] OK #1332
[SENSOR] T=31.2 C  H=76.0%  Light=32
[POST] OK #1333
[SENSOR] T=31.2 C  H=75.9%  Light=29
[POST] OK #1334
[SENSOR] T=31.2 C  H=76.0%  Light=30
[POST] OK #1335
[SENSOR] T=31.2 C  H=76.0%  Light=78
[POST] OK #1336
[SENSOR] Read error #1. Skipping POST.
[SENSOR] T=31.2 C  H=76.0%  Light=75
[POST] OK #1337
[SENSOR] T=31.2 C  H=76.0%  Light=75
[POST] OK #1338
[SENSOR] T=31.2 C  H=76.1%  Light=61
[POST] OK #1339
[SENSOR] T=31.2 C  H=76.1%  Light=76
[POST] OK #1340
[SENSOR] T=31.2 C  H=76.1%  Light=76
[POST] OK #1341
[SENSOR] T=31.2 C  H=76.2%  Light=79
[POST] OK #1342
[SENSOR] T=31.3 C  H=76.2%  Light=77
[POST] OK #1343
[SENSOR] T=31.3 C  H=76.3%  Light=76
[POST] OK #1344
[SENSOR] T=31.3 C  H=76.3%  Light=75
[POST] OK #1345
[SENSOR] T=31.3 C  H=76.3%  Light=75
[POST] OK #1346
[SENSOR] T=31.3 C  H=76.4%  Light=78
[POST] OK #1347
[SENSOR] T=31.3 C  H=76.5%  Light=77
[POST] OK #1348
[SENSOR] T=31.3 C  H=76.5%  Light=76
[POST] OK #1349
[SENSOR] T=31.3 C  H=76.6%  Light=74
[POST] OK #1350
[SENSOR] T=31.3 C  H=76.6%  Light=78
[POST] OK #1351
[SENSOR] T=31.3 C  H=76.7%  Light=74
[POST] OK #1352
[SENSOR] T=31.3 C  H=76.7%  Light=74
[POST] OK #1353
[SENSOR] T=31.3 C  H=76.7%  Light=77
[POST] OK #1354
[SENSOR] T=31.3 C  H=76.8%  Light=78
[POST] OK #1355
[SENSOR] T=31.3 C  H=76.8%  Light=77
[POST] OK #1356
[SENSOR] T=31.3 C  H=76.9%  Light=83
[POST] OK #1357
[SENSOR] T=31.3 C  H=77.0%  Light=76
[POST] OK #1358
[SENSOR] T=31.3 C  H=77.0%  Light=38
[POST] OK #1359
[SENSOR] T=31.3 C  H=77.0%  Light=79
[POST] OK #1360
[SENSOR] T=31.3 C  H=77.1%  Light=78
[POST] OK #1361
[SENSOR] T=31.3 C  H=77.1%  Light=47
[POST] OK #1362
[SENSOR] T=31.3 C  H=77.1%  Light=30
[POST] OK #1363
[SENSOR] T=31.3 C  H=77.1%  Light=32
[POST] OK #1364
[SENSOR] T=31.3 C  H=77.2%  Light=7
[POST] OK #1365
[SENSOR] T=31.3 C  H=77.2%  Light=4
[POST] OK #1366
[SENSOR] T=31.3 C  H=77.3%  Light=6
[POST] OK #1367
[SENSOR] T=31.3 C  H=77.3%  Light=23
[POST] OK #1368
[SENSOR] T=31.3 C  H=77.3%  Light=36
[POST] OK #1369
[SENSOR] T=31.3 C  H=77.4%  Light=39
[POST] OK #1370
[SENSOR] T=31.4 C  H=77.4%  Light=14
[POST] OK #1371
[SENSOR] T=31.3 C  H=77.4%  Light=17
[POST] OK #1372
[SENSOR] T=31.4 C  H=77.4%  Light=79
[POST] OK #1373
[SENSOR] T=31.4 C  H=77.5%  Light=77
[POST] OK #1374
[SENSOR] T=31.3 C  H=77.5%  Light=75
[POST] OK #1375
[SENSOR] T=31.4 C  H=77.5%  Light=78
[POST] OK #1376
[SENSOR] T=31.3 C  H=77.6%  Light=81
[POST] OK #1377
[SENSOR] T=31.4 C  H=77.6%  Light=84
[POST] OK #1378
[SENSOR] T=31.4 C  H=77.6%  Light=77
[POST] OK #1379
[SENSOR] T=31.4 C  H=77.7%  Light=3
[POST] OK #1380
[SENSOR] T=31.4 C  H=77.7%  Light=29
[POST] OK #1381
[SENSOR] T=31.4 C  H=77.7%  Light=25
[POST] OK #1382
[SENSOR] T=31.4 C  H=77.7%  Light=13
[POST] OK #1383
[SENSOR] T=31.4 C  H=77.7%  Light=21
[POST] OK #1384
[SENSOR] T=31.4 C  H=77.8%  Light=73
[POST] OK #1385
[SENSOR] T=31.4 C  H=77.9%  Light=78
[POST] OK #1386
[SENSOR] T=31.4 C  H=77.8%  Light=75
[POST] OK #1387
[SENSOR] T=31.4 C  H=77.9%  Light=78
[POST] OK #1388
[SENSOR] T=31.4 C  H=77.9%  Light=78
[POST] OK #1389
[SENSOR] T=31.4 C  H=78.0%  Light=66
[POST] OK #1390
[SENSOR] T=31.4 C  H=78.0%  Light=67
[POST] OK #1391
[SENSOR] T=31.4 C  H=78.0%  Light=71
[POST] OK #1392
[SENSOR] T=31.4 C  H=78.0%  Light=79
[POST] OK #1393
[SENSOR] T=31.4 C  H=78.1%  Light=81
[POST] OK #1394
[SENSOR] T=31.4 C  H=78.1%  Light=78
[POST] OK #1395
[SENSOR] T=31.4 C  H=78.2%  Light=77
[POST] OK #1396
[SENSOR] T=31.4 C  H=78.1%  Light=74
[POST] OK #1397
[SENSOR] T=31.4 C  H=78.1%  Light=80
[POST] OK #1398
[SENSOR] T=31.4 C  H=78.2%  Light=78
[POST] OK #1399
[SENSOR] T=31.4 C  H=78.2%  Light=77
[POST] OK #1400
[SENSOR] T=31.4 C  H=78.2%  Light=71
[POST] OK #1401
[SENSOR] T=31.4 C  H=78.2%  Light=78
[POST] OK #1402
[SENSOR] T=31.4 C  H=78.2%  Light=74
[POST] OK #1403
[SENSOR] T=31.4 C  H=78.2%  Light=79
[POST] OK #1404
[SENSOR] T=31.4 C  H=78.3%  Light=75
[POST] OK #1405
[SENSOR] T=31.4 C  H=78.3%  Light=75
[POST] OK #1406
[SENSOR] T=31.4 C  H=78.3%  Light=77
[POST] OK #1407
[SENSOR] T=31.4 C  H=78.4%  Light=77
[POST] OK #1408
[SENSOR] T=31.4 C  H=78.4%  Light=78
[POST] OK #1409
[SENSOR] T=31.4 C  H=78.4%  Light=73
[POST] OK #1410
[SENSOR] T=31.4 C  H=78.4%  Light=78
[POST] OK #1411
[SENSOR] T=31.4 C  H=78.4%  Light=79
[POST] OK #1412
[SENSOR] T=31.4 C  H=78.5%  Light=80
[POST] OK #1413
[SENSOR] T=31.4 C  H=78.5%  Light=80
[POST] OK #1414
[SENSOR] T=31.4 C  H=78.5%  Light=78
[POST] OK #1415
[SENSOR] T=31.4 C  H=78.5%  Light=78
[POST] OK #1416
[SENSOR] T=31.4 C  H=78.4%  Light=73
[POST] OK #1417
[SENSOR] T=31.4 C  H=78.6%  Light=82
[POST] OK #1418
[SENSOR] T=31.4 C  H=78.6%  Light=77
[POST] OK #1419
[SENSOR] T=31.4 C  H=78.6%  Light=69
[POST] OK #1420
[SENSOR] T=31.4 C  H=78.6%  Light=79
[POST] OK #1421
[SENSOR] T=31.4 C  H=78.6%  Light=78
[POST] OK #1422
[SENSOR] T=31.4 C  H=78.7%  Light=78
[POST] OK #1423
����������������������������������������������������������������t=76
[POST] OK #1424
[SENSOR] T=31.4 C  H=78.6%  Light=82
[POST] OK #1425
[SENSOR] T=31.4 C  H=78.7%  Light=88
[POST] OK #1426
[SENSOR] T=31.4 C  H=78.6%  Light=76
[POST] OK #1427
[SENSOR] T=31.4 C  H=78.7%  Light=79
[POST] OK #1428
[SENSOR] T=31.4 C  H=78.8%  Light=71
[POST] OK #1429
[SENSOR] T=31.4 C  H=78.8%  Light=72
[POST] OK #1430
[SENSOR] T=31.4 C  H=78.8%  Light=83
[POST] OK #1431
[SENSOR] T=31.4 C  H=78.8%  Light=75
[POST] OK #1432
[SENSOR] T=31.4 C  H=78.8%  Light=77
[POST] OK #1433
[SENSOR] T=31.4 C  H=78.9%  Light=78
[POST] OK #1434
[SENSOR] T=31.4 C  H=78.9%  Light=70
[POST] OK #1435
[SENSOR] T=31.4 C  H=78.9%  Light=54
[POST] OK #1436
[SENSOR] T=31.4 C  H=78.9%  Light=79
[POST] OK #1437
[SENSOR] T=31.4 C  H=78.9%  Light=75
[POST] OK #1438
[SENSOR] T=31.4 C  H=78.9%  Light=77
[POST] OK #1439
[SENSOR] T=31.4 C  H=79.0%  Light=75
[POST] OK #1440
[SENSOR] T=31.4 C  H=79.0%  Light=79
[POST] OK #1441
[SENSOR] T=31.4 C  H=78.9%  Light=75
[POST] OK #1442
[SENSOR] T=31.4 C  H=79.0%  Light=82
[POST] OK #1443
[SENSOR] T=31.4 C  H=78.9%  Light=76
[POST] OK #1444
[SENSOR] T=31.4 C  H=79.0%  Light=79
[POST] OK #1445
[SENSOR] T=31.4 C  H=79.0%  Light=72
[POST] OK #1446
[SENSOR] T=31.4 C  H=79.0%  Light=80
[POST] OK #1447
[SENSOR] T=31.4 C  H=79.0%  Light=74
[POST] OK #1448
[SENSOR] T=31.4 C  H=79.1%  Light=75
[POST] OK #1449
[SENSOR] T=31.4 C  H=79.0%  Light=74
[POST] OK #1450
[SENSOR] T=31.4 C  H=79.0%  Light=72
[POST] OK #1451
[SENSOR] T=31.4 C  H=79.0%  Light=75
[POST] OK #1452
[SENSOR] T=31.4 C  H=79.1%  Light=69
[POST] OK #1453
[SENSOR] T=31.4 C  H=79.1%  Light=77
[POST] OK #1454
[SENSOR] T=31.4 C  H=79.1%  Light=29
[POST] OK #1455
[SENSOR] T=31.4 C  H=79.1%  Light=75
[POST] OK #1456
[SENSOR] T=31.4 C  H=79.1%  Light=77
[POST] OK #1457
[SENSOR] T=31.4 C  H=79.3%  Light=77
[POST] OK #1458
[SENSOR] T=31.4 C  H=79.2%  Light=77
[POST] OK #1459
[SENSOR] T=31.4 C  H=79.3%  Light=75
[POST] OK #1460
[SENSOR] T=31.4 C  H=79.3%  Light=76
[POST] OK #1461
[SENSOR] T=31.4 C  H=79.3%  Light=84
[POST] OK #1462
[SENSOR] T=31.4 C  H=79.3%  Light=77
[POST] OK #1463
[SENSOR] T=31.4 C  H=79.3%  Light=78
[POST] OK #1464
[SENSOR] T=31.4 C  H=79.3%  Light=76
[POST] OK #1465
[SENSOR] T=31.4 C  H=79.4%  Light=80
[POST] OK #1466
[SENSOR] T=31.4 C  H=79.3%  Light=84
[POST] OK #1467
[SENSOR] T=31.4 C  H=79.4%  Light=77
[POST] OK #1468
[SENSOR] T=31.4 C  H=79.4%  Light=72
[POST] OK #1469
[SENSOR] T=31.4 C  H=79.4%  Light=78
[POST] OK #1470
[SENSOR] T=31.4 C  H=79.4%  Light=85
[POST] OK #1471
[SENSOR] T=31.4 C  H=79.5%  Light=78
[POST] OK #1472
[SENSOR] T=31.4 C  H=79.5%  Light=77
[POST] OK #1473
[SENSOR] T=31.4 C  H=79.5%  Light=77
[POST] OK #1474
[SENSOR] T=31.4 C  H=79.5%  Light=76
[POST] OK #1475
[SENSOR] T=31.4 C  H=79.4%  Light=76
[POST] OK #1476
[SENSOR] T=31.4 C  H=79.6%  Light=76
[POST] OK #1477
[SENSOR] T=31.4 C  H=79.5%  Light=79
[POST] OK #1478
[SENSOR] T=31.4 C  H=79.6%  Light=77
[POST] OK #1479
[SENSOR] T=31.4 C  H=79.6%  Light=81
[POST] OK #1480
[SENSOR] T=31.4 C  H=79.6%  Light=76
[POST] OK #1481
[SENSOR] T=31.4 C  H=79.6%  Light=79
[POST] OK #1482
[SENSOR] T=31.4 C  H=79.6%  Light=86
[POST] OK #1483
[SENSOR] T=31.4 C  H=79.6%  Light=70
[POST] OK #1484
[SENSOR] T=31.4 C  H=79.6%  Light=76
[POST] OK #1485
[SENSOR] T=31.4 C  H=79.6%  Light=76
[POST] OK #1486
[SENSOR] T=31.4 C  H=79.7%  Light=79
[POST] OK #1487
[SENSOR] T=31.4 C  H=79.7%  Light=82
[POST] OK #1488
[SENSOR] T=31.4 C  H=79.6%  Light=78
[POST] OK #1489
[SENSOR] T=31.4 C  H=79.7%  Light=76
[POST] OK #1490
[SENSOR] T=31.4 C  H=79.6%  Light=78
[POST] OK #1491
[SENSOR] T=31.4 C  H=79.7%  Light=78
[POST] OK #1492
[SENSOR] T=31.4 C  H=79.7%  Light=75
[POST] OK #1493
[SENSOR] T=31.4 C  H=79.7%  Light=76
[POST] OK #1494
[SENSOR] T=31.4 C  H=79.7%  Light=79
[POST] OK #1495
[SENSOR] T=31.4 C  H=79.7%  Light=82
[POST] OK #1496
[SENSOR] T=31.4 C  H=79.7%  Light=74
[POST] OK #1497
[SENSOR] T=31.4 C  H=79.8%  Light=77
[POST] OK #1498
[SENSOR] T=31.4 C  H=79.8%  Light=77
[POST] OK #1499
[SENSOR] T=31.4 C  H=79.8%  Light=74
[POST] OK #1500
[SENSOR] T=31.4 C  H=79.8%  Light=80
[POST] OK #1501
[SENSOR] T=31.4 C  H=79.8%  Light=72
[POST] OK #1502
[SENSOR] T=31.4 C  H=79.7%  Light=77
[POST] OK #1503
[SENSOR] T=31.4 C  H=79.8%  Light=78
[POST] OK #1504
[SENSOR] T=31.4 C  H=79.8%  Light=81
[POST] OK #1505
[SENSOR] T=31.4 C  H=79.8%  Light=72
[POST] OK #1506
[SENSOR] T=31.4 C  H=79.8%  Light=77
[POST] OK #1507
[SENSOR] T=31.4 C  H=79.8%  Light=77
[POST] OK #1508
[SENSOR] T=31.4 C  H=79.9%  Light=77
[POST] OK #1509
[SENSOR] T=31.4 C  H=79.8%  Light=76
[POST] OK #1510
[SENSOR] T=31.4 C  H=79.8%  Light=76
[POST] OK #1511
[SENSOR] T=31.4 C  H=79.9%  Light=80
[POST] OK #1512
[SENSOR] T=31.3 C  H=79.9%  Light=85
[POST] OK #1513
[SENSOR] T=31.4 C  H=79.9%  Light=75
[POST] OK #1514
[SENSOR] T=31.3 C  H=79.9%  Light=74
[POST] OK #1515
[SENSOR] T=31.4 C  H=79.9%  Light=77
[POST] OK #1516
[SENSOR] T=31.4 C  H=79.8%  Light=75
[POST] OK #1517
[SENSOR] T=31.3 C  H=79.9%  Light=81
[POST] OK #1518
[SENSOR] T=31.4 C  H=79.9%  Light=69
[POST] OK #1519
[SENSOR] T=31.3 C  H=79.9%  Light=80
[POST] OK #1520
[SENSOR] T=31.3 C  H=79.9%  Light=80
[POST] OK #1521
[SENSOR] T=31.4 C  H=79.9%  Light=77
[POST] OK #1522
[SENSOR] T=31.4 C  H=80.0%  Light=77
[POST] OK #1523
[SENSOR] T=31.3 C  H=79.9%  Light=82
[POST] OK #1524
[SENSOR] T=31.4 C  H=79.9%  Light=78
[POST] OK #1525
[SENSOR] T=31.3 C  H=80.0%  Light=76
[POST] OK #1526
[SENSOR] T=31.3 C  H=79.9%  Light=63
[POST] OK #1527
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1528
[SENSOR] T=31.3 C  H=79.9%  Light=68
[POST] OK #1529
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1530
[SENSOR] T=31.4 C  H=80.0%  Light=70
[POST] OK #1531
[SENSOR] T=31.3 C  H=79.9%  Light=76
[POST] OK #1532
[SENSOR] T=31.4 C  H=79.9%  Light=77
[POST] OK #1533
[SENSOR] T=31.4 C  H=80.0%  Light=77
[POST] OK #1534
[SENSOR] T=31.4 C  H=79.9%  Light=86
[POST] OK #1535
[SENSOR] T=31.3 C  H=79.9%  Light=77
[POST] OK #1536
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1537
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1538
[SENSOR] T=31.3 C  H=80.0%  Light=76
[POST] OK #1539
[SENSOR] T=31.4 C  H=80.0%  Light=83
[POST] OK #1540
[SENSOR] T=31.4 C  H=79.9%  Light=76
[POST] OK #1541
[SENSOR] T=31.4 C  H=79.9%  Light=77
[POST] OK #1542
[SENSOR] T=31.4 C  H=80.0%  Light=71
[POST] OK #1543
[SENSOR] T=31.4 C  H=80.0%  Light=81
[POST] OK #1544
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1545
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1546
[SENSOR] T=31.4 C  H=80.0%  Light=77
[POST] OK #1547
[SENSOR] T=31.4 C  H=80.0%  Light=83
[POST] OK #1548
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1549
[SENSOR] T=31.4 C  H=79.9%  Light=78
[POST] OK #1550
[SENSOR] T=31.4 C  H=79.9%  Light=70
[POST] OK #1551
[SENSOR] T=31.4 C  H=79.9%  Light=76
[POST] OK #1552
[SENSOR] T=31.3 C  H=79.9%  Light=75
[POST] OK #1553
[SENSOR] T=31.4 C  H=79.9%  Light=79
[POST] OK #1554
[SENSOR] T=31.4 C  H=79.9%  Light=75
[POST] OK #1555
[SENSOR] T=31.4 C  H=79.9%  Light=76
[POST] OK #1556
[SENSOR] T=31.4 C  H=79.8%  Light=76
[POST] OK #1557
[SENSOR] T=31.4 C  H=79.9%  Light=78
[POST] OK #1558
[SENSOR] T=31.4 C  H=79.9%  Light=73
[POST] OK #1559
[SENSOR] T=31.4 C  H=79.9%  Light=78
[POST] OK #1560
[SENSOR] T=31.3 C  H=79.9%  Light=77
[POST] OK #1561
[SENSOR] T=31.4 C  H=79.9%  Light=71
[POST] OK #1562
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1563
[SENSOR] T=31.4 C  H=79.9%  Light=83
[POST] OK #1564
[SENSOR] T=31.4 C  H=80.0%  Light=80
[POST] OK #1565
[SENSOR] T=31.4 C  H=79.9%  Light=79
[POST] OK #1566
[SENSOR] T=31.4 C  H=79.9%  Light=76
[POST] OK #1567
[SENSOR] T=31.4 C  H=79.8%  Light=76
[POST] OK #1568
[SENSOR] T=31.4 C  H=79.9%  Light=59
[POST] OK #1569
[SENSOR] T=31.4 C  H=79.9%  Light=69
[POST] OK #1570
[SENSOR] T=31.4 C  H=79.9%  Light=76
[POST] OK #1571
[SENSOR] T=31.4 C  H=79.9%  Light=83
[POST] OK #1572
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1573
[SENSOR] T=31.4 C  H=79.9%  Light=81
[POST] OK #1574
[SENSOR] T=31.4 C  H=79.9%  Light=76
[POST] OK #1575
[SENSOR] T=31.4 C  H=79.9%  Light=77
[POST] OK #1576
[SENSOR] T=31.4 C  H=79.9%  Light=81
[POST] OK #1577
[SENSOR] T=31.4 C  H=79.9%  Light=78
[POST] OK #1578
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1579
[SENSOR] T=31.4 C  H=80.0%  Light=81
[POST] OK #1580
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1581
[SENSOR] T=31.4 C  H=80.0%  Light=75
[POST] OK #1582
[SENSOR] T=31.4 C  H=80.0%  Light=82
[POST] OK #1583
[SENSOR] T=31.4 C  H=80.0%  Light=75
[POST] OK #1584
[SENSOR] T=31.4 C  H=80.0%  Light=82
[POST] OK #1585
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1586
[SENSOR] T=31.4 C  H=80.0%  Light=75
[POST] OK #1587
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1588
[SENSOR] T=31.4 C  H=80.0%  Light=83
[POST] OK #1589
[SENSOR] T=31.4 C  H=80.0%  Light=80
[POST] OK #1590
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1591
[SENSOR] T=31.4 C  H=80.1%  Light=83
[POST] OK #1592
[SENSOR] T=31.4 C  H=80.0%  Light=66
[POST] OK #1593
[SENSOR] T=31.4 C  H=80.0%  Light=66
[POST] OK #1594
[SENSOR] T=31.4 C  H=80.0%  Light=74
[POST] OK #1595
[SENSOR] T=31.4 C  H=80.0%  Light=72
[POST] OK #1596
[SENSOR] T=31.4 C  H=80.0%  Light=82
[POST] OK #1597
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1598
[SENSOR] T=31.4 C  H=80.1%  Light=76
[POST] OK #1599
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1600
[SENSOR] T=31.4 C  H=80.1%  Light=72
[POST] OK #1601
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1602
[SENSOR] T=31.4 C  H=80.0%  Light=81
[POST] OK #1603
[SENSOR] T=31.4 C  H=80.1%  Light=76
[POST] OK #1604
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1605
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1606
[SENSOR] T=31.4 C  H=80.0%  Light=73
[POST] OK #1607
[SENSOR] T=31.4 C  H=80.1%  Light=77
[POST] OK #1608
[SENSOR] T=31.4 C  H=80.0%  Light=83
[POST] OK #1609
[SENSOR] T=31.4 C  H=80.1%  Light=76
[POST] OK #1610
[SENSOR] T=31.4 C  H=80.0%  Light=70
[POST] OK #1611
[SENSOR] T=31.4 C  H=80.1%  Light=79
[POST] OK #1612
[SENSOR] T=31.4 C  H=80.0%  Light=77
[POST] OK #1613
[SENSOR] T=31.4 C  H=80.0%  Light=80
[POST] OK #1614
[SENSOR] T=31.4 C  H=80.1%  Light=81
[POST] OK #1615
[SENSOR] T=31.4 C  H=80.1%  Light=77
[POST] OK #1616
[SENSOR] T=31.4 C  H=80.1%  Light=77
[POST] OK #1617
[SENSOR] T=31.4 C  H=80.0%  Light=81
[POST] OK #1618
[SENSOR] T=31.4 C  H=80.1%  Light=82
[POST] OK #1619
[SENSOR] T=31.4 C  H=80.1%  Light=82
[POST] OK #1620
[SENSOR] T=31.4 C  H=80.1%  Light=77
[POST] OK #1621
[SENSOR] T=31.4 C  H=80.0%  Light=68
[POST] OK #1622
[SENSOR] T=31.4 C  H=80.1%  Light=72
[POST] OK #1623
[SENSOR] T=31.4 C  H=80.1%  Light=82
[POST] OK #1624
[SENSOR] T=31.4 C  H=80.1%  Light=73
[POST] OK #1625
[SENSOR] T=31.4 C  H=80.1%  Light=75
[POST] OK #1626
[SENSOR] T=31.4 C  H=80.1%  Light=72
[POST] OK #1627
[SENSOR] T=31.4 C  H=80.1%  Light=80
[POST] OK #1628
[SENSOR] T=31.4 C  H=80.1%  Light=78
[POST] OK #1629
[SENSOR] T=31.4 C  H=80.1%  Light=73
[POST] OK #1630
[SENSOR] T=31.4 C  H=80.1%  Light=86
[POST] OK #1631
[SENSOR] T=31.4 C  H=80.1%  Light=78
[POST] OK #1632
[SENSOR] T=31.4 C  H=80.1%  Light=77
[POST] OK #1633
[SENSOR] T=31.5 C  H=80.1%  Light=75
[POST] OK #1634
[SENSOR] T=31.4 C  H=80.1%  Light=84
[POST] OK #1635
[SENSOR] T=31.4 C  H=80.0%  Light=77
[POST] OK #1636
[SENSOR] T=31.4 C  H=80.1%  Light=70
[POST] OK #1637
[SENSOR] T=31.4 C  H=80.1%  Light=74
[POST] OK #1638
[SENSOR] T=31.4 C  H=80.1%  Light=80
[POST] OK #1639
[SENSOR] T=31.4 C  H=80.1%  Light=81
[POST] OK #1640
[SENSOR] T=31.4 C  H=80.1%  Light=80
[POST] OK #1641
[SENSOR] T=31.4 C  H=80.1%  Light=79
[POST] OK #1642
[SENSOR] T=31.5 C  H=80.1%  Light=62
[POST] OK #1643
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1644
[SENSOR] T=31.5 C  H=80.1%  Light=65
[POST] OK #1645
[SENSOR] T=31.4 C  H=80.1%  Light=38
[POST] OK #1646
[SENSOR] T=31.5 C  H=80.1%  Light=10
[POST] OK #1647
[SENSOR] T=31.4 C  H=80.1%  Light=79
[POST] OK #1648
[SENSOR] T=31.4 C  H=80.1%  Light=77
[POST] OK #1649
[SENSOR] T=31.4 C  H=80.2%  Light=79
[POST] OK #1650
[SENSOR] T=31.4 C  H=80.1%  Light=76
[POST] OK #1651
[SENSOR] T=31.4 C  H=80.1%  Light=78
[POST] OK #1652
[SENSOR] T=31.4 C  H=80.2%  Light=75
[POST] OK #1653
[SENSOR] T=31.4 C  H=80.1%  Light=84
[POST] OK #1654
[SENSOR] T=31.5 C  H=80.2%  Light=74
[POST] OK #1655
[SENSOR] T=31.4 C  H=80.2%  Light=79
[POST] OK #1656
[SENSOR] T=31.5 C  H=80.2%  Light=81
[POST] OK #1657
[SENSOR] T=31.5 C  H=80.2%  Light=77
[POST] OK #1658
[SENSOR] T=31.4 C  H=80.2%  Light=75
[POST] OK #1659
[SENSOR] T=31.4 C  H=80.2%  Light=78
[POST] OK #1660
[SENSOR] T=31.4 C  H=80.2%  Light=78
[POST] OK #1661
[SENSOR] T=31.4 C  H=80.2%  Light=76
[POST] OK #1662
[SENSOR] T=31.4 C  H=80.2%  Light=79
[POST] OK #1663
[SENSOR] T=31.5 C  H=80.2%  Light=79
[POST] OK #1664
[SENSOR] T=31.5 C  H=80.2%  Light=70
[POST] OK #1665
[SENSOR] T=31.4 C  H=80.2%  Light=67
[POST] OK #1666
[SENSOR] T=31.5 C  H=80.2%  Light=66
[POST] OK #1667
[SENSOR] T=31.4 C  H=80.3%  Light=67
[POST] OK #1668
[SENSOR] T=31.4 C  H=80.2%  Light=67
[POST] OK #1669
[SENSOR] T=31.4 C  H=80.3%  Light=66
[POST] OK #1670
[SENSOR] T=31.4 C  H=80.3%  Light=70
[POST] OK #1671
[SENSOR] T=31.4 C  H=80.2%  Light=86
[POST] OK #1672
[SENSOR] T=31.4 C  H=80.3%  Light=72
[POST] OK #1673
[SENSOR] T=31.5 C  H=80.2%  Light=69
[POST] OK #1674
[SENSOR] T=31.4 C  H=80.1%  Light=75
[POST] OK #1675
[SENSOR] T=31.4 C  H=80.1%  Light=79
[POST] OK #1676
[SENSOR] T=31.4 C  H=80.1%  Light=74
[POST] OK #1677
[SENSOR] T=31.4 C  H=80.1%  Light=80
[POST] OK #1678
[SENSOR] T=31.4 C  H=80.0%  Light=69
[POST] OK #1679
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1680
[SENSOR] T=31.4 C  H=80.0%  Light=31
[POST] OK #1681
[SENSOR] T=31.4 C  H=79.9%  Light=90
[POST] OK #1682
[SENSOR] T=31.4 C  H=79.9%  Light=24
[POST] OK #1683
[SENSOR] T=31.4 C  H=80.0%  Light=23
[POST] OK #1684
[SENSOR] T=31.4 C  H=79.9%  Light=26
[POST] OK #1685
[SENSOR] T=31.4 C  H=79.9%  Light=19
[POST] OK #1686
[SENSOR] T=31.3 C  H=79.9%  Light=72
[POST] OK #1687
[SENSOR] T=31.4 C  H=79.9%  Light=74
[POST] OK #1688
[SENSOR] T=31.3 C  H=79.9%  Light=81
[POST] OK #1689
[SENSOR] T=31.3 C  H=79.9%  Light=80
[POST] OK #1690
[SENSOR] T=31.3 C  H=79.9%  Light=41
[POST] OK #1691
[SENSOR] T=31.3 C  H=79.8%  Light=11
[POST] OK #1692
[SENSOR] T=31.3 C  H=79.9%  Light=67
[POST] OK #1693
[SENSOR] T=31.3 C  H=79.9%  Light=76
[POST] OK #1694
[SENSOR] T=31.3 C  H=79.9%  Light=43
[POST] OK #1695
[SENSOR] T=31.3 C  H=80.0%  Light=4
[POST] OK #1696
[SENSOR] T=31.3 C  H=79.9%  Light=12
[POST] OK #1697
[SENSOR] T=31.4 C  H=79.9%  Light=75
[POST] OK #1698
[SENSOR] T=31.4 C  H=79.9%  Light=78
[POST] OK #1699
[SENSOR] T=31.3 C  H=79.9%  Light=84
[POST] OK #1700
[SENSOR] T=31.4 C  H=79.9%  Light=82
[POST] OK #1701
[SENSOR] T=31.3 C  H=79.9%  Light=83
[POST] OK #1702
[SENSOR] T=31.3 C  H=80.0%  Light=77
[POST] OK #1703
[SENSOR] T=31.3 C  H=80.0%  Light=74
[POST] OK #1704
[SENSOR] T=31.4 C  H=80.0%  Light=80
[POST] OK #1705
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1706
[SENSOR] T=31.4 C  H=80.0%  Light=63
[POST] OK #1707
[SENSOR] T=31.4 C  H=80.0%  Light=66
[POST] OK #1708
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1709
[SENSOR] T=31.4 C  H=80.0%  Light=77
[POST] OK #1710
[SENSOR] T=31.3 C  H=80.0%  Light=83
[POST] OK #1711
[SENSOR] T=31.4 C  H=80.0%  Light=75
[POST] OK #1712
[SENSOR] T=31.4 C  H=80.0%  Light=82
[POST] OK #1713
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1714
[SENSOR] T=31.4 C  H=80.0%  Light=69
[POST] OK #1715
[SENSOR] T=31.4 C  H=80.1%  Light=76
[POST] OK #1716
[SENSOR] T=31.4 C  H=80.0%  Light=79
[POST] OK #1717
[SENSOR] T=31.3 C  H=80.1%  Light=84
[POST] OK #1718
[SENSOR] T=31.4 C  H=80.1%  Light=75
[POST] OK #1719
[SENSOR] T=31.4 C  H=80.1%  Light=78
[POST] OK #1720
[SENSOR] T=31.4 C  H=80.0%  Light=75
[POST] OK #1721
[SENSOR] T=31.3 C  H=80.0%  Light=40
[POST] OK #1722
[SENSOR] T=31.4 C  H=80.1%  Light=81
[POST] OK #1723
[SENSOR] T=31.4 C  H=80.1%  Light=81
[POST] OK #1724
[SENSOR] T=31.4 C  H=80.1%  Light=72
[POST] OK #1725
[SENSOR] T=31.4 C  H=80.0%  Light=76
[POST] OK #1726
[SENSOR] T=31.4 C  H=80.0%  Light=78
[POST] OK #1727
[SENSOR] T=31.4 C  H=80.0%  Light=77
[POST] OK #1728
[SENSOR] T=31.4 C  H=80.1%  Light=78
[POST] OK #1729
[SENSOR] T=31.4 C  H=80.1%  Light=71
[POST] OK #1730
[SENSOR] T=31.4 C  H=80.1%  Light=78
[POST] OK #1731
[SENSOR] T=31.3 C  H=80.0%  Light=74
[POST] OK #1732
[SENSOR] T=31.3 C  H=80.1%  Light=75
[POST] OK #1733
[SENSOR] T=31.3 C  H=80.1%  Light=79
[POST] OK #1734
[SENSOR] T=31.4 C  H=80.1%  Light=80
[POST] OK #1735
[SENSOR] T=31.3 C  H=80.2%  Light=31
[POST] OK #1736
[SENSOR] T=31.4 C  H=80.1%  Light=25
[POST] OK #1737
[SENSOR] T=31.3 C  H=80.1%  Light=51
[POST] OK #1738
[SENSOR] T=31.3 C  H=80.1%  Light=58
[POST] OK #1739
[SENSOR] T=31.3 C  H=80.1%  Light=62
[POST] OK #1740
[SENSOR] T=31.3 C  H=80.1%  Light=69
[POST] OK #1741
[SENSOR] T=31.3 C  H=80.2%  Light=69
[POST] OK #1742
[SENSOR] T=31.3 C  H=80.2%  Light=63
[POST] OK #1743
[SENSOR] T=31.3 C  H=80.1%  Light=71
[POST] OK #1744
[SENSOR] T=31.3 C  H=80.1%  Light=68
[POST] OK #1745
[SENSOR] T=31.3 C  H=80.1%  Light=71
[POST] OK #1746
[SENSOR] T=31.3 C  H=80.1%  Light=75
[POST] OK #1747
[SENSOR] T=31.3 C  H=80.2%  Light=75
[POST] OK #1748
[SENSOR] T=31.3 C  H=80.1%  Light=86
[POST] OK #1749
[SENSOR] T=31.3 C  H=80.2%  Light=31
[POST] OK #1750
[SENSOR] T=31.3 C  H=80.1%  Light=77
[POST] OK #1751
[SENSOR] T=31.2 C  H=80.2%  Light=24
[POST] OK #1752
[SENSOR] T=31.3 C  H=80.1%  Light=78
[POST] OK #1753
[SENSOR] T=31.3 C  H=80.1%  Light=62
[POST] OK #1754
[SENSOR] T=31.2 C  H=80.1%  Light=85
[POST] OK #1755
[SENSOR] T=31.2 C  H=80.1%  Light=69
[POST] OK #1756
[SENSOR] T=31.2 C  H=80.1%  Light=58
[POST] OK #1757
[SENSOR] T=31.2 C  H=80.1%  Light=77
[POST] OK #1758
[SENSOR] T=31.2 C  H=80.0%  Light=30
[POST] OK #1759
[SENSOR] T=31.2 C  H=80.1%  Light=46
[POST] OK #1760
[SENSOR] T=31.2 C  H=80.1%  Light=37
[POST] OK #1761
[SENSOR] T=31.2 C  H=80.0%  Light=46
[POST] OK #1762
[SENSOR] T=31.2 C  H=80.1%  Light=42
[POST] OK #1763
[SENSOR] T=31.2 C  H=80.1%  Light=55
[POST] OK #1764
[SENSOR] T=31.2 C  H=80.0%  Light=75
[POST] OK #1765
[SENSOR] T=31.2 C  H=80.1%  Light=73
[POST] OK #1766
[SENSOR] T=31.2 C  H=80.0%  Light=75
[POST] OK #1767
[SENSOR] T=31.2 C  H=80.0%  Light=80
[POST] OK #1768
[SENSOR] T=31.2 C  H=80.1%  Light=80
[POST] OK #1769
[SENSOR] T=31.2 C  H=80.1%  Light=76
[POST] OK #1770
[SENSOR] T=31.2 C  H=80.1%  Light=71
[POST] OK #1771
[SENSOR] T=31.2 C  H=80.0%  Light=75
[POST] OK #1772
[SENSOR] T=31.2 C  H=80.0%  Light=76
[POST] OK #1773
[SENSOR] T=31.2 C  H=80.0%  Light=74
[POST] OK #1774
[SENSOR] T=31.2 C  H=80.1%  Light=74
[POST] OK #1775
[SENSOR] T=31.2 C  H=80.2%  Light=78
[POST] OK #1776
[SENSOR] T=31.2 C  H=80.1%  Light=74
[POST] OK #1777
[SENSOR] T=31.2 C  H=80.1%  Light=74
[POST] OK #1778
[SENSOR] T=31.2 C  H=80.1%  Light=78
[POST] OK #1779
[SENSOR] T=31.2 C  H=80.1%  Light=80
[POST] OK #1780
[SENSOR] T=31.2 C  H=80.1%  Light=83
[POST] OK #1781
[SENSOR] T=31.2 C  H=80.1%  Light=74
[POST] OK #1782
[SENSOR] T=31.2 C  H=80.1%  Light=80
[POST] OK #1783
[SENSOR] T=31.2 C  H=80.1%  Light=76
[POST] OK #1784
[SENSOR] T=31.2 C  H=80.1%  Light=79
[POST] OK #1785
[SENSOR] T=31.2 C  H=80.1%  Light=79
[POST] OK #1786
[SENSOR] T=31.2 C  H=80.1%  Light=82
[POST] OK #1787
[SENSOR] T=31.2 C  H=80.1%  Light=94
[POST] OK #1788
[SENSOR] T=31.2 C  H=80.1%  Light=76
[POST] OK #1789
[SENSOR] T=31.2 C  H=80.1%  Light=75
[POST] OK #1790
[SENSOR] T=31.2 C  H=80.0%  Light=72
[POST] OK #1791
[SENSOR] T=31.2 C  H=80.1%  Light=84
[POST] OK #1792
[SENSOR] T=31.2 C  H=80.1%  Light=80
[POST] OK #1793
[SENSOR] T=31.2 C  H=80.1%  Light=74
[POST] OK #1794
[SENSOR] T=31.1 C  H=80.0%  Light=78
[POST] OK #1795
[SENSOR] T=31.2 C  H=80.1%  Light=78
[POST] OK #1796
[SENSOR] T=31.1 C  H=80.1%  Light=76
[POST] OK #1797
[SENSOR] T=31.2 C  H=80.1%  Light=74
[POST] OK #1798
[SENSOR] T=31.2 C  H=80.1%  Light=72
[POST] OK #1799
[SENSOR] T=31.1 C  H=80.1%  Light=78
[POST] OK #1800
[SENSOR] T=31.2 C  H=80.1%  Light=80
[POST] OK #1801
[SENSOR] T=31.2 C  H=80.0%  Light=72
[POST] OK #1802
[SENSOR] T=31.2 C  H=79.9%  Light=74
[POST] OK #1803
[SENSOR] T=31.2 C  H=80.0%  Light=79
[POST] OK #1804
[SENSOR] T=31.2 C  H=80.0%  Light=77
[POST] OK #1805
[SENSOR] T=31.2 C  H=79.9%  Light=81
[POST] OK #1806
[SENSOR] T=31.2 C  H=79.9%  Light=75
[POST] OK #1807
[SENSOR] T=31.2 C  H=80.0%  Light=85
[POST] OK #1808
[SENSOR] T=31.2 C  H=79.9%  Light=79
[POST] OK #1809
[SENSOR] T=31.2 C  H=79.9%  Light=75
[POST] OK #1810
[SENSOR] T=31.2 C  H=79.9%  Light=81
[POST] OK #1811
[SENSOR] T=31.1 C  H=80.0%  Light=82
[POST] OK #1812
[SENSOR] T=31.2 C  H=79.9%  Light=79
[POST] OK #1813
[SENSOR] T=31.2 C  H=79.9%  Light=76
[POST] OK #1814
[SENSOR] T=31.2 C  H=79.9%  Light=74
[POST] OK #1815
[SENSOR] T=31.2 C  H=79.9%  Light=77
[POST] OK #1816
[SENSOR] T=31.2 C  H=79.9%  Light=77
[POST] OK #1817
[SENSOR] T=31.2 C  H=79.9%  Light=69
[POST] OK #1818
[SENSOR] T=31.2 C  H=79.8%  Light=73
[POST] OK #1819
[SENSOR] T=31.2 C  H=79.8%  Light=76
[POST] OK #1820
[SENSOR] T=31.2 C  H=79.9%  Light=77
[POST] OK #1821
[SENSOR] T=31.2 C  H=79.9%  Light=76
[POST] OK #1822
[SENSOR] T=31.2 C  H=79.9%  Light=68
[POST] OK #1823
[SENSOR] T=31.2 C  H=79.8%  Light=75
[POST] OK #1824
[SENSOR] T=31.2 C  H=79.8%  Light=77
[POST] OK #1825
[SENSOR] T=31.2 C  H=79.8%  Light=70
[POST] OK #1826
[SENSOR] T=31.2 C  H=79.8%  Light=71
[POST] OK #1827
[SENSOR] T=31.2 C  H=79.8%  Light=73
[POST] OK #1828
[SENSOR] T=31.2 C  H=79.8%  Light=77
[POST] OK #1829
[SENSOR] T=31.2 C  H=79.8%  Light=77
[POST] OK #1830
[SENSOR] T=31.2 C  H=79.8%  Light=73
[POST] OK #1831
[SENSOR] T=31.2 C  H=79.8%  Light=79
[POST] OK #1832
[SENSOR] T=31.2 C  H=79.8%  Light=79
[POST] OK #1833
[SENSOR] T=31.2 C  H=79.8%  Light=79
[POST] OK #1834
[SENSOR] T=31.2 C  H=79.7%  Light=74
[POST] OK #1835
[SENSOR] T=31.2 C  H=79.8%  Light=79
[POST] OK #1836
[SENSOR] T=31.2 C  H=79.7%  Light=78
[POST] OK #1837
[SENSOR] T=31.2 C  H=79.8%  Light=78
[POST] OK #1838
[SENSOR] T=31.2 C  H=79.7%  Light=79
[POST] OK #1839
[SENSOR] T=31.2 C  H=79.8%  Light=75
[POST] OK #1840
[SENSOR] T=31.2 C  H=79.7%  Light=79
[POST] OK #1841
[SENSOR] T=31.2 C  H=79.7%  Light=76
[POST] OK #1842
[SENSOR] T=31.2 C  H=79.7%  Light=79
[POST] OK #1843
[SENSOR] T=31.2 C  H=79.7%  Light=76
[POST] OK #1844
[SENSOR] T=31.2 C  H=79.7%  Light=75
[POST] OK #1845
[SENSOR] T=31.2 C  H=79.7%  Light=81
[POST] OK #1846
[SENSOR] T=31.2 C  H=79.7%  Light=77
[POST] OK #1847
[SENSOR] T=31.1 C  H=79.7%  Light=76
[POST] OK #1848
[SENSOR] T=31.2 C  H=79.7%  Light=81
[POST] OK #1849
[SENSOR] T=31.2 C  H=79.7%  Light=78
[POST] OK #1850
[SENSOR] T=31.1 C  H=79.7%  Light=80
[POST] OK #1851
[SENSOR] T=31.2 C  H=79.7%  Light=82
[POST] OK #1852
[SENSOR] T=31.2 C  H=79.7%  Light=80
[POST] OK #1853
[SENSOR] T=31.2 C  H=79.6%  Light=75
[POST] OK #1854
[SENSOR] T=31.2 C  H=79.7%  Light=79
[POST] OK #1855
[SENSOR] T=31.2 C  H=79.6%  Light=79
[POST] OK #1856
[SENSOR] T=31.2 C  H=79.6%  Light=78
[POST] OK #1857
[SENSOR] T=31.2 C  H=79.6%  Light=77
[POST] OK #1858
[SENSOR] T=31.2 C  H=79.6%  Light=81
[POST] OK #1859
[SENSOR] T=31.2 C  H=79.6%  Light=75
[POST] OK #1860
[SENSOR] T=31.2 C  H=79.6%  Light=79
[POST] OK #1861
[SENSOR] T=31.2 C  H=79.5%  Light=78
[POST] OK #1862
[SENSOR] T=31.2 C  H=79.6%  Light=73
[POST] OK #1863
[SENSOR] T=31.2 C  H=79.6%  Light=79
[POST] OK #1864
[SENSOR] T=31.2 C  H=79.5%  Light=80
[POST] OK #1865
[SENSOR] T=31.2 C  H=79.6%  Light=76
[POST] OK #1866
[SENSOR] T=31.2 C  H=79.6%  Light=76
[POST] OK #1867
[SENSOR] T=31.2 C  H=79.6%  Light=16
[POST] OK #1868
[SENSOR] T=31.2 C  H=79.6%  Light=77
[POST] OK #1869
[SENSOR] T=31.2 C  H=79.6%  Light=78
[POST] OK #1870
[SENSOR] T=31.2 C  H=79.6%  Light=82
[POST] OK #1871
[SENSOR] T=31.3 C  H=79.5%  Light=75
[POST] OK #1872
[SENSOR] T=31.2 C  H=79.5%  Light=76
[POST] OK #1873
[SENSOR] T=31.2 C  H=79.6%  Light=76
[POST] OK #1874
[SENSOR] T=31.3 C  H=79.6%  Light=47
[POST] OK #1875
[SENSOR] T=31.2 C  H=79.6%  Light=70
[POST] OK #1876
[SENSOR] T=31.2 C  H=79.5%  Light=62
[POST] OK #1877
[SENSOR] T=31.2 C  H=79.5%  Light=76
[POST] OK #1878
[SENSOR] T=31.2 C  H=79.6%  Light=74
[POST] OK #1879
[SENSOR] T=31.2 C  H=79.5%  Light=77
[POST] OK #1880
[SENSOR] T=31.3 C  H=79.5%  Light=78
[POST] OK #1881
[SENSOR] T=31.2 C  H=79.5%  Light=81
[POST] OK #1882
[SENSOR] T=31.2 C  H=79.5%  Light=54
[POST] OK #1883
[SENSOR] T=31.3 C  H=79.5%  Light=69
[POST] OK #1884
[SENSOR] T=31.3 C  H=79.5%  Light=75
[POST] OK #1885
[SENSOR] T=31.3 C  H=79.5%  Light=75
[POST] OK #1886
[SENSOR] T=31.3 C  H=79.5%  Light=33
[POST] OK #1887
[SENSOR] T=31.3 C  H=79.5%  Light=36
[POST] OK #1888
[SENSOR] T=31.3 C  H=79.5%  Light=35
[POST] OK #1889
����������������������������������������������������������������t=74
[POST] OK #1890
[SENSOR] T=31.3 C  H=79.5%  Light=77
[POST] OK #1891
[SENSOR] T=31.3 C  H=79.5%  Light=81
[POST] OK #1892
[SENSOR] T=31.2 C  H=79.4%  Light=70
[POST] OK #1893
[SENSOR] T=31.3 C  H=79.4%  Light=43
[POST] OK #1894
[SENSOR] T=31.3 C  H=79.4%  Light=51
[POST] OK #1895
[SENSOR] T=31.2 C  H=79.4%  Light=61
[POST] OK #1896
[SENSOR] T=31.2 C  H=79.4%  Light=59
[POST] OK #1897
[SENSOR] T=31.2 C  H=79.4%  Light=22
[POST] OK #1898
[SENSOR] T=31.2 C  H=79.4%  Light=20
[POST] OK #1899
[SENSOR] T=31.2 C  H=79.4%  Light=80
[POST] OK #1900
[SENSOR] T=31.2 C  H=79.4%  Light=74
[POST] OK #1901
[SENSOR] T=31.2 C  H=79.4%  Light=75
[POST] OK #1902
[SENSOR] T=31.2 C  H=79.4%  Light=79
[POST] OK #1903
[SENSOR] T=31.3 C  H=79.4%  Light=78
[POST] OK #1904
[SENSOR] T=31.2 C  H=79.2%  Light=79
[POST] OK #1905
[SENSOR] T=31.2 C  H=79.3%  Light=26
[POST] OK #1906
[SENSOR] T=31.2 C  H=79.3%  Light=42
[POST] OK #1907
[SENSOR] T=31.2 C  H=79.3%  Light=80
[POST] OK #1908
[SENSOR] T=31.2 C  H=79.3%  Light=26
[POST] OK #1909
[SENSOR] T=31.2 C  H=79.3%  Light=8
[POST] OK #1910
[SENSOR] T=31.2 C  H=79.3%  Light=85
[POST] OK #1911
[SENSOR] T=31.2 C  H=79.3%  Light=84
[POST] OK #1912
[SENSOR] T=31.2 C  H=79.3%  Light=80
[POST] OK #1913
[SENSOR] T=31.2 C  H=79.3%  Light=72
[POST] OK #1914
[SENSOR] T=31.2 C  H=79.2%  Light=0
[POST] OK #1915
[SENSOR] T=31.2 C  H=79.2%  Light=19
[POST] OK #1916
[SENSOR] T=31.2 C  H=79.3%  Light=44
[POST] OK #1917
[SENSOR] T=31.2 C  H=79.3%  Light=39
[POST] OK #1918
[SENSOR] T=31.2 C  H=79.3%  Light=60
[POST] OK #1919
[SENSOR] T=31.2 C  H=79.2%  Light=61
[POST] OK #1920
[SENSOR] T=31.2 C  H=79.2%  Light=10
[POST] OK #1921
[SENSOR] T=31.3 C  H=79.2%  Light=2
[POST] OK #1922
[SENSOR] T=31.2 C  H=79.2%  Light=80
[POST] OK #1923
[SENSOR] T=31.2 C  H=79.1%  Light=70
[POST] OK #1924
[SENSOR] T=31.2 C  H=79.1%  Light=76
[POST] OK #1925
[SENSOR] T=31.2 C  H=79.2%  Light=63
[POST] OK #1926
[SENSOR] T=31.2 C  H=79.2%  Light=79
[POST] OK #1927
[SENSOR] T=31.2 C  H=79.2%  Light=83
[POST] OK #1928
[SENSOR] T=31.2 C  H=79.2%  Light=26
[POST] OK #1929
[SENSOR] T=31.2 C  H=79.2%  Light=71
[POST] OK #1930
[SENSOR] T=31.2 C  H=79.2%  Light=85
[POST] OK #1931
[SENSOR] T=31.2 C  H=79.2%  Light=80
[POST] OK #1932
[SENSOR] T=31.2 C  H=79.2%  Light=20
[POST] OK #1933
[SENSOR] T=31.2 C  H=79.2%  Light=5
[POST] OK #1934

*/