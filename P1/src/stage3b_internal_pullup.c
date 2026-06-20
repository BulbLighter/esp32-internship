// NO external resistor — ESP32's internal ~45kΩ pull-up does the job
// Button connects GPIO 22 to GND when pressed

const int BUTTON_PIN = 22;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // <- this is the only change
  Serial.println("Stage 3b started. Internal pull-up active.");
  Serial.println("Released = 1, Pressed = 0");
}

void loop() {
  int state = digitalRead(BUTTON_PIN);
  Serial.println(state);
  delay(100);
}
