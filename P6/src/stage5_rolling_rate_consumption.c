#define BUTTON_PIN 21

volatile unsigned long pulseCount = 0;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

unsigned long countAtWindowStart = 0;
unsigned long windowStartTime = 0;
float lastRate = 0;

void IRAM_ATTR buttonISR() {
  portENTER_CRITICAL_ISR(&mux);
  pulseCount++;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  windowStartTime = millis();
}

float computeRate() {
  unsigned long elapsed = millis() - windowStartTime;
  if (elapsed >= 60000) {
    portENTER_CRITICAL(&mux);
    unsigned long count = pulseCount;
    portEXIT_CRITICAL(&mux);
    lastRate = (count - countAtWindowStart) * 60000.0 / elapsed;
    countAtWindowStart = count;
    windowStartTime = millis();
  }
  return lastRate;
}

void loop() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 5000) { // print every 5s so you can watch it update
    portENTER_CRITICAL(&mux);
    unsigned long safeCount = pulseCount;
    portEXIT_CRITICAL(&mux);
    float rate = computeRate();
    Serial.print("Total: ");
    Serial.print(safeCount);
    Serial.print(" | Rate (per min): ");
    Serial.println(rate);
    lastPrint = millis();
  }
}

// Total: 53 | Rate (per min): 0.00
// Total: 106 | Rate (per min): 0.00
// Total: 159 | Rate (per min): 0.00
// Total: 212 | Rate (per min): 0.00
// Total: 265 | Rate (per min): 0.00
// Total: 318 | Rate (per min): 0.00
// Total: 372 | Rate (per min): 0.00
// Total: 424 | Rate (per min): 0.00
// Total: 476 | Rate (per min): 0.00
// Total: 530 | Rate (per min): 0.00
// Total: 583 | Rate (per min): 0.00
// Total: 636 | Rate (per min): 0.00
// Total: 690 | Rate (per min): 637.28
// Total: 743 | Rate (per min): 637.28
// Total: 796 | Rate (per min): 637.28
// Total: 846 | Rate (per min): 637.28
// Total: 901 | Rate (per min): 637.28
// Total: 954 | Rate (per min): 637.28
// Total: 1010 | Rate (per min): 637.28
// Total: 1063 | Rate (per min): 637.28