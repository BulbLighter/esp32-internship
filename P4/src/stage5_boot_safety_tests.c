#define RELAY_PIN 22
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

void setup() {
  Serial.begin(115200);
  digitalWrite(RELAY_PIN, RELAY_OFF);  // set state BEFORE pinMode to avoid glitch
  pinMode(RELAY_PIN, OUTPUT);
  delay(2000);  // hold OFF for 2 full seconds, visible to the eye
  Serial.println("Setup complete, entering loop");
}

void loop() {
  Serial.println("Relay ON");
  digitalWrite(RELAY_PIN, RELAY_ON);
  delay(3000);

  Serial.println("Relay OFF");
  digitalWrite(RELAY_PIN, RELAY_OFF);
  delay(3000);
}

//RST button on ESP32-C3 causes a reset, which is expected. The bootloader will run again, and the program will start from setup() again. The relay will be OFF for 2 seconds, then the loop will start again. This is normal behavior for a microcontroller reset.
// Setup complete, entering loop 
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
// ets Jul 29 2019 12:21:46
// rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
// configsip: 0, SPIWP:0xee
// clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
// mode:DIO, clock div:1
// load:0x3fff0030,len:4876
// ho 0 tail 12 room 4
// load:0x40078000,len:16560
// load:0x40080400,len:3500
// entry 0x400805b4
// Setup complete, entering loop
// Relay ON
// Relay OFF
// Relay ON
// ets Jul 29 2019 12:21:46
// rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
// configsip: 0, SPIWP:0xee
// clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
// mode:DIO, clock div:1
// load:0x3fff0030,len:4876
// ho 0 tail 12 room 4
// load:0x40078000,len:16560
// load:0x40080400,len:3500
// entry 0x400805b4
// Setup complete, entering loop
// Relay ON
// ets Jul 29 2019 12:21:46
// rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
// configsip: 0, SPIWP:0xee
// clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
// mode:DIO, clock div:1
// load:0x3fff0030,len:4876
// ho 0 tail 12 room 4
// load:0x40078000,len:16560
// load:0x40080400,len:3500
// entry 0x400805b4
// Setup complete, entering loop
// Relay ON
// ets Jul 29 2019 12:21:46
// rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
// configsip: 0, SPIWP:0xee
// clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
// mode:DIO, clock div:1
// load:0x3fff0030,len:4876
// ho 0 tail 12 room 4
// load:0x40078000,len:16560
// load:0x40080400,len:3500
// entry 0x400805b4
// Setup complete, entering loop
// Relay ON
// �ets Jul 29 2019 12:21:46
// rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
// configsip: 0, SPIWP:0xee
// clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
// mode:DIO, clock div:1
// load:0x3fff0030,len:4876
// ho 0 tail 12 room 4
// load:0x40078000,len:16560
// load:0x40080400,len:3500
// entry 0x400805b4
// Setup complete, entering loop
// Relay ON
// ets Jul 29 2019 12:21:46
// rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
// configsip: 0, SPIWP:0xee
// clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
// mode:DIO, clock div:1
// load:0x3fff0030,len:4876
// ho 0 tail 12 room 4
// load:0x40078000,len:16560
// load:0x40080400,len:3500
// entry 0x400805b4
// Setup complete, entering loop
// Relay ON
// ets Jul 29 2019 12:21:46
// rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
// configsip: 0, SPIWP:0xee
// clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
// mode:DIO, clock div:1
// load:0x3fff0030,len:4876
// ho 0 tail 12 room 4
// load:0x40078000,len:16560
// load:0x40080400,len:3500
// entry 0x400805b4
// Setup complete, entering loop
// Relay ON
// Relay OFF
// Relay ON
// Relay OFF
// Relay ON
