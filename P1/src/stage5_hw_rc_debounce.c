// External 10kΩ pull-up + 100nF capacitor across GPIO22-to-GND
// tau = R*C = 10kΩ * 100nF = 1ms, settle time ~3-5ms

const int BUTTON_PIN = 22;

int pressCount = 0;
int lastState = HIGH;  // pull-up means unpressed = HIGH

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);  // external resistor handles pull-up now
  Serial.println("Stage 5 started. RC debounce active.");
  Serial.println("Press button 5 times slowly.");
}

void loop() {
  int currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW) {
    pressCount++;
    Serial.print("Press detected! Count: ");
    Serial.println(pressCount);
  }

  lastState = currentState;
}

// 10 presses.

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