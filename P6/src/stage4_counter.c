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
  static unsigned long lastPrint = 0;
  static unsigned long seconds = 0;
  if (millis() - lastPrint >= 1000) {
    seconds++;
    portENTER_CRITICAL(&mux);
    unsigned long safeCount = pulseCount;
    portEXIT_CRITICAL(&mux);
    Serial.print(seconds);
    Serial.print("s -> count = ");
    Serial.println(safeCount);
    lastPrint = millis();
  }
}



// 2s -> count = 21
// 3s -> count = 32
// 4s -> count = 44
// 5s -> count = 54
// 6s -> count = 64
// 7s -> count = 74
// 8s -> count = 84
// 9s -> count = 94
// 10s -> count = 104
// 11s -> count = 114
// 12s -> count = 124
// 13s -> count = 134
// 14s -> count = 145
// 15s -> count = 155
// 16s -> count = 165
// 17s -> count = 175
// 18s -> count = 185
// 19s -> count = 195
// 20s -> count = 205
// 21s -> count = 218
// 22s -> count = 228
// 23s -> count = 238
// 24s -> count = 248
// 25s -> count = 258
// 26s -> count = 268
// 27s -> count = 278
// 28s -> count = 289
// 29s -> count = 299
// 30s -> count = 309
// 31s -> count = 320
// 32s -> count = 331
// 33s -> count = 341
// 34s -> count = 351
// 35s -> count = 362
// 36s -> count = 372
// 37s -> count = 383
// 38s -> count = 393
// 39s -> count = 404
// 40s -> count = 414
// 41s -> count = 424
// 42s -> count = 434
// 43s -> count = 445
// 44s -> count = 455
// 45s -> count = 466
// 46s -> count = 477
// 47s -> count = 487
// 48s -> count = 497
// 49s -> count = 508
// 50s -> count = 518
// 51s -> count = 528
// 52s -> count = 538
// 53s -> count = 548
// 54s -> count = 558
// 55s -> count = 568
// 56s -> count = 578
// 57s -> count = 588
// 58s -> count = 598
// 59s -> count = 608
// 60s -> count = 618
// 61s -> count = 628
// 62s -> count = 638
// 63s -> count = 648
// 64s -> count = 658
// 65s -> count = 668
// 66s -> count = 678
// 67s -> count = 688
// 68s -> count = 698
// 69s -> count = 708
// 70s -> count = 718
// 71s -> count = 728
// 72s -> count = 738
// 73s -> count = 748
// 74s -> count = 758
// 75s -> count = 768
// 76s -> count = 778
// 77s -> count = 788
// 78s -> count = 798
// 79s -> count = 808
