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

// 5 presses

// Press detected! Count: 1
// Press detected! Count: 2
// Press detected! Count: 3
// Press detected! Count: 4
// Press detected! Count: 5
// Press detected! Count: 6
// Press detected! Count: 7
// Press detected! Count: 8
// Press detected! Count: 9
// Press detected! Count: 10
// Press detected! Count: 11
// Press detected! Count: 12
// Press detected! Count: 13