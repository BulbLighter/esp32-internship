#include <WiFi.h>
#include <WebServer.h>

#define RELAY_PIN 22
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

#define BUTTON_PIN 21

const char* ssid     = "wifi name";
const char* password = "pw";

WebServer server(80);
bool relayState = false;

int lastButtonReading = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void updateOutputs() {
  if (relayState) {
    digitalWrite(RELAY_PIN, RELAY_ON);
    Serial.println("Relay ON");
  } else {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    Serial.println("Relay OFF");
  }
}

void handleRoot() {
  String html = "<html><body style='font-family:sans-serif;text-align:center;margin-top:50px;'>";
  html += "<h1>ESP32 Relay Control</h1>";
  html += "<p>Current state: <b>";
  html += relayState ? "ON" : "OFF";
  html += "</b></p>";
  html += "<a href='/on'><button style='font-size:24px;padding:20px;margin:10px;'>ON</button></a>";
  html += "<a href='/off'><button style='font-size:24px;padding:20px;margin:10px;'>OFF</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOn() {
  relayState = true;
  updateOutputs();
  handleRoot();
}

void handleOff() {
  relayState = false;
  updateOutputs();
  handleRoot();
}

void setup() {
  Serial.begin(115200);

  digitalWrite(RELAY_PIN, RELAY_OFF);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  updateOutputs();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    static int stableState = HIGH;
    if (reading != stableState) {
      stableState = reading;
      if (stableState == LOW) {
        relayState = !relayState;
        updateOutputs();
      }
    }
  }
  lastButtonReading = reading;
}

// ...
// Connected! IP address: 172.22.15.18
// Web server started
// Relay ON
// Relay OFF
// Relay ON
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
