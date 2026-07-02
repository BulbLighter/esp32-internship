#define RELAY_PIN 22

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // start in a known state first
}

void loop() {
  Serial.println("Relay pin HIGH");
  digitalWrite(RELAY_PIN, HIGH);
  delay(3000);

  Serial.println("Relay pin LOW");
  digitalWrite(RELAY_PIN, LOW);
  delay(3000);
}

// moment relay pin turns high to low, it turns on green led
// moment relay pin turns low to high, it turns off green led

// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW
// Relay pin HIGH
// Relay pin LOW