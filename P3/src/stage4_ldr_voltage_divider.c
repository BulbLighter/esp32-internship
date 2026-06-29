// PROJECT 3 — Stage 4: LDR Voltage Divider
// Wire: 3.3V → LDR → ADC_PIN → 10kΩ → GND

const int LDR_PIN = 35;  // ADC1 pin
const float ADC_REF_VOLTAGE = 3.3;
const int ADC_MAX_COUNT = 4095;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  analogSetAttenuation(ADC_11db);  // Full 0–3.3V range
  
  Serial.println("=== PROJECT 3 — Stage 4: LDR Voltage Divider ===");
  Serial.println("Wire: 3.3V → LDR → ADC → 10kΩ → GND");
  Serial.println();
}

void loop() {
  int raw_count = analogRead(LDR_PIN);
  float voltage = (raw_count / (float)ADC_MAX_COUNT) * ADC_REF_VOLTAGE;
  
  Serial.print("ADC Raw: ");
  Serial.print(raw_count);
  Serial.print(" → Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");
  
  delay(1000);
}

// ADC Raw: 82 → Voltage: 0.07 V
// ADC Raw: 81 → Voltage: 0.07 V
// ADC Raw: 76 → Voltage: 0.06 V
// ADC Raw: 45 → Voltage: 0.04 V
// ADC Raw: 71 → Voltage: 0.06 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 76 → Voltage: 0.06 V
// ADC Raw: 2 → Voltage: 0.00 V
// ADC Raw: 4 → Voltage: 0.00 V
// ADC Raw: 1 → Voltage: 0.00 V
// ADC Raw: 12 → Voltage: 0.01 V
// ADC Raw: 20 → Voltage: 0.02 V
// ADC Raw: 37 → Voltage: 0.03 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 3 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 2 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 112 → Voltage: 0.09 V
// ADC Raw: 223 → Voltage: 0.18 V
// ADC Raw: 262 → Voltage: 0.21 V
// ADC Raw: 259 → Voltage: 0.21 V
// ADC Raw: 266 → Voltage: 0.21 V
// ADC Raw: 265 → Voltage: 0.21 V
// ADC Raw: 287 → Voltage: 0.23 V
// ADC Raw: 316 → Voltage: 0.25 V
// ADC Raw: 253 → Voltage: 0.20 V
// ADC Raw: 294 → Voltage: 0.24 V
// ADC Raw: 304 → Voltage: 0.24 V
// ADC Raw: 304 → Voltage: 0.24 V
// ADC Raw: 332 → Voltage: 0.27 V
// ADC Raw: 311 → Voltage: 0.25 V
// ADC Raw: 64 → Voltage: 0.05 V
// ADC Raw: 65 → Voltage: 0.05 V
// ADC Raw: 13 → Voltage: 0.01 V
// ADC Raw: 16 → Voltage: 0.01 V
// ADC Raw: 23 → Voltage: 0.02 V
// ADC Raw: 112 → Voltage: 0.09 V
// ADC Raw: 286 → Voltage: 0.23 V
// ADC Raw: 314 → Voltage: 0.25 V
// ADC Raw: 305 → Voltage: 0.25 V
// ADC Raw: 317 → Voltage: 0.26 V
// ADC Raw: 327 → Voltage: 0.26 V
// ADC Raw: 272 → Voltage: 0.22 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 1361 → Voltage: 1.10 V
// ADC Raw: 3150 → Voltage: 2.54 V
// ADC Raw: 3394 → Voltage: 2.74 V
// ADC Raw: 3388 → Voltage: 2.73 V
// ADC Raw: 3330 → Voltage: 2.68 V
// ADC Raw: 3208 → Voltage: 2.59 V
// ADC Raw: 3216 → Voltage: 2.59 V
// ADC Raw: 3190 → Voltage: 2.57 V
// ADC Raw: 3169 → Voltage: 2.55 V
// ADC Raw: 3109 → Voltage: 2.51 V
// ADC Raw: 2911 → Voltage: 2.35 V
// ADC Raw: 2864 → Voltage: 2.31 V
// ADC Raw: 2917 → Voltage: 2.35 V
// ADC Raw: 2860 → Voltage: 2.30 V
// ADC Raw: 2881 → Voltage: 2.32 V
// ADC Raw: 2934 → Voltage: 2.36 V
// ADC Raw: 2870 → Voltage: 2.31 V
// ADC Raw: 2844 → Voltage: 2.29 V
// ADC Raw: 2832 → Voltage: 2.28 V
// ADC Raw: 2794 → Voltage: 2.25 V
// ADC Raw: 2864 → Voltage: 2.31 V
// ADC Raw: 2864 → Voltage: 2.31 V
// ADC Raw: 2865 → Voltage: 2.31 V
// ADC Raw: 2837 → Voltage: 2.29 V
// ADC Raw: 3001 → Voltage: 2.42 V
// ADC Raw: 2958 → Voltage: 2.38 V
// ADC Raw: 2751 → Voltage: 2.22 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 0 → Voltage: 0.00 V
// ADC Raw: 269 → Voltage: 0.22 V
// ADC Raw: 278 → Voltage: 0.22 V
// ADC Raw: 267 → Voltage: 0.22 V
// ADC Raw: 261 → Voltage: 0.21 V
// ADC Raw: 197 → Voltage: 0.16 V
// ADC Raw: 111 → Voltage: 0.09 V
// ADC Raw: 62 → Voltage: 0.05 V
// ADC Raw: 75 → Voltage: 0.06 V
// ADC Raw: 85 → Voltage: 0.07 V
