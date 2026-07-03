#define BUTTON_PIN 21

volatile unsigned long pulseCount = 0;
volatile bool interruptEnabled = true;
volatile unsigned long disableTime = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR buttonISR() {
  portENTER_CRITICAL_ISR(&mux);
  if (interruptEnabled) {
    pulseCount++;
    interruptEnabled = false;
    disableTime = millis();
    detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
}

void loop() {
  if (!interruptEnabled && millis() - disableTime > 200) {
    interruptEnabled = true;
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  }

  portENTER_CRITICAL(&mux);
  unsigned long safeCount = pulseCount;
  portEXIT_CRITICAL(&mux);
  Serial.println(safeCount);
  delay(400);
}

// 5 clicks
// 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
// 0 0 0 0 0 0 0 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 
// 4 4 4 4 4 4 5 5 5 5 5 5 5 5 5 5