// Stage 4: Switch Bounce Demonstration
// INPUT_PULLUP — pressed = LOW, released = HIGH
// Counts every LOW detection as a press — will overcount due to bounce

const int BUTTON_PIN = 22;

int pressCount = 0;
int lastState = HIGH;  // unpressed state with pull-up is HIGH

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Stage 4 started. Press button 5 times slowly.");
  Serial.println("Watch the count — it won't be 5.");
}

void loop() {
  int currentState = digitalRead(BUTTON_PIN);

  // Detect HIGH -> LOW transition (button press)
  if (lastState == HIGH && currentState == LOW) {
    pressCount++;
    Serial.print("Press detected! Count: ");
    Serial.println(pressCount);
  }

  lastState = currentState;
}
