#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 10000; // 10 seconds

void setup() {
  Serial.begin(115200);
  delay(1000); // brief settle time, not the forbidden blocking-loop delay, just startup
  Serial.println("DHT22 Stage 1 - Basic Read Test");
  dht.begin();
}

void loop() {
  unsigned long now = millis();

  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;

    float humidity = dht.readHumidity();
    float tempC = dht.readTemperature(); // Celsius by default

    if (isnan(humidity) || isnan(tempC)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println(" *C");
  }

  // other non-blocking work could go here later (WiFi, etc.)
}

 
//Humidity: 57.80 %	Temperature: 33.60 *C
// Humidity: 57.80 %	Temperature: 33.70 *C
// Humidity: 57.80 %	Temperature: 33.60 *C
// Humidity: 57.80 %	Temperature: 33.60 *C
// Humidity: 58.50 %	Temperature: 33.60 *C
// Humidity: 58.30 %	Temperature: 33.60 *C
// Humidity: 59.50 %	Temperature: 33.60 *C
// Humidity: 60.40 %	Temperature: 33.60 *C
// Humidity: 60.30 %	Temperature: 33.70 *C
// Humidity: 59.60 %	Temperature: 33.70 *C
// Humidity: 60.60 %	Temperature: 33.70 *C
// Humidity: 60.60 %	Temperature: 33.70 *C
// Humidity: 60.40 %	Temperature: 33.70 *C
// Humidity: 60.40 %	Temperature: 33.70 *C
// Humidity: 61.80 %	Temperature: 33.70 *C
// Humidity: 61.00 %	Temperature: 33.70 *C
// Humidity: 61.60 %	Temperature: 33.70 *C

//Floating input
// Failed to read from DHT sensor!
// Failed to read from DHT sensor!
// Failed to read from DHT sensor!
// Failed to read from DHT sensor!
// Failed to read from DHT sensor!