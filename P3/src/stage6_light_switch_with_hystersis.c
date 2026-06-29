// PROJECT 3 — Stage 6: Threshold Light Switch with Hysteresis
// LED turns ON when dark (ADC < DARK_THRESHOLD)
// LED turns OFF when bright (ADC > BRIGHT_THRESHOLD)
// Built on top of Stage 5B's capacitor + 16-sample averaging filter

const int LDR_PIN = 35;
const int LED_PIN = 22;        // Use your Project 1 LED setup, or wire a new one
const int NUM_SAMPLES = 16;

// Hysteresis thresholds (tune these based on your room)
const int DARK_THRESHOLD = 150;   // Below this → turn LED ON
const int BRIGHT_THRESHOLD = 350; // Above this → turn LED OFF

bool ledState = false;  // Track current LED state

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  analogSetAttenuation(ADC_11db);
  
  Serial.println("=== Stage 6: Threshold Light Switch with Hysteresis ===");
  Serial.print("Dark threshold (LED ON below): ");
  Serial.println(DARK_THRESHOLD);
  Serial.print("Bright threshold (LED OFF above): ");
  Serial.println(BRIGHT_THRESHOLD);
  Serial.println();
}

void loop() {
  // Get filtered ADC reading (16-sample average, same as Stage 5B)
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(LDR_PIN);
    delayMicroseconds(100);
  }
  int averaged = sum / NUM_SAMPLES;
  
  // Hysteresis logic
  if (averaged < DARK_THRESHOLD && ledState == false) {
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("  >>> LED ON (dark detected) <<<");
  } 
  else if (averaged > BRIGHT_THRESHOLD && ledState == true) {
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("  >>> LED OFF (bright detected) <<<");
  }
  
  // Print current reading and state
  Serial.print("Averaged ADC: ");
  Serial.print(averaged);
  Serial.print(" | LED: ");
  Serial.println(ledState ? "ON" : "OFF");
  
  delay(200);
}

// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
//   >>> LED OFF (bright detected) <<<
// Averaged ADC: 1894 | LED: OFF
// Averaged ADC: 1905 | LED: OFF
// Averaged ADC: 1907 | LED: OFF
// Averaged ADC: 1901 | LED: OFF
// Averaged ADC: 1911 | LED: OFF
// Averaged ADC: 1905 | LED: OFF
// Averaged ADC: 1906 | LED: OFF
// Averaged ADC: 1899 | LED: OFF
// Averaged ADC: 1898 | LED: OFF
// Averaged ADC: 1898 | LED: OFF
// Averaged ADC: 1899 | LED: OFF
// Averaged ADC: 1897 | LED: OFF
// Averaged ADC: 1899 | LED: OFF
// Averaged ADC: 1893 | LED: OFF
// Averaged ADC: 1894 | LED: OFF
// Averaged ADC: 1894 | LED: OFF
// Averaged ADC: 1882 | LED: OFF
// Averaged ADC: 1890 | LED: OFF
// Averaged ADC: 1892 | LED: OFF
// Averaged ADC: 1888 | LED: OFF
// Averaged ADC: 1893 | LED: OFF
// Averaged ADC: 1887 | LED: OFF
// Averaged ADC: 1894 | LED: OFF
// Averaged ADC: 1891 | LED: OFF
// Averaged ADC: 1893 | LED: OFF
// Averaged ADC: 1882 | LED: OFF
// Averaged ADC: 1897 | LED: OFF
// Averaged ADC: 1894 | LED: OFF
// Averaged ADC: 1890 | LED: OFF
// Averaged ADC: 1890 | LED: OFF
// Averaged ADC: 1891 | LED: OFF
// Averaged ADC: 1892 | LED: OFF
// Averaged ADC: 1888 | LED: OFF
// Averaged ADC: 1897 | LED: OFF
// Averaged ADC: 1883 | LED: OFF
// Averaged ADC: 1886 | LED: OFF
// Averaged ADC: 1893 | LED: OFF
// Averaged ADC: 1890 | LED: OFF
// Averaged ADC: 1888 | LED: OFF
// Averaged ADC: 1888 | LED: OFF
// Averaged ADC: 1891 | LED: OFF
// Averaged ADC: 1888 | LED: OFF
// Averaged ADC: 1891 | LED: OFF
// Averaged ADC: 1893 | LED: OFF
// Averaged ADC: 1898 | LED: OFF
// Averaged ADC: 1882 | LED: OFF
// Averaged ADC: 1890 | LED: OFF
// Averaged ADC: 1887 | LED: OFF
// Averaged ADC: 1890 | LED: OFF
// Averaged ADC: 1891 | LED: OFF
// Averaged ADC: 1881 | LED: OFF
// Averaged ADC: 1885 | LED: OFF
// Averaged ADC: 1887 | LED: OFF
// Averaged ADC: 1889 | LED: OFF
// Averaged ADC: 1890 | LED: OFF
// Averaged ADC: 1885 | LED: OFF
// Averaged ADC: 1883 | LED: OFF
// Averaged ADC: 1886 | LED: OFF
// Averaged ADC: 1892 | LED: OFF
//   >>> LED ON (dark detected) <<<
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
//   >>> LED OFF (bright detected) <<<
// Averaged ADC: 923 | LED: OFF
// Averaged ADC: 958 | LED: OFF
// Averaged ADC: 961 | LED: OFF
// Averaged ADC: 970 | LED: OFF
// Averaged ADC: 963 | LED: OFF
// Averaged ADC: 975 | LED: OFF
// Averaged ADC: 974 | LED: OFF
// Averaged ADC: 989 | LED: OFF
// Averaged ADC: 991 | LED: OFF
// Averaged ADC: 1004 | LED: OFF
// Averaged ADC: 996 | LED: OFF
// Averaged ADC: 1001 | LED: OFF
// Averaged ADC: 1001 | LED: OFF
// Averaged ADC: 1006 | LED: OFF
// Averaged ADC: 1006 | LED: OFF
// Averaged ADC: 1017 | LED: OFF
// Averaged ADC: 1034 | LED: OFF
// Averaged ADC: 1040 | LED: OFF
// Averaged ADC: 1032 | LED: OFF
// Averaged ADC: 1033 | LED: OFF
// Averaged ADC: 1028 | LED: OFF
// Averaged ADC: 1021 | LED: OFF
// Averaged ADC: 1016 | LED: OFF
// Averaged ADC: 1019 | LED: OFF
// Averaged ADC: 1021 | LED: OFF
// Averaged ADC: 1019 | LED: OFF
// Averaged ADC: 1018 | LED: OFF
// Averaged ADC: 1016 | LED: OFF
// Averaged ADC: 1008 | LED: OFF
// Averaged ADC: 1007 | LED: OFF
// Averaged ADC: 1014 | LED: OFF
// Averaged ADC: 1001 | LED: OFF
// Averaged ADC: 1000 | LED: OFF
// Averaged ADC: 1002 | LED: OFF
// Averaged ADC: 1001 | LED: OFF
// Averaged ADC: 1012 | LED: OFF
// Averaged ADC: 1007 | LED: OFF
// Averaged ADC: 1005 | LED: OFF
// Averaged ADC: 990 | LED: OFF
// Averaged ADC: 998 | LED: OFF
// Averaged ADC: 999 | LED: OFF
// Averaged ADC: 984 | LED: OFF
// Averaged ADC: 999 | LED: OFF
//   >>> LED ON (dark detected) <<<
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
// Averaged ADC: 0 | LED: ON
