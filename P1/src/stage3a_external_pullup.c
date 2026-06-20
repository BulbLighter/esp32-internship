// 10kΩ from 3.3V to GPIO 22
// Button connects GPIO 22 to GND when pressed

const int BUTTON_PIN = 22;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);  // plain INPUT — external resistor does the work
  Serial.println("Stage 3a started. External pull-up active.");
  Serial.println("Released = 1, Pressed = 0");
}

void loop() {
  int state = digitalRead(BUTTON_PIN);
  Serial.println(state);
  delay(100);
}
