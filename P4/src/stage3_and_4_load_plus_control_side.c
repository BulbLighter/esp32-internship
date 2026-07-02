#define RELAY_PIN 22
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

void setup() {
  Serial.begin(115200);
  digitalWrite(RELAY_PIN, RELAY_OFF);  // set state BEFORE pinMode to avoid glitch
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  Serial.println("Relay ON");
  digitalWrite(RELAY_PIN, RELAY_ON);
  delay(3000);

  Serial.println("Relay OFF");
  digitalWrite(RELAY_PIN, RELAY_OFF);
  delay(3000);
}

// Relay OFF -> fan turns off
// Relay ON ->  fan turns on
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON