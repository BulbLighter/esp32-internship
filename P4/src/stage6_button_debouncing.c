#define RELAY_PIN 22
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

#define BUTTON_PIN 21

bool relayState = false;  // false = OFF, true = ON
int lastButtonReading = HIGH;    // INPUT_PULLUP idle = HIGH
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void updateOutputs() {
  if (relayState) {
    digitalWrite(RELAY_PIN, RELAY_ON);
    Serial.println("Relay ON");
  } else {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    Serial.println("Relay OFF");
  }
}

void setup() {
  Serial.begin(115200);

  digitalWrite(RELAY_PIN, RELAY_OFF);   // boot-safe: set before pinMode
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  updateOutputs();  // start in known OFF state, LEDs matched
  Serial.println("Setup complete, entering loop");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // reading has been stable for debounceDelay
    static int stableState = HIGH;
    if (reading != stableState) {
      stableState = reading;
      if (stableState == LOW) {  // button pressed (INPUT_PULLUP: press = LOW)
        relayState = !relayState;
        updateOutputs();
      }
    }
  }

  lastButtonReading = reading;
}

// the green led on relay module turns on or off based on click. if its 
// on it turns off,if its off it turns on
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
