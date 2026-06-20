//GPIO 22 connected to one button leg. Nothing else.

const int BUTTON_PIN = 22;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);  // INPUT, not INPUT_PULLUP — no internal resistor
  Serial.println("Stage 1 started. Watching GPIO 22...");
}

void loop() {
  int state = digitalRead(BUTTON_PIN);
  Serial.println(state);   // prints 0 (LOW) or 1 (HIGH)
  delay(100);               // read 10 times per second, readable pace
}
