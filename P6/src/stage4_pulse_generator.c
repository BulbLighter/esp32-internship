#define PULSE_PIN 23

void setup() {
  pinMode(PULSE_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, LOW);
}

void loop() {
  digitalWrite(PULSE_PIN, HIGH);
  delay(50);
  digitalWrite(PULSE_PIN, LOW);
  delay(50);
}