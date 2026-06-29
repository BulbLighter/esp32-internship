// Stage 8B Part A — ADC2 test, NO WiFi
const int ADC2_TEST_PIN = 25;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ADC2 Test (GPIO 25) — WiFi OFF ===");
}

void loop() {
  int raw = analogRead(ADC2_TEST_PIN);
  Serial.print("GPIO 25 raw (WiFi OFF): ");
  Serial.println(raw);
  delay(500);
}

// part A

// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 64
// GPIO 25 raw (WiFi OFF): 226
// GPIO 25 raw (WiFi OFF): 443
// GPIO 25 raw (WiFi OFF): 659
// GPIO 25 raw (WiFi OFF): 806
// GPIO 25 raw (WiFi OFF): 859
// GPIO 25 raw (WiFi OFF): 798
// GPIO 25 raw (WiFi OFF): 496
// GPIO 25 raw (WiFi OFF): 158
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 71
// GPIO 25 raw (WiFi OFF): 241
// GPIO 25 raw (WiFi OFF): 432
// GPIO 25 raw (WiFi OFF): 658
// GPIO 25 raw (WiFi OFF): 812
// GPIO 25 raw (WiFi OFF): 862
// GPIO 25 raw (WiFi OFF): 738
// GPIO 25 raw (WiFi OFF): 463
// GPIO 25 raw (WiFi OFF): 130
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 112
// GPIO 25 raw (WiFi OFF): 310
// GPIO 25 raw (WiFi OFF): 542
// GPIO 25 raw (WiFi OFF): 753
// GPIO 25 raw (WiFi OFF): 851
// GPIO 25 raw (WiFi OFF): 825
// GPIO 25 raw (WiFi OFF): 627
// GPIO 25 raw (WiFi OFF): 279
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 76
// GPIO 25 raw (WiFi OFF): 260
// GPIO 25 raw (WiFi OFF): 511
// GPIO 25 raw (WiFi OFF): 743
// GPIO 25 raw (WiFi OFF): 914
// GPIO 25 raw (WiFi OFF): 930
// GPIO 25 raw (WiFi OFF): 725
// GPIO 25 raw (WiFi OFF): 383
// GPIO 25 raw (WiFi OFF): 9
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 48
// GPIO 25 raw (WiFi OFF): 208
// GPIO 25 raw (WiFi OFF): 464
// GPIO 25 raw (WiFi OFF): 717
// GPIO 25 raw (WiFi OFF): 895
// GPIO 25 raw (WiFi OFF): 941
// GPIO 25 raw (WiFi OFF): 767
// GPIO 25 raw (WiFi OFF): 431
// GPIO 25 raw (WiFi OFF): 48
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 42
// GPIO 25 raw (WiFi OFF): 203
// GPIO 25 raw (WiFi OFF): 452
// GPIO 25 raw (WiFi OFF): 715
// GPIO 25 raw (WiFi OFF): 900
// GPIO 25 raw (WiFi OFF): 953
// GPIO 25 raw (WiFi OFF): 798
// GPIO 25 raw (WiFi OFF): 461
// GPIO 25 raw (WiFi OFF): 73
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 43
// GPIO 25 raw (WiFi OFF): 208
// GPIO 25 raw (WiFi OFF): 479
// GPIO 25 raw (WiFi OFF): 749
// GPIO 25 raw (WiFi OFF): 920
// GPIO 25 raw (WiFi OFF): 944
// GPIO 25 raw (WiFi OFF): 720
// GPIO 25 raw (WiFi OFF): 345
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 129
// GPIO 25 raw (WiFi OFF): 353
// GPIO 25 raw (WiFi OFF): 653
// GPIO 25 raw (WiFi OFF): 889
// GPIO 25 raw (WiFi OFF): 963
// GPIO 25 raw (WiFi OFF): 796
// GPIO 25 raw (WiFi OFF): 458
// GPIO 25 raw (WiFi OFF): 50
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 80
// GPIO 25 raw (WiFi OFF): 304
// GPIO 25 raw (WiFi OFF): 581
// GPIO 25 raw (WiFi OFF): 841
// GPIO 25 raw (WiFi OFF): 961
// GPIO 25 raw (WiFi OFF): 907
// GPIO 25 raw (WiFi OFF): 599
// GPIO 25 raw (WiFi OFF): 197
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 31
// GPIO 25 raw (WiFi OFF): 198
// GPIO 25 raw (WiFi OFF): 475
// GPIO 25 raw (WiFi OFF): 741
// GPIO 25 raw (WiFi OFF): 912
// GPIO 25 raw (WiFi OFF): 959
// GPIO 25 raw (WiFi OFF): 746
// GPIO 25 raw (WiFi OFF): 374
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 89
// GPIO 25 raw (WiFi OFF): 309
// GPIO 25 raw (WiFi OFF): 586
// GPIO 25 raw (WiFi OFF): 840
// GPIO 25 raw (WiFi OFF): 961
// GPIO 25 raw (WiFi OFF): 933
// GPIO 25 raw (WiFi OFF): 655
// GPIO 25 raw (WiFi OFF): 278
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 111
// GPIO 25 raw (WiFi OFF): 336
// GPIO 25 raw (WiFi OFF): 610
// GPIO 25 raw (WiFi OFF): 858
// GPIO 25 raw (WiFi OFF): 970
// GPIO 25 raw (WiFi OFF): 918
// GPIO 25 raw (WiFi OFF): 624
// GPIO 25 raw (WiFi OFF): 239
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0
// GPIO 25 raw (WiFi OFF): 0