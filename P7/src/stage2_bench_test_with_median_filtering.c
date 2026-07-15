#define TRIG_PIN 23
#define ECHO_PIN 22

float measureDistanceOnce() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1; // signal "no echo"

  return (duration * 0.0343) / 2.0;
}

float measureDistanceMedian(int samples) {
  float readings[samples];
  int validCount = 0;

  for (int i = 0; i < samples; i++) {
    float d = measureDistanceOnce();
    if (d > 0) {
      readings[validCount] = d;
      validCount++;
    }
    delay(30); // small gap between pulses so echoes don't overlap
  }

  if (validCount == 0) return -1; // all readings failed

  // Bubble sort the valid readings
  for (int i = 0; i < validCount - 1; i++) {
    for (int j = 0; j < validCount - i - 1; j++) {
      if (readings[j] > readings[j + 1]) {
        float temp = readings[j];
        readings[j] = readings[j + 1];
        readings[j + 1] = temp;
      }
    }
  }

  return readings[validCount / 2]; // middle element
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
}

void loop() {
  float dist = measureDistanceMedian(5);

  if (dist < 0) {
    Serial.println("All readings failed - check wiring/target");
  } else {
    Serial.print("Median distance: ");
    Serial.print(dist);
    Serial.println(" cm");
  }

  delay(500);
}

/*
Median distance: 9.47 cm
Median distance: 9.11 cm
Median distance: 9.11 cm
Median distance: 10.12 cm
Median distance: 9.79 cm
Median distance: 10.12 cm
Median distance: 9.11 cm
Median distance: 9.45 cm
Median distance: 9.45 cm
Median distance: 9.79 cm
Median distance: 9.12 cm
Median distance: 9.11 cm
Median distance: 11.82 cm
Median distance: 24.18 cm
Median distance: 29.50 cm
Median distance: 30.03 cm
Median distance: 29.52 cm
Median distance: 30.03 cm
Median distance: 29.52 cm
Median distance: 29.21 cm
Median distance: 29.52 cm
Median distance: 29.50 cm
Median distance: 29.52 cm
Median distance: 29.77 cm
Median distance: 40.15 cm
Median distance: 54.67 cm
Median distance: 61.53 cm
Median distance: 61.83 cm
Median distance: 59.70 cm
Median distance: 60.40 cm
Median distance: 60.54 cm
Median distance: 60.83 cm
Median distance: 61.11 cm
Median distance: 60.40 cm
Median distance: 60.25 cm
Median distance: 60.11 cm
Median distance: 59.85 cm
Median distance: 59.12 cm
Median distance: 51.21 cm
Median distance: 17.20 cm
Median distance: 85.53 cm
Median distance: 85.96 cm
Median distance: 85.53 cm
Median distance: 85.56 cm
Median distance: 85.96 cm
Median distance: 85.94 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.53 cm
Median distance: 85.96 cm
Median distance: 85.97 cm
Median distance: 85.96 cm
Median distance: 85.53 cm
Median distance: 85.53 cm
Median distance: 85.96 cm
Median distance: 85.53 cm
Median distance: 85.97 cm
Median distance: 85.53 cm
Median distance: 85.96 cm
Median distance: 85.53 cm
Median distance: 85.53 cm
Median distance: 85.96 cm
Median distance: 85.13 cm
Median distance: 85.53 cm
Median distance: 85.60 cm
Median distance: 85.96 cm
Median distance: 85.97 cm
Median distance: 85.58 cm
Median distance: 85.13 cm
Median distance: 85.53 cm
Median distance: 85.54 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.58 cm
Median distance: 85.96 cm
Median distance: 85.97 cm
Median distance: 85.53 cm
Median distance: 85.53 cm
Median distance: 85.53 cm
Median distance: 85.54 cm
Median distance: 85.58 cm
Median distance: 85.13 cm
Median distance: 85.54 cm
Median distance: 85.53 cm
Median distance: 85.53 cm
Median distance: 85.13 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.53 cm
Median distance: 85.97 cm
Median distance: 85.97 cm
Median distance: 85.58 cm
Median distance: 85.97 cm
Median distance: 85.58 cm
Median distance: 85.53 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.97 cm
Median distance: 85.56 cm
Median distance: 85.53 cm
Median distance: 85.53 cm
Median distance: 85.58 cm
Median distance: 85.97 cm
Median distance: 85.15 cm
Median distance: 85.13 cm
Median distance: 85.96 cm
Median distance: 85.13 cm
Median distance: 85.60 cm
Median distance: 85.54 cm
Median distance: 85.60 cm
Median distance: 85.58 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.97 cm
Median distance: 85.58 cm
Median distance: 85.13 cm
Median distance: 85.53 cm
Median distance: 85.97 cm
Median distance: 85.58 cm
Median distance: 85.54 cm
Median distance: 85.56 cm
Median distance: 85.58 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.56 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.53 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.96 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.53 cm
Median distance: 85.13 cm
Median distance: 85.58 cm
Median distance: 85.56 cm
Median distance: 85.96 cm
Median distance: 85.56 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.56 cm
Median distance: 85.94 cm
Median distance: 85.96 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.58 cm
Median distance: 85.96 cm
Median distance: 85.58 cm
Median distance: 85.56 cm
Median distance: 85.97 cm
Median distance: 85.56 cm
Median distance: 85.15 cm
Median distance: 85.51 cm
Median distance: 85.56 cm
Median distance: 85.58 cm
Median distance: 85.58 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.15 cm
Median distance: 85.53 cm
Median distance: 85.58 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.97 cm
Median distance: 85.97 cm
Median distance: 85.97 cm
Median distance: 85.58 cm
Median distance: 85.96 cm
Median distance: 85.58 cm
Median distance: 85.97 cm
Median distance: 85.96 cm
Median distance: 85.15 cm
Median distance: 85.13 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.13 cm
Median distance: 85.56 cm
Median distance: 85.13 cm
Median distance: 85.54 cm
Median distance: 85.96 cm
Median distance: 85.15 cm
Median distance: 85.15 cm
Median distance: 85.56 cm
Median distance: 85.97 cm
Median distance: 85.53 cm
Median distance: 85.58 cm
Median distance: 85.96 cm
Median distance: 85.53 cm
Median distance: 85.13 cm
Median distance: 85.97 cm
Median distance: 85.94 cm
Median distance: 85.97 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.97 cm
Median distance: 85.97 cm
Median distance: 85.96 cm
Median distance: 85.13 cm
Median distance: 85.96 cm
Median distance: 85.96 cm
Median distance: 85.58 cm
Median distance: 85.96 cm
Median distance: 85.97 cm
*/