#define BUTTON_PIN 21

volatile unsigned long pulseCount = 0;

void IRAM_ATTR buttonISR() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
}

void loop() {
  Serial.println(pulseCount);
  delay(400); // same "busy" simulation as before
}

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