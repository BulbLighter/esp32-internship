#define BUTTON_PIN 21

volatile unsigned long pulseCount = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR buttonISR() {
  portENTER_CRITICAL_ISR(&mux);
  pulseCount++;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
}

void loop() {
  portENTER_CRITICAL(&mux);
  unsigned long safeCount = pulseCount;
  portEXIT_CRITICAL(&mux);

  Serial.println(safeCount);
  delay(400);
}

// Stage 2: ISR Check
// 5 clicks

// 0
// 0
// 0
// 0
// 0
// 0
// 1
// 1
// 1
// 1
// 1
// 1
// 1
// 1
// 1
// 1
// 2
// 2
// 2
// 2
// 2
// 2
// 2
// 2
// 2
// 2
// 3
// 3
// 3
// 3
// 3
// 3
// 3
// 3
// 3
// 3
// 3
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 4
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5
// 5

//Inflated baseline
// 20 clicks

// 0
// 0
// 0
// 0
// 0
// 0
// 0
// 2
// 3
// 4
// 6
// 8
// 9
// 11
// 13
// 14
// 15
// 16
// 17
// 18
// 19
// 20
// 22
// 23
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24
// 24