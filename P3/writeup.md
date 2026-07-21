```markdown
# Project 3: Analog Sensing — LDR Voltage Divider, ADC, and Threshold Light Switch

## Objective
Read an LDR and a potentiometer through the ESP32's 12-bit ADC, convert raw counts to volts, and build a threshold light switch with hysteresis. Extended with WiFi to POST light levels to the Project 2 Flask server.

---

## 1. Potentiometer (Baseline ADC Test)

Wired a 10kΩ potentiometer as a voltage divider (wiper → GPIO 35, an ADC1 pin). With `analogSetAttenuation(ADC_11db)` set, ADC readings swept the full 0–4095 range as the knob was turned, corresponding to 0.00V–3.30V. Multimeter readings at the wiper matched Serial Monitor voltage within expected tolerance.

**Note:** the physical pot was wired with its ends reversed (turning left increased voltage instead of right). This doesn't affect the ADC — it reads whatever voltage is present regardless of knob direction — and was left as-is.

---

## 2. LDR Resistance Measurement

Measured with multimeter (resistance mode, probes on LDR legs, loose from breadboard):

| Condition | Measured Range | Value Used |
|---|---|---|
| Bright light | 1kΩ – 5kΩ | 3kΩ (midpoint) |
| Darkness | 50kΩ – 100kΩ | 75kΩ (midpoint) |

## 3. Choosing the Fixed Resistor

The ideal fixed resistor is the geometric mean of the two extremes, since LDR resistance varies multiplicatively (not additively) across a wide range:

```
R_ideal = √(R_bright × R_dark) = √(3,000 × 75,000) = √225,000,000 ≈ 15,000 Ω
```

10kΩ is the closest standard resistor value to this ideal (22kΩ was the other nearby option but further away), so **10kΩ** was chosen.

---

## 4. Voltage Divider — Hand Calculation

Wiring: `3.3V → LDR → ADC (GPIO 35) → 10kΩ → GND`

Formula (derived from Ohm's Law, series resistors share current):

```
Vout = Vin × R_bottom / (R_top + R_bottom) = 3.3 × 10k / (R_LDR + 10k)
```

**Bright light** (R_LDR = 3kΩ):
```
Vout = 3.3 × 10k / (3k + 10k) = 3.3 × 0.769 = 2.54 V
ADC  = 2.54 / 3.3 × 4095 ≈ 3156
```

**Darkness** (R_LDR = 75kΩ):
```
Vout = 3.3 × 10k / (75k + 10k) = 3.3 × 0.118 = 0.39 V
ADC  = 0.39 / 3.3 × 4095 ≈ 483
```

### Predicted vs Measured

| Condition | Predicted V | Predicted ADC | Measured |
|---|---|---|---|
| Bright (phone flashlight) | 2.54 V | ~3156 | 2.2–2.7 V, ADC 2750–3394 |
| Dark (room lights off) | 0.39 V | ~483 | ~0.00 V, ADC 0 |

Bright-light measurements matched the prediction closely. The dark measurement went lower than predicted because room lights fully off gave darker conditions than the 75kΩ midpoint used in the calculation — consistent, not contradictory.

---

## 5. ADC Resolution and Attenuation

The ESP32 ADC is 12-bit (0–4095 counts) across a 0–3.3V range when configured with 11dB attenuation (`ADC_11db`); default attenuation (0dB) only covers 0–1.1V.

**Attenuation experiment:** removed the `analogSetAttenuation(ADC_11db)` call and expected saturation at 4095 above 1.1V. No saturation was observed — readings were nearly identical with or without the call. Conclusion: Arduino-ESP32 core v3.3.10 applies 11dB attenuation internally by default inside `analogRead()`, making the explicit call redundant on this core version, though it remains good practice for portability and clarity.

**Non-linearity:** documented per ESP32 datasheet — accuracy degrades below ~100mV and above ~3.1V; the LDR divider's working range (0.3–3.0V) stays mostly within the accurate zone.

---

## 6. ADC Noise: Before/After Filtering

**Setup for fair comparison:** phone flashlight resting on the table (not hand-held) over the LDR, room lights off, completely undisturbed for the full test.

**Filtering added:**
- 100nF capacitor wired in parallel with the 10kΩ resistor (junction node → GND) — hardware low-pass filter, τ = R×C = 10kΩ × 100nF = 1ms.
- 16-sample software average with `delayMicroseconds(100)` between samples.

| Metric | Raw (no averaging) | Averaged (16-sample) |
|---|---|---|
| Typical cluster | 1895–1940 | 1895–1913 |
| Lowest value seen | 1728 | 1877 |
| Highest value seen | 2041 | 1919 |
| Spread (max − min) | 313 counts | 42 counts |
| Stray spikes >50 counts from mean | ~15–20 occurrences / 1100 readings | 0 occurrences / 200 readings |

**Conclusion:** averaging didn't just reduce typical jitter — it eliminated outlier spikes entirely. A single corrupted sample only shifts a 16-sample average by 1/16th of its error, so large one-off glitches get absorbed rather than showing up in the output.

**Capacitor-removal experiment:** physically removed the 100nF cap and repeated the same steady-light raw-reading test. Noise dips became more frequent and occurred in tighter clusters (e.g. runs of readings dropping 30–100+ counts below baseline) compared to the with-capacitor data — confirming the capacitor measurably reduces the frequency and severity of transient dips.

---

## 7. ESP32 ADC Quirks — ADC2/WiFi Conflict

GPIO 25 (an ADC2 pin, left floating) was read with WiFi off, then again with WiFi actively connected:

| Condition | Observed Pattern |
|---|---|
| WiFi OFF | Smooth, repeating oscillation (0 → rising → peak ~950 → falling → 0), ambient mains-frequency pickup typical of a floating pin |
| WiFi ON | Flatlined at 0 for 75+ consecutive readings, zero variation |

**Conclusion:** ADC2 hardware shares circuitry with the WiFi radio and becomes unusable once WiFi is active. ADC1 pins (GPIO 35, used for the LDR throughout this project) have no such conflict — confirmed directly, since the LDR readings stayed clean and correct throughout Stage 8 while WiFi was connected and actively POSTing.

**Design rule applied:** always use ADC1 pins (32–39) for sensors in any WiFi-enabled ESP32 design.

---

## 8. Threshold Light Switch with Hysteresis

Two thresholds, recalibrated from the generic spec (originally 450/550) to this room's actual measured ADC range (near-0 in darkness, 100–300 in normal dim room light):

```
DARK_THRESHOLD   = 150   → LED turns ON  (dark)
BRIGHT_THRESHOLD = 350   → LED turns OFF (bright)
```

Logic: LED only switches ON when averaged ADC drops below 150, and only switches OFF when it rises above 350. Between 150–350, the LED holds its current state regardless of fluctuation.

**Why hysteresis prevents flicker:** with a single threshold, noise causing the reading to bounce back and forth across that one value would rapidly toggle the LED. A 200-count dead zone means the light level must clearly cross fully into "dark" or "bright" territory before the state changes — small in-range fluctuations are ignored.

**Verification:** during a drift test, the averaged ADC wandered between 923 and 1040 over ~40 readings (light source held loosely, drifting but staying well above 350). The LED remained OFF the entire time with zero flicker, confirming the dead zone correctly absorbs fluctuations. Transitions at the true boundaries (0 → 1894 and back) triggered exactly once each, instantly and cleanly.

---

## 9. WiFi Extension

Added a `/light` POST endpoint to a Flask server, with the ESP32 sending `{light_raw, light_volts, ts}` as JSON every 5 seconds using `millis()`-based non-blocking timing (so ADC sampling and hysteresis logic keep running uninterrupted between POSTs). WiFi auto-reconnect logic checks `WiFi.status()` every loop and re-attempts connection if dropped.

Confirmed working end-to-end: WiFi connected (IP assigned), LDR readings and LED hysteresis continued normally, and `[POST] Success, code: 200` appeared every 5 seconds, with data logged to `light_readings.csv` on the server.

---

## Conclusion

Built a working LDR-based threshold light switch with hysteresis, verified against hand-calculated voltage divider predictions, with a hardware + software noise filter that eliminated outlier spikes almost entirely (313-count worst-case spread reduced to 42 counts). Confirmed real ESP32 ADC quirks firsthand — the attenuation-default behavior for this core version, and a hard ADC2/WiFi conflict proven with clean floating-pin data — rather than taking them from documentation alone. Extended the system to report light levels over WiFi every 5 seconds without disrupting local sensing or control logic.
```