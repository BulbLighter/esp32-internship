// Stage 7: Final Version
// Hardware RC debounce on GPIO 22 (button input)
// LED blink output on GPIO 23, blinks N times per press where N = press count

const int BUTTON_PIN = 22;
const int LED_PIN = 23;

int pressCount = 0;
int lastState = HIGH;  // external pull-up + RC debounce = unpressed is HIGH

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);   // external 10kΩ pull-up handles this, not internal
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);   // start with LED off
  Serial.println("Stage 7 started. Press button — LED will blink press count.");
}

void loop() {
  int currentState = digitalRead(BUTTON_PIN);

  // Hardware RC debounce means we can trust raw transitions here
  if (lastState == HIGH && currentState == LOW) {
    pressCount++;
    Serial.print("Press detected! Count: ");
    Serial.println(pressCount);

    blinkLED(pressCount);
  }

  lastState = currentState;
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

