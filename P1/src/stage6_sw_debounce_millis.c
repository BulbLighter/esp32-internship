// Stage 6: Software Debounce using millis()
// INPUT_PULLUP, no external resistor or capacitor
// Debounce window: 50ms

const int BUTTON_PIN = 22;

int pressCount = 0;
int lastState = HIGH;           // unpressed with pull-up = HIGH
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;  // milliseconds

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Stage 6 started. Software debounce active.");
  Serial.println("Press button 5-10 times at various speeds.");
}

void loop() {
  int currentState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  // Only consider this a valid transition if enough time has passed
  // since the last accepted transition
  if (currentState != lastState && (now - lastDebounceTime) > DEBOUNCE_DELAY) {

    // We only count actual presses (HIGH -> LOW with pull-up logic)
    if (lastState == HIGH && currentState == LOW) {
      pressCount++;
      Serial.print("Press detected! Count: ");
      Serial.println(pressCount);
    }

    lastDebounceTime = now;   // reset the debounce timer
    lastState = currentState; // update state ONLY when accepted
  }
}

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
// Press detected! Count: 14
// Press detected! Count: 15
// Press detected! Count: 16
