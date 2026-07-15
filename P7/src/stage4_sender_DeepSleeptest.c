#include <esp_now.h>
#include <WiFi.h>
#include "esp_sleep.h"

#define TRIG_PIN 23
#define ECHO_PIN 22

uint8_t receiverMAC[] = {0xB0, 0xCB, 0xD8, 0x0A, 0x73, 0xEC};

typedef struct {
  int counter;
  float distance_cm;
} Message;

Message myData;
esp_now_peer_info_t peerInfo;

// RTC memory - survives deep sleep, resets only on power loss/manual reset
RTC_DATA_ATTR int bootCount = 0;

void onDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

float measureDistanceOnce() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;

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
    delay(30);
  }

  if (validCount == 0) return -1;

  for (int i = 0; i < validCount - 1; i++) {
    for (int j = 0; j < validCount - i - 1; j++) {
      if (readings[j] > readings[j + 1]) {
        float temp = readings[j];
        readings[j] = readings[j + 1];
        readings[j + 1] = temp;
      }
    }
  }

  return readings[validCount / 2];
}

void setup() {
  unsigned long startTime = millis(); // track total awake time

  Serial.begin(115200);
  delay(500); // let serial settle - we'll remove/shrink this later for real deployment

  bootCount++;
  Serial.println("=====================================");
  Serial.print("Boot count: ");
  Serial.println(bootCount);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // 1. Measure distance
  float dist = measureDistanceMedian(5);
  Serial.print("Median distance: ");
  Serial.println(dist);

  // 2. Set up ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
  } else {
    esp_now_register_send_cb(onDataSent);
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    // 3. Send data
    myData.counter = bootCount;
    myData.distance_cm = dist;
    esp_now_send(receiverMAC, (uint8_t *)&myData, sizeof(myData));
    delay(200); // give the send callback time to fire before sleeping
  }

  unsigned long awakeTime = millis() - startTime;
  Serial.print("Awake time (ms): ");
  Serial.println(awakeTime);

  // 4. Configure sleep timer and sleep
  // NOTE: using 15 seconds for now (not 60s) so we don't wait too long between test cycles
  esp_sleep_enable_timer_wakeup(15 * 1000000ULL);
  Serial.println("Going to sleep now...");
  Serial.flush(); // make sure all serial output is sent before sleep cuts power
  esp_deep_sleep_start();
}

void loop() {
  // NEVER REACHED
}

/*
rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d}�p_flash: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 2
Median distance: 85.49
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d�f�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 3
Median distance: 85.08
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d��}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 4
Median distance: 85.49
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d=�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 5
Median distance: 85.49
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d=�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 6
Median distance: 85.06
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d�f����͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 7
Median distance: 85.49
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d��}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 8
Median distance: 85.48
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d=�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 9
Median distance: 85.49
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d��}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 10
Median distance: 85.06
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d=枙��͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 11
Median distance: 85.48
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d��}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 12
Median distance: 85.49
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d�f�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 13
Median distance: 35.86
Send status: Success
Awake time (ms): 893
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d=�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 14
Median distance: 55.03
Send status: Success
Awake time (ms): 898
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d=�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 15
Median distance: 85.48
Send status: Success
Awake time (ms): 908
Going to sleep now...
ets Jul 29 2019 12:21:46

rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16560
load:0x40080400,len:3500
entry 0x400805b4
E (65) esp_core_d=�}���͡: Core dump data check failed:
Calculated checksum='6ff493ab'
Image checksum='ffffffff'
=====================================
Boot count: 16
Median distance: 85.05
Send status: Success
Awake time (ms): 908
Going to sleep now...
*/