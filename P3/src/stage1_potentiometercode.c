// PROJECT 3 — Stage 1: Potentiometer Voltage Divider
// Read 10kΩ potentiometer wiper on GPIO 35 (ADC1_CH7)
// Map raw ADC counts (0–4095) to voltage (0–3.3V)

const int POT_PIN = 35;  // ADC1 pin (safe with WiFi)
const float ADC_REF_VOLTAGE = 3.3;  // ESP32 reference
const int ADC_MAX_COUNT = 4095;     // 12-bit ADC

void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for Serial Monitor to connect
  
  // CRITICAL: Set attenuation to read full 0–3.3V range
  analogSetAttenuation(ADC_11db);
  
  Serial.println("=== PROJECT 3 — Stage 1: Potentiometer ===");
  Serial.println("ADC attenuation set to ADC_11db (0–3.3V range)");
  Serial.println("Turn potentiometer and watch readings change");
  Serial.println();
}

void loop() {
  // Read raw ADC count
  int raw_count = analogRead(POT_PIN);
  
  // Convert count to voltage: V = count / 4095 × 3.3
  float voltage = (raw_count / (float)ADC_MAX_COUNT) * ADC_REF_VOLTAGE;
  
  // Print with labels
  Serial.print("ADC Raw: ");
  Serial.print(raw_count);
  Serial.print(" → Voltage: ");
  Serial.print(voltage, 2);  // 2 decimal places
  Serial.println(" V");
  
  delay(500);  // Read every 500ms (not too fast to spam)
}

// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 178 → Voltage: 0.14 V
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
// ADC Raw: 389 → Voltage: 0.31 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 4095 → Voltage: 3.30 V
// ADC Raw: 972 → Voltage: 0.78 V
// ADC Raw: 64 → Voltage: 0.05 V
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


// i moved it all the way to left it comes 3.3(towards gnd)
// i move it all the way to right it comes 0.0(towards 3.3)