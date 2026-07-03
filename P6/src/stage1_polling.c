#define BUTTON_PIN 21

volatile unsigned long pollCount = 0;
bool lastState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  bool currentState = digitalRead(BUTTON_PIN);
  if (currentState == LOW && lastState == HIGH) {
    pollCount++;
    Serial.println(pollCount);
  }
  lastState = currentState;

  delay(400); // long enough that a full press-release cycle can hide inside it
}

// 21 clicks

// result

// 1
// 2
// 3